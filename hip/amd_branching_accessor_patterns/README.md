# Branching accessor 5 パターン（A〜E）― どれも fault しない、その理由

OpenLB の Branching cyclic accessor（域外 base `start1 = buf-(N-shift)`）を模した 5 通り。
**いずれも落ちない**ことを示し、「なぜ削ぎ落とした standalone は再現しないか」を記録する対照実験。
実際に落ちる版は `../amd_flat_miscompile_repro`。

## ビルドと実行
```sh
make          # repro_O0 .. repro_O3
make run      # 4 段階すべて "mismatches=0"（落ちない）
```

## 5 パターンとは
| | 中身 | fault しない理由 |
|---|---|---|
| A | `base = buf-1000; base[i]` | global_load 化（flat 強制せず）|
| B | OpenLB 二本ポインタ `(i>rem)?start1[i]:start0[i]` | 同上 |
| C | `buf[C - i]`（#4389 の `f[1-i]` 型）だが global 限定 | flat 強制せず＝global_load |
| D | coupling 形状 Q=19 ＋実行時 shift ＋近傍集約 | 同上 |
| E | ポインタをメモリ表からロードして flat 強制を試行 | ROCm 7.1 が provenance 復元→global_load |

## 結論
**fault の 3 要素（①flat 強制 ②offset 畳み込み ③base が割り当て外）のうち①か③が欠ける**ため、
コンパイラは安全な `global_load` を選ぶ／base が域内に戻る。だから正しく動く。
3 要素を全部満たす最小例は `../amd_flat_miscompile_repro/repro_4389.hip.cpp`。
詳細: `~/X-output/2026-08-14-amdgpu-flat-address-offset-miscompile.md` の §8.8。
