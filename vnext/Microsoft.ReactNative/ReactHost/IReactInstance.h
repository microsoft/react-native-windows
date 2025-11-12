// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <InstanceManager.h>

#include <DevSettings.h>
#include <folly/dynamic.h>

#if !defined(CORE_ABI) && !defined(USE_FABRIC)
#include "XamlView.h"
#endif

#include <functional>
#include <string>

namespace Microsoft::ReactNative {

struct INativeUIManager;
class ExpressionAnimationStore;
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
struct IXamlRootView;
#endif

typedef unsigned int LiveReloadCallbackCookie;
typedef unsigned int ErrorCallbackCookie;

} // namespace Microsoft::ReactNative
