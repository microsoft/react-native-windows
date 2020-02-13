// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Modules/AppThemeModule.h>
#include <ReactUWP/Modules/I18nModule.h>
#include <ReactWindowsCore/NativeModuleProvider.h>
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
    std::shared_ptr<facebook::react::IUIManager> uiManager,
    const std::shared_ptr<facebook::react::MessageQueueThread> &messageQueue,
    std::shared_ptr<DeviceInfo> deviceInfo,
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    const I18nModule::I18nInfo &&i18nInfo,
    std::shared_ptr<facebook::react::AppState> appstate,
    std::shared_ptr<react::windows::AppTheme> appTheme,
    const std::shared_ptr<IReactInstance>& uwpInstance) noexcept;

} // namespace react::uwp
