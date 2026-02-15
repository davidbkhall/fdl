// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#ifndef FDL_COMPAT_H
#define FDL_COMPAT_H

#include <cstring>

#ifdef _MSC_VER
#define fdl_strdup _strdup
#else
#define fdl_strdup strdup
#endif

#endif // FDL_COMPAT_H
