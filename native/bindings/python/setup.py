# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""
Minimal setup.py to force platform-specific wheel tags.

When fdl_ffi bundles a native shared library (libfdl_core.dylib/so/dll),
the wheel must not be tagged as 'py3-none-any'. This override ensures
the wheel gets proper platform tags like 'cp312-cp312-macosx_14_0_arm64'.
"""

from setuptools import setup
from setuptools.dist import Distribution


class BinaryDistribution(Distribution):
    def has_ext_modules(self):
        return True


setup(distclass=BinaryDistribution)
