// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/AppThemeModuleUwp.h>
#include <Modules/AppearanceModule.h>
#include <React.h>
#include <Shared/NativeModuleProvider.h>
#include <smartPtr/cntPtr.h>
#include <memory>
#include <vector>

namespace facebook::react {
class AppState;
struct DevSettings;
class IUIManager;
class MessageQueueThread;
} // namespace facebook::react

namespace react::uwp {

class DeviceInfo;
struct IReactInstance;
struct ViewManagerProvider;

std::vector<facebook::react::NativeModuleDescription> GetCoreModules(
    const std::shared_ptr<facebook::react::MessageQueueThread> &batchingUIMessageQueue,
    const std::shared_ptr<facebook::react::MessageQueueThread> &jsMessageQueue,
    std::shared_ptr<react::uwp::AppTheme> &&appTheme,
    Mso::CntPtr<AppearanceChangeListener> &&appearanceListener,
    Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;

} // namespace react::uwp
