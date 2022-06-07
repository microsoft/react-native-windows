// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "unknwn.h"

#include <windows.h>
#include <winrt/base.h>
#include <mutex>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

#include <Base/CxxReactIncludes.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif