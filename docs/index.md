# ASC FDL

ASC FDL is a toolkit for parsing, validating, creating, and transforming
[Framing Decision List (FDL)](https://theasc.com/society/ascmitc/asc-framing-decision-list) files.
It is built around a shared C core library (`libfdl_core`) with idiomatic bindings for
**Python** and **C++**, and conforms to the official FDL
[specification](https://github.com/ascmitc/fdl/tree/main/Specification).

## Install

### Python

```shell
pip install fdl
```

### C++

The C++ bindings are a single header-only file wrapping the C ABI:

```cpp
#include "fdl/fdl.hpp"
```

Link against `libfdl_core` (built via CMake from `native/core/`).

## Features

| Feature                    | Python | C++ | Notes                                                |
|:---------------------------|:------:|:---:|------------------------------------------------------|
| Parse & write FDL files    |   ✔    |  ✔  |                                                      |
| Validate IDs & relations   |   ✔    |  ✔  | Enforces unique IDs and valid cross-references       |
| JSON Schema validation     |   ✔    |  ✔  |                                                      |
| Canvas templates           |   ✔    |  ✔  | Apply templates to produce new canvases              |
| Rounding strategies        |   ✔    |  ✔  | Global and per-dimension rounding control            |
| Custom attributes          |   ✔    |  ✔  | Scalar + composite types (PointFloat, DimensionsFloat, DimensionsInt) |
| CLI tool                   |   ✔    |     | `fdl-validate` for command-line validation           |

## API Reference

- **Python API** — see [Getting Started](getting_started.md) and the [FDL Classes](FDL Classes/fdl.md) section
- **C/C++ API** — [Doxygen Reference](https://ascmitc.github.io/fdl/api/) (auto-generated from source)
