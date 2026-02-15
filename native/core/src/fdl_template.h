#ifndef FDL_TEMPLATE_INTERNAL_H
#define FDL_TEMPLATE_INTERNAL_H

#include "fdl/fdl_core.h"

namespace fdl::detail {

fdl_template_result_t apply_canvas_template(
    const fdl_canvas_template_t* tmpl,
    const fdl_canvas_t* source_canvas,
    const fdl_framing_decision_t* source_framing,
    const char* new_canvas_id,
    const char* new_fd_name,
    const char* source_context_label,
    const char* context_creator);

} // namespace fdl::detail

#endif // FDL_TEMPLATE_INTERNAL_H
