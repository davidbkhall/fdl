// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#include "fdl/fdl_core.h"
#include "fdl_template.h"

#include <cstdlib>

extern "C" {

fdl_template_result_t fdl_apply_canvas_template(
    const fdl_canvas_template_t* tmpl,
    const fdl_canvas_t* source_canvas,
    const fdl_framing_decision_t* source_framing,
    const char* new_canvas_id,
    const char* new_fd_name,
    const char* source_context_label,
    const char* context_creator) {
    return fdl::detail::apply_canvas_template(
        tmpl, source_canvas, source_framing,
        new_canvas_id, new_fd_name, source_context_label, context_creator);
}

void fdl_template_result_free(fdl_template_result_t* result) {
    if (!result) return;
    if (result->output_fdl) {
        fdl_doc_free(result->output_fdl);
        result->output_fdl = nullptr;
    }
    if (result->context_label) {
        free(const_cast<char*>(result->context_label));
        result->context_label = nullptr;
    }
    if (result->canvas_id) {
        free(const_cast<char*>(result->canvas_id));
        result->canvas_id = nullptr;
    }
    if (result->framing_decision_id) {
        free(const_cast<char*>(result->framing_decision_id));
        result->framing_decision_id = nullptr;
    }
    if (result->error) {
        free(const_cast<char*>(result->error));
        result->error = nullptr;
    }
}

} // extern "C"
