# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""
fdl_ffi — Python ctypes bindings for libfdl_core.

Usage:
    from fdl_ffi import lib

    ver = lib.fdl_abi_version()
    print(f"ABI {ver.major}.{ver.minor}.{ver.patch}")
"""

from __future__ import annotations

import threading

from ._enums import *  # noqa: F403
from ._loader import load_library
from ._structs import *  # noqa: F403

# Expected ABI compatibility range
_ABI_MAJOR = 0
_ABI_MINOR_MIN = 3

_lib = None
_lib_lock = threading.Lock()


def get_lib():
    """Get the loaded and bound library singleton (thread-safe)."""
    global _lib
    if _lib is None:
        with _lib_lock:
            if _lib is None:
                _lib = _load_and_bind()
    return _lib


def _load_and_bind():
    from ._functions import bind_functions

    loaded = load_library()
    bind_functions(loaded)

    # ABI version check
    ver = loaded.fdl_abi_version()
    if ver.major != _ABI_MAJOR or ver.minor < _ABI_MINOR_MIN:
        msg = f"fdl_core ABI {ver.major}.{ver.minor}.{ver.patch} is incompatible. Expected {_ABI_MAJOR}.>={_ABI_MINOR_MIN}.x"
        raise RuntimeError(msg)

    return loaded


def is_available() -> bool:
    """Check if libfdl_core can be loaded without raising."""
    try:
        get_lib()
        return True
    except (OSError, RuntimeError):
        return False
