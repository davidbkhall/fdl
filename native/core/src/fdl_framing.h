// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#ifndef FDL_FRAMING_INTERNAL_H
#define FDL_FRAMING_INTERNAL_H

#include "fdl/fdl_core.h"

namespace fdl::detail {

fdl_from_intent_result_t compute_framing_from_intent(
    fdl_dimensions_f64_t canvas_dims,
    fdl_dimensions_f64_t working_dims,
    double squeeze,
    fdl_dimensions_i64_t aspect_ratio,
    double protection,
    fdl_round_strategy_t rounding);

} // namespace fdl::detail

#endif // FDL_FRAMING_INTERNAL_H
