// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include <DevSettings.h>
#include "IRedBoxHandler.h"
#include "ReactHost/React.h"

namespace Mso::React {

std::shared_ptr<IRedBoxHandler> CreateRedBoxHandler(
    winrt::Microsoft::ReactNative::IRedBoxHandler const &redBoxHandler) noexcept;

#ifndef CORE_ABI
std::shared_ptr<IRedBoxHandler> CreateDefaultRedBoxHandler(
    Mso::WeakPtr<IReactHost> &&weakReactHost,
    Mso::DispatchQueue &&uiQueue) noexcept;
#endif
} // namespace Mso::React
