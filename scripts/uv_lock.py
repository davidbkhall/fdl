#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""Regenerate uv.lock using public PyPI index.

Overrides any internal/corporate index URL so the lock file is portable.

Usage:
    python scripts/uv_lock.py
"""

import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
INDEX_URL = "https://pypi.org/simple"


def main() -> int:
    print(f"Regenerating uv.lock with index: {INDEX_URL}")
    result = subprocess.run(
        ["uv", "lock", "--index-url", INDEX_URL],
        cwd=REPO_ROOT,
        env={"UV_INDEX_URL": INDEX_URL, "PATH": subprocess.os.environ["PATH"]},
    )
    if result.returncode == 0:
        print("uv.lock regenerated successfully.")
    else:
        print("uv lock failed.", file=sys.stderr)
    return result.returncode


if __name__ == "__main__":
    sys.exit(main())
