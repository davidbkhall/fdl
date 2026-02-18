# Application Packages

Beyond the core `fdl` library, the project includes three application packages
in the `packages/` directory. Each is a standalone Python package within the
uv workspace.

## fdl-frameline-generator

Generates frameline overlay images from FDL documents. Supports multiple output
formats: EXR, PNG, TIFF, and SVG.

**Install:** `pip install fdl-frameline-generator`

**CLI:** `fdl-frameline`

```shell
fdl-frameline --input scene.fdl --output framelines.exr --context "Camera A"
```

**Architecture:**

- `cli.py` — command-line entry point
- `config.py` — rendering configuration
- `renderer.py` — rasterized output (EXR, PNG, TIFF)
- `svg_backend.py` — vector SVG output

**Source:** `packages/fdl_frameline_generator/`

## fdl-imaging

OpenImageIO-based image processing for FDL workflows. Extracts framing regions
from images, resizes, and compares results.

**Install:** `pip install fdl-imaging`

Requires [OpenImageIO](https://github.com/AcademySoftwareFoundation/OpenImageIO)
to be available on the system.

**Key module:** `fdl_imaging._processing` — core image manipulation functions.

**Source:** `packages/fdl_imaging/`

## fdl-viewer

PySide6 desktop application for visualizing FDL documents interactively.
Displays source and output canvases with HUD overlays, supports template
transformations, and provides image export.

**Install:** `pip install fdl-viewer`

**Launch:** `fdl-viewer`

**Architecture (MVC):**

- `models/` — data models for FDL documents and rendering state
- `views/` — Qt widget hierarchy (source/output tabs, comparison, details)
- `controllers/` — business logic connecting models to views
- `main.py` — application entry point

**Source:** `packages/fdl_viewer/`

## Core CLI: fdl-validate

The core `fdl` package also provides a command-line validation tool:

```shell
fdl-validate scene.fdl              # validate a single file
fdl-validate *.fdl --strict         # validate multiple files
```

**Source:** `native/bindings/python/fdl/cli.py`
