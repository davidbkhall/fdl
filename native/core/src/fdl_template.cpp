#include "fdl_template.h"
#include "fdl_doc.h"
#include "fdl_enum_map.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>

// PATH_HIERARCHY order (matches Python constants.py):
// 0: canvas.dimensions
// 1: canvas.effective_dimensions
// 2: framing_decision.protection_dimensions
// 3: framing_decision.dimensions

// Alignment factor: LEFT/TOP=0.0, CENTER=0.5, RIGHT/BOTTOM=1.0
static double alignment_factor_h(fdl_halign_t align) {
    if (align == FDL_HALIGN_LEFT) return 0.0;
    if (align == FDL_HALIGN_RIGHT) return 1.0;
    return 0.5;
}

static double alignment_factor_v(fdl_valign_t align) {
    if (align == FDL_VALIGN_TOP) return 0.0;
    if (align == FDL_VALIGN_BOTTOM) return 1.0;
    return 0.5;
}

// Populate one layer of the geometry from source canvas/framing.
// Delegates to fdl_resolve_geometry_layer for the actual handle reads.
static void populate_layer(
    fdl_geometry_t& geo,
    fdl_geometry_path_t path,
    const fdl_canvas_t* canvas,
    const fdl_framing_decision_t* framing) {

    fdl_dimensions_f64_t dims;
    fdl_point_f64_t anchor;
    if (fdl_resolve_geometry_layer(canvas, framing, path, &dims, &anchor) != 0) return;

    switch (path) {
    case FDL_GEOMETRY_PATH_CANVAS_DIMENSIONS:
        geo.canvas_dims = dims; break;
    case FDL_GEOMETRY_PATH_CANVAS_EFFECTIVE_DIMENSIONS:
        geo.effective_dims = dims; geo.effective_anchor = anchor; break;
    case FDL_GEOMETRY_PATH_FRAMING_PROTECTION_DIMENSIONS:
        geo.protection_dims = dims; geo.protection_anchor = anchor; break;
    case FDL_GEOMETRY_PATH_FRAMING_DIMENSIONS:
        geo.framing_dims = dims; geo.framing_anchor = anchor; break;
    }
}

// Populate geometry starting from path and going to innermost layer
static void populate_from_path(
    fdl_geometry_t& geo,
    fdl_geometry_path_t start_path,
    const fdl_canvas_t* canvas,
    const fdl_framing_decision_t* framing) {

    for (uint32_t p = start_path; p <= FDL_GEOMETRY_PATH_FRAMING_DIMENSIONS; ++p) {
        populate_layer(geo, static_cast<fdl_geometry_path_t>(p), canvas, framing);
    }
}

// Helper: safely copy a C string accessor result to std::string.
// Returns empty string if the pointer is null.
static std::string safe_copy(const char* s) {
    return s ? std::string(s) : std::string();
}

namespace fdl::detail {

fdl_template_result_t apply_canvas_template(
    const fdl_canvas_template_t* tmpl,
    const fdl_canvas_t* source_canvas,
    const fdl_framing_decision_t* source_framing,
    const char* new_canvas_id,
    const char* new_fd_name,
    const char* source_context_label,
    const char* context_creator) {

    fdl_template_result_t result = {};

    if (!tmpl || !source_canvas || !source_framing || !new_canvas_id) {
        result.error = strdup("NULL parameter");
        return result;
    }

    // --- Read template fields ---
    double input_squeeze = fdl_canvas_get_anamorphic_squeeze(source_canvas);
    if (input_squeeze == 0.0) input_squeeze = 1.0;

    double target_squeeze = fdl_canvas_template_get_target_anamorphic_squeeze(tmpl);
    if (target_squeeze == 0.0) target_squeeze = input_squeeze;

    fdl_fit_method_t fit_method = fdl_canvas_template_get_fit_method(tmpl);
    fdl_geometry_path_t fit_source = fdl_canvas_template_get_fit_source(tmpl);
    fdl_halign_t h_align = fdl_canvas_template_get_alignment_method_horizontal(tmpl);
    fdl_valign_t v_align = fdl_canvas_template_get_alignment_method_vertical(tmpl);
    bool has_preserve = fdl_canvas_template_has_preserve_from_source_canvas(tmpl) != 0;
    fdl_geometry_path_t preserve_path = has_preserve
        ? fdl_canvas_template_get_preserve_from_source_canvas(tmpl)
        : FDL_GEOMETRY_PATH_CANVAS_DIMENSIONS;

    bool has_max_dims = fdl_canvas_template_has_maximum_dimensions(tmpl) != 0;
    fdl_dimensions_i64_t max_dims_i = has_max_dims
        ? fdl_canvas_template_get_maximum_dimensions(tmpl)
        : fdl_dimensions_i64_t{0, 0};
    double max_w = static_cast<double>(max_dims_i.width);
    double max_h = static_cast<double>(max_dims_i.height);
    bool pad_to_max = fdl_canvas_template_get_pad_to_maximum(tmpl) != 0;

    fdl_round_strategy_t rounding = fdl_canvas_template_get_round(tmpl);

    fdl_dimensions_i64_t target_dims_i = fdl_canvas_template_get_target_dimensions(tmpl);
    fdl_dimensions_f64_t target_dims = {
        static_cast<double>(target_dims_i.width),
        static_cast<double>(target_dims_i.height)
    };

    // --- Validate that fit_source and preserve paths exist in source ---
    auto validate_path_exists = [&](fdl_geometry_path_t path, const char* field_name) -> bool {
        if (path == FDL_GEOMETRY_PATH_CANVAS_EFFECTIVE_DIMENSIONS &&
            !fdl_canvas_has_effective_dimensions(source_canvas)) {
            std::string msg = "Template ";
            msg += field_name;
            msg += " references 'canvas.effective_dimensions' but the source canvas "
                   "does not have effective_dimensions defined.";
            result.error = strdup(msg.c_str());
            return false;
        }
        if (path == FDL_GEOMETRY_PATH_FRAMING_PROTECTION_DIMENSIONS &&
            !fdl_framing_decision_has_protection(source_framing)) {
            std::string msg = "Template ";
            msg += field_name;
            msg += " references 'framing_decision.protection_dimensions' but the source "
                   "framing decision does not have protection_dimensions defined.";
            result.error = strdup(msg.c_str());
            return false;
        }
        return true;
    };

    if (!validate_path_exists(fit_source, "fit_source")) return result;
    if (has_preserve && !validate_path_exists(preserve_path, "preserve_from_source_canvas")) return result;

    // --- Phase 3: Populate source geometry ---
    fdl_geometry_t geometry = {};

    if (has_preserve) {
        populate_from_path(geometry, preserve_path, source_canvas, source_framing);
    }
    // Populate from fit_source (may overwrite preserve values for overlapping layers)
    populate_from_path(geometry, fit_source, source_canvas, source_framing);

    // --- Phase 4: Prepare hierarchy and calculate anchor offset ---
    fdl_dimensions_f64_t fit_dims;
    fdl_point_f64_t fit_anchor_raw;
    fdl_geometry_get_dims_anchor_from_path(&geometry, fit_source, &fit_dims, &fit_anchor_raw);

    fdl_point_f64_t anchor_offset = fit_anchor_raw;
    if (has_preserve) {
        fdl_dimensions_f64_t preserve_dims;
        fdl_point_f64_t preserve_anchor;
        fdl_geometry_get_dims_anchor_from_path(&geometry, preserve_path, &preserve_dims, &preserve_anchor);
        if (!fdl_dimensions_is_zero(preserve_dims)) {
            anchor_offset = preserve_anchor;
        }
    }

    geometry = fdl_geometry_fill_hierarchy_gaps(geometry, anchor_offset);

    // --- Calculate scale factor ---
    fdl_dimensions_f64_t fit_norm = fdl_dimensions_normalize(fit_dims, input_squeeze);
    fdl_dimensions_f64_t target_norm = fdl_dimensions_normalize(target_dims, target_squeeze);
    double scale_factor = fdl_calculate_scale_factor(fit_norm, target_norm, fit_method);

    // --- Phase 5: Scale and round ---
    geometry = fdl_geometry_normalize_and_scale(geometry, input_squeeze, scale_factor, target_squeeze);
    geometry = fdl_geometry_round(geometry, rounding);

    // Extract scaled values BEFORE crop
    fdl_dimensions_f64_t scaled_fit;
    fdl_point_f64_t scaled_fit_anchor;
    fdl_geometry_get_dims_anchor_from_path(&geometry, fit_source, &scaled_fit, &scaled_fit_anchor);
    fdl_dimensions_f64_t scaled_bounding_box = geometry.canvas_dims;

    // --- Phases 6-8: Output canvas size and content translation ---
    double out_w = fdl_output_size_for_axis(geometry.canvas_dims.width, max_w, has_max_dims ? 1 : 0, pad_to_max ? 1 : 0);
    double out_h = fdl_output_size_for_axis(geometry.canvas_dims.height, max_h, has_max_dims ? 1 : 0, pad_to_max ? 1 : 0);

    bool is_center_h = (h_align == FDL_HALIGN_CENTER);
    bool is_center_v = (v_align == FDL_VALIGN_CENTER);
    double af_h = alignment_factor_h(h_align);
    double af_v = alignment_factor_v(v_align);

    double shift_x = fdl_alignment_shift(
        scaled_fit.width, scaled_fit_anchor.x,
        out_w, geometry.canvas_dims.width, target_dims.width,
        is_center_h ? 1 : 0, af_h, pad_to_max ? 1 : 0);
    double shift_y = fdl_alignment_shift(
        scaled_fit.height, scaled_fit_anchor.y,
        out_h, geometry.canvas_dims.height, target_dims.height,
        is_center_v ? 1 : 0, af_v, pad_to_max ? 1 : 0);

    fdl_point_f64_t content_translation = {shift_x, shift_y};
    geometry.canvas_dims = {out_w, out_h};

    // --- Phase 8b: Apply offsets ---
    fdl_point_f64_t theo_eff, theo_prot, theo_fram;
    geometry = fdl_geometry_apply_offset(geometry, content_translation, &theo_eff, &theo_prot, &theo_fram);

    // --- Phase 9: Crop ---
    geometry = fdl_geometry_crop(geometry, theo_eff, theo_prot, theo_fram);

    // --- Phase 10: Create output FDL ---
    // Copy all string accessor results to std::string locals to avoid
    // thread-local buffer aliasing (different accessors share per-key buffers).
    std::string label_str = safe_copy(fdl_canvas_template_get_label(tmpl));
    if (label_str.empty()) label_str = "Template";

    std::string source_canvas_id_s = safe_copy(fdl_canvas_get_id(source_canvas));
    std::string source_fi_id_s = safe_copy(fdl_framing_decision_get_framing_intent_id(source_framing));

    std::string canvas_label;
    if (source_context_label) {
        std::string src_canvas_label_s = safe_copy(fdl_canvas_get_label(source_canvas));
        canvas_label = label_str + ": " + source_context_label + " " + src_canvas_label_s;
    } else {
        canvas_label = label_str;
    }

    // Build new FDL document
    fdl_doc_t* out_doc = fdl_doc_create_with_header(
        "00000000-0000-0000-0000-000000000000", // placeholder UUID
        2, 0,
        context_creator ? context_creator : "",
        source_fi_id_s.c_str());

    if (!out_doc) {
        result.error = strdup("Failed to create output document");
        return result;
    }

    // Add default framing intent (using source's framing_intent_id)
    fdl_doc_add_framing_intent(out_doc, source_fi_id_s.c_str(), "Default", 1, 1, 0.0);

    // Add context with source canvas and new canvas
    auto* out_ctx = fdl_doc_add_context(out_doc, label_str.c_str(), context_creator);

    // Add source canvas (copy) — copy all string properties to locals first
    auto src_dims = fdl_canvas_get_dimensions(source_canvas);
    std::string src_label_s = safe_copy(fdl_canvas_get_label(source_canvas));
    std::string src_scid_s = safe_copy(fdl_canvas_get_source_canvas_id(source_canvas));
    auto* src_cvs = fdl_context_add_canvas(out_ctx,
        source_canvas_id_s.c_str(),
        src_label_s.c_str(),
        src_scid_s.c_str(),
        src_dims.width, src_dims.height,
        fdl_canvas_get_anamorphic_squeeze(source_canvas));

    if (fdl_canvas_has_effective_dimensions(source_canvas)) {
        auto src_eff = fdl_canvas_get_effective_dimensions(source_canvas);
        auto src_eff_a = fdl_canvas_get_effective_anchor_point(source_canvas);
        fdl_canvas_set_effective_dimensions(src_cvs, src_eff, src_eff_a);
    }

    // Copy source framing decisions — copy strings to locals in each iteration
    uint32_t fd_count = fdl_canvas_framing_decisions_count(source_canvas);
    for (uint32_t i = 0; i < fd_count; ++i) {
        auto* sfd = fdl_canvas_framing_decision_at(
            const_cast<fdl_canvas_t*>(source_canvas), i);
        if (!sfd) continue;

        std::string sfd_id_s = safe_copy(fdl_framing_decision_get_id(sfd));
        std::string sfd_label_s = safe_copy(fdl_framing_decision_get_label(sfd));
        std::string sfd_fi_id_s = safe_copy(fdl_framing_decision_get_framing_intent_id(sfd));
        auto sfd_dims = fdl_framing_decision_get_dimensions(sfd);
        auto sfd_anchor = fdl_framing_decision_get_anchor_point(sfd);

        auto* copied_fd = fdl_canvas_add_framing_decision(src_cvs,
            sfd_id_s.c_str(),
            sfd_label_s.c_str(),
            sfd_fi_id_s.c_str(),
            sfd_dims.width, sfd_dims.height,
            sfd_anchor.x, sfd_anchor.y);

        if (fdl_framing_decision_has_protection(sfd)) {
            auto sfd_prot = fdl_framing_decision_get_protection_dimensions(sfd);
            auto sfd_prot_a = fdl_framing_decision_get_protection_anchor_point(sfd);
            fdl_framing_decision_set_protection(copied_fd, sfd_prot, sfd_prot_a);
        }
    }

    // Add new canvas (output of transformation)
    int64_t out_canvas_w = static_cast<int64_t>(std::round(geometry.canvas_dims.width));
    int64_t out_canvas_h = static_cast<int64_t>(std::round(geometry.canvas_dims.height));
    auto* new_cvs = fdl_context_add_canvas(out_ctx,
        new_canvas_id, canvas_label.c_str(), source_canvas_id_s.c_str(),
        out_canvas_w, out_canvas_h, target_squeeze);

    // Set effective dimensions on new canvas
    bool has_eff = !fdl_dimensions_is_zero(geometry.effective_dims);
    if (has_eff) {
        fdl_dimensions_i64_t eff_dims = {
            static_cast<int64_t>(std::round(geometry.effective_dims.width)),
            static_cast<int64_t>(std::round(geometry.effective_dims.height))
        };
        fdl_canvas_set_effective_dimensions(new_cvs, eff_dims, geometry.effective_anchor);
    }

    // Build framing decision ID: new_canvas_id-source_fi_id
    std::string fd_id = std::string(new_canvas_id) + "-" + source_fi_id_s;

    // Add new framing decision
    auto* new_fd = fdl_canvas_add_framing_decision(new_cvs,
        fd_id.c_str(),
        new_fd_name ? new_fd_name : "",
        source_fi_id_s.c_str(),
        geometry.framing_dims.width, geometry.framing_dims.height,
        geometry.framing_anchor.x, geometry.framing_anchor.y);

    // Set protection if present
    if (!fdl_dimensions_is_zero(geometry.protection_dims)) {
        fdl_framing_decision_set_protection(new_fd,
            geometry.protection_dims, geometry.protection_anchor);
    }

    // Copy canvas template into output FDL
    std::string tmpl_id_s = safe_copy(fdl_canvas_template_get_id(tmpl));
    auto* out_ct = fdl_doc_add_canvas_template(out_doc,
        tmpl_id_s.c_str(), label_str.c_str(),
        target_dims_i.width, target_dims_i.height, target_squeeze,
        fit_source, fit_method, h_align, v_align, rounding);

    if (has_preserve) {
        fdl_canvas_template_set_preserve_from_source_canvas(out_ct, preserve_path);
    }
    if (has_max_dims) {
        fdl_canvas_template_set_maximum_dimensions(out_ct, max_dims_i);
    }
    if (pad_to_max) {
        fdl_canvas_template_set_pad_to_maximum(out_ct, 1);
    }

    // Populate result
    result.output_fdl = out_doc;
    result.scale_factor = scale_factor;
    result.scaled_bounding_box = scaled_bounding_box;
    result.content_translation = content_translation;
    result.context_label = strdup(label_str.c_str());
    result.canvas_id = strdup(new_canvas_id);
    result.framing_decision_id = strdup(fd_id.c_str());
    result.error = nullptr;

    return result;
}

} // namespace fdl::detail
