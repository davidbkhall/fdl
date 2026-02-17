// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
/**
 * @file fdl_rounding.h
 * @brief Banker's rounding (half-to-even) implementation.
 *
 * Provides the core rounding primitive that matches Python's built-in round()
 * behavior, ensuring consistent results across the C++ and Python implementations.
 */
#ifndef FDL_ROUNDING_INTERNAL_H
#define FDL_ROUNDING_INTERNAL_H

#include <cmath>
#include <cstdint>

namespace fdl::detail {

/**
 * Banker's rounding (half-to-even), matching Python's built-in round().
 *
 * At exact halfway points (e.g. 0.5, 1.5), rounds to the nearest even integer.
 * This avoids systematic rounding bias.
 *
 * @param value  The floating-point value to round.
 * @return Rounded integer value.
 */
inline int64_t bankers_round(double value) {
    double rounded = std::round(value);
    // Check if we're exactly at the halfway point
    double remainder = value - std::floor(value);
    if (std::abs(remainder - 0.5) < 1e-15) {
        // Halfway case: round to even
        auto r = static_cast<int64_t>(rounded);
        if (r % 2 != 0) {
            // rounded is odd, go the other way
            rounded = std::floor(value + 0.5);
            if (static_cast<int64_t>(rounded) % 2 != 0) {
                rounded = std::ceil(value - 0.5);
            }
        }
    }
    return static_cast<int64_t>(rounded);
}

} // namespace fdl::detail

#endif // FDL_ROUNDING_INTERNAL_H
