// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

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

namespace Microsoft::ReactNative {

struct DeviceInfo;
struct IReactInstance;
struct ViewManagerProvider;

std::vector<facebook::react::NativeModuleDescription> GetCoreModules(
    const std::shared_ptr<facebook::react::MessageQueueThread> &batchingUIMessageQueue,
    const std::shared_ptr<facebook::react::MessageQueueThread> &jsMessageQueue,
    Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;

} // namespace Microsoft::ReactNative
