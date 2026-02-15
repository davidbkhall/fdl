#include "fdl_doc.h"

#include "fdl_canonical.h"

#include <string_view>

namespace fdl::detail {

Document Document::parse(const char* json_str, size_t json_len) {
    auto data = ojson::parse(std::string_view(json_str, json_len));
    return Document(std::move(data));
}

std::string Document::get_uuid() const {
    if (data_.contains("uuid") && data_["uuid"].is_string()) {
        return data_["uuid"].as<std::string>();
    }
    return {};
}

std::string Document::get_fdl_creator() const {
    if (data_.contains("fdl_creator") && data_["fdl_creator"].is_string()) {
        return data_["fdl_creator"].as<std::string>();
    }
    return {};
}

std::string Document::to_canonical_json(int indent) const {
    ojson cleaned = strip_nulls(data_);
    ojson ordered = reorder_object(cleaned, "root");
    std::string buffer;
    if (indent > 0) {
        jsoncons::json_options options;
        options.indent_size(indent);
        ordered.dump_pretty(buffer, options);
    } else {
        ordered.dump(buffer);
    }
    return buffer;
}

} // namespace fdl::detail
