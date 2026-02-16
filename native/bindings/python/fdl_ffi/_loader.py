# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""
Platform-aware loader for libfdl_core shared library.

Search order:
  1. FDL_CORE_LIB_PATH env var (explicit path)
  2. Bundled alongside this package (for wheel installs)
  3. In-tree build location (for dev builds)
  4. System library path (DYLD_LIBRARY_PATH / LD_LIBRARY_PATH)
"""

from __future__ import annotations

import ctypes
import os
import sys
from pathlib import Path

_LIB_NAMES = {
    "darwin": "libfdl_core.dylib",
    "linux": "libfdl_core.so",
    "win32": "fdl_core.dll",
}


def _lib_name() -> str:
    return _LIB_NAMES.get(sys.platform, "libfdl_core.so")


def _candidate_paths() -> list[Path]:
    paths: list[Path] = []

    # 1. Explicit env var
    env_path = os.environ.get("FDL_CORE_LIB_PATH")
    if env_path:
        paths.append(Path(env_path))

    # 2. Bundled alongside this package (inside wheel)
    pkg_dir = Path(__file__).parent
    paths.append(pkg_dir / _lib_name())

    # 3. In-tree build locations (relative to this file)
    native_root = pkg_dir.parent.parent.parent  # bindings/python/fdl_ffi -> native/
    paths.append(native_root / "core" / "build" / _lib_name())

    return paths


def load_library() -> ctypes.CDLL:
    """Load libfdl_core, raising OSError if not found."""
    # Try explicit paths first
    for path in _candidate_paths():
        if path.exists():
            return ctypes.CDLL(str(path))

    # Fall back to system search
    try:
        return ctypes.CDLL(_lib_name())
    except OSError:
        pass

    checked = [str(p) for p in _candidate_paths()]
    msg = f"Could not load {_lib_name()}. Set FDL_CORE_LIB_PATH or build the library first.\nSearched: {checked}"
    raise OSError(msg)
