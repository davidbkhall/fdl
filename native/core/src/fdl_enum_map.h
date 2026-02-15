// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#ifndef FDL_ENUM_MAP_H
#define FDL_ENUM_MAP_H

#include "fdl/fdl_core.h"

#include <string>
#include <string_view>

namespace fdl::detail {

// --- Fit method ---
inline fdl_fit_method_t fit_method_from_string(std::string_view s) {
    if (s == "width") return FDL_FIT_METHOD_WIDTH;
    if (s == "height") return FDL_FIT_METHOD_HEIGHT;
    if (s == "fit_all") return FDL_FIT_METHOD_FIT_ALL;
    if (s == "fill") return FDL_FIT_METHOD_FILL;
    return FDL_FIT_METHOD_WIDTH; // default
}

// --- Geometry path (fit_source / preserve_from_source_canvas) ---
inline fdl_geometry_path_t geometry_path_from_string(std::string_view s) {
    if (s == "canvas.dimensions") return FDL_GEOMETRY_PATH_CANVAS_DIMENSIONS;
    if (s == "canvas.effective_dimensions") return FDL_GEOMETRY_PATH_CANVAS_EFFECTIVE_DIMENSIONS;
    if (s == "framing_decision.protection_dimensions") return FDL_GEOMETRY_PATH_FRAMING_PROTECTION_DIMENSIONS;
    if (s == "framing_decision.dimensions") return FDL_GEOMETRY_PATH_FRAMING_DIMENSIONS;
    return FDL_GEOMETRY_PATH_FRAMING_DIMENSIONS; // default
}

// --- Horizontal alignment ---
inline fdl_halign_t halign_from_string(std::string_view s) {
    if (s == "left") return FDL_HALIGN_LEFT;
    if (s == "center") return FDL_HALIGN_CENTER;
    if (s == "right") return FDL_HALIGN_RIGHT;
    return FDL_HALIGN_CENTER; // default
}

// --- Vertical alignment ---
inline fdl_valign_t valign_from_string(std::string_view s) {
    if (s == "top") return FDL_VALIGN_TOP;
    if (s == "center") return FDL_VALIGN_CENTER;
    if (s == "bottom") return FDL_VALIGN_BOTTOM;
    return FDL_VALIGN_CENTER; // default
}

// --- Rounding even ---
inline fdl_rounding_even_t rounding_even_from_string(std::string_view s) {
    if (s == "even") return FDL_ROUNDING_EVEN_EVEN;
    if (s == "whole") return FDL_ROUNDING_EVEN_WHOLE;
    return FDL_ROUNDING_EVEN_WHOLE; // default
}

// --- Rounding mode ---
inline fdl_rounding_mode_t rounding_mode_from_string(std::string_view s) {
    if (s == "up") return FDL_ROUNDING_MODE_UP;
    if (s == "down") return FDL_ROUNDING_MODE_DOWN;
    if (s == "round") return FDL_ROUNDING_MODE_ROUND;
    return FDL_ROUNDING_MODE_ROUND; // default
}

// -----------------------------------------------------------------------
// Enum-to-string (reverse mappings for builder/serialization)
// -----------------------------------------------------------------------

inline const char* fit_method_to_string(fdl_fit_method_t m) {
    switch (m) {
        case FDL_FIT_METHOD_WIDTH:   return "width";
        case FDL_FIT_METHOD_HEIGHT:  return "height";
        case FDL_FIT_METHOD_FIT_ALL: return "fit_all";
        case FDL_FIT_METHOD_FILL:    return "fill";
        default: return "width";
    }
}

inline const char* geometry_path_to_string(fdl_geometry_path_t p) {
    switch (p) {
        case FDL_GEOMETRY_PATH_CANVAS_DIMENSIONS:            return "canvas.dimensions";
        case FDL_GEOMETRY_PATH_CANVAS_EFFECTIVE_DIMENSIONS:   return "canvas.effective_dimensions";
        case FDL_GEOMETRY_PATH_FRAMING_PROTECTION_DIMENSIONS: return "framing_decision.protection_dimensions";
        case FDL_GEOMETRY_PATH_FRAMING_DIMENSIONS:            return "framing_decision.dimensions";
        default: return "framing_decision.dimensions";
    }
}

inline const char* halign_to_string(fdl_halign_t h) {
    switch (h) {
        case FDL_HALIGN_LEFT:   return "left";
        case FDL_HALIGN_CENTER: return "center";
        case FDL_HALIGN_RIGHT:  return "right";
        default: return "center";
    }
}

inline const char* valign_to_string(fdl_valign_t v) {
    switch (v) {
        case FDL_VALIGN_TOP:    return "top";
        case FDL_VALIGN_CENTER: return "center";
        case FDL_VALIGN_BOTTOM: return "bottom";
        default: return "center";
    }
}

inline const char* rounding_even_to_string(fdl_rounding_even_t e) {
    switch (e) {
        case FDL_ROUNDING_EVEN_WHOLE: return "whole";
        case FDL_ROUNDING_EVEN_EVEN:  return "even";
        default: return "whole";
    }
}

inline const char* rounding_mode_to_string(fdl_rounding_mode_t m) {
    switch (m) {
        case FDL_ROUNDING_MODE_UP:    return "up";
        case FDL_ROUNDING_MODE_DOWN:  return "down";
        case FDL_ROUNDING_MODE_ROUND: return "round";
        default: return "round";
    }
}

} // namespace fdl::detail

#endif // FDL_ENUM_MAP_H
