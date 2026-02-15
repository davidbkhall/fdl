"""
Testing utilities for FDL.

Provides reusable testing utilities for FDL comparison and scenario configuration.
"""

from fdl.testing.base import BaseFDLTestCase
from fdl.testing.fdl_comparison import FDLComparison
from fdl.testing.scenario_config import (
    SCENARIO_CONFIGS,
    ScenarioConfig,
    SourceVariant,
    build_test_params,
    get_scenario_paths,
    get_scenario_test_id,
    get_variant_by_letter,
)
from fdl.testing.template_tests import (
    TestFDLTemplatesParameterized,
    TestFitSourceValidation,
    TestPreserveFromSourceCanvasValidation,
)

__all__ = [
    "SCENARIO_CONFIGS",
    "BaseFDLTestCase",
    "FDLComparison",
    "ScenarioConfig",
    "SourceVariant",
    "TestFDLTemplatesParameterized",
    "TestFitSourceValidation",
    "TestPreserveFromSourceCanvasValidation",
    "build_test_params",
    "get_scenario_paths",
    "get_scenario_test_id",
    "get_variant_by_letter",
]
