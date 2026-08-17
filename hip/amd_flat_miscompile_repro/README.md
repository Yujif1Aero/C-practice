# AMD flat-address「offset 畳み込み」ミスコンパイル ― 最小 fault 再現コード

ROCm issue #4389 のカーネルを、gfx1100 / ROCm 7.x で**実際に走らせて**クラッシュを再現する最小コード。
OpenLB の grid-refinement が AMD GPU で `HIP error 700`（不正メモリアクセス）で落ちる原因と同じもの。

---

## ビルドと実行

```sh
make          # ./repro4389 を作る
make disasm   # 誤った命令を表示:  flat_load_b32 v0, v[0:1] offset:4
make run      # 実行 -> "an illegal memory access"（＝ HIP error 700）
```

期待される出力（gfx1100 / ROCm 7.1.1 で決定論的に 3/3）:
```
b=0 i= 2 : launch=an illegal memory access was encountered sync=... <<< FAULT (miscompile)
>>> reproduced #4389-style fault at b=0 i=2
```
手動なら:
```sh
hipcc -O3 -std=c++17 --offload-arch=gfx1100 repro_4389.hip.cpp -o repro4389
HIP_VISIBLE_DEVICES=0 HIP_LAUNCH_BLOCKING=1 ./repro4389
```

---

# ゼロから全部説明する（初心者 → プロ）

「用語も何をしてるかも分からない」状態から読めるように、**コンピュータのメモリの基礎**→**ポインタ**→
**GPU**→**このコード1行ずつ**→**なぜ落ちるか**の順で説明します。

## 1. メモリ・バイト・アドレス（いちばん土台）
- コンピュータの**メモリ（RAM）**は、**1 バイト（=8 ビット）ごとに区切られた巨大なロッカーの列**。
- 各ロッカーには**通し番号**が付いている。これが**アドレス（番地）**。例：0 番地, 1 番地, 2 番地, …
- `double` 型の数は 8 バイト＝ロッカー 8 個ぶん。`double` を 1 個置くと、例えば 100〜107 番地を占める。
- **「変数を読む」＝「その番地のロッカーの中身を取り出す」**。

## 2. ポインタ・配列・`p[i]`
- **ポインタ**＝「**番地そのものを値として持つ変数**」。`double* p` は「ある double が置かれた番地」を指す。
- **配列** `double a[N]` ＝ double を N 個、番地が**連続して**並んだもの。`a` は先頭の番地。
- `p[i]` は **`*(p + i)`** の意味。「p の番地から **i 個ぶん（＝ i×8 バイト）先**のロッカーを読む」。
  - 例：`p` が 100 番地なら `p[3]` は 100 + 3×8 = 124 番地を読む。
- `p - K` は「K 個ぶん**手前**の番地」。**配列の外を指すポインタを作るのは C++ では反則（未定義動作）**。
  だが今回の話ではまさにそれが起きる。

## 3. GPU の基礎（CPU と何が違う？）
- **GPU** は**同じ処理を何千スレッドも同時に**走らせる並列プロセッサ。
- GPU で動く関数を**カーネル**と呼ぶ。`__global__` を付けた関数がそれ（CPU から呼び、GPU で走る）。
- `__device__` は「**GPU 上でだけ呼べる補助関数**」（カーネルの中から呼ぶ）。
- カーネル起動 `kernel<<<ブロック数, スレッド数>>>(引数…)`。例 `<<<1,32>>>` は 32 スレッドを 1 グループ実行。
- 各スレッドは自分の番号 `threadIdx.x`（0,1,2,…）を持ち、それで担当データを変える。

## 4. GPU の「4 種類のメモリ」と flat
GPU には物理的に別のメモリがある（ここが今回の核心）:
| 種類 | 何 | 速さ/大きさ | このコードで |
|---|---|---|---|
| **global** | ボード上の VRAM | 大きい・遅い | `g`（`hipMalloc` で確保）|
| **shared (LDS)** | 各スレッドグループ内の共有小メモリ | 小さい・速い | `__shared__ float s[32]` |
| **private** | スレッド専用（レジスタ退避）| 極小 | （今回は出番なし）|
| **flat / generic** | 「上のどれか、実行時まで不明」を表す統一番地 | ― | `b ? s : g` の結果 |

**flat 番地**は、番地の**上位ビットに「どの種類か」を示す印（aperture＝窓）**を埋め込む。
ハードはロード時にこの印を見て「これは global／shared／private」と振り分ける。

## 5. コードを 1 行ずつ

```c
__device__ float explodePlease(float *f, int i){ return f[1 - i]; }
```
- GPU 補助関数。ポインタ `f` と整数 `i` を受け取り、**`f[1 - i]`**（＝ `*(f + (1-i))`）を返すだけ。
- `i=2` なら `f[1-2] = f[-1]` ＝ **f の 1 個手前**を読む（＝配列の外！）。名前どおり「爆発してplease」。

```c
__global__ void kernel(float *g, int i, bool b){
    __shared__ float s[32];        // (A) グループ共有の小配列（shared 地区）
    s[threadIdx.x] = 0;            // (B) 各スレッドが自分のマスを 0 で初期化
    __syncthreads();               // (C) 全スレッドが (B) を終えるまで待つ（足並み揃え）
    *g = explodePlease(b ? s : g, i);  // (D) ここが問題
}
```
- (A) `__shared__ float s[32]`：**shared（LDS）地区**に 32 個の float を置く。
- (B) `s[threadIdx.x]=0`：スレッド 0 は s[0]、スレッド 1 は s[1]…を 0 に。
- (C) `__syncthreads()`：グループ内の**全スレッドの待ち合わせ**（バリア）。全員 (B) 完了を保証。
- (D) `b ? s : g`：**b が真なら shared の s、偽なら global の g** を `explodePlease` に渡す。
  → ポインタが「shared かも / global かも」で、**コンパイル時に種類を確定できない＝flat 扱いになる**。
  これが「**flat を強制する**」トリック。渡した先で `f[1-i]` を読む。

## 6. なぜ「flat が強制」されると危ないか
`b ? s : g` のせいで、`f[1-i]` のロードは**flat_load 命令**になる（種類が実行時まで不明だから）。
コンパイラは速度のため `f[1-i]` を「**基準番地 base ＋ 即値オフセット**」に分解して 1 命令に畳む（offset folding）。
`make disasm` で見えるのがそれ：
```
flat_load_b32 v0, v[0:1] offset:4      # v[0:1]=base, +4 バイト読む
```

**バグの本体（重要）**：ハードは「**どの地区か**」を、**offset を足す前の base だけで判定**する。
- `i=2` のとき `f[-1]`。畳み込みで `base = g - 8`（g の 1 個＝8 バイト手前）、`offset:4` となる。
- 最終番地は `g - 8 + 4 = g - 4`（まだ g の外）だが、**ハードは base（g-8）で地区判定**する。
- **g の割り当ての外**を指す base は、地区の印がおかしくなる／未マップ領域を指す → **不正アクセスでクラッシュ**。

つまり「**最終地点ではなく出発点(base)で地区を決める**」のが誤り。郵便で例えると、
「建物 X から +4 軒目」の配達で、**配達員が地区を建物 X（＝配達先の手前）で判断**してしまう感じ。

## 7. `main()` と HIP ランタイム用語
```c
float* g; hipMalloc(&g, sizeof(float)*64);   // GPU の global メモリを 64 個ぶん確保。g はその先頭番地
hipMemset(g, 0, sizeof(float)*64);           // その領域を 0 埋め
...
kernel<<<1,32>>>(g, i, (bool)b);             // カーネル起動（32 スレッド）
hipError_t le = hipGetLastError();           // 起動自体のエラー
hipError_t se = hipDeviceSynchronize();      // GPU の完了待ち＋実行エラー回収
```
- `hipMalloc`：GPU 側メモリの確保（CPU の malloc の GPU 版）。
- `hipMemset`：その領域をある値で埋める。
- `hipDeviceSynchronize`：**GPU の処理が終わるまで CPU が待つ**。GPU は普段**非同期**（投げっぱなし）なので、
  エラーは後でここに現れる。返り値が「不正アクセス」なら落ちている。
- `HIP_LAUNCH_BLOCKING=1`（実行時の環境変数）：**各カーネルを同期実行**にして、**落ちた瞬間に**エラーを出す。
  デバッグで「どこで落ちたか」を正確に取るための設定。
- `an illegal memory access was encountered` ＝ **`hipErrorIllegalAddress`、値は 700**。
  つまり **これが「HIP error 700」の正体**（文言が違うだけ）。

## 8. なぜ `i` を振る？（-2..4 をループ）
どの `i` で base が「割り当ての外」に落ちるかは、畳み込み結果と確保位置で決まる。
このコードは `i=-2..4`, `b=0/1` を順に試し、**最初に落ちた条件を報告**する。本機では `b=0, i=2` で落ちる。

## 9. これが OpenLB とどう繋がるか
OpenLB の GPU streaming（population の循環バッファ）「Branching」方式は、`base[(shift+i) mod N]` を
mod の代わりに **`start1 = base - (N - shift)`（配列の手前を指すポインタ）** を使って表す。
これは #4389 と**同型**（域外 base ＋ flat_load offset 畳み込み）。
- **-O3 で落ち／-O0 で消える**（畳み込みは -O3 のみ）。
- **解像度 RES8 で落ち／RES4 で落ちない**（確保サイズで base の着地が変わる＝配置依存）。
- 回避策：`FEATURES := GPU_CYCLIC_VMEM`（VMM の `ptr[i]` はこの形にならない）、または refinement を -O0。
- 恒久対応：AMD/ROCm へ codegen バグ報告（#4389 と同型）。

## 10. まとめ（プロ向け 1 行）
`flat_load [base] offset:K` で **base が自分の割り当ての外**を指すと、ハードが base で address space を
誤判定し不正アクセス。**①flat 強制（`b?s:g`）②即値 offset 畳み込み（`f[1-i]`）③base が割り当て外（`i=2`）**
の 3 つが揃うと決定論的に発火。`-O0`／`VMM`／`CUDA(nvcc)` で消えるのはこの形にならないから。

## 参考
- ROCm Issue #4389 — Flat address space operations can miscompile
  https://github.com/ROCm/ROCm/issues/4389
- 詳細解説: `~/X-output/2026-08-14-amdgpu-flat-address-offset-miscompile.md`
- 隣の実験: `../amd_flat_miscompile_instruction_only/`（命令の形だけ再現・fault しない例）,
  `../amd_branching_accessor_patterns/`（fault しなかった 5 パターンと、その理由）
