# SET 9: String Sorting

This repository contains standalone CodeForces solutions for adapted string sorting tasks and the benchmark harness for the string sorting empirical study.

Public repository: https://github.com/itzephir/set9-string-sorting

## Build

```bash
cmake --build cmake-build-debug --target benchmark
```

## Run Benchmark

Full benchmark:

```bash
./cmake-build-debug/benchmark
```

Fast smoke check:

```bash
./cmake-build-debug/benchmark --smoke --results-dir /private/tmp/string_sort_smoke_results
```

The full run writes:

- `results/measurements.csv`
- `results/summary.csv`
- `results/README.md`

The Kotlin Notebook report is in:

- `report/string_sorting_report.ipynb`

## Standalone Task Files

- `merge_sort.cpp`
- `quick_sort.cpp`
- `radix_sort.cpp`
- `quick_radix_sort.cpp`
