#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""Verify exported C ABI symbols match fdl_api.yaml function list.

Usage:
    python scripts/check_abi_symbols.py                          # auto-detect library
    python scripts/check_abi_symbols.py path/to/libfdl_core.so   # explicit path
"""

import argparse
import platform
import subprocess
import sys
from pathlib import Path

import yaml

REPO_ROOT = Path(__file__).resolve().parent.parent
YAML_PATH = REPO_ROOT / "native" / "api" / "fdl_api.yaml"


def find_library() -> Path | None:
    """Auto-detect the built fdl_core shared library."""
    system = platform.system()
    if system == "Darwin":
        ext = "dylib"
    elif system == "Windows":
        ext = "dll"
    else:
        ext = "so"

    candidates = [
        REPO_ROOT / "native" / "core" / "build" / f"libfdl_core.{ext}",
        REPO_ROOT / "build" / f"libfdl_core.{ext}",
    ]
    if system == "Windows":
        candidates = [
            REPO_ROOT / "native" / "core" / "build" / "Release" / f"fdl_core.{ext}",
            REPO_ROOT / "native" / "core" / "build" / "Debug" / f"fdl_core.{ext}",
            REPO_ROOT / "build" / "Release" / f"fdl_core.{ext}",
        ]

    for p in candidates:
        if p.exists():
            return p
    return None


def extract_yaml_functions(yaml_path: Path) -> set[str]:
    """Extract expected function names from fdl_api.yaml."""
    with open(yaml_path) as f:
        api = yaml.safe_load(f)
    return {fn["name"] for fn in api["functions"]}


def extract_library_symbols(lib_path: Path) -> set[str]:
    """Extract exported text symbols from a shared library."""
    system = platform.system()
    if system == "Darwin":
        cmd = ["nm", "-gU", str(lib_path)]
    elif system == "Windows":
        cmd = ["dumpbin", "/EXPORTS", str(lib_path)]
    else:
        cmd = ["nm", "-D", str(lib_path)]

    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"ERROR: {cmd[0]} failed: {result.stderr.strip()}", file=sys.stderr)
        sys.exit(2)

    symbols = set()
    if system == "Windows":
        # dumpbin output: ordinal hint RVA name
        for line in result.stdout.splitlines():
            parts = line.split()
            if len(parts) >= 4 and parts[-1].startswith("fdl_"):
                symbols.add(parts[-1])
    else:
        for line in result.stdout.splitlines():
            parts = line.split()
            if len(parts) >= 3 and parts[1] == "T":
                sym = parts[2]
                if sym.startswith("_"):
                    sym = sym[1:]  # strip macOS leading underscore
                symbols.add(sym)
    return symbols


def main() -> int:
    parser = argparse.ArgumentParser(description="Check ABI symbol parity")
    parser.add_argument("library", nargs="?", help="Path to libfdl_core shared library")
    args = parser.parse_args()

    if args.library:
        lib_path = Path(args.library)
    else:
        lib_path = find_library()

    if not lib_path or not lib_path.exists():
        print("ERROR: Could not find libfdl_core shared library.", file=sys.stderr)
        print("Build with CMake first, or pass the path explicitly.", file=sys.stderr)
        return 2

    yaml_fns = extract_yaml_functions(YAML_PATH)
    lib_syms = extract_library_symbols(lib_path)

    missing = sorted(yaml_fns - lib_syms)
    extra = sorted(lib_syms - yaml_fns)

    print(f"YAML functions:   {len(yaml_fns)}")
    print(f"Library symbols:  {len(lib_syms)}")

    ok = True
    if missing:
        ok = False
        print(f"\nMissing from library ({len(missing)}):")
        for s in missing:
            print(f"  - {s}")

    if extra:
        ok = False
        print(f"\nExtra in library, not in YAML ({len(extra)}):")
        for s in extra:
            print(f"  + {s}")

    if ok:
        print("\n=== ABI symbols match ===")
        return 0
    else:
        print("\n=== ABI symbol mismatch detected ===", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
