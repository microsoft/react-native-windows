// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOGDI
#define NOGDI
#endif

#include <hstring.h>
#include <oaidl.h>
#include <werapi.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include "HResult.h"

#include <cxxreact/CxxModule.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <fbsystrace.h>
#include <folly/Memory.h>
#include <folly/Optional.h>
#include <folly/dynamic.h>
#include <folly/json.h>

#include <condition_variable>
#include <functional>
#include <memory>
#include <string>
