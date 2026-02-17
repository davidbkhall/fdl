// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
/**
 * @file fdl_collections_api.cpp
 * @brief C ABI for collection traversal (count, at, find_by_id/label) and handle allocation.
 */
#include "fdl/fdl_core.h"
#include "fdl_compat.h"
#include "fdl_doc.h"

#include <cinttypes>
#include <cstring>
#include <string_view>

using ojson = jsoncons::ojson;

extern "C" {

// -----------------------------------------------------------------------
// Framing intents (root-level collection)
// -----------------------------------------------------------------------

uint32_t fdl_doc_framing_intents_count(fdl_doc_t* doc) {
    if (!doc) return 0;
    doc_lock lock(doc);
    auto& data = doc->doc.data();
    if (!data.contains("framing_intents") || !data["framing_intents"].is_array()) return 0;
    return static_cast<uint32_t>(data["framing_intents"].size());
}

fdl_framing_intent_t* fdl_doc_framing_intent_at(fdl_doc_t* doc, uint32_t index) {
    if (!doc) return nullptr;
    doc_lock lock(doc);
    auto& data = doc->doc.data();
    if (!data.contains("framing_intents") || !data["framing_intents"].is_array()) return nullptr;
    if (index >= data["framing_intents"].size()) return nullptr;

    auto it = doc->handles.fi_by_index.find(index);
    if (it != doc->handles.fi_by_index.end()) return it->second;

    auto handle = std::make_unique<fdl_framing_intent>();
    handle->owner = doc;
    handle->fi_index = index;
    auto* ptr = handle.get();
    doc->handles.framing_intents.push_back(std::move(handle));
    doc->handles.fi_by_index[index] = ptr;
    return ptr;
}

fdl_framing_intent_t* fdl_doc_framing_intent_find_by_id(fdl_doc_t* doc, const char* id) {
    if (!doc || !id) return nullptr;
    doc_lock lock(doc);
    auto& data = doc->doc.data();
    if (!data.contains("framing_intents") || !data["framing_intents"].is_array()) return nullptr;

    std::string_view target(id);
    auto& arr = data["framing_intents"];
    for (size_t i = 0; i < arr.size(); ++i) {
        auto& elem = arr[i];
        if (elem.contains("id") && elem["id"].is_string() &&
            elem["id"].as<std::string_view>() == target) {
            uint32_t index = static_cast<uint32_t>(i);

            auto it = doc->handles.fi_by_index.find(index);
            if (it != doc->handles.fi_by_index.end()) return it->second;

            auto handle = std::make_unique<fdl_framing_intent>();
            handle->owner = doc;
            handle->fi_index = index;
            auto* ptr = handle.get();
            doc->handles.framing_intents.push_back(std::move(handle));
            doc->handles.fi_by_index[index] = ptr;
            return ptr;
        }
    }
    return nullptr;
}

// -----------------------------------------------------------------------
// Contexts (root-level collection)
// -----------------------------------------------------------------------

uint32_t fdl_doc_contexts_count(fdl_doc_t* doc) {
    if (!doc) return 0;
    doc_lock lock(doc);
    auto& data = doc->doc.data();
    if (!data.contains("contexts") || !data["contexts"].is_array()) return 0;
    return static_cast<uint32_t>(data["contexts"].size());
}

fdl_context_t* fdl_doc_context_at(fdl_doc_t* doc, uint32_t index) {
    if (!doc) return nullptr;
    doc_lock lock(doc);
    auto& data = doc->doc.data();
    if (!data.contains("contexts") || !data["contexts"].is_array()) return nullptr;
    if (index >= data["contexts"].size()) return nullptr;

    auto it = doc->handles.ctx_by_index.find(index);
    if (it != doc->handles.ctx_by_index.end()) return it->second;

    auto handle = std::make_unique<fdl_context>();
    handle->owner = doc;
    handle->ctx_index = index;
    auto* ptr = handle.get();
    doc->handles.contexts.push_back(std::move(handle));
    doc->handles.ctx_by_index[index] = ptr;
    return ptr;
}

fdl_context_t* fdl_doc_context_find_by_label(fdl_doc_t* doc, const char* label) {
    if (!doc || !label) return nullptr;
    doc_lock lock(doc);
    auto& data = doc->doc.data();
    if (!data.contains("contexts") || !data["contexts"].is_array()) return nullptr;

    std::string_view target(label);
    auto& arr = data["contexts"];
    for (size_t i = 0; i < arr.size(); ++i) {
        auto& elem = arr[i];
        if (elem.contains("label") && elem["label"].is_string() &&
            elem["label"].as<std::string_view>() == target) {
            uint32_t index = static_cast<uint32_t>(i);

            auto it = doc->handles.ctx_by_index.find(index);
            if (it != doc->handles.ctx_by_index.end()) return it->second;

            auto handle = std::make_unique<fdl_context>();
            handle->owner = doc;
            handle->ctx_index = index;
            auto* ptr = handle.get();
            doc->handles.contexts.push_back(std::move(handle));
            doc->handles.ctx_by_index[index] = ptr;
            return ptr;
        }
    }
    return nullptr;
}

// -----------------------------------------------------------------------
// Canvas templates (root-level collection)
// -----------------------------------------------------------------------

uint32_t fdl_doc_canvas_templates_count(fdl_doc_t* doc) {
    if (!doc) return 0;
    doc_lock lock(doc);
    auto& data = doc->doc.data();
    if (!data.contains("canvas_templates") || !data["canvas_templates"].is_array()) return 0;
    return static_cast<uint32_t>(data["canvas_templates"].size());
}

fdl_canvas_template_t* fdl_doc_canvas_template_at(fdl_doc_t* doc, uint32_t index) {
    if (!doc) return nullptr;
    doc_lock lock(doc);
    auto& data = doc->doc.data();
    if (!data.contains("canvas_templates") || !data["canvas_templates"].is_array()) return nullptr;
    if (index >= data["canvas_templates"].size()) return nullptr;

    auto it = doc->handles.ct_by_index.find(index);
    if (it != doc->handles.ct_by_index.end()) return it->second;

    auto handle = std::make_unique<fdl_canvas_template>();
    handle->owner = doc;
    handle->ct_index = index;
    auto* ptr = handle.get();
    doc->handles.canvas_templates.push_back(std::move(handle));
    doc->handles.ct_by_index[index] = ptr;
    return ptr;
}

fdl_canvas_template_t* fdl_doc_canvas_template_find_by_id(fdl_doc_t* doc, const char* id) {
    if (!doc || !id) return nullptr;
    doc_lock lock(doc);
    auto& data = doc->doc.data();
    if (!data.contains("canvas_templates") || !data["canvas_templates"].is_array()) return nullptr;

    std::string_view target(id);
    auto& arr = data["canvas_templates"];
    for (size_t i = 0; i < arr.size(); ++i) {
        auto& elem = arr[i];
        if (elem.contains("id") && elem["id"].is_string() &&
            elem["id"].as<std::string_view>() == target) {
            uint32_t index = static_cast<uint32_t>(i);

            auto it = doc->handles.ct_by_index.find(index);
            if (it != doc->handles.ct_by_index.end()) return it->second;

            auto handle = std::make_unique<fdl_canvas_template>();
            handle->owner = doc;
            handle->ct_index = index;
            auto* ptr = handle.get();
            doc->handles.canvas_templates.push_back(std::move(handle));
            doc->handles.ct_by_index[index] = ptr;
            return ptr;
        }
    }
    return nullptr;
}

// -----------------------------------------------------------------------
// Canvases (child of context)
// -----------------------------------------------------------------------

uint32_t fdl_context_canvases_count(const fdl_context_t* ctx) {
    if (!ctx) return 0;
    doc_lock lock(ctx->owner);
    auto* n = ctx->node();
    if (!n || !n->contains("canvases") || !(*n)["canvases"].is_array()) return 0;
    return static_cast<uint32_t>((*n)["canvases"].size());
}

fdl_canvas_t* fdl_context_canvas_at(fdl_context_t* ctx, uint32_t index) {
    if (!ctx) return nullptr;
    doc_lock lock(ctx->owner);
    auto* n = ctx->node();
    if (!n || !n->contains("canvases") || !(*n)["canvases"].is_array()) return nullptr;
    if (index >= (*n)["canvases"].size()) return nullptr;

    uint64_t key = pack_key(ctx->ctx_index, index);
    auto it = ctx->owner->handles.cvs_by_key.find(key);
    if (it != ctx->owner->handles.cvs_by_key.end()) return it->second;

    auto handle = std::make_unique<fdl_canvas>();
    handle->owner = ctx->owner;
    handle->ctx_index = ctx->ctx_index;
    handle->cvs_index = index;
    auto* ptr = handle.get();
    ctx->owner->handles.canvases.push_back(std::move(handle));
    ctx->owner->handles.cvs_by_key[key] = ptr;
    return ptr;
}

fdl_canvas_t* fdl_context_find_canvas_by_id(fdl_context_t* ctx, const char* id) {
    if (!ctx || !id) return nullptr;
    doc_lock lock(ctx->owner);
    auto* n = ctx->node();
    if (!n || !n->contains("canvases") || !(*n)["canvases"].is_array()) return nullptr;

    std::string_view target(id);
    auto& arr = (*n)["canvases"];
    for (size_t i = 0; i < arr.size(); ++i) {
        auto& elem = arr[i];
        if (elem.contains("id") && elem["id"].is_string() &&
            elem["id"].as<std::string_view>() == target) {
            uint32_t cvs_index = static_cast<uint32_t>(i);

            uint64_t key = pack_key(ctx->ctx_index, cvs_index);
            auto it = ctx->owner->handles.cvs_by_key.find(key);
            if (it != ctx->owner->handles.cvs_by_key.end()) return it->second;

            auto handle = std::make_unique<fdl_canvas>();
            handle->owner = ctx->owner;
            handle->ctx_index = ctx->ctx_index;
            handle->cvs_index = cvs_index;
            auto* ptr = handle.get();
            ctx->owner->handles.canvases.push_back(std::move(handle));
            ctx->owner->handles.cvs_by_key[key] = ptr;
            return ptr;
        }
    }
    return nullptr;
}

// -----------------------------------------------------------------------
// Framing decisions (child of canvas)
// -----------------------------------------------------------------------

uint32_t fdl_canvas_framing_decisions_count(const fdl_canvas_t* canvas) {
    if (!canvas) return 0;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n || !n->contains("framing_decisions") || !(*n)["framing_decisions"].is_array()) return 0;
    return static_cast<uint32_t>((*n)["framing_decisions"].size());
}

fdl_framing_decision_t* fdl_canvas_framing_decision_at(fdl_canvas_t* canvas, uint32_t index) {
    if (!canvas) return nullptr;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n || !n->contains("framing_decisions") || !(*n)["framing_decisions"].is_array()) return nullptr;
    if (index >= (*n)["framing_decisions"].size()) return nullptr;

    uint64_t key = pack_key3(canvas->ctx_index, canvas->cvs_index, index);
    auto it = canvas->owner->handles.fd_by_key.find(key);
    if (it != canvas->owner->handles.fd_by_key.end()) return it->second;

    auto handle = std::make_unique<fdl_framing_decision>();
    handle->owner = canvas->owner;
    handle->ctx_index = canvas->ctx_index;
    handle->cvs_index = canvas->cvs_index;
    handle->fd_index = index;
    auto* ptr = handle.get();
    canvas->owner->handles.framing_decisions.push_back(std::move(handle));
    canvas->owner->handles.fd_by_key[key] = ptr;
    return ptr;
}

fdl_framing_decision_t* fdl_canvas_find_framing_decision_by_id(fdl_canvas_t* canvas, const char* id) {
    if (!canvas || !id) return nullptr;
    doc_lock lock(canvas->owner);
    auto* n = canvas->node();
    if (!n || !n->contains("framing_decisions") || !(*n)["framing_decisions"].is_array()) return nullptr;

    std::string_view target(id);
    auto& arr = (*n)["framing_decisions"];
    for (size_t i = 0; i < arr.size(); ++i) {
        auto& elem = arr[i];
        if (elem.contains("id") && elem["id"].is_string() &&
            elem["id"].as<std::string_view>() == target) {
            uint32_t fd_index = static_cast<uint32_t>(i);

            uint64_t key = pack_key3(canvas->ctx_index, canvas->cvs_index, fd_index);
            auto it = canvas->owner->handles.fd_by_key.find(key);
            if (it != canvas->owner->handles.fd_by_key.end()) return it->second;

            auto handle = std::make_unique<fdl_framing_decision>();
            handle->owner = canvas->owner;
            handle->ctx_index = canvas->ctx_index;
            handle->cvs_index = canvas->cvs_index;
            handle->fd_index = fd_index;
            auto* ptr = handle.get();
            canvas->owner->handles.framing_decisions.push_back(std::move(handle));
            canvas->owner->handles.fd_by_key[key] = ptr;
            return ptr;
        }
    }
    return nullptr;
}

// -----------------------------------------------------------------------
// Canvas resolution
// -----------------------------------------------------------------------

fdl_resolve_canvas_result_t fdl_context_resolve_canvas_for_dimensions(
    fdl_context_t* ctx,
    fdl_dimensions_f64_t input_dims,
    fdl_canvas_t* canvas,
    fdl_framing_decision_t* framing) {

    fdl_resolve_canvas_result_t result = {};

    if (!ctx || !canvas || !framing) {
        result.error = fdl_strdup("NULL argument to fdl_context_resolve_canvas_for_dimensions");
        return result;
    }

    doc_lock lock(ctx->owner);

    // Get canvas dimensions
    auto* cvs_node = canvas->node();
    if (!cvs_node || !cvs_node->contains("dimensions")) {
        result.error = fdl_strdup("Canvas has no dimensions");
        return result;
    }
    auto& dims = (*cvs_node)["dimensions"];
    int64_t cvs_w = dims["width"].as<int64_t>();
    int64_t cvs_h = dims["height"].as<int64_t>();

    int64_t input_w = static_cast<int64_t>(input_dims.width);
    int64_t input_h = static_cast<int64_t>(input_dims.height);

    // Fast path: dimensions already match
    if (cvs_w == input_w && cvs_h == input_h) {
        result.canvas = canvas;
        result.framing_decision = framing;
        result.was_resolved = 0;
        return result;
    }

    // Check if this is a derived canvas (id != source_canvas_id)
    std::string canvas_id = cvs_node->contains("id")
        ? (*cvs_node)["id"].as<std::string>() : "";
    std::string source_id = cvs_node->contains("source_canvas_id")
        ? (*cvs_node)["source_canvas_id"].as<std::string>() : "";

    if (canvas_id != source_id) {
        // Get framing label for matching
        auto* fd_node = framing->node();
        std::string fd_label = (fd_node && fd_node->contains("label"))
            ? (*fd_node)["label"].as<std::string>() : "";

        // Search sibling canvases in this context
        auto* ctx_node = ctx->node();
        if (ctx_node && ctx_node->contains("canvases") && (*ctx_node)["canvases"].is_array()) {
            auto& canvases = (*ctx_node)["canvases"];
            uint32_t n = static_cast<uint32_t>(canvases.size());

            for (uint32_t i = 0; i < n; ++i) {
                auto& other = canvases[i];
                if (!other.contains("dimensions")) continue;

                auto& other_dims = other["dimensions"];
                int64_t ow = other_dims["width"].as<int64_t>();
                int64_t oh = other_dims["height"].as<int64_t>();

                if (ow == input_w && oh == input_h) {
                    // Found matching dimensions — search framing decisions by label
                    if (other.contains("framing_decisions") && other["framing_decisions"].is_array()) {
                        auto& fds = other["framing_decisions"];
                        for (uint32_t j = 0; j < fds.size(); ++j) {
                            std::string other_label = fds[j].contains("label")
                                ? fds[j]["label"].as<std::string>() : "";
                            if (other_label == fd_label) {
                                // Get or create deduped canvas handle
                                uint64_t cvs_key = pack_key(ctx->ctx_index, i);
                                fdl_canvas_t* matched_cvs;
                                auto cvs_it = ctx->owner->handles.cvs_by_key.find(cvs_key);
                                if (cvs_it != ctx->owner->handles.cvs_by_key.end()) {
                                    matched_cvs = cvs_it->second;
                                } else {
                                    auto h = std::make_unique<fdl_canvas>();
                                    h->owner = ctx->owner;
                                    h->ctx_index = ctx->ctx_index;
                                    h->cvs_index = i;
                                    matched_cvs = h.get();
                                    ctx->owner->handles.canvases.push_back(std::move(h));
                                    ctx->owner->handles.cvs_by_key[cvs_key] = matched_cvs;
                                }

                                // Get or create deduped framing decision handle
                                uint64_t fd_key = pack_key3(ctx->ctx_index, i, j);
                                fdl_framing_decision_t* matched_fd;
                                auto fd_it = ctx->owner->handles.fd_by_key.find(fd_key);
                                if (fd_it != ctx->owner->handles.fd_by_key.end()) {
                                    matched_fd = fd_it->second;
                                } else {
                                    auto h = std::make_unique<fdl_framing_decision>();
                                    h->owner = ctx->owner;
                                    h->ctx_index = ctx->ctx_index;
                                    h->cvs_index = i;
                                    h->fd_index = j;
                                    matched_fd = h.get();
                                    ctx->owner->handles.framing_decisions.push_back(std::move(h));
                                    ctx->owner->handles.fd_by_key[fd_key] = matched_fd;
                                }

                                result.canvas = matched_cvs;
                                result.framing_decision = matched_fd;
                                result.was_resolved = 1;
                                return result;
                            }
                        }
                    }
                }
            }
        }
    }

    // No match found
    char buf[256];
    snprintf(buf, sizeof(buf),
        "Canvas dimensions (%" PRId64 "x%" PRId64 ") do not match input dimensions (%" PRId64 "x%" PRId64 ")",
        cvs_w, cvs_h, input_w, input_h);
    result.error = fdl_strdup(buf);
    return result;
}

} // extern "C"
