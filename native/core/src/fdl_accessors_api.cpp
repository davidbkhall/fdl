// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#include "fdl/fdl_core.h"
#include "fdl_canonical.h"
#include "fdl_doc.h"
#include "fdl_enum_map.h"

#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>

using ojson = jsoncons::ojson;

// -----------------------------------------------------------------------
// Helpers for reading typed fields from ojson nodes
// -----------------------------------------------------------------------

// Compound key for thread-local string buffer: (node address, field name).
// This ensures that reading the same field from different nodes (e.g.,
// canvas.label vs context.label) returns independent buffers.
struct StringBufKey {
    uintptr_t node_addr;
    std::string field;
    bool operator==(const StringBufKey& o) const {
        return node_addr == o.node_addr && field == o.field;
    }
};

struct StringBufKeyHash {
    size_t operator()(const StringBufKey& k) const {
        return std::hash<uintptr_t>{}(k.node_addr) ^
               (std::hash<std::string>{}(k.field) << 1);
    }
};

// Per-(node, key) thread-local buffer. Pointers are valid until the next
// call for the SAME node AND SAME key on the SAME thread.
static const char* get_string(const ojson* node, const char* key) {
    if (!node || !node->contains(key) || !(*node)[key].is_string()) return nullptr;
    static thread_local std::unordered_map<StringBufKey, std::string, StringBufKeyHash> bufs;
    StringBufKey bk{reinterpret_cast<uintptr_t>(node), key};
    auto& buf = bufs[bk];
    buf = (*node)[key].as<std::string>();
    return buf.c_str();
}

static double get_double(const ojson* node, const char* key, double default_val) {
    if (!node || !node->contains(key)) return default_val;
    return (*node)[key].as<double>();
}

static fdl_dimensions_f64_t get_dims_f64(const ojson* node, const char* key) {
    fdl_dimensions_f64_t result = {0.0, 0.0};
    if (!node || !node->contains(key) || !(*node)[key].is_object()) return result;
    const auto& obj = (*node)[key];
    if (obj.contains("width")) result.width = obj["width"].as<double>();
    if (obj.contains("height")) result.height = obj["height"].as<double>();
    return result;
}

static fdl_dimensions_i64_t get_dims_i64(const ojson* node, const char* key) {
    fdl_dimensions_i64_t result = {0, 0};
    if (!node || !node->contains(key) || !(*node)[key].is_object()) return result;
    const auto& obj = (*node)[key];
    if (obj.contains("width")) result.width = obj["width"].as<int64_t>();
    if (obj.contains("height")) result.height = obj["height"].as<int64_t>();
    return result;
}

static fdl_point_f64_t get_point_f64(const ojson* node, const char* key) {
    fdl_point_f64_t result = {0.0, 0.0};
    if (!node || !node->contains(key) || !(*node)[key].is_object()) return result;
    const auto& obj = (*node)[key];
    if (obj.contains("x")) result.x = obj["x"].as<double>();
    if (obj.contains("y")) result.y = obj["y"].as<double>();
    return result;
}

extern "C" {

// -----------------------------------------------------------------------
// Context accessors
// -----------------------------------------------------------------------

const char* fdl_context_get_label(const fdl_context_t* ctx) {
    if (!ctx) return nullptr;
    doc_lock lock(ctx->owner);
    auto* n = ctx->node();
    if (!n) return nullptr;
    return get_string(n, "label");
}

const char* fdl_context_get_context_creator(const fdl_context_t* ctx) {
    if (!ctx) return nullptr;
    doc_lock lock(ctx->owner);
    auto* n = ctx->node();
    if (!n) return nullptr;
    return get_string(n, "context_creator");
}

// -----------------------------------------------------------------------
// Canvas accessors
// -----------------------------------------------------------------------

const char* fdl_canvas_get_label(const fdl_canvas_t* canvas) {
    if (!canvas) return nullptr;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return nullptr;
    return get_string(n, "label");
}

const char* fdl_canvas_get_id(const fdl_canvas_t* canvas) {
    if (!canvas) return nullptr;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return nullptr;
    return get_string(n, "id");
}

const char* fdl_canvas_get_source_canvas_id(const fdl_canvas_t* canvas) {
    if (!canvas) return nullptr;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return nullptr;
    return get_string(n, "source_canvas_id");
}

fdl_dimensions_i64_t fdl_canvas_get_dimensions(const fdl_canvas_t* canvas) {
    if (!canvas) return {0, 0};
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return {0, 0};
    return get_dims_i64(n, "dimensions");
}

int fdl_canvas_has_effective_dimensions(const fdl_canvas_t* canvas) {
    if (!canvas) return 0;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return 0;
    return n->contains("effective_dimensions") ? 1 : 0;
}

fdl_dimensions_i64_t fdl_canvas_get_effective_dimensions(const fdl_canvas_t* canvas) {
    if (!canvas) return {0, 0};
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return {0, 0};
    return get_dims_i64(n, "effective_dimensions");
}

fdl_point_f64_t fdl_canvas_get_effective_anchor_point(const fdl_canvas_t* canvas) {
    if (!canvas) return {0.0, 0.0};
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return {0.0, 0.0};
    return get_point_f64(n, "effective_anchor_point");
}

double fdl_canvas_get_anamorphic_squeeze(const fdl_canvas_t* canvas) {
    if (!canvas) return 1.0;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return 1.0;
    return get_double(n, "anamorphic_squeeze", 1.0);
}

int fdl_canvas_has_photosite_dimensions(const fdl_canvas_t* canvas) {
    if (!canvas) return 0;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return 0;
    return n->contains("photosite_dimensions") ? 1 : 0;
}

fdl_dimensions_i64_t fdl_canvas_get_photosite_dimensions(const fdl_canvas_t* canvas) {
    if (!canvas) return {0, 0};
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return {0, 0};
    return get_dims_i64(n, "photosite_dimensions");
}

int fdl_canvas_has_physical_dimensions(const fdl_canvas_t* canvas) {
    if (!canvas) return 0;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return 0;
    return n->contains("physical_dimensions") ? 1 : 0;
}

fdl_dimensions_f64_t fdl_canvas_get_physical_dimensions(const fdl_canvas_t* canvas) {
    if (!canvas) return {0.0, 0.0};
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return {0.0, 0.0};
    return get_dims_f64(n, "physical_dimensions");
}

int fdl_context_has_clip_id(const fdl_context_t* ctx) {
    if (!ctx) return 0;
    doc_lock lock(ctx->owner);
    auto* n = ctx->node();
    if (!n) return 0;
    return n->contains("clip_id") ? 1 : 0;
}

const char* fdl_context_get_clip_id(const fdl_context_t* ctx) {
    if (!ctx) return nullptr;
    doc_lock lock(ctx->owner);
    auto* n = ctx->node();
    if (!n || !n->contains("clip_id")) return nullptr;
    const auto& val = (*n)["clip_id"];
    if (val.is_string()) {
        return strdup(val.as<std::string>().c_str());
    }
    if (val.is_object()) {
        return fdl::detail::node_to_canonical_json(&val, "clip_id", 0);
    }
    return nullptr;
}

fdl_clip_id_t fdl_context_get_clip_id_struct(const fdl_context_t* ctx) {
    fdl_clip_id_t result = {};
    if (!ctx) return result;
    doc_lock lock(ctx->owner);
    auto* n = ctx->node();
    if (!n || !n->contains("clip_id")) return result;

    const auto& cid = (*n)["clip_id"];

    if (cid.contains("clip_name") && cid["clip_name"].is_string()) {
        result.clip_name = strdup(cid["clip_name"].as<std::string>().c_str());
    }

    if (cid.contains("file") && cid["file"].is_string()) {
        result.has_file = 1;
        result.file = strdup(cid["file"].as<std::string>().c_str());
    }

    if (cid.contains("sequence") && cid["sequence"].is_object()) {
        result.has_sequence = 1;
        const auto& seq = cid["sequence"];
        if (seq.contains("value") && seq["value"].is_string()) {
            result.sequence.value = strdup(seq["value"].as<std::string>().c_str());
        }
        if (seq.contains("idx") && seq["idx"].is_string()) {
            result.sequence.idx = strdup(seq["idx"].as<std::string>().c_str());
        }
        result.sequence.min = seq.contains("min") ? seq["min"].as<int64_t>() : 0;
        result.sequence.max = seq.contains("max") ? seq["max"].as<int64_t>() : 0;
    }

    return result;
}

void fdl_clip_id_free(fdl_clip_id_t* clip_id) {
    if (!clip_id) return;
    free(const_cast<char*>(clip_id->clip_name));
    free(const_cast<char*>(clip_id->file));
    if (clip_id->has_sequence) {
        free(const_cast<char*>(clip_id->sequence.value));
        free(const_cast<char*>(clip_id->sequence.idx));
    }
    *clip_id = {};
}

// -----------------------------------------------------------------------
// Framing decision accessors
// -----------------------------------------------------------------------

const char* fdl_framing_decision_get_label(const fdl_framing_decision_t* fd) {
    if (!fd) return nullptr;
    doc_lock lock(fd->owner);
    auto* n = fd->node();
    if (!n) return nullptr;
    return get_string(n, "label");
}

const char* fdl_framing_decision_get_id(const fdl_framing_decision_t* fd) {
    if (!fd) return nullptr;
    doc_lock lock(fd->owner);
    auto* n = fd->node();
    if (!n) return nullptr;
    return get_string(n, "id");
}

const char* fdl_framing_decision_get_framing_intent_id(const fdl_framing_decision_t* fd) {
    if (!fd) return nullptr;
    doc_lock lock(fd->owner);
    auto* n = fd->node();
    if (!n) return nullptr;
    return get_string(n, "framing_intent_id");
}

fdl_dimensions_f64_t fdl_framing_decision_get_dimensions(const fdl_framing_decision_t* fd) {
    if (!fd) return {0.0, 0.0};
    doc_lock lock(fd->owner);
    auto* n = fd->node();
    if (!n) return {0.0, 0.0};
    return get_dims_f64(n, "dimensions");
}

fdl_point_f64_t fdl_framing_decision_get_anchor_point(const fdl_framing_decision_t* fd) {
    if (!fd) return {0.0, 0.0};
    doc_lock lock(fd->owner);
    auto* n = fd->node();
    if (!n) return {0.0, 0.0};
    return get_point_f64(n, "anchor_point");
}

int fdl_framing_decision_has_protection(const fdl_framing_decision_t* fd) {
    if (!fd) return 0;
    doc_lock lock(fd->owner);
    auto* n = fd->node();
    if (!n) return 0;
    return n->contains("protection_dimensions") ? 1 : 0;
}

fdl_dimensions_f64_t fdl_framing_decision_get_protection_dimensions(const fdl_framing_decision_t* fd) {
    if (!fd) return {0.0, 0.0};
    doc_lock lock(fd->owner);
    auto* n = fd->node();
    if (!n) return {0.0, 0.0};
    return get_dims_f64(n, "protection_dimensions");
}

fdl_point_f64_t fdl_framing_decision_get_protection_anchor_point(const fdl_framing_decision_t* fd) {
    if (!fd) return {0.0, 0.0};
    doc_lock lock(fd->owner);
    auto* n = fd->node();
    if (!n) return {0.0, 0.0};
    return get_point_f64(n, "protection_anchor_point");
}

// -----------------------------------------------------------------------
// Framing intent accessors
// -----------------------------------------------------------------------

const char* fdl_framing_intent_get_label(const fdl_framing_intent_t* fi) {
    if (!fi) return nullptr;
    doc_lock lock(fi->owner);
    auto* n = fi->node();
    if (!n) return nullptr;
    return get_string(n, "label");
}

const char* fdl_framing_intent_get_id(const fdl_framing_intent_t* fi) {
    if (!fi) return nullptr;
    doc_lock lock(fi->owner);
    auto* n = fi->node();
    if (!n) return nullptr;
    return get_string(n, "id");
}

fdl_dimensions_i64_t fdl_framing_intent_get_aspect_ratio(const fdl_framing_intent_t* fi) {
    if (!fi) return {0, 0};
    doc_lock lock(fi->owner);
    auto* n = fi->node();
    if (!n) return {0, 0};
    return get_dims_i64(n, "aspect_ratio");
}

double fdl_framing_intent_get_protection(const fdl_framing_intent_t* fi) {
    if (!fi) return 0.0;
    doc_lock lock(fi->owner);
    auto* n = fi->node();
    if (!n) return 0.0;
    return get_double(n, "protection", 0.0);
}

// -----------------------------------------------------------------------
// Canvas template accessors
// -----------------------------------------------------------------------

const char* fdl_canvas_template_get_label(const fdl_canvas_template_t* ct) {
    if (!ct) return nullptr;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n) return nullptr;
    return get_string(n, "label");
}

const char* fdl_canvas_template_get_id(const fdl_canvas_template_t* ct) {
    if (!ct) return nullptr;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n) return nullptr;
    return get_string(n, "id");
}

fdl_dimensions_i64_t fdl_canvas_template_get_target_dimensions(const fdl_canvas_template_t* ct) {
    if (!ct) return {0, 0};
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n) return {0, 0};
    return get_dims_i64(n, "target_dimensions");
}

double fdl_canvas_template_get_target_anamorphic_squeeze(const fdl_canvas_template_t* ct) {
    if (!ct) return 1.0;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n) return 1.0;
    return get_double(n, "target_anamorphic_squeeze", 1.0);
}

fdl_geometry_path_t fdl_canvas_template_get_fit_source(const fdl_canvas_template_t* ct) {
    if (!ct) return FDL_GEOMETRY_PATH_FRAMING_DIMENSIONS;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n || !n->contains("fit_source")) return FDL_GEOMETRY_PATH_FRAMING_DIMENSIONS;
    return fdl::detail::geometry_path_from_string((*n)["fit_source"].as<std::string_view>());
}

fdl_fit_method_t fdl_canvas_template_get_fit_method(const fdl_canvas_template_t* ct) {
    if (!ct) return FDL_FIT_METHOD_WIDTH;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n || !n->contains("fit_method")) return FDL_FIT_METHOD_WIDTH;
    return fdl::detail::fit_method_from_string((*n)["fit_method"].as<std::string_view>());
}

fdl_halign_t fdl_canvas_template_get_alignment_method_horizontal(const fdl_canvas_template_t* ct) {
    if (!ct) return FDL_HALIGN_CENTER;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n || !n->contains("alignment_method_horizontal")) return FDL_HALIGN_CENTER;
    return fdl::detail::halign_from_string((*n)["alignment_method_horizontal"].as<std::string_view>());
}

fdl_valign_t fdl_canvas_template_get_alignment_method_vertical(const fdl_canvas_template_t* ct) {
    if (!ct) return FDL_VALIGN_CENTER;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n || !n->contains("alignment_method_vertical")) return FDL_VALIGN_CENTER;
    return fdl::detail::valign_from_string((*n)["alignment_method_vertical"].as<std::string_view>());
}

int fdl_canvas_template_has_preserve_from_source_canvas(const fdl_canvas_template_t* ct) {
    if (!ct) return 0;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n) return 0;
    return n->contains("preserve_from_source_canvas") ? 1 : 0;
}

fdl_geometry_path_t fdl_canvas_template_get_preserve_from_source_canvas(const fdl_canvas_template_t* ct) {
    if (!ct) return FDL_GEOMETRY_PATH_CANVAS_DIMENSIONS;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n || !n->contains("preserve_from_source_canvas"))
        return FDL_GEOMETRY_PATH_CANVAS_DIMENSIONS;
    return fdl::detail::geometry_path_from_string(
        (*n)["preserve_from_source_canvas"].as<std::string_view>());
}

int fdl_canvas_template_has_maximum_dimensions(const fdl_canvas_template_t* ct) {
    if (!ct) return 0;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n) return 0;
    return n->contains("maximum_dimensions") ? 1 : 0;
}

fdl_dimensions_i64_t fdl_canvas_template_get_maximum_dimensions(const fdl_canvas_template_t* ct) {
    if (!ct) return {0, 0};
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n) return {0, 0};
    return get_dims_i64(n, "maximum_dimensions");
}

int fdl_canvas_template_get_pad_to_maximum(const fdl_canvas_template_t* ct) {
    if (!ct) return 0;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n || !n->contains("pad_to_maximum")) return 0;
    return (*n)["pad_to_maximum"].as<bool>() ? 1 : 0;
}

fdl_round_strategy_t fdl_canvas_template_get_round(const fdl_canvas_template_t* ct) {
    fdl_round_strategy_t result = {FDL_ROUNDING_EVEN_WHOLE, FDL_ROUNDING_MODE_ROUND};
    if (!ct) return result;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n || !n->contains("round") || !(*n)["round"].is_object())
        return result;

    const auto& rnd = (*n)["round"];
    if (rnd.contains("even") && rnd["even"].is_string())
        result.even = fdl::detail::rounding_even_from_string(rnd["even"].as<std::string_view>());
    if (rnd.contains("mode") && rnd["mode"].is_string())
        result.mode = fdl::detail::rounding_mode_from_string(rnd["mode"].as<std::string_view>());
    return result;
}

// -----------------------------------------------------------------------
// Document header accessors (version)
// -----------------------------------------------------------------------

int fdl_doc_get_version_major(const fdl_doc_t* doc) {
    if (!doc) return 0;
    doc_lock lock(doc);
    const auto& data = doc->doc.data();
    if (!data.contains("version") || !data["version"].is_object()) return 0;
    const auto& ver = data["version"];
    if (!ver.contains("major")) return 0;
    return ver["major"].as<int>();
}

int fdl_doc_get_version_minor(const fdl_doc_t* doc) {
    if (!doc) return 0;
    doc_lock lock(doc);
    const auto& data = doc->doc.data();
    if (!data.contains("version") || !data["version"].is_object()) return 0;
    const auto& ver = data["version"];
    if (!ver.contains("minor")) return 0;
    return ver["minor"].as<int>();
}

// -----------------------------------------------------------------------
// Sub-object serialization
// -----------------------------------------------------------------------

char* fdl_context_to_json(const fdl_context_t* ctx, int indent) {
    if (!ctx) return nullptr;
    doc_lock lock(ctx->owner);
    auto* n = ctx->node();
    if (!n) return nullptr;
    return fdl::detail::node_to_canonical_json(n, "context", indent);
}

char* fdl_canvas_to_json(const fdl_canvas_t* canvas, int indent) {
    if (!canvas) return nullptr;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n) return nullptr;
    return fdl::detail::node_to_canonical_json(n, "canvas", indent);
}

char* fdl_framing_decision_to_json(const fdl_framing_decision_t* fd, int indent) {
    if (!fd) return nullptr;
    doc_lock lock(fd->owner);
    auto* n = fd->node();
    if (!n) return nullptr;
    return fdl::detail::node_to_canonical_json(n, "framing_decision", indent);
}

char* fdl_framing_intent_to_json(const fdl_framing_intent_t* fi, int indent) {
    if (!fi) return nullptr;
    doc_lock lock(fi->owner);
    auto* n = fi->node();
    if (!n) return nullptr;
    return fdl::detail::node_to_canonical_json(n, "framing_intent", indent);
}

char* fdl_canvas_template_to_json(const fdl_canvas_template_t* ct, int indent) {
    if (!ct) return nullptr;
    doc_lock lock(ct->owner);
    auto* n = ct->node();
    if (!n) return nullptr;
    return fdl::detail::node_to_canonical_json(n, "canvas_template", indent);
}

} // extern "C"
