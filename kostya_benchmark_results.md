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
|  C++ DAW JSON Link | 0.13 ± 00.03 |  103.04 ± 02.34 |  2.06 ± 00.08 |
|           GDC fast | 0.13 ± 00.00 |  231.91 ± 00.08 |  1.11 ± 00.02 |
|   Rust Serde Typed | 0.17 ± 00.00 |  120.13 ± 00.05 |  1.72 ± 00.01 |
|  Rust Serde Custom | 0.18 ± 00.00 |  108.62 ± 00.06 |  1.65 ± 00.01 |
|          C++ gason | 0.20 ± 00.00 |  313.24 ± 00.08 |  1.99 ± 00.05 |
|       C++ simdjson | 0.22 ± 00.00 |  496.44 ± 02.25 |  2.13 ± 00.04 |
|      C++ RapidJSON | 0.26 ± 00.00 |  345.28 ± 00.08 |  2.59 ± 00.06 |
|  C++ RapidJSON SAX | 0.57 ± 00.00 |  110.21 ± 00.08 |  5.66 ± 00.11 |
| Rust Serde Untyped | 0.74 ± 00.01 |  916.60 ± 00.05 |  7.19 ± 00.07 |
|            V Clang | 1.43 ± 00.01 |  592.59 ± 00.06 | 14.03 ± 00.12 |
|         C++ json-c | 1.97 ± 00.01 | 1756.33 ± 00.07 | 19.16 ± 00.20 |
|                GDC | 3.23 ± 00.08 |  713.84 ± 00.07 | 33.26 ± 00.61 |
|          C++ Boost | 4.18 ± 00.04 | 1549.93 ± 00.07 | 43.55 ± 02.29 |
|            Rust jq | 4.50 ± 00.00 | 1013.57 ± 01.60 | 40.61 ± 00.38 |

### Linux 5.3.0-29 32GB Intel(R) Core(TM) i7-7500U CPU @ 2.70GHz
clang 10.0.0, rustc 1.37.0, V 0.1.24 80d936a

|           Language |      Time, s |     Memory, MiB |     Energy, J |
| :----------------- | -----------: | --------------: | ------------: |
|  C++ DAW JSON Link | 0.13 ± 00.01 |   99.70 ± 02.81 |  2.25 ± 00.06 |
|           GDC fast | 0.13 ± 00.00 |  231.90 ± 00.04 |  1.10 ± 00.01 |
|  Rust Serde Custom | 0.17 ± 00.00 |  108.63 ± 00.07 |  1.65 ± 00.02 |
|   Rust Serde Typed | 0.17 ± 00.00 |  120.14 ± 00.14 |  1.72 ± 00.02 |
|       C++ simdjson | 0.21 ± 00.00 |  491.33 ± 01.09 |  2.09 ± 00.07 |
|          C++ gason | 0.22 ± 00.00 |  313.31 ± 00.09 |  2.28 ± 00.09 |
|      C++ RapidJSON | 0.33 ± 00.00 |  345.23 ± 00.10 |  3.52 ± 00.32 |
|  C++ RapidJSON SAX | 0.59 ± 00.00 |  110.23 ± 00.09 |  6.17 ± 00.39 |
| Rust Serde Untyped | 0.74 ± 00.01 |  916.62 ± 00.08 |  7.19 ± 00.12 |
|            V Clang | 1.43 ± 00.00 |  592.57 ± 00.07 | 13.99 ± 00.12 |
|         C++ json-c | 1.98 ± 00.02 | 1756.33 ± 00.07 | 19.30 ± 00.39 |
|                GDC | 3.21 ± 00.02 |  713.84 ± 00.08 | 33.01 ± 00.32 |
|            Rust jq | 4.50 ± 00.01 | 1013.10 ± 01.35 | 40.37 ± 00.31 |
|          C++ Boost | 4.58 ± 00.02 | 1549.93 ± 00.09 | 47.95 ± 01.89 |

