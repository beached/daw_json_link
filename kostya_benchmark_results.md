This is the results of kostya json benchmark
https://github.com/kostya/benchmarks/
Each machine has a different data set, so the relative times/memory/power is what is important

### 2017 Macbook Air. 8GB Ram Intel(R) Core(TM) i5-5350U CPU @ 1.80GHz
Memory/Energy is not available on Macos

|           Language |       Time, s |
| :----------------- | ------------: |
|  C++ DAW JSON Link |  0.19 ± 00.01 |
|   Rust Serde Typed |  0.21 ± 00.00 |
|  Rust Serde Custom |  0.21 ± 00.00 |
|       C++ simdjson |  0.31 ± 00.01 |
|          C++ gason |  0.35 ± 00.00 |
|      C++ RapidJSON |  0.53 ± 00.09 |
|  C++ RapidJSON SAX |  0.78 ± 00.02 |
|            Node.js |  1.10 ± 00.01 |
|     Crystal Schema |  1.19 ± 00.01 |
|       Crystal Pull |  1.19 ± 00.02 |
| Rust Serde Untyped |  1.34 ± 00.02 |
|            Crystal |  1.66 ± 00.05 |
|                 Go |  2.12 ± 00.01 |
|  CPython UltraJSON |  2.43 ± 00.04 |
|             Python |  2.88 ± 00.04 |
|            V Clang |  3.20 ± 00.04 |
|         C++ json-c |  3.95 ± 00.08 |
|               Ruby |  4.10 ± 00.10 |
|            Rust jq |  5.45 ± 00.26 |
|          C++ Boost | 10.42 ± 00.11 |

### Linux 5.3.0-29 32GB Intel(R) Core(TM) i7-7500U CPU @ 2.70GHz
gcc 9.2.1, rustc 1.37.0, V 0.1.24 80d936a

|           Language |      Time, s |     Memory, MiB |     Energy, J |
| :----------------- | -----------: | --------------: | ------------: |
|  C++ DAW JSON Link | 0.13 ± 00.03 |  103.43 ± 01.74 |  2.06 ± 00.09 |
|  Rust Serde Custom | 0.17 ± 00.00 |  108.59 ± 00.06 |  1.68 ± 00.09 |
|   Rust Serde Typed | 0.17 ± 00.00 |  120.24 ± 00.22 |  1.76 ± 00.07 |
|          C++ gason | 0.20 ± 00.00 |  313.28 ± 00.06 |  1.99 ± 00.06 |
|       C++ simdjson | 0.22 ± 00.00 |  497.83 ± 00.85 |  2.16 ± 00.07 |
|      C++ RapidJSON | 0.26 ± 00.00 |  345.24 ± 00.08 |  2.60 ± 00.09 |
|  C++ RapidJSON SAX | 0.57 ± 00.00 |  110.23 ± 00.06 |  5.66 ± 00.17 |
| Rust Serde Untyped | 0.73 ± 00.00 |  916.59 ± 00.06 |  7.24 ± 00.24 |
|            V Clang | 1.43 ± 00.00 |  592.54 ± 00.07 | 14.20 ± 00.77 |
|         C++ json-c | 1.99 ± 00.04 | 1756.30 ± 00.07 | 19.35 ± 00.64 |
|          C++ Boost | 4.16 ± 00.01 | 1549.97 ± 00.09 | 42.49 ± 00.95 |
|            Rust jq | 4.51 ± 00.03 | 1013.43 ± 01.70 | 41.10 ± 01.07 |

### Linux 5.3.0-29 32GB Intel(R) Core(TM) i7-7500U CPU @ 2.70GHz
clang 10.0.0, rustc 1.37.0, V 0.1.24 80d936a

|           Language |      Time, s |     Memory, MiB |     Energy, J |
| :----------------- | -----------: | --------------: | ------------: |
|  C++ DAW JSON Link | 0.14 ± 00.03 |  102.20 ± 03.07 |  2.26 ± 00.10 |
|  Rust Serde Custom | 0.17 ± 00.00 |  108.63 ± 00.09 |  1.65 ± 00.01 |
|   Rust Serde Typed | 0.17 ± 00.00 |  120.13 ± 00.06 |  1.72 ± 00.02 |
|       C++ simdjson | 0.21 ± 00.00 |  491.70 ± 01.46 |  2.05 ± 00.02 |
|          C++ gason | 0.22 ± 00.00 |  313.28 ± 00.09 |  2.24 ± 00.02 |
|      C++ RapidJSON | 0.33 ± 00.00 |  345.26 ± 00.09 |  3.39 ± 00.03 |
|  C++ RapidJSON SAX | 0.59 ± 00.00 |  110.25 ± 00.11 |  6.00 ± 00.21 |
| Rust Serde Untyped | 0.73 ± 00.00 |  916.62 ± 00.07 |  7.16 ± 00.08 |
|            V Clang | 1.43 ± 00.00 |  592.59 ± 00.07 | 14.32 ± 00.86 |
|         C++ json-c | 1.97 ± 00.01 | 1756.33 ± 00.06 | 19.38 ± 01.00 |
|            Rust jq | 4.50 ± 00.01 | 1012.97 ± 01.36 | 40.61 ± 00.65 |
|          C++ Boost | 4.58 ± 00.01 | 1549.91 ± 00.09 | 46.99 ± 01.49 |

