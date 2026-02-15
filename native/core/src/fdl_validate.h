// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#ifndef FDL_VALIDATE_INTERNAL_H
#define FDL_VALIDATE_INTERNAL_H

#include <jsoncons/json.hpp>
#include <string>
#include <vector>

namespace fdl::detail {

using ojson = jsoncons::ojson;

struct ValidationResult {
    std::vector<std::string> errors;
};

// Run schema validation followed by semantic validators.
// Schema errors are prefixed with "Schema Error:".
// Semantic errors are only run if schema validation passes.
ValidationResult validate(const ojson& fdl);

} // namespace fdl::detail

#endif // FDL_VALIDATE_INTERNAL_H
