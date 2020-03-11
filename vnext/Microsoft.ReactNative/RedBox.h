// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once
#include <DevSettings.h>
#include "RedBoxHandler.h"
#include "ReactHost/React.h"

namespace Mso::React {

std::shared_ptr<IRedBoxHandler> CreateRedBoxHandler(
    Mso::WeakPtr<IReactHost> weakReactHost,
    std::shared_ptr<facebook::react::MessageQueueThread> uiMessageQueue) noexcept;

} // namespace Mso::React
