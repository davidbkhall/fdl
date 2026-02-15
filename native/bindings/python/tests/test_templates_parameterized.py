"""
Pure FDL template scenario tests (no imaging, no Nuke).

Exercises all 53 parameterized template scenarios at the fdl package level.
Parameterized via pytest_generate_tests hook in conftest.py.
"""

from fdl.testing.template_tests import TestFDLTemplatesParameterized  # noqa: F401
