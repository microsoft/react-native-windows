// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/AppThemeModuleUwp.h>
#include <Modules/AppearanceModule.h>
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
    const std::shared_ptr<facebook::react::IUIManager> &uiManager,
    const std::shared_ptr<facebook::react::MessageQueueThread> &messageQueue,
    const std::shared_ptr<facebook::react::MessageQueueThread> &uiMessageQueue,
    std::shared_ptr<react::uwp::AppTheme> &&appTheme,
    Mso::CntPtr<AppearanceChangeListener> &&appearanceListener,
    const std::shared_ptr<IReactInstance> &uwpInstance) noexcept;

} // namespace react::uwp
