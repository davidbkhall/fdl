# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
# AUTO-GENERATED from fdl_api.yaml — DO NOT EDIT
# ruff: noqa: I001
"""FDL Core Context facade."""

from __future__ import annotations

import ctypes
import json

from .types import DimensionsFloat, DimensionsInt

from .base import (
    CollectionWrapper,
    HandleWrapper,
    _decode_str,
)
from .converters import (
    _to_c_dims_f64,
)
from .clipid import ClipID
from dataclasses import dataclass

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from .canvas import Canvas
    from .framing_decision import FramingDecision


@dataclass
class ResolveCanvasResult:
    """Result of resolving canvas for given input dimensions."""

    canvas: Canvas
    framing_decision: FramingDecision
    was_resolved: bool


class Context(HandleWrapper):
    """Context facade wrapping a C fdl_context_t handle."""

    def __init__(
        self,
        *,
        label: str,
        context_creator: str | None = None,
        canvases: object = None,
    ) -> None:
        from fdl_ffi import get_lib

        lib = get_lib()
        from .fdl import FDL
        _doc_h = lib.fdl_doc_create_with_header(
            b"00000000-0000-0000-0000-000000000000", 2, 0, b"_", None,
        )
        _backing = FDL._from_handle(_doc_h, lib)
        handle = lib.fdl_doc_add_context(
            _doc_h,
            label.encode("utf-8"),
            context_creator.encode("utf-8") if context_creator else None,
        )
        if not handle:
            raise RuntimeError("fdl_doc_add_context returned NULL")
        HandleWrapper.__init__(self, handle, lib, _backing)

    @property
    def label(self) -> str:
        self._check_handle()
        raw = self._lib.fdl_context_get_label(self._handle)
        return _decode_str(raw)

    @property
    def context_creator(self) -> str | None:
        self._check_handle()
        raw = self._lib.fdl_context_get_context_creator(self._handle)
        return _decode_str(raw)

    @property
    def clip_id(self) -> ClipID | None:
        self._check_handle()
        if not self._lib.fdl_context_has_clip_id(self._handle):
            return None
        raw = self._lib.fdl_context_get_clip_id_struct(self._handle)
        return ClipID._from_c(raw, self._lib)

    @clip_id.setter
    def clip_id(self, value: ClipID | None) -> None:
        self._check_handle()
        if value is None:
            self._lib.fdl_context_remove_clip_id(self._handle)
            return
        _json = json.dumps(value.to_dict()).encode("utf-8")
        _err = self._lib.fdl_context_set_clip_id_json(self._handle, _json, len(_json))
        if _err:
            _msg = ctypes.string_at(_err).decode("utf-8")
            self._lib.fdl_free(_err)
            raise ValueError(_msg)

    @property
    def canvases(self) -> CollectionWrapper[Canvas]:
        self._check_handle()
        from .canvas import Canvas
        return CollectionWrapper(
            lib=self._lib,
            parent_handle=self._handle,
            item_cls=Canvas,
            count_fn=self._lib.fdl_context_canvases_count,
            at_fn=self._lib.fdl_context_canvas_at,
            find_by_id_fn=self._lib.fdl_context_find_canvas_by_id,
            find_by_label_fn=None,
            doc_ref=self._doc_ref,
        )

    def __eq__(self, other: object) -> bool:
        if isinstance(other, Context):
            return self.label == other.label
        if isinstance(other, str):
            return self.label == other
        return NotImplemented

    def __hash__(self):
        return hash((self.label,))

    def as_dict(self) -> dict:
        self._check_handle()
        json_ptr = self._lib.fdl_context_to_json(self._handle, 0)
        if not json_ptr:
            raise RuntimeError("fdl_context_to_json returned NULL")
        result = json.loads(ctypes.string_at(json_ptr))
        self._lib.fdl_free(json_ptr)
        return result

    def add_canvas(
        self,
        id: str,
        label: str,
        source_canvas_id: str,
        dimensions: DimensionsInt,
        anamorphic_squeeze: float,
    ) -> Canvas:
        """Add a canvas to this context."""
        self._check_handle()
        from .canvas import Canvas
        handle = self._lib.fdl_context_add_canvas(
            self._handle,
            id.encode("utf-8"),
            label.encode("utf-8"),
            source_canvas_id.encode("utf-8"),
            int(dimensions.width),
            int(dimensions.height),
            float(anamorphic_squeeze),
        )
        if not handle:
            raise RuntimeError("fdl_context_add_canvas returned NULL")
        return Canvas._from_handle(handle, self._lib, self._doc_ref)

    def resolve_canvas_for_dimensions(
        self,
        input_dims: DimensionsFloat,
        canvas: Canvas,
        framing: FramingDecision,
    ) -> ResolveCanvasResult:
        """Find matching canvas when input dimensions differ from selected canvas."""
        self._check_handle()
        from .canvas import Canvas
        from .framing_decision import FramingDecision
        result = self._lib.fdl_context_resolve_canvas_for_dimensions(
            self._handle,
            _to_c_dims_f64(input_dims),
            canvas._handle,
            framing._handle,
        )
        if result.error:
            msg = ctypes.string_at(result.error).decode("utf-8")
            self._lib.fdl_free(result.error)
            raise ValueError(msg)
        _canvas = Canvas._from_handle(result.canvas, self._lib, self._doc_ref)
        _framing_decision = FramingDecision._from_handle(result.framing_decision, self._lib, self._doc_ref)
        _was_resolved = bool(result.was_resolved)
        return ResolveCanvasResult(
            canvas=_canvas,
            framing_decision=_framing_decision,
            was_resolved=_was_resolved,
        )

