# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""
Language-neutral context builders for code generation.

These functions transform the parsed IDL into template-ready dictionaries
consumed by Jinja2 templates for *all* target languages.  Nothing here
references Python types, converters, or adapters.
"""

from __future__ import annotations

from .fdl_idl import EnumType


# -----------------------------------------------------------------------
# Utilities
# -----------------------------------------------------------------------


def camel_to_upper_snake(name: str) -> str:
    """Convert CamelCase to UPPER_SNAKE: GeometryPath → GEOMETRY_PATH."""
    result = ""
    for i, ch in enumerate(name):
        if ch.isupper() and i > 0:
            result += "_"
        result += ch.upper()
    return result


# -----------------------------------------------------------------------
# Enum context builders
# -----------------------------------------------------------------------


def build_enum_context(idl_enum: EnumType) -> dict:
    """Build template context for one enum's forward/reverse maps."""
    prefix = idl_enum.facade_prefix
    values = []
    for ev in idl_enum.values:
        member = ev.name[len(prefix) :]  # strip prefix to get Python member name
        values.append({"name": ev.name, "member": member})

    python_class = idl_enum.facade_class
    map_name = camel_to_upper_snake(python_class)

    return {
        "idl_name": idl_enum.name,
        "python_class": python_class,
        "prefix": prefix,
        "map_name": map_name,
        "entries": values,
    }


def build_constants_enum_context(idl_enum: EnumType) -> dict:
    """Build template context for one StrEnum class in _constants.py."""
    prefix = idl_enum.facade_prefix
    str_values = idl_enum.string_values or {}
    members = []
    for ev in idl_enum.values:
        member_name = ev.name[len(prefix) :]
        str_value = str_values.get(member_name, member_name.lower())
        members.append({"name": member_name, "str_value": str_value})

    return {
        "python_class": idl_enum.facade_class,
        "members": members,
    }
