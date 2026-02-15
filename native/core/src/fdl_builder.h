#ifndef FDL_BUILDER_INTERNAL_H
#define FDL_BUILDER_INTERNAL_H

#include "fdl/fdl_core.h"
#include "fdl_doc.h"
#include "fdl_handles.h"

#include <jsoncons/json.hpp>
#include <string>

namespace fdl::detail {

using ojson = jsoncons::ojson;

// Build helpers — create ojson objects with keys in canonical order

ojson make_version(int major, int minor);
ojson make_dimensions_int(int64_t width, int64_t height);
ojson make_dimensions_float(double width, double height);
ojson make_point_float(double x, double y);
ojson make_round_strategy(const char* even, const char* mode);

// Build a minimal FDL root document object with canonical key order
ojson make_root(const char* uuid, int version_major, int version_minor,
                const char* fdl_creator, const char* default_framing_intent);

// Build sub-objects in canonical key order
ojson make_framing_intent(const char* id, const char* label,
                          int64_t aspect_w, int64_t aspect_h, double protection);

ojson make_context(const char* label, const char* context_creator);

ojson make_canvas(const char* id, const char* label, const char* source_canvas_id,
                  int64_t dim_w, int64_t dim_h, double squeeze);

ojson make_framing_decision(const char* id, const char* label,
                            const char* framing_intent_id,
                            double dim_w, double dim_h,
                            double anchor_x, double anchor_y);

ojson make_canvas_template(const char* id, const char* label,
                           int64_t target_w, int64_t target_h, double target_squeeze,
                           fdl_geometry_path_t fit_source, fdl_fit_method_t fit_method,
                           fdl_halign_t halign, fdl_valign_t valign,
                           fdl_round_strategy_t rounding);

} // namespace fdl::detail

#endif // FDL_BUILDER_INTERNAL_H
