// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#ifndef FDL_GEOMETRY_INTERNAL_H
#define FDL_GEOMETRY_INTERNAL_H

#include "fdl/fdl_core.h"

namespace fdl::detail {

// Fill gaps in the geometry hierarchy by propagating populated dimensions upward.
// Protection is NEVER filled from framing (special case).
// Anchors are adjusted by subtracting anchor_offset, then clamped >= 0.
fdl_geometry_t geometry_fill_hierarchy_gaps(
    fdl_geometry_t geo,
    fdl_point_f64_t anchor_offset);

// Normalize and scale all 7 fields of the geometry.
fdl_geometry_t geometry_normalize_and_scale(
    fdl_geometry_t geo,
    double source_squeeze,
    double scale_factor,
    double target_squeeze);

// Round all 7 fields of the geometry.
fdl_geometry_t geometry_round(
    fdl_geometry_t geo,
    fdl_round_strategy_t strategy);

// Apply offset to all anchors. Returns clamped geometry + 3 theoretical (unclamped) anchors.
fdl_geometry_t geometry_apply_offset(
    fdl_geometry_t geo,
    fdl_point_f64_t offset,
    fdl_point_f64_t* theo_eff,
    fdl_point_f64_t* theo_prot,
    fdl_point_f64_t* theo_fram);

// Crop all dimensions to visible portion within canvas.
// Enforces hierarchy: canvas >= effective >= protection >= framing.
fdl_geometry_t geometry_crop(
    fdl_geometry_t geo,
    fdl_point_f64_t theo_eff,
    fdl_point_f64_t theo_prot,
    fdl_point_f64_t theo_fram);

// Extract dimensions and anchor from geometry by path.
// Returns 0 on success, -1 on invalid path.
int geometry_get_dims_anchor_from_path(
    const fdl_geometry_t* geo,
    fdl_geometry_path_t path,
    fdl_dimensions_f64_t* out_dims,
    fdl_point_f64_t* out_anchor);

} // namespace fdl::detail

#endif // FDL_GEOMETRY_INTERNAL_H
