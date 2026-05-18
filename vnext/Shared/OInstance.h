// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// public header

#pragma once

#include <ReactPropertyBag.h>
#include <TurboModuleManager.h>
#include "InstanceManager.h"

// React Native
#include <react/bridging/LongLivedObject.h>
#include <react/renderer/runtimescheduler/RuntimeScheduler.h>

// Standard Library
#include <memory>
#include <string>
#include <vector>

namespace facebook {
namespace react {

struct IDevSupportManager;

} // namespace react
} // namespace facebook

namespace Microsoft::ReactNative {

void LoadRemoteUrlScript(
    std::shared_ptr<facebook::react::DevSettings> devsettings,
    std::shared_ptr<facebook::react::IDevSupportManager> devManager,
    std::string &&jsBundleRelativePath,
    std::function<void(std::unique_ptr<const facebook::react::JSBigStdString> script, const std::string &sourceURL)>
        fnLoadScriptCallback) noexcept;

std::unique_ptr<const facebook::react::JSBigString> JsBigStringFromPath(
    std::shared_ptr<facebook::react::DevSettings> devsettings,
    const std::string &jsBundleRelativePath) noexcept;

} // namespace Microsoft::ReactNative
