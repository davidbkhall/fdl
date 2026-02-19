#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""Run all codegen targets to regenerate bindings from fdl_api.yaml.

Usage:
    python scripts/run_codegen.py            # regenerate all targets
    python scripts/run_codegen.py --check    # regenerate and fail if output changed
"""

import argparse
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
TOOLS_DIR = REPO_ROOT / "native" / "tools"

TARGETS = ["python-ffi", "python-facade", "cpp-raii"]

GENERATED_PATHS = [
    "native/bindings/python/fdl_ffi/",
    "native/bindings/python/fdl/",
    "native/bindings/cpp/fdl/",
]

GENERATED_PYTHON_PATHS = [
    "native/bindings/python/fdl_ffi/",
    "native/bindings/python/fdl/",
]

GENERATED_CPP_PATHS = [
    "native/bindings/cpp/fdl/",
]


def run_codegen() -> int:
    for target in TARGETS:
        print(f"=== Generating: {target} ===")
        result = subprocess.run(
            [sys.executable, "-m", "codegen.generate", "--target", target],
            cwd=TOOLS_DIR,
        )
        if result.returncode != 0:
            print(f"FAILED: codegen target '{target}' exited {result.returncode}", file=sys.stderr)
            return result.returncode
    print("=== All codegen targets completed ===")

    # Post-process: format generated Python with ruff
    print("=== Formatting generated Python ===")
    abs_py_paths = [str(REPO_ROOT / p) for p in GENERATED_PYTHON_PATHS]
    fmt = subprocess.run(
        [sys.executable, "-m", "ruff", "format", *abs_py_paths],
        cwd=REPO_ROOT,
    )
    if fmt.returncode != 0:
        print("WARNING: ruff format failed (is ruff installed?)", file=sys.stderr)

    # Post-process: format generated C++ with clang-format
    print("=== Formatting generated C++ ===")
    cpp_files = []
    for p in GENERATED_CPP_PATHS:
        d = REPO_ROOT / p
        if d.exists():
            cpp_files.extend(str(f) for f in d.rglob("*.hpp"))
            cpp_files.extend(str(f) for f in d.rglob("*.h"))
    if cpp_files:
        cfmt = subprocess.run(["clang-format", "-i", *cpp_files], cwd=REPO_ROOT)
        if cfmt.returncode != 0:
            print("WARNING: clang-format failed (is clang-format installed?)", file=sys.stderr)

    return 0


def check_drift() -> int:
    print("=== Checking for codegen drift ===")
    result = subprocess.run(
        ["git", "diff", "--exit-code", "--stat", *GENERATED_PATHS],
        cwd=REPO_ROOT,
    )
    if result.returncode != 0:
        print(
            "FAILED: generated files differ from committed versions.\nRun 'python scripts/run_codegen.py' and commit the result.",
            file=sys.stderr,
        )
        return 1

    # Also catch untracked generated files
    untracked = subprocess.run(
        ["git", "ls-files", "--others", "--exclude-standard", *GENERATED_PATHS],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
    )
    if untracked.stdout.strip():
        print(
            f"FAILED: untracked generated files found:\n{untracked.stdout}Run 'python scripts/run_codegen.py' and commit the result.",
            file=sys.stderr,
        )
        return 1

    print("=== No codegen drift detected ===")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check",
        action="store_true",
        help="After regeneration, fail if generated files differ from committed versions.",
    )
    args = parser.parse_args()

    rc = run_codegen()
    if rc != 0:
        return rc

    if args.check:
        return check_drift()
    return 0


if __name__ == "__main__":
    sys.exit(main())
