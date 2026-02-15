# ASC Framing Decision List (FDL)

Within this directory you will find many files and folders for the ASC FDL.
Here is a breakdown of their purpose:

**ASC FDL One Sheet**
This is the first document you should read.
This is a very brief introduction to what the FDL is and the problem it has set out to solve.

**ASC FDL User Guide**
This is a document intended for end users to better understand how the FDL could be used within their workflows.
This will also be a great reference for implementers to better understand the intention behind FDL usability.

**ASC FDL DataPoints**
This is a spreadsheet which breaks down each attribute within an FDL.
These are also documented in the specification, but made available within this sheet in case it's helpful for implementers.

**ASC FDL Specification**
This is the technical specification for implementers.

**ASC FDL ERD**
ERD file created to help implementers have quick access to attribute information.

**ASC FDL Schema**
ASC FDL Schema for implementers.

-----------------------------------------

**Google Drive Directory**
https://drive.google.com/drive/folders/1L1opQmDl6qhAik2wx2NEVz7-xbSFQ-ns?usp=sharing

Contains:
 - Sample Files For Implementers
Please use the following link for sample source files, with .FDL's and the resulting expected images.  This will be very helpful for software/hardware developers to test their FDL implementation against our expected results.

 - SMPTE ATC ASCFDL 20211028
This is a recording from a SMPTE presentation introducing the challenges dailies operators face with managing framing, and how the FDL could help.

 - ASC FDL Logos

-----------------------------------------

## Repository Overview

This repository contains the ASC FDL reference implementation: a native C library with language bindings and Python application packages for working with Framing Decision List files.

### Directory Structure

```
.
├── native/                          # Native (C/C++) core and bindings
│   ├── core/                        # libfdl_core — shared C library
│   │   ├── include/fdl/             #   Public C ABI header (fdl_core.h)
│   │   ├── src/                     #   Implementation (parsing, validation, geometry, pipeline, templates)
│   │   └── tests/                   #   Catch2 C++ unit tests with JSON test vectors
│   ├── bindings/
│   │   ├── python/                  # fdl — Python ctypes bindings & facade
│   │   │   ├── fdl/                 #   FDL, Canvas, Context, FramingDecision, etc.
│   │   │   ├── fdl_ffi/             #   Low-level ctypes FFI loader and function declarations
│   │   │   └── tests/              #   Python unit tests (351 tests)
│   │   ├── cpp/fdl/                 # fdl.hpp — C++ RAII header-only wrapper
│   │   └── node/                    # Node.js bindings (placeholder)
│   ├── api/                         # OpenAPI spec (fdl_api.yaml)
│   └── tools/                       # Code generation and test vector extraction scripts
│
├── packages/                        # Python application packages
│   ├── fdl_imaging/                 # fdl-imaging — FDL-based image processing (OpenImageIO)
│   │   ├── src/fdl_imaging/         #   Image extraction, resizing, comparison utilities
│   │   └── tests/                   #   Image processing tests (6 tests)
│   ├── fdl_frameline_generator/     # fdl-frameline-generator — frameline overlay generation
│   │   ├── src/fdl_frameline_generator/  # EXR/PNG/TIFF/SVG frameline rendering
│   │   └── tests/                   #   Renderer + regression tests (157 tests)
│   └── fdl_viewer/                  # fdl-viewer — PySide6 desktop application
│       ├── src/fdl_viewer/          #   MVC architecture (models, views, controllers)
│       └── tests/                   #   UI, controller, model, and visual QC tests (130+ tests)
│
├── schema/                          # JSON Schema definitions (v0.1, v1.0, v2.0, v2.0.1)
├── resources/FDL/                   # Test resources
│   ├── Original_Source_Files/       #   Source FDL + TIFF image pairs
│   ├── Scenarios_For_Implementers/  #   30+ numbered scenarios with expected results
│   └── EdgeCases/                   #   Generated edge-case FDL files
├── docs/                            # Documentation (template logic, implementer guide)
├── Specification/                   # ASC FDL specification documents
└── pyproject.toml                   # uv workspace root
```

### Key Components

**`libfdl_core`** (`native/core/`) — The core engine, written in C++ and exposed via a C ABI. Handles FDL parsing, serialization (JSON), schema validation (Draft 2020-12 via jsoncons), geometry calculations, canvas resolution pipeline, and template application. Built with CMake.

**`fdl`** (`native/bindings/python/`) — Python package providing a high-level facade over `libfdl_core` via ctypes. Classes include `FDL`, `Canvas`, `Context`, `FramingIntent`, `FramingDecision`, and `CanvasTemplate`. Also provides rounding strategies, validation, and a `fdl-validate` CLI tool.

**`fdl-imaging`** (`packages/fdl_imaging/`) — Image processing utilities using OpenImageIO. Extracts framing regions from source images based on FDL geometry and produces output images matching target canvas dimensions.

**`fdl-frameline-generator`** (`packages/fdl_frameline_generator/`) — Generates frameline overlay images from FDL files. Supports EXR, PNG, TIFF (via OIIO), and SVG vector output. Includes a `fdl-frameline` CLI tool.

**`fdl-viewer`** (`packages/fdl_viewer/`) — A PySide6 desktop application for interactively viewing, transforming, and exporting FDL files. Supports source/output scene visualization with HUD overlays, template-based transforms, and image export. Built with an MVC architecture.

-----------------------------------------

## Developer Guide

### Prerequisites

- **Python** 3.10+
- **uv** (Python package manager) — [install instructions](https://docs.astral.sh/uv/getting-started/installation/)
- **CMake** 3.20+
- **C++ compiler** with C++17 support (Clang, GCC, or MSVC)
- **OpenImageIO** 2.4+ (required by `fdl-imaging` and `fdl-frameline-generator`)

### Clone the Repository

```bash
git clone https://github.com/ascmitc/fdl.git
cd fdl
```

### Build the Native Library

The Python packages depend on `libfdl_core`, which must be compiled before running tests or using the Python bindings.

```bash
# Configure
cmake -S native/core -B native/core/build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build native/core/build --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)
```

This produces `native/core/build/libfdl_core.dylib` (macOS), `libfdl_core.so` (Linux), or `fdl_core.dll` (Windows). The Python FFI loader finds it automatically at this path.

### Set Up the Python Environment

```bash
# Create the virtual environment and install all workspace packages + dev dependencies
uv sync --extra dev
```

This installs the full workspace (`fdl`, `fdl-imaging`, `fdl-frameline-generator`, `fdl-viewer`) in editable mode along with test dependencies (pytest, pytest-xdist, pytest-qt, pytest-cov, mktestdocs).

### Run the Tests

**FDL core (Python bindings):**
```bash
uv run pytest native/bindings/python/tests/ -v
```

**FDL Imaging:**
```bash
uv run pytest packages/fdl_imaging/tests/ -v
```

**FDL Frameline Generator:**
```bash
uv run pytest packages/fdl_frameline_generator/tests/ -v
```

**FDL Viewer:**
```bash
# All viewer tests (can take a long time due to UI scenario matrix)
uv run pytest packages/fdl_viewer/tests/ -v

# Exclude slow visual QC tests
uv run pytest packages/fdl_viewer/tests/ -v \
    --ignore=packages/fdl_viewer/tests/test_visual_qc_matrix.py \
    --ignore=packages/fdl_viewer/tests/test_visual_qc_edge_cases.py

# Run only a single UI scenario (recommended for quick validation)
uv run pytest packages/fdl_viewer/tests/ -v \
    --ignore=packages/fdl_viewer/tests/test_visual_qc_matrix.py \
    --ignore=packages/fdl_viewer/tests/test_visual_qc_edge_cases.py \
    -k "not test_scenario_workflow or scen1_B"
```

**Run everything (fast subset):**
```bash
uv run pytest native/bindings/python/tests/ packages/fdl_imaging/tests/ packages/fdl_frameline_generator/tests/ -v
```

### C++ Core Tests

The native C++ tests use Catch2 and can be built and run separately:

```bash
cmake -S native/core -B native/core/build -DCMAKE_BUILD_TYPE=Debug -DFDL_BUILD_TESTS=ON
cmake --build native/core/build --config Debug -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)
ctest --test-dir native/core/build --output-on-failure
```
