// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <InstanceManager.h>

#include <DevSettings.h>
#include <Folly/dynamic.h>
#include "XamlView.h"

#include <functional>
#include <string>

namespace Microsoft::ReactNative {
struct INativeUIManager;
} // namespace Microsoft::ReactNative

namespace react::uwp {

struct IXamlRootView;
class ExpressionAnimationStore;

typedef unsigned int LiveReloadCallbackCookie;
typedef unsigned int ErrorCallbackCookie;
typedef unsigned int DebuggerAttachCallbackCookie;

} // namespace react::uwp
