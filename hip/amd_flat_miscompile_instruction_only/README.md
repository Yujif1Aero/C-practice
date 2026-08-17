# flat-offset miscompile ― 「命令の形」だけ（SAFE）と、引数で落とす（FAULT）の対比

`../amd_flat_miscompile_repro` の**対照＋実演**。同じ 3 要素（①flat 強制 ②offset 畳み込み
③base が割り当ての外）のうち、**③を満たすかどうかで correct↔fault が切り替わる**ことを、
1 本のコードで**引数で**見せる。

## ビルドと実行
```sh
make          # ./ff を作る
make disasm   # flat_load ... offset: が出る（②達成）
make run      # 引数なし = SAFE モード -> "correct"（落ちない）
make fault    # i=0..3 を試す -> i>=2 で FAULT（HIP 700）
```
手動:
```sh
./ff            # SAFE:  base[i + c]  (i は正) -> correct
./ff 2          # FAULT: base[1 - i] (i は負係数, #4389 型) -> illegal memory access (700)
./ff 0 ; ./ff 1 # i=0,1 は base がまだ域内 -> 落ちない
```

## 「引数を与えれば base を g-8 に置いて落とせる？」への答え
- **SAFE カーネル（`base[i + c]`, i が正）は、引数だけでは絶対に落ちない。**
  コンパイラは畳み込み後の base を `start1 + i` と**配列の中に戻す**（`offset:` は小さい +c だけ）。
  base が有効なのでハードの地区誤判定が起きない。
- **落とすには「負の係数」アクセス `buf[1 - i]`（#4389 型）にコードを変える必要がある。**
  こうすると畳み込み後の base = `buf - i` となり、**i>0 で base 自身が割り当ての外**へ。
  そこで初めて**引数 i が「base をどれだけ外に出すか」を決める**：本機では `i>=2` で 700。
- つまり **鍵は"引数"ではなく"アクセスの形（base を域外に出すか）"**。引数は最後の微調整。

## この 1 本で分かること
| 実行 | アクセス | 畳み込み後の base | 結果 |
|---|---|---|---|
| `./ff` | `base[i + c]`（i 正）| `start1 + i`（域内に戻る）| correct |
| `./ff 0` | `buf[1 - 0]=buf[1]` | `buf`（域内）| correct |
| `./ff 1` | `buf[1 - 1]=buf[0]` | `buf - 8`（境界すれすれ・同ページ）| correct |
| `./ff 2` | `buf[1 - 2]=buf[-1]` | `buf - 16`（域外）| **FAULT (700)** |

＝ **base が割り当ての外に出た瞬間に落ちる**。これが `repro_4389` と同じ機序。
詳細: `~/X-output/2026-08-14-amdgpu-flat-address-offset-miscompile.md` の §8.6。
