// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include <DevSettings.h>
#include <ReactPropertyBag.h>
#include "IReactDispatcher.h"
#include "IRedBoxHandler.h"
#include "ReactHost/React.h"

namespace Mso::React {

std::shared_ptr<IRedBoxHandler> CreateRedBoxHandler(
    winrt::Microsoft::ReactNative::IRedBoxHandler const &redBoxHandler) noexcept;

std::shared_ptr<IRedBoxHandler> CreateDefaultRedBoxHandler(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &propBag,
    Mso::WeakPtr<IReactHost> &&weakReactHost,
    const Mso::React::IDispatchQueue2 &uiQueue) noexcept;
} // namespace Mso::React
