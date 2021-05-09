// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef WINRT_LEAN_AND_MEAN
#define WINRT_LEAN_AND_MEAN
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

// Define environment variable RNW_FASTBUILD=true to improve compilation speed on your PC.
#ifdef RNW_FASTBUILD
#include <UI.Composition.h>
#include <UI.Xaml.Automation.Peers.h>
#include <UI.Xaml.Automation.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Media.h>
#include <UI.Xaml.Shapes.h>

#include <Utils/CppWinrtLessExceptions.h>

#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.ViewManagement.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.Headers.h>

#include "Base/CxxReactIncludes.h"

#include <boost/algorithm/string.hpp>
#endif // RNW_FASTBUILD

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
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <activeObject/activeObject.h>
#include <functional/functorRef.h>
#include <future/future.h>
