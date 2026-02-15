#ifndef FDL_HANDLES_INTERNAL_H
#define FDL_HANDLES_INTERNAL_H

#include <jsoncons/json.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

// Forward declare the doc handle
struct fdl_doc;

// Internal handle types — each stores path indices into the document's ojson tree.
// Handles resolve their node lazily via node(), making them stable across array
// reallocations (e.g., push_back). Lifetime is managed by the owning fdl_doc.

struct fdl_framing_intent {
    fdl_doc* owner;
    uint32_t fi_index;
    jsoncons::ojson* node() const;
};

struct fdl_context {
    fdl_doc* owner;
    uint32_t ctx_index;
    jsoncons::ojson* node() const;
};

struct fdl_canvas_template {
    fdl_doc* owner;
    uint32_t ct_index;
    jsoncons::ojson* node() const;
};

struct fdl_canvas {
    fdl_doc* owner;
    uint32_t ctx_index;
    uint32_t cvs_index;
    jsoncons::ojson* node() const;
};

struct fdl_framing_decision {
    fdl_doc* owner;
    uint32_t ctx_index;
    uint32_t cvs_index;
    uint32_t fd_index;
    jsoncons::ojson* node() const;
};

// Helpers to pack multi-level indices into map keys for deduplication
inline uint64_t pack_key(uint32_t a, uint32_t b) {
    return (static_cast<uint64_t>(a) << 32) | b;
}
inline uint64_t pack_key3(uint32_t a, uint32_t b, uint32_t c) {
    return (static_cast<uint64_t>(a) << 40) | (static_cast<uint64_t>(b) << 20) | c;
}

// Cache container for all handle types — lives on fdl_doc.
// Includes deduplication maps to avoid creating duplicate handles for the same index.
struct fdl_handle_cache {
    // Ownership vectors
    std::vector<std::unique_ptr<fdl_context>> contexts;
    std::vector<std::unique_ptr<fdl_canvas>> canvases;
    std::vector<std::unique_ptr<fdl_framing_decision>> framing_decisions;
    std::vector<std::unique_ptr<fdl_framing_intent>> framing_intents;
    std::vector<std::unique_ptr<fdl_canvas_template>> canvas_templates;

    // Deduplication maps (index path -> raw pointer into ownership vectors)
    std::unordered_map<uint32_t, fdl_context*> ctx_by_index;
    std::unordered_map<uint32_t, fdl_framing_intent*> fi_by_index;
    std::unordered_map<uint32_t, fdl_canvas_template*> ct_by_index;
    std::unordered_map<uint64_t, fdl_canvas*> cvs_by_key;
    std::unordered_map<uint64_t, fdl_framing_decision*> fd_by_key;

    void clear() {
        contexts.clear();
        canvases.clear();
        framing_decisions.clear();
        framing_intents.clear();
        canvas_templates.clear();
        ctx_by_index.clear();
        fi_by_index.clear();
        ct_by_index.clear();
        cvs_by_key.clear();
        fd_by_key.clear();
    }
};

#endif // FDL_HANDLES_INTERNAL_H
