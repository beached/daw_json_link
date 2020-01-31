This is taken from https://github.com/serde-rs/json-benchmark/ with DAW JSON Link Added

### 2017 Macbook Air. 8GB Ram Intel(R) Core(TM) i5-5350U CPU @ 1.80GHz

|       Parser/File     | Dom     | Dom       | Struct  | Struct   |
|-----------------------|---------|-----------|---------|----------|
| serde_json            |   parse |stringify  |   parse |stringify |
|data/canada.json       | 150 MB/s| 300 MB/s  | 350 MB/s|  230 MB/s|
|data/citm_catalog.json | 230 MB/s| 270 MB/s  | 640 MB/s|  440 MB/s|
|data/twitter.json      | 170 MB/s| 420 MB/s  | 390 MB/s|  490 MB/s|

|       Parser/File     | Dom     | Dom       | Struct  | Struct   |
|-----------------------|---------|-----------|---------|----------|
| json-rust             |   parse |stringify  |   parse |stringify |
|data/canada.json       | 280 MB/s|  520 MB/s |         |          |
|data/citm_catalog.json | 410 MB/s|  360 MB/s |         |          |
|data/twitter.json      | 300 MB/s|  500 MB/s |         |          |

|       Parser/File     | Dom     | Dom       | Struct  | Struct   |
|-----------------------|---------|-----------|---------|----------|
| rustc_serialize       |   parse |stringify  |   parse |stringify |
|data/canada.json       | 110 MB/s|   40 MB/s |  79 MB/s|  29 MB/s |
|data/citm_catalog.json | 110 MB/s|   95 MB/s |  81 MB/s| 140 MB/s |
|data/twitter.json      |  63 MB/s|  210 MB/s |  50 MB/s| 240 MB/s |

|       Parser/File     | Dom     | Dom       | Struct  | Struct   |
|-----------------------|---------|-----------|---------|----------|
| simd-json             |   parse |stringify  |   parse |stringify |
|data/canada.json       | 250 MB/s|  300 MB/s | 450 MB/s|          |
|data/citm_catalog.json | 670 MB/s|  340 MB/s | 990 MB/s|          |
|data/twitter.json      | 600 MB/s|  470 MB/s | 660 MB/s|          |

|       Parser/File     | Dom     | Dom       | Struct  | Struct   |
|-----------------------|---------|-----------|---------|----------|
| DAW JSON Link         |   parse |stringify  |   parse |stringify |
|data/canada.json       |         |           | 655 MB/s|  58 MB/s |
|data/citm_catalog.json |         |           | 590 MB/s| 811 MB/s |
|data/twitter.json      |         |           | 296 MB/s| 277 MB/s |

         
         
         

