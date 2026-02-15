#ifndef FDL_DOC_INTERNAL_H
#define FDL_DOC_INTERNAL_H

#include <jsoncons/json.hpp>
#include <mutex>
#include <string>

#include "fdl_handles.h"

namespace fdl::detail {

// Use ojson to preserve key insertion order for canonical serialization
using ojson = jsoncons::ojson;

class Document {
public:
    Document() = default;
    explicit Document(ojson data) : data_(std::move(data)) {}

    // Parse JSON string, throws jsoncons::ser_error on failure
    static Document parse(const char* json_str, size_t json_len);

    // Access the internal JSON data
    const ojson& data() const { return data_; }
    ojson& data() { return data_; }

    // Header accessors (return empty string if missing)
    std::string get_uuid() const;
    std::string get_fdl_creator() const;

    // Canonical JSON serialization
    // Reorders keys per FDL type-specific tables, strips nulls
    std::string to_canonical_json(int indent = 2) const;

private:
    ojson data_;
};

} // namespace fdl::detail

// Opaque handle definition — shared across ABI translation units
struct fdl_doc {
    fdl::detail::Document doc;
    fdl_handle_cache handles;
    mutable std::mutex mtx;
};

// RAII lock helper for per-document synchronization.
// Acquires the document's mutex on construction, releases on destruction.
// Safe with const handles because the mutex is mutable.
struct doc_lock {
    std::unique_lock<std::mutex> lock_;
    explicit doc_lock(const fdl_doc* doc) {
        if (doc) lock_ = std::unique_lock<std::mutex>(doc->mtx);
    }
};

#endif // FDL_DOC_INTERNAL_H
