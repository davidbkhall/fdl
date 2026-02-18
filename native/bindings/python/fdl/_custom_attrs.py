# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
# AUTO-GENERATED from fdl_api.yaml — DO NOT EDIT
# ruff: noqa: I001
"""Shared helpers for custom attribute access on all handle types."""

from __future__ import annotations

import ctypes

from .base import _decode_str


def _set(lib, handle, prefix: str, name: str, value: str | int | float) -> None:
    """Set a custom attribute, dispatching to the typed C setter.

    Args:
        lib: The loaded FFI library.
        handle: The opaque C handle.
        prefix: C function name prefix (e.g. ``"fdl_canvas_"``).
        name: Attribute name (without ``_`` prefix).
        value: Attribute value (str, int, or float).

    Raises:
        TypeError: If value is not str, int, or float.
        ValueError: If an attribute with the same name exists with a different type.
    """
    _name = name.encode("utf-8")
    if isinstance(value, str):
        rc = getattr(lib, f"{prefix}set_custom_attr_string")(handle, _name, value.encode("utf-8"))
    elif isinstance(value, int) and not isinstance(value, bool):
        rc = getattr(lib, f"{prefix}set_custom_attr_int")(handle, _name, value)
    elif isinstance(value, float):
        rc = getattr(lib, f"{prefix}set_custom_attr_float")(handle, _name, value)
    else:
        raise TypeError(f"Custom attribute value must be str, int, or float, got {type(value).__name__}")
    if rc != 0:
        raise ValueError(f"Failed to set custom attribute '{name}' — type mismatch with existing value")


def _get(lib, handle, prefix: str, name: str) -> str | int | float | None:
    """Get a custom attribute value by name.

    Args:
        lib: The loaded FFI library.
        handle: The opaque C handle.
        prefix: C function name prefix.
        name: Attribute name (without ``_`` prefix).

    Returns:
        The attribute value, or None if not found.
    """
    _name = name.encode("utf-8")
    attr_type = getattr(lib, f"{prefix}get_custom_attr_type")(handle, _name)
    if attr_type == 0:
        return None
    if attr_type == 1:
        raw = getattr(lib, f"{prefix}get_custom_attr_string")(handle, _name)
        return _decode_str(raw)
    if attr_type == 2:
        out = ctypes.c_int64()
        getattr(lib, f"{prefix}get_custom_attr_int")(handle, _name, ctypes.byref(out))
        return out.value
    if attr_type == 3:
        out = ctypes.c_double()
        getattr(lib, f"{prefix}get_custom_attr_float")(handle, _name, ctypes.byref(out))
        return out.value
    return None


def _has(lib, handle, prefix: str, name: str) -> bool:
    """Check if a custom attribute exists."""
    return bool(getattr(lib, f"{prefix}has_custom_attr")(handle, name.encode("utf-8")))


def _remove(lib, handle, prefix: str, name: str) -> bool:
    """Remove a custom attribute. Returns True if removed, False if not found."""
    return getattr(lib, f"{prefix}remove_custom_attr")(handle, name.encode("utf-8")) == 0


def _count(lib, handle, prefix: str) -> int:
    """Return the number of custom attributes."""
    return int(getattr(lib, f"{prefix}custom_attrs_count")(handle))


def _all(lib, handle, prefix: str) -> dict[str, str | int | float]:
    """Return all custom attributes as a dictionary."""
    result: dict = {}
    count = getattr(lib, f"{prefix}custom_attrs_count")(handle)
    for i in range(count):
        name_ptr = getattr(lib, f"{prefix}custom_attr_name_at")(handle, i)
        if not name_ptr:
            continue
        name = _decode_str(name_ptr)
        value = _get(lib, handle, prefix, name)
        if value is not None:
            result[name] = value
    return result
