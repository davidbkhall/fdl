// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#ifndef FDL_CANONICAL_INTERNAL_H
#define FDL_CANONICAL_INTERNAL_H

#include <jsoncons/json.hpp>
#include <string>

namespace fdl::detail {

using ojson = jsoncons::ojson;

// Strip null values recursively; empty arrays stay as [].
ojson strip_nulls(const ojson& val);

// Reorder an object's keys according to the specified type's canonical order.
// type_hint: "root", "context", "canvas", "framing_decision",
//            "framing_intent", "canvas_template", "version",
//            "dimensions", "point", "round_strategy", "clip_id"
ojson reorder_object(const ojson& obj, const std::string& type_hint);

// Serialize an ojson node to canonical JSON: strip_nulls + reorder + format.
// Returns heap-allocated C string (caller owns, free with fdl_free / std::free).
// Returns nullptr if node is nullptr.
char* node_to_canonical_json(const ojson* node, const std::string& type_hint, int indent);

} // namespace fdl::detail

#endif // FDL_CANONICAL_INTERNAL_H
