"""fdl — handle-backed Python facade for libfdl_core."""
# ruff: noqa: I001

from .fdl import FDL  # noqa: F401
from .canvas import Canvas  # noqa: F401
from .canvas_template import CanvasTemplate  # noqa: F401
from .context import Context  # noqa: F401
from .framing_decision import FramingDecision  # noqa: F401
from .framing_intent import FramingIntent  # noqa: F401
from .constants import (  # noqa: F401
    FitMethod,
    GeometryPath,
    HAlign,
    RoundingEven,
    RoundingMode,
    VAlign,
)
from .context import ResolveCanvasResult  # noqa: F401
from .canvas_template import TemplateResult  # noqa: F401
from .errors import (  # noqa: F401
    FDLError,
    FDLValidationError,
)
from .rounding import (  # noqa: F401
    RoundStrategy,
    calculate_scale_factor,
    fdl_round,
)
from .clipid import (  # noqa: F401
    ClipID,
    FileSequence,
)
from .types import (  # noqa: F401
    DimensionsFloat,
    DimensionsInt,
    PointFloat,
    Rect,
)
from .utils import (  # noqa: F401
    DEFAULT_ROUNDING_STRATEGY,
    find_by_id,
    find_by_label,
    get_anchor_from_path,
    get_dimensions_from_path,
    get_rounding,
    make_rect,
    read_from_file,
    read_from_string,
    set_rounding,
    write_to_file,
    write_to_string,
)
from .header import Version  # noqa: F401
