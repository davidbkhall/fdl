// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
/**
 * @file fdl_validate.cpp
 * @brief Schema (Draft 2020-12) and semantic validation using embedded schema data.
 */
#include "fdl_validate.h"
#include "fdl_constants.h"
#include "fdl_schema_data.h"

#include <jsoncons_ext/jsonschema/jsonschema.hpp>

#include <algorithm>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <stdexcept>

namespace fdl::detail {

/** Offset for adjacent-pair iteration (levels[i] vs levels[i + kAdjacentPairStep]). */
constexpr size_t kAdjacentPairStep = 1;

// ---------------------------------------------------------------------------
// Schema registry — version-based lookup matching Python's patch-preference
// ---------------------------------------------------------------------------

// Key: {major, minor} -> compiled schema for latest patch
/** @brief Type alias for schema registry key. */
using SchemaKey = std::pair<int, int>;
/** @brief Type alias for a compiled JSON Schema object. */
using CompiledSchema = jsoncons::jsonschema::json_schema<ojson>;

/**
 * @brief Compile a JSON Schema string into a reusable schema object.
 * @param json_str  Raw JSON Schema text (Draft 2020-12).
 * @return Compiled schema ready for validation.
 */
static CompiledSchema compile_schema(const char* json_str) {
    auto schema_doc = ojson::parse(std::string_view(json_str));
    auto options = jsoncons::jsonschema::evaluation_options{}
                       .default_version(jsoncons::jsonschema::schema_version::draft202012())
                       .require_format_validation(true);
    return jsoncons::jsonschema::make_json_schema(schema_doc, options);
}

/**
 * @brief Return the lazily-built schema registry, keyed by (major, minor).
 * @return Const reference to the map of compiled schemas.
 */
static const std::map<SchemaKey, CompiledSchema>& get_schema_registry() {
    static const auto registry = [] {
        // Group by (major, minor), pick latest patch
        std::map<SchemaKey, const SchemaEntry*> best;
        for (size_t i = 0; i < SCHEMA_REGISTRY_SIZE; ++i) {
            const auto& e = SCHEMA_REGISTRY[i];
            SchemaKey key{e.major, e.minor};
            auto it = best.find(key);
            if (it == best.end() || e.patch > it->second->patch) {
                best[key] = &e;
            }
        }
        // Compile each winning schema
        std::map<SchemaKey, CompiledSchema> compiled;
        for (auto& [key, entry] : best) {
            compiled.emplace(key, compile_schema(entry->data));
        }
        return compiled;
    }();
    return registry;
}

// ---------------------------------------------------------------------------
// Schema validation
// ---------------------------------------------------------------------------

/**
 * @brief Validate an FDL document against the appropriate JSON Schema.
 * @param fdl     Parsed FDL document.
 * @param errors  Output vector that receives any schema-level error strings.
 */
static void validate_schema(const ojson& fdl, std::vector<std::string>& errors) {
    // Extract version from document (default to 2.0 like Python)
    int major = fdl::constants::kDefaultVersionMajor, minor = 0;
    if (fdl.contains("version") && fdl["version"].is_object()) {
        const auto& ver = fdl["version"];
        if (ver.contains("major") && ver["major"].is_number()) {
            major = ver["major"].as<int>();
        }
        if (ver.contains("minor") && ver["minor"].is_number()) {
            minor = ver["minor"].as<int>();
        }
    }

    const auto& registry = get_schema_registry();
    auto it = registry.find({major, minor});
    if (it == registry.end()) {
        errors.push_back(
            "Schema Error: /: No schema found for version " + std::to_string(major) + "." + std::to_string(minor));
        return;
    }

    auto reporter =
        [&errors](const jsoncons::jsonschema::validation_message& msg) -> jsoncons::jsonschema::walk_result {
        std::string loc = msg.instance_location().string();
        if (loc.empty()) {
            loc = "/";
        }
        errors.push_back("Schema Error: " + loc + ": " + msg.message());
        return jsoncons::jsonschema::walk_result::advance;
    };

    it->second.validate(fdl, reporter);
}

// ---------------------------------------------------------------------------
// Helper: get string with fallback
// ---------------------------------------------------------------------------

/**
 * @brief Get a string value from a JSON object, returning a fallback if absent.
 * @param obj       JSON object to query.
 * @param key       Key to look up.
 * @param fallback  Value returned when the key is missing or non-string.
 * @return The string value or @p fallback.
 */
static std::string get_str(const ojson& obj, const std::string& key, const std::string& fallback = "") {
    if (obj.contains(key) && obj[key].is_string()) {
        return obj[key].as<std::string>();
    }
    return fallback;
}

/**
 * @brief Get a numeric value from a JSON object, returning a fallback if absent.
 * @param obj       JSON object to query.
 * @param key       Key to look up.
 * @param fallback  Value returned when the key is missing or non-numeric.
 * @return The numeric value or @p fallback.
 */
static double get_num(const ojson& obj, const std::string& key, double fallback = 0.0) {
    if (obj.contains(key) && obj[key].is_number()) {
        return obj[key].as<double>();
    }
    return fallback;
}

// ---------------------------------------------------------------------------
// Validator 1: ID Tree
// ---------------------------------------------------------------------------

/**
 * @brief Validate uniqueness and referential integrity of all IDs in the FDL.
 * @param fdl     Parsed FDL document.
 * @param errors  Output vector that receives any ID-tree error strings.
 */
static void validate_id_tree(const ojson& fdl, std::vector<std::string>& errors) {
    try {
        std::set<std::string> fi_ids;

        if (fdl.contains("framing_intents") && fdl["framing_intents"].is_array()) {
            for (const auto& fi : fdl["framing_intents"].array_range()) {
                if (!fi.contains("id")) {
                    throw std::runtime_error("Framing Intent missing 'id'");
                }
                auto fi_id = fi["id"].as<std::string>();
                auto fi_label = get_str(fi, "label", "(no label)");
                if (fi_ids.count(fi_id)) {
                    throw std::runtime_error("Framing Intent " + fi_id + " (" + fi_label + "): ID duplicated");
                }
                fi_ids.insert(fi_id);
            }
        }

        if (fdl.contains("default_framing_intent") && !fdl["default_framing_intent"].is_null()) {
            auto default_fi = fdl["default_framing_intent"].as<std::string>();
            if (!fi_ids.count(default_fi)) {
                throw std::runtime_error("Default Framing Intent " + default_fi + ": Not in framing_intents");
            }
        }

        std::set<std::string> cv_ids;
        std::set<std::string> cv_source_canvas_ids;
        std::set<std::string> fd_ids;

        if (fdl.contains("contexts") && fdl["contexts"].is_array()) {
            for (const auto& cx : fdl["contexts"].array_range()) {
                auto cx_label = get_str(cx, "label", "(no label)");

                if (cx.contains("canvases") && cx["canvases"].is_array()) {
                    for (const auto& cv : cx["canvases"].array_range()) {
                        auto cv_id = cv["id"].as<std::string>();
                        auto cv_label = get_str(cv, "label", "(no label)");
                        auto cv_source_canvas_id = cv["source_canvas_id"].as<std::string>();
                        cv_source_canvas_ids.insert(cv_source_canvas_id);

                        if (cv_ids.count(cv_id)) {
                            throw std::runtime_error(
                                "Context (" + cx_label + ") > Canvas " + cv_id + " (" + cv_label + "): ID duplicated");
                        }
                        cv_ids.insert(cv_id);

                        if (cv.contains("framing_decisions") && cv["framing_decisions"].is_array()) {
                            for (const auto& fd : cv["framing_decisions"].array_range()) {
                                auto fd_id = fd["id"].as<std::string>();

                                if (fd_ids.count(fd_id)) {
                                    throw std::runtime_error(
                                        "Context (" + cx_label + ") > Canvas " + cv_id + " (" + cv_label +
                                        ") > Framing Decision " + fd_id + ": ID duplicated");
                                }
                                fd_ids.insert(fd_id);

                                auto fd_fi_id = fd["framing_intent_id"].as<std::string>();
                                if (!fi_ids.count(fd_fi_id)) {
                                    throw std::runtime_error(
                                        "Context (" + cx_label + ") > Canvas " + cv_id + " (" + cv_label +
                                        ") > Framing Decision " + fd_id + ": Framing Intent ID " + fd_fi_id +
                                        " not in framing_intents");
                                }

                                auto expected_fd_id = cv_id + "-" + fd_fi_id;
                                if (fd_id != expected_fd_id) {
                                    throw std::runtime_error(
                                        "Context (" + cx_label + ") > Canvas " + cv_id + " (" + cv_label +
                                        ") > Framing Decision " + fd_id + ": ID doesn't match expected " +
                                        expected_fd_id);
                                }
                            }
                        }
                    }
                }
            }
        }

        // Check unrecognized source canvas IDs
        std::vector<std::string> unrecognised;
        for (auto& id : cv_source_canvas_ids) {
            if (!cv_ids.count(id)) {
                unrecognised.push_back(id);
            }
        }
        if (!unrecognised.empty()) {
            std::ostringstream oss;
            oss << "Source Canvas IDs [";
            for (size_t i = 0; i < unrecognised.size(); ++i) {
                if (i > 0) {
                    oss << ", ";
                }
                oss << "'" << unrecognised[i] << "'";
            }
            oss << "] not in canvases";
            throw std::runtime_error(oss.str());
        }

        // Canvas template IDs
        std::set<std::string> ct_ids;
        if (fdl.contains("canvas_templates") && fdl["canvas_templates"].is_array()) {
            for (const auto& ct : fdl["canvas_templates"].array_range()) {
                auto ct_id = ct["id"].as<std::string>();
                auto ct_label = get_str(ct, "label", "(no label)");
                if (ct_ids.count(ct_id)) {
                    throw std::runtime_error("Canvas Template " + ct_id + " (" + ct_label + "): ID duplicated");
                }
                ct_ids.insert(ct_id);
            }
        }

    } catch (const std::runtime_error& e) {
        errors.push_back(std::string("ID Tree Error: ") + e.what());
    }
}

// ---------------------------------------------------------------------------
// Validator 2: Dimension Hierarchy
// ---------------------------------------------------------------------------

/** @brief Geometry path for canvas dimensions. */
static const std::string PATH_CANVAS_DIMENSIONS = "canvas.dimensions";
/** @brief Geometry path for canvas effective dimensions. */
static const std::string PATH_CANVAS_EFFECTIVE = "canvas.effective_dimensions";
/** @brief Geometry path for framing-decision protection dimensions. */
static const std::string PATH_FRAMING_PROTECTION = "framing_decision.protection_dimensions";
/** @brief Geometry path for framing-decision dimensions. */
static const std::string PATH_FRAMING_DIMENSIONS = "framing_decision.dimensions";

/** @brief Ordered hierarchy of geometry paths (outermost first). */
static const std::vector<std::string> PATH_HIERARCHY = {
    PATH_CANVAS_DIMENSIONS,
    PATH_CANVAS_EFFECTIVE,
    PATH_FRAMING_PROTECTION,
    PATH_FRAMING_DIMENSIONS,
};

/** @brief Dimension + anchor pair for hierarchy validation. */
struct DimAnchor {
    std::string path;    /**< Geometry path that produced this entry. */
    double dim_w;        /**< Width in pixels. */
    double dim_h;        /**< Height in pixels. */
    double anchor_x;     /**< Anchor X offset. */
    double anchor_y;     /**< Anchor Y offset. */
    ojson dim_w_json;    /**< Original JSON width for formatting. */
    ojson dim_h_json;    /**< Original JSON height for formatting. */
    ojson anchor_x_json; /**< Original JSON anchor X for formatting. */
    ojson anchor_y_json; /**< Original JSON anchor Y for formatting. */
};

/**
 * @brief Return a JSON value by key, or JSON 0 if absent.
 * @param obj  JSON object to query.
 * @param key  Key to look up.
 * @return The value at @p key, or ojson(0).
 */
static ojson get_json_val(const ojson& obj, const std::string& key) {
    if (obj.contains(key)) {
        return obj[key];
    }
    return ojson(0);
}

/**
 * @brief Populate width/height fields of a DimAnchor from a dimensions object.
 * @param out   DimAnchor to fill.
 * @param dims  JSON object containing "width" and "height".
 */
static void fill_dims(DimAnchor& out, const ojson& dims) {
    out.dim_w = get_num(dims, "width");
    out.dim_h = get_num(dims, "height");
    out.dim_w_json = get_json_val(dims, "width");
    out.dim_h_json = get_json_val(dims, "height");
}

/**
 * @brief Populate anchor fields of a DimAnchor from an anchor-point object.
 * @param out     DimAnchor to fill.
 * @param anchor  JSON object containing "x" and "y".
 */
static void fill_anchor(DimAnchor& out, const ojson& anchor) {
    out.anchor_x = get_num(anchor, "x");
    out.anchor_y = get_num(anchor, "y");
    out.anchor_x_json = get_json_val(anchor, "x");
    out.anchor_y_json = get_json_val(anchor, "y");
}

/**
 * @brief Set the anchor fields of a DimAnchor to zero.
 * @param out  DimAnchor whose anchor fields are zeroed.
 */
static void fill_zero_anchor(DimAnchor& out) {
    out.anchor_x = 0;
    out.anchor_y = 0;
    out.anchor_x_json = ojson(0);
    out.anchor_y_json = ojson(0);
}

/**
 * @brief Extract dimensions and anchor for a given geometry path.
 * @param path    One of the PATH_* constants identifying the geometry level.
 * @param canvas  Canvas JSON object.
 * @param fd      Framing-decision JSON object.
 * @param out     DimAnchor populated on success.
 * @return True if the requested level is present in the document.
 */
static bool get_dims_and_anchor(const std::string& path, const ojson& canvas, const ojson& fd, DimAnchor& out) {
    out.path = path;
    if (path == PATH_CANVAS_DIMENSIONS) {
        if (!canvas.contains("dimensions")) {
            return false;
        }
        fill_dims(out, canvas["dimensions"]);
        fill_zero_anchor(out);
        return true;
    }
    if (path == PATH_CANVAS_EFFECTIVE) {
        if (!canvas.contains("effective_dimensions") || canvas["effective_dimensions"].is_null()) {
            return false;
        }
        fill_dims(out, canvas["effective_dimensions"]);
        if (canvas.contains("effective_anchor_point") && !canvas["effective_anchor_point"].is_null()) {
            fill_anchor(out, canvas["effective_anchor_point"]);
        } else {
            fill_zero_anchor(out);
        }
        return true;
    }
    if (path == PATH_FRAMING_PROTECTION) {
        if (!fd.contains("protection_dimensions") || fd["protection_dimensions"].is_null()) {
            return false;
        }
        fill_dims(out, fd["protection_dimensions"]);
        if (fd.contains("protection_anchor_point") && !fd["protection_anchor_point"].is_null()) {
            fill_anchor(out, fd["protection_anchor_point"]);
        } else {
            fill_zero_anchor(out);
        }
        return true;
    }
    if (path == PATH_FRAMING_DIMENSIONS) {
        if (!fd.contains("dimensions") || fd["dimensions"].is_null()) {
            return false;
        }
        fill_dims(out, fd["dimensions"]);
        if (fd.contains("anchor_point") && !fd["anchor_point"].is_null()) {
            fill_anchor(out, fd["anchor_point"]);
        } else {
            fill_zero_anchor(out);
        }
        return true;
    }
    return false;
}

/**
 * @brief Format a geometry path for human-readable error messages.
 * @param path  Dot-and-underscore-delimited path string.
 * @return Path with underscores and dots replaced by spaces.
 */
static std::string format_path(const std::string& path) {
    std::string result = path;
    std::replace(result.begin(), result.end(), '_', ' ');
    std::replace(result.begin(), result.end(), '.', ' ');
    return result;
}

/**
 * @brief Format a JSON number to match Python str() output.
 * @param v  JSON value (integer or float).
 * @return String representation (e.g. "4096" or "5000.0").
 */
static std::string format_json_num(const ojson& v) {
    if (v.is_int64() || v.is_uint64()) {
        return std::to_string(v.as<int64_t>());
    }
    // Float: use Python-like representation
    std::ostringstream oss;
    double d = v.as<double>();
    // Python prints floats like 5000.0, -10.0, 223.0 etc.
    // Use default stream formatting which includes .0 for whole numbers
    oss << std::defaultfloat << d;
    std::string s = oss.str();
    // Ensure there's a decimal point (matching Python float repr)
    if (s.find('.') == std::string::npos && s.find('e') == std::string::npos) {
        s += ".0";
    }
    return s;
}

/**
 * @brief Validate that dimensions and anchors respect the outer-to-inner hierarchy.
 * @param fdl     Parsed FDL document.
 * @param errors  Output vector that receives any hierarchy error strings.
 */
static void validate_dimension_hierarchy(const ojson& fdl, std::vector<std::string>& errors) {
    try {
        if (!fdl.contains("contexts") || !fdl["contexts"].is_array()) {
            return;
        }

        for (const auto& ctx : fdl["contexts"].array_range()) {
            auto ctx_label = get_str(ctx, "label", "(no label)");

            if (!ctx.contains("canvases") || !ctx["canvases"].is_array()) {
                continue;
            }
            for (const auto& canvas : ctx["canvases"].array_range()) {
                auto cv_id = get_str(canvas, "id", "(no id)");
                auto cv_label = get_str(canvas, "label", "(no label)");
                auto location = "Context (" + ctx_label + ") > Canvas " + cv_id + " (" + cv_label + ")";

                if (!canvas.contains("framing_decisions") || !canvas["framing_decisions"].is_array()) {
                    continue;
                }
                for (const auto& fd : canvas["framing_decisions"].array_range()) {
                    auto fd_id = get_str(fd, "id", "(no id)");
                    auto fd_location = location + " > FD " + fd_id;

                    // Collect present levels
                    std::vector<DimAnchor> levels;
                    for (auto& path : PATH_HIERARCHY) {
                        DimAnchor da;
                        if (get_dims_and_anchor(path, canvas, fd, da)) {
                            levels.push_back(da);
                        }
                    }

                    // Check adjacent pairs
                    for (size_t i = 0; i + kAdjacentPairStep < levels.size(); ++i) {
                        auto& outer = levels[i];
                        auto& inner = levels[i + kAdjacentPairStep];

                        auto outer_name = format_path(outer.path);
                        auto inner_name = format_path(inner.path);

                        // Dimensions: outer >= inner
                        if (inner.dim_w > outer.dim_w || inner.dim_h > outer.dim_h) {
                            throw std::runtime_error(
                                fd_location + ": " + inner_name + " (" + format_json_num(inner.dim_w_json) + "x" +
                                format_json_num(inner.dim_h_json) + ") exceeds " + outer_name + " (" +
                                format_json_num(outer.dim_w_json) + "x" + format_json_num(outer.dim_h_json) + "). " +
                                "Required hierarchy: " + outer_name + " >= " + inner_name);
                        }

                        // Anchors: outer <= inner
                        if (inner.anchor_x < outer.anchor_x || inner.anchor_y < outer.anchor_y) {
                            throw std::runtime_error(
                                fd_location + ": " + inner_name + " anchor (" + format_json_num(inner.anchor_x_json) +
                                ", " + format_json_num(inner.anchor_y_json) + ") is outside " + outer_name +
                                " anchor (" + format_json_num(outer.anchor_x_json) + ", " +
                                format_json_num(outer.anchor_y_json) + "). " + "Required hierarchy: " + outer_name +
                                " anchor <= " + inner_name + " anchor");
                        }
                    }
                }
            }
        }
    } catch (const std::runtime_error& e) {
        errors.push_back(std::string("Dimension Hierarchy Error: ") + e.what());
    }
}

// ---------------------------------------------------------------------------
// Validator 3: Non-Negative Anchors
// ---------------------------------------------------------------------------

/**
 * @brief Validate that all anchor coordinates are non-negative.
 * @param fdl     Parsed FDL document.
 * @param errors  Output vector that receives any negative-anchor error strings.
 */
static void validate_non_negative_anchors(const ojson& fdl, std::vector<std::string>& errors) {
    try {
        if (!fdl.contains("contexts") || !fdl["contexts"].is_array()) {
            return;
        }

        for (const auto& ctx : fdl["contexts"].array_range()) {
            auto ctx_label = get_str(ctx, "label", "(no label)");

            if (!ctx.contains("canvases") || !ctx["canvases"].is_array()) {
                continue;
            }
            for (const auto& canvas : ctx["canvases"].array_range()) {
                auto cv_id = get_str(canvas, "id", "(no id)");
                auto cv_label = get_str(canvas, "label", "(no label)");
                auto location = "Context (" + ctx_label + ") > Canvas " + cv_id + " (" + cv_label + ")";

                // Check effective_anchor_point
                if (canvas.contains("effective_anchor_point") && !canvas["effective_anchor_point"].is_null()) {
                    const auto& eff = canvas["effective_anchor_point"];
                    double x = get_num(eff, "x");
                    double y = get_num(eff, "y");
                    if (x < 0 || y < 0) {
                        throw std::runtime_error(
                            location + ": effective_anchor_point (" + format_json_num(get_json_val(eff, "x")) + ", " +
                            format_json_num(get_json_val(eff, "y")) +
                            ") has negative values. Anchor coordinates must be >= 0.");
                    }
                }

                if (!canvas.contains("framing_decisions") || !canvas["framing_decisions"].is_array()) {
                    continue;
                }
                for (const auto& fd : canvas["framing_decisions"].array_range()) {
                    auto fd_id = get_str(fd, "id", "(no id)");
                    auto fd_location = location + " > FD " + fd_id;

                    // Check anchor_point
                    if (fd.contains("anchor_point") && !fd["anchor_point"].is_null()) {
                        const auto& anchor = fd["anchor_point"];
                        double x = get_num(anchor, "x");
                        double y = get_num(anchor, "y");
                        if (x < 0 || y < 0) {
                            throw std::runtime_error(
                                fd_location + ": anchor_point (" + format_json_num(get_json_val(anchor, "x")) + ", " +
                                format_json_num(get_json_val(anchor, "y")) +
                                ") has negative values. Anchor coordinates must be >= 0.");
                        }
                    }

                    // Check protection_anchor_point
                    if (fd.contains("protection_anchor_point") && !fd["protection_anchor_point"].is_null()) {
                        const auto& prot = fd["protection_anchor_point"];
                        double x = get_num(prot, "x");
                        double y = get_num(prot, "y");
                        if (x < 0 || y < 0) {
                            throw std::runtime_error(
                                fd_location + ": protection_anchor_point (" + format_json_num(get_json_val(prot, "x")) +
                                ", " + format_json_num(get_json_val(prot, "y")) +
                                ") has negative values. Anchor coordinates must be >= 0.");
                        }
                    }
                }
            }
        }
    } catch (const std::runtime_error& e) {
        errors.push_back(std::string("Non-Negative Anchors Error: ") + e.what());
    }
}

// ---------------------------------------------------------------------------
// Validator 4: Anchors Within Canvas
// ---------------------------------------------------------------------------

/**
 * @brief Validate that all anchor points fall within canvas dimensions.
 * @param fdl     Parsed FDL document.
 * @param errors  Output vector that receives any out-of-bounds error strings.
 */
static void validate_anchors_within_canvas(const ojson& fdl, std::vector<std::string>& errors) {
    try {
        if (!fdl.contains("contexts") || !fdl["contexts"].is_array()) {
            return;
        }

        for (const auto& ctx : fdl["contexts"].array_range()) {
            auto ctx_label = get_str(ctx, "label", "(no label)");

            if (!ctx.contains("canvases") || !ctx["canvases"].is_array()) {
                continue;
            }
            for (const auto& canvas : ctx["canvases"].array_range()) {
                auto cv_id = get_str(canvas, "id", "(no id)");
                auto cv_label = get_str(canvas, "label", "(no label)");
                auto location = "Context (" + ctx_label + ") > Canvas " + cv_id + " (" + cv_label + ")";

                const auto& canvas_dims = canvas["dimensions"];
                double canvas_width = get_num(canvas_dims, "width");
                double canvas_height = get_num(canvas_dims, "height");
                auto cw_json = get_json_val(canvas_dims, "width");
                auto ch_json = get_json_val(canvas_dims, "height");

                // Check effective_anchor_point
                if (canvas.contains("effective_anchor_point") && !canvas["effective_anchor_point"].is_null()) {
                    const auto& eff = canvas["effective_anchor_point"];
                    double x = get_num(eff, "x");
                    double y = get_num(eff, "y");
                    if (x > canvas_width || y > canvas_height) {
                        throw std::runtime_error(
                            location + ": effective_anchor_point (" + format_json_num(get_json_val(eff, "x")) + ", " +
                            format_json_num(get_json_val(eff, "y")) + ") exceeds canvas dimensions (" +
                            format_json_num(cw_json) + "x" + format_json_num(ch_json) +
                            "). Anchor coordinates must be within canvas bounds.");
                    }
                }

                if (!canvas.contains("framing_decisions") || !canvas["framing_decisions"].is_array()) {
                    continue;
                }
                for (const auto& fd : canvas["framing_decisions"].array_range()) {
                    auto fd_id = get_str(fd, "id", "(no id)");
                    auto fd_location = location + " > FD " + fd_id;

                    // Check anchor_point
                    if (fd.contains("anchor_point") && !fd["anchor_point"].is_null()) {
                        const auto& anchor = fd["anchor_point"];
                        double x = get_num(anchor, "x");
                        double y = get_num(anchor, "y");
                        if (x > canvas_width || y > canvas_height) {
                            throw std::runtime_error(
                                fd_location + ": anchor_point (" + format_json_num(get_json_val(anchor, "x")) + ", " +
                                format_json_num(get_json_val(anchor, "y")) + ") exceeds canvas dimensions (" +
                                format_json_num(cw_json) + "x" + format_json_num(ch_json) +
                                "). Anchor coordinates must be within canvas bounds.");
                        }
                    }

                    // Check protection_anchor_point
                    if (fd.contains("protection_anchor_point") && !fd["protection_anchor_point"].is_null()) {
                        const auto& prot = fd["protection_anchor_point"];
                        double x = get_num(prot, "x");
                        double y = get_num(prot, "y");
                        if (x > canvas_width || y > canvas_height) {
                            throw std::runtime_error(
                                fd_location + ": protection_anchor_point (" + format_json_num(get_json_val(prot, "x")) +
                                ", " + format_json_num(get_json_val(prot, "y")) + ") exceeds canvas dimensions (" +
                                format_json_num(cw_json) + "x" + format_json_num(ch_json) +
                                "). Anchor coordinates must be within canvas bounds.");
                        }
                    }
                }
            }
        }
    } catch (const std::runtime_error& e) {
        errors.push_back(std::string("Anchors Within Canvas Error: ") + e.what());
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

ValidationResult validate(const ojson& fdl) {
    ValidationResult result;

    // Schema validation first — catches structural/type issues
    validate_schema(fdl, result.errors);

    // Only run semantic validators if schema passes, since semantic
    // validators assume the document is structurally valid
    if (result.errors.empty()) {
        validate_id_tree(fdl, result.errors);
        validate_dimension_hierarchy(fdl, result.errors);
        validate_non_negative_anchors(fdl, result.errors);
        validate_anchors_within_canvas(fdl, result.errors);
    }

    return result;
}

} // namespace fdl::detail
