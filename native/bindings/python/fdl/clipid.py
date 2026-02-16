# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
# AUTO-GENERATED from fdl_api.yaml — DO NOT EDIT
"""FDL Core JSON wrapper types with typed attribute access."""

from __future__ import annotations

import ctypes
from dataclasses import dataclass


@dataclass
class FileSequence:
    """File sequence information for a clip."""

    value: str
    idx: str
    min: int
    max: int

    def __post_init__(self) -> None:
        if self.idx is not None and len(self.idx) > 1:
            raise ValueError("idx must be at most 1 character(s)")
        if self.min is not None and self.min < 0:
            raise ValueError("min must be >= 0")
        if self.max is not None and self.max < 0:
            raise ValueError("max must be >= 0")

    @classmethod
    def from_dict(cls, data: dict) -> FileSequence:
        """Construct from a JSON-decoded dictionary."""
        return cls(
            value=data["value"],
            idx=data["idx"],
            min=data["min"],
            max=data["max"],
        )

    def to_dict(self) -> dict:
        """Serialize to a JSON-compatible dictionary, excluding None values."""
        d: dict = {}
        d["value"] = self.value
        d["idx"] = self.idx
        d["min"] = self.min
        d["max"] = self.max
        return d

    @classmethod
    def _from_c(cls, c_struct: object, lib: object) -> FileSequence:
        """Construct from a C fdl_file_sequence_t struct. Frees owned strings."""
        _value = ctypes.string_at(c_struct.value).decode("utf-8") if c_struct.value else ""
        _idx = ctypes.string_at(c_struct.idx).decode("utf-8") if c_struct.idx else ""
        _min = int(c_struct.min)
        _max = int(c_struct.max)
        return cls(
            value=_value,
            idx=_idx,
            min=_min,
            max=_max,
        )


_CLIPID_MUTEX = ["file", "sequence"]


@dataclass
class ClipID:
    """Clip identification for a context or canvas."""

    clip_name: str
    file: str | None = None
    sequence: FileSequence | None = None

    def __post_init__(self) -> None:
        _set = [f for f in _CLIPID_MUTEX if getattr(self, f) is not None]
        if len(_set) > 1:
            raise ValueError(f"Both {_set[0]} and {_set[1]} attributes are provided, only one is allowed.")

    def __setattr__(self, name: str, value: object) -> None:
        super().__setattr__(name, value)
        if name in _CLIPID_MUTEX:
            _set = [f for f in _CLIPID_MUTEX if getattr(self, f, None) is not None]
            if len(_set) > 1:
                raise ValueError(f"Both {_set[0]} and {_set[1]} attributes are provided, only one is allowed.")

    @classmethod
    def from_dict(cls, data: dict) -> ClipID:
        """Construct from a JSON-decoded dictionary."""
        _raw_sequence = data.get("sequence")
        _sequence = FileSequence.from_dict(_raw_sequence) if isinstance(_raw_sequence, dict) else None
        return cls(
            clip_name=data["clip_name"],
            file=data.get("file"),
            sequence=_sequence,
        )

    def to_dict(self) -> dict:
        """Serialize to a JSON-compatible dictionary, excluding None values."""
        d: dict = {}
        d["clip_name"] = self.clip_name
        if self.file is not None:
            d["file"] = self.file
        if self.sequence is not None:
            d["sequence"] = self.sequence.to_dict()
        return d

    @classmethod
    def _from_c(cls, c_struct: object, lib: object) -> ClipID:
        """Construct from a C fdl_clip_id_t struct. Frees owned strings."""
        _clip_name = ctypes.string_at(c_struct.clip_name).decode("utf-8") if c_struct.clip_name else ""
        _file: str | None = None
        if c_struct.has_file:
            _file = ctypes.string_at(c_struct.file).decode("utf-8") if c_struct.file else None
        _sequence = None
        if c_struct.has_sequence:
            _sequence = FileSequence._from_c(c_struct.sequence, lib)
        lib.fdl_clip_id_free(ctypes.byref(c_struct))
        return cls(
            clip_name=_clip_name,
            file=_file,
            sequence=_sequence,
        )
