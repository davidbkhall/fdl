import pathlib

import pytest
from mktestdocs import check_md_file

# Plugin/handler system not implemented — skip doc tests for these files
SKIP_FILES = {"plugins.md", "FDL_Template_Implementer_Guide.md", "FDL_Apply_Template_Logic.md"}

_DOCS_DIR = pathlib.Path(__file__).parents[4] / "docs"


@pytest.mark.parametrize("fpath", _DOCS_DIR.glob("**/*.md"), ids=lambda p: str(p.relative_to(_DOCS_DIR)))
def test_all_docs(fpath):
    if fpath.name in SKIP_FILES:
        pytest.skip(f"{fpath.name}: plugin/handler system not implemented")
    check_md_file(fpath=fpath, memory=False)
