// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef WINRT_LEAN_AND_MEAN
#define WINRT_LEAN_AND_MEAN
#endif

#ifndef NOGDI
#define NOGDI
#endif

#undef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP

#include <combaseapi.h>
#include <guiddef.h>
#include <intrin.h>
#include <unknwn.h>
#include <windows.h>
// When WINAPI_FAMILY is DESKTOP_APP, windows.h creates a macro for GetCurrentTime, which conflicts with other headers
#undef GetCurrentTime

#include "CppWinRTIncludes.h"
#include "HResult.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <activeObject/activeObject.h>
#include <future/future.h>
