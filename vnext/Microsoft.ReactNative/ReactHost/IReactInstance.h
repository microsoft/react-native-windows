// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <InstanceManager.h>

#include <DevSettings.h>
#include <folly/dynamic.h>

#include <functional>
#include <string>

namespace Microsoft::ReactNative {

struct INativeUIManager;
class ExpressionAnimationStore;

typedef unsigned int LiveReloadCallbackCookie;
typedef unsigned int ErrorCallbackCookie;

} // namespace Microsoft::ReactNative
