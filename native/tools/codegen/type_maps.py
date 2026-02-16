# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""
Per-language type mappings from IDL type keys to target language types.

Each mapping resolves the language-neutral ``type_key`` from the IR
(which matches C ABI types or IDL value-type names) to the concrete
type string used in generated code.

Currently only Python is implemented. C++ and Node.js maps will be
added in a later phase.
"""

from __future__ import annotations

# -----------------------------------------------------------------------
# Python
# -----------------------------------------------------------------------

PYTHON_TYPES: dict[str, str] = {
    # Scalars
    "string": "str",
    "double": "float",
    "int": "int",
    "int64_t": "int",
    "uint32_t": "int",
    "bool": "bool",
    "bytes": "bytes",
    "handle": "object",
    "json_value": "object",
    # Value types (FDL-specific)
    "fdl_dimensions_i64_t": "DimensionsInt",
    "fdl_dimensions_f64_t": "DimensionsFloat",
    "fdl_point_f64_t": "PointFloat",
    "fdl_rect_t": "Rect",
    "fdl_round_strategy_t": "RoundStrategy",
    "fdl_geometry_path_t": "GeometryPath",
    "fdl_fit_method_t": "FitMethod",
    "fdl_halign_t": "HAlign",
    "fdl_valign_t": "VAlign",
    "fdl_geometry_t": "Geometry",
    "clip_id": "ClipID",
}


# -----------------------------------------------------------------------
# Converter patterns (type_key → converter name used by facade templates)
# -----------------------------------------------------------------------

PYTHON_CONVERTERS: dict[str, str] = {
    "string": "string",
    "double": "float",
    "int": "int",
    "int64_t": "int",
    "uint32_t": "int",
    "bool": "bool",
    "json_value": "json_value",
    "fdl_dimensions_i64_t": "dims_i64",
    "fdl_dimensions_f64_t": "dims_f64",
    "fdl_point_f64_t": "point_f64",
    "fdl_rect_t": "rect",
    "fdl_round_strategy_t": "round_strategy",
    "fdl_geometry_path_t": "enum_geometry_path",
    "fdl_fit_method_t": "enum_fit_method",
    "fdl_halign_t": "enum_halign",
    "fdl_valign_t": "enum_valign",
    "clip_id": "clip_id",
}


def resolve_python_type(type_key: str, *, nullable: bool = False) -> str:
    """Resolve an IDL type key to a Python type annotation string."""
    base = PYTHON_TYPES.get(type_key, type_key)
    if nullable:
        return f"{base} | None"
    return base


# -----------------------------------------------------------------------
# C++
# -----------------------------------------------------------------------

CPP_TYPES: dict[str, str] = {
    # Scalars
    "string": "std::string",
    "double": "double",
    "int": "int",
    "int64_t": "int64_t",
    "uint32_t": "uint32_t",
    "bool": "bool",
    "bytes": "std::string",
    # Value types — used directly as C types
    "fdl_dimensions_i64_t": "fdl_dimensions_i64_t",
    "fdl_dimensions_f64_t": "fdl_dimensions_f64_t",
    "fdl_point_f64_t": "fdl_point_f64_t",
    "fdl_rect_t": "fdl_rect_t",
    "fdl_round_strategy_t": "fdl_round_strategy_t",
    "fdl_geometry_path_t": "fdl_geometry_path_t",
    "fdl_fit_method_t": "fdl_fit_method_t",
    "fdl_halign_t": "fdl_halign_t",
    "fdl_valign_t": "fdl_valign_t",
    "fdl_geometry_t": "fdl_geometry_t",
}


def resolve_cpp_type(type_key: str, *, nullable: bool = False) -> str:
    """Resolve an IDL type key to a C++ type string."""
    base = CPP_TYPES.get(type_key, type_key)
    if nullable:
        return f"std::optional<{base}>"
    return base
