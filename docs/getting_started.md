# Getting Started

## About rounding
As different parts of a pipeline requires different levels of precision we have an option to
round values of dimensions accordingly.
A canvas+framing decision for a "raw" camera canvas should in theory keep more precision than a
canvas+framing decision for a conformed VFX plate.

The rules for rounding strategy are the same as for [CanvasTemplate.round](FDL Classes/common.md#fdl.RoundStrategy)


The [default](FDL Classes/common.md#fdl.DEFAULT_ROUNDING_STRATEGY) strategy is to not apply rounding and keep float values where applicable, but this may be
overridden by setting the rounding strategy via the
[`set_rounding()`](FDL Classes/common.md#fdl.set_rounding) function

> **NOTE!** The rounding strategy is set globally for where rounding applies except for `CanvasTemplate.round`
> which follows its own rules.

### Setting the global rounding strategy
Here are a some examples of how to set the rounding strategy:

```python
import fdl

# Reset to the default rounding strategy
fdl.set_rounding(fdl.DEFAULT_ROUNDING_STRATEGY)

# For other requirements pass a RoundStrategy instance with the desired rules
fdl.set_rounding(fdl.RoundStrategy(even="whole", mode="up"))
```

## Usage Examples
### Create an FDL from scratch

```python
import fdl
from fdl import DimensionsInt, DimensionsFloat, PointFloat, write_to_file
from tempfile import NamedTemporaryFile

# A clean FDL instance populates the header with a valid starting point
my_fdl = fdl.FDL()

# Let's create a framing intent and add it to our FDL
framing_intent = my_fdl.add_framing_intent(
    id="FDLSMP03",
    label="1.78-1 Framing",
    aspect_ratio=DimensionsInt(width=16, height=9),
    protection=0.088,
)

# Now let's create a context and add a canvas to it
context = my_fdl.add_context(label="PanavisionDXL2")
canvas = context.add_canvas(
    id="20220310",
    label="Open Gate RAW",
    source_canvas_id="20220310",
    dimensions=DimensionsInt(width=5184, height=4320),
    anamorphic_squeeze=1.30,
)

# Set optional canvas properties
canvas.set_effective(
    dims=DimensionsInt(width=5184, height=4320),
    anchor=PointFloat(x=0, y=0),
)
canvas.photosite_dimensions = DimensionsInt(width=5184, height=4320)
canvas.physical_dimensions = DimensionsFloat(width=25.92, height=21.60)

# Add a framing decision referencing our framing intent
canvas.add_framing_decision(
    id="20220310-FDLSMP03",
    label="1.78-1 Framing",
    framing_intent_id=framing_intent.id,
    dimensions=DimensionsFloat(width=5184.0, height=2916.0),
    anchor_point=PointFloat(x=0.0, y=702.0),
)

# Validate our FDL and save it
with NamedTemporaryFile(suffix='.fdl', delete=False) as f:
    write_to_file(my_fdl, f.name, validate=True)
```

### Apply a Canvas Template
```python
import fdl
from fdl import read_from_file, write_to_file
from pathlib import Path
from tempfile import NamedTemporaryFile

# Load an FDL that contains canvas templates
fdl_dir = Path(fdl.__file__).parents[1] / 'tests' / 'sample_data'
my_fdl = read_from_file(fdl_dir / 'Scenario-9__OriginalFDL_UsedToMakePlate.fdl')

# Select the first canvas in the first context
context = my_fdl.contexts[0]
source_canvas = context.canvases[0]
source_framing = source_canvas.framing_decisions[0]

# Select the first canvas template
canvas_template = my_fdl.canvas_templates[0]

# Apply the template to produce a new FDL with the transformed canvas
result = canvas_template.apply(
    source_canvas=source_canvas,
    source_framing=source_framing,
    new_canvas_id="new_canvas",
    new_fd_name="",
    source_context_label=context.label,
)

# result.fdl contains the new FDL
with NamedTemporaryFile(suffix='.fdl', delete=False) as f:
    write_to_file(result.fdl, f.name, validate=False)
```

### Resolve Canvas for Input Dimensions
When input image dimensions don't match the selected canvas, use
`Context.resolve_canvas_for_dimensions()` to find the correct sibling canvas.

The method searches sibling canvases (only if the selected canvas is derived, i.e.
`id != source_canvas_id`) for one whose dimensions match the input, with a framing
decision that has the same label. Returns a `ResolveCanvasResult` with `canvas`,
`framing_decision`, and `was_resolved` fields.

```
from fdl import DimensionsFloat

# Given a context with multiple canvases (source and derived):
input_dims = DimensionsFloat(width=1920.0, height=1080.0)

result = context.resolve_canvas_for_dimensions(
    input_dims, selected_canvas, selected_fd
)
resolved_canvas = result.canvas
resolved_fd = result.framing_decision
was_resolved = result.was_resolved  # True if a different canvas was found
```
