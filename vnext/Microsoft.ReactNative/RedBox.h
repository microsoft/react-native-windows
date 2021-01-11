// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include <DevSettings.h>
#include "IRedBoxHandler.h"
#include "ReactHost/React.h"

namespace Mso::React {

std::shared_ptr<IRedBoxHandler> CreateRedBoxHandler(
<<<<<<< HEAD
    winrt::Microsoft::ReactNative::IRedBoxHandler const &redBoxHandler) noexcept;

std::shared_ptr<IRedBoxHandler> CreateDefaultRedBoxHandler(
    Mso::WeakPtr<IReactHost> &&weakReactHost,
    Mso::DispatchQueue &&uiQueue) noexcept;
||||||| 811c767bf
    Mso::WeakPtr<IReactHost> &&weakReactHost,
    std::shared_ptr<facebook::react::MessageQueueThread> &&uiMessageQueue) noexcept;
=======
    winrt::Microsoft::ReactNative::IRedBoxHandler const &redBoxHandler) noexcept;
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa

#ifndef CORE_ABI
std::shared_ptr<IRedBoxHandler> CreateDefaultRedBoxHandler(
    Mso::WeakPtr<IReactHost> &&weakReactHost,
    Mso::DispatchQueue &&uiQueue) noexcept;
#endif
} // namespace Mso::React
