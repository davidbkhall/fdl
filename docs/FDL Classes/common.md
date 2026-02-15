# Common

## Global Variables
Version numbers are used as default values in [Header](header.md) and to select a matching json schema file
if no version is set.

::: fdl.FDL_SCHEMA_MAJOR
::: fdl.FDL_SCHEMA_MINOR
::: fdl.FDL_SCHEMA_VERSION
---

Different workflows have different requirements for precision, so we are flexible in how to apply 
rounding of values

::: fdl.DEFAULT_ROUNDING_STRATEGY
This is the default behavior for rounding the values of dimensions. The rules are the same as for
`CanvasTemplate.round`.

::: fdl.NO_ROUNDING
This will disable rounding of values in dimensions. Exception being `Canvas.dimensions` when
created by a canvas templates

## Global Rounding Functions
Use these functions to set the global rounding strategy for values of dimensions

::: fdl.set_rounding_strategy

::: fdl.get_rounding_strategy

---

## Base Classes

Below is a collection of the common classes that are used by other classes.

::: fdl.Base
    options:
        inherited_members: false

::: fdl.TypedCollection
    options:
        inherited_members: false

::: fdl.Dimensions
    options:
        inherited_members: false

::: fdl.Point
    options:
        inherited_members: false

::: fdl.RoundStrategy
    options:
        inherited_members: false
