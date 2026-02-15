#ifndef FDL_PIPELINE_INTERNAL_H
#define FDL_PIPELINE_INTERNAL_H

#include "fdl/fdl_core.h"

namespace fdl::detail {

double calculate_scale_factor(
    fdl_dimensions_f64_t fit_norm,
    fdl_dimensions_f64_t target_norm,
    fdl_fit_method_t fit_method);

double output_size_for_axis(
    double canvas_size,
    double max_size,
    bool has_max,
    bool pad_to_max);

double alignment_shift(
    double fit_size,
    double fit_anchor,
    double output_size,
    double canvas_size,
    double target_size,
    bool is_center,
    double align_factor,
    bool pad_to_max);

fdl_dimensions_f64_t dimensions_clamp_to_dims(
    fdl_dimensions_f64_t dims,
    fdl_dimensions_f64_t clamp_dims,
    fdl_point_f64_t* out_delta);

} // namespace fdl::detail

#endif // FDL_PIPELINE_INTERNAL_H
