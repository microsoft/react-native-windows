// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Logging.h>
#include <cxxreact/JSBigString.h>
#include <react/bridging/LongLivedObject.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "DevSettings.h"
#include "IDevSupportManager.h"
#include "IReactRootView.h"

namespace Microsoft::ReactNative {
const std::shared_ptr<facebook::react::IDevSupportManager> &GetSharedDevManager() noexcept;
}
