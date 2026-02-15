#include "fdl/fdl_core.h"
#include "fdl_builder.h"
#include "fdl_doc.h"
#include "fdl_framing.h"

extern "C" {

fdl_from_intent_result_t fdl_compute_framing_from_intent(
    fdl_dimensions_f64_t canvas_dims,
    fdl_dimensions_f64_t working_dims,
    double squeeze,
    fdl_dimensions_i64_t aspect_ratio,
    double protection,
    fdl_round_strategy_t rounding) {
    return fdl::detail::compute_framing_from_intent(
        canvas_dims, working_dims, squeeze, aspect_ratio, protection, rounding);
}

// Helper: compute alignment offset for a single axis
static double align_offset(double container, double content, uint32_t align, int is_horizontal) {
    // CENTER enum value is 1 for both halign and valign
    if (align == 1) return (container - content) / 2.0;
    // RIGHT (halign=2) or BOTTOM (valign=2)
    if (align == 2) return container - content;
    // LEFT (halign=0) or TOP (valign=0)
    return 0.0;
}

void fdl_framing_decision_adjust_anchor(
    fdl_framing_decision_t* fd,
    const fdl_canvas_t* canvas,
    fdl_halign_t h_align,
    fdl_valign_t v_align) {
    if (!fd || !canvas) return;

    doc_lock lock(fd->owner);
    auto* fd_n = fd->node();
    auto* canvas_n = canvas->node();
    if (!fd_n || !canvas_n) return;

    double canvas_w = static_cast<double>((*canvas_n)["dimensions"]["width"].as<int64_t>());
    double canvas_h = static_cast<double>((*canvas_n)["dimensions"]["height"].as<int64_t>());
    double fd_w = (*fd_n)["dimensions"]["width"].as<double>();
    double fd_h = (*fd_n)["dimensions"]["height"].as<double>();

    double anchor_x = align_offset(canvas_w, fd_w, h_align, 1);
    double anchor_y = align_offset(canvas_h, fd_h, v_align, 0);

    fd_n->insert_or_assign("anchor_point",
        fdl::detail::make_point_float(anchor_x, anchor_y));
}

void fdl_framing_decision_adjust_protection_anchor(
    fdl_framing_decision_t* fd,
    const fdl_canvas_t* canvas,
    fdl_halign_t h_align,
    fdl_valign_t v_align) {
    if (!fd || !canvas) return;

    doc_lock lock(fd->owner);
    auto* fd_n = fd->node();
    auto* canvas_n = canvas->node();
    if (!fd_n || !canvas_n) return;

    double canvas_w = static_cast<double>((*canvas_n)["dimensions"]["width"].as<int64_t>());
    double canvas_h = static_cast<double>((*canvas_n)["dimensions"]["height"].as<int64_t>());
    double prot_w = (*fd_n)["protection_dimensions"]["width"].as<double>();
    double prot_h = (*fd_n)["protection_dimensions"]["height"].as<double>();

    double anchor_x = align_offset(canvas_w, prot_w, h_align, 1);
    double anchor_y = align_offset(canvas_h, prot_h, v_align, 0);

    fd_n->insert_or_assign("protection_anchor_point",
        fdl::detail::make_point_float(anchor_x, anchor_y));
}

void fdl_framing_decision_populate_from_intent(
    fdl_framing_decision_t* fd,
    const fdl_canvas_t* canvas,
    const fdl_framing_intent_t* intent,
    fdl_round_strategy_t rounding) {
    if (!fd || !canvas || !intent) return;

    doc_lock lock(fd->owner);
    auto* fd_n = fd->node();
    auto* canvas_n = canvas->node();
    auto* intent_n = intent->node();
    if (!fd_n || !canvas_n || !intent_n) return;

    // Get canvas dimensions (int -> double)
    fdl_dimensions_f64_t canvas_dims = {
        static_cast<double>((*canvas_n)["dimensions"]["width"].as<int64_t>()),
        static_cast<double>((*canvas_n)["dimensions"]["height"].as<int64_t>())
    };

    // Working dims: effective if available, else canvas
    fdl_dimensions_f64_t working_dims = canvas_dims;
    if (canvas_n->contains("effective_dimensions")) {
        working_dims.width = static_cast<double>(
            (*canvas_n)["effective_dimensions"]["width"].as<int64_t>());
        working_dims.height = static_cast<double>(
            (*canvas_n)["effective_dimensions"]["height"].as<int64_t>());
    }

    // Get squeeze
    double squeeze = canvas_n->contains("anamorphic_squeeze") ?
        (*canvas_n)["anamorphic_squeeze"].as<double>() : 1.0;

    // Get aspect ratio from intent
    fdl_dimensions_i64_t aspect_ratio = {
        (*intent_n)["aspect_ratio"]["width"].as<int64_t>(),
        (*intent_n)["aspect_ratio"]["height"].as<int64_t>()
    };

    // Get protection from intent
    double protection = intent_n->contains("protection") ?
        (*intent_n)["protection"].as<double>() : 0.0;

    // Compute
    fdl_from_intent_result_t result = fdl::detail::compute_framing_from_intent(
        canvas_dims, working_dims, squeeze, aspect_ratio, protection, rounding);

    // Write to FD
    fd_n->insert_or_assign("dimensions",
        fdl::detail::make_dimensions_float(result.dimensions.width, result.dimensions.height));
    fd_n->insert_or_assign("anchor_point",
        fdl::detail::make_point_float(result.anchor_point.x, result.anchor_point.y));

    if (result.has_protection) {
        fd_n->insert_or_assign("protection_dimensions",
            fdl::detail::make_dimensions_float(
                result.protection_dimensions.width, result.protection_dimensions.height));
        fd_n->insert_or_assign("protection_anchor_point",
            fdl::detail::make_point_float(
                result.protection_anchor_point.x, result.protection_anchor_point.y));
    }
}

} // extern "C"
