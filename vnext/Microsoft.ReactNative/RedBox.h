// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once
#include <DevSettings.h>
#include "ReactHost/JSExceptionInfo.h"

namespace Mso::React {

std::function<void(facebook::react::JSExceptionInfo &&)> CreateRedBoxExceptionCallback(
    std::shared_ptr<facebook::react::MessageQueueThread> uiMessageQueue,
    std::function<void()> &&reloadCallback) noexcept;

} // namespace Mso::React
