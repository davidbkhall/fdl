# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
# AUTO-GENERATED from fdl_api.yaml — DO NOT EDIT
# ruff: noqa: I001
"""FDL Core FramingIntent facade."""

from __future__ import annotations

import ctypes
import json

from .types import DimensionsInt

from .base import (
    HandleWrapper,
    _decode_str,
)
from .converters import (
    _dims_i64,
    _to_c_dims_i64,
)


class FramingIntent(HandleWrapper):
    """FramingIntent facade wrapping a C fdl_framing_intent_t handle."""

    def __init__(
        self,
        *,
        id: str,
        label: str = "",
        aspect_ratio: DimensionsInt,
        protection: float = 0.0,
    ) -> None:
        from fdl_ffi import get_lib

        lib = get_lib()
        from .fdl import FDL

        _doc_h = lib.fdl_doc_create_with_header(
            b"00000000-0000-0000-0000-000000000000",
            2,
            0,
            b"_",
            None,
        )
        _backing = FDL._from_handle(_doc_h, lib)
        handle = lib.fdl_doc_add_framing_intent(
            _doc_h,
            id.encode("utf-8"),
            label.encode("utf-8"),
            int(aspect_ratio.width),
            int(aspect_ratio.height),
            float(protection),
        )
        if not handle:
            raise RuntimeError("fdl_doc_add_framing_intent returned NULL")
        HandleWrapper.__init__(self, handle, lib, _backing)

    @property
    def id(self) -> str:
        self._check_handle()
        raw = self._lib.fdl_framing_intent_get_id(self._handle)
        return _decode_str(raw)

    @property
    def label(self) -> str:
        self._check_handle()
        raw = self._lib.fdl_framing_intent_get_label(self._handle)
        return _decode_str(raw)

    @property
    def aspect_ratio(self) -> DimensionsInt:
        self._check_handle()
        raw = self._lib.fdl_framing_intent_get_aspect_ratio(self._handle)
        return _dims_i64(raw)

    @aspect_ratio.setter
    def aspect_ratio(self, value: DimensionsInt) -> None:
        self._check_handle()
        self._lib.fdl_framing_intent_set_aspect_ratio(self._handle, _to_c_dims_i64(value))

    @property
    def protection(self) -> float:
        self._check_handle()
        raw = self._lib.fdl_framing_intent_get_protection(self._handle)
        return float(raw)

    @protection.setter
    def protection(self, value: float) -> None:
        self._check_handle()
        self._lib.fdl_framing_intent_set_protection(self._handle, ctypes.c_double(value))

    def __eq__(self, other: object) -> bool:
        if isinstance(other, FramingIntent):
            return self.id == other.id
        if isinstance(other, str):
            return self.id == other
        return NotImplemented

    def __hash__(self):
        return hash((self.id,))

    def as_dict(self) -> dict:
        self._check_handle()
        json_ptr = self._lib.fdl_framing_intent_to_json(self._handle, 0)
        if not json_ptr:
            raise RuntimeError("fdl_framing_intent_to_json returned NULL")
        result = json.loads(ctypes.string_at(json_ptr))
        self._lib.fdl_free(json_ptr)
        return result
