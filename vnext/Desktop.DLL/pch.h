// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// React.Cpp.props also passes /D_USE_MATH_DEFINES. Guard this so the PCH
// does not redefine the macro and fail with C4005/C2220.
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <Math.h>

#include <windows.h>

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif