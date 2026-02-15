#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""Run all codegen targets to regenerate bindings from fdl_api.yaml."""

import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
TOOLS_DIR = REPO_ROOT / "native" / "tools"

TARGETS = ["python-ffi", "python-facade", "cpp-raii"]


def main() -> int:
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
    return 0


if __name__ == "__main__":
    sys.exit(main())
