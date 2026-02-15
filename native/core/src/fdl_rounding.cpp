#include "fdl/fdl_core.h"
#include "fdl_rounding.h"

#include <cmath>
#include <cstdlib>

// Port of Python fdl_round() from rounding.py.
// Critical: Python round() uses banker's rounding (half-to-even),
// C++ std::lround() uses half-away-from-zero. We use our own
// bankers_round() implementation in fdl_rounding.h.

int64_t fdl_round(double value, fdl_rounding_even_t even, fdl_rounding_mode_t mode) {
    int64_t sign = 1;
    if (value < 0.0) {
        sign = -1;
        value = std::abs(value);
    }

    int64_t v;
    if (mode == FDL_ROUNDING_MODE_UP) {
        v = static_cast<int64_t>(std::ceil(value));
    } else if (mode == FDL_ROUNDING_MODE_DOWN) {
        v = static_cast<int64_t>(std::floor(value));
    } else {
        // ROUND — banker's rounding to match Python
        v = fdl::detail::bankers_round(value);
    }

    if (even == FDL_ROUNDING_EVEN_EVEN) {
        if (mode == FDL_ROUNDING_MODE_UP) {
            v = (v % 2 == 0) ? v : v + 1;
        } else if (mode == FDL_ROUNDING_MODE_DOWN) {
            v = (v % 2 == 0) ? v : v - 1;
        } else {
            if (v % 2 != 0) {
                int64_t up = v + 1;
                int64_t down = v - 1;
                v = (std::abs(static_cast<double>(up) - value) <=
                     std::abs(static_cast<double>(down) - value))
                        ? up
                        : down;
            }
        }
    }

    return sign * v;
}

fdl_dimensions_f64_t fdl_round_dimensions(
    fdl_dimensions_f64_t dims,
    fdl_rounding_even_t even,
    fdl_rounding_mode_t mode
) {
    return {
        static_cast<double>(fdl_round(dims.width, even, mode)),
        static_cast<double>(fdl_round(dims.height, even, mode))
    };
}

fdl_point_f64_t fdl_round_point(
    fdl_point_f64_t point,
    fdl_rounding_even_t even,
    fdl_rounding_mode_t mode
) {
    return {
        static_cast<double>(fdl_round(point.x, even, mode)),
        static_cast<double>(fdl_round(point.y, even, mode))
    };
}
