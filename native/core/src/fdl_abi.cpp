// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#include "fdl/fdl_core.h"

fdl_abi_version_t fdl_abi_version(void) {
    return {FDL_ABI_VERSION_MAJOR, FDL_ABI_VERSION_MINOR, FDL_ABI_VERSION_PATCH};
}
