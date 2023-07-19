// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Folly
#include <folly/dynamic.h>

// React Native
#include <cxxreact/Instance.h>

// React Native Windows
#include <ReactContext.h>

// Standard Library
#include <memory>
#include <string>

namespace Microsoft::React::Modules {

void SendEvent(
    std::weak_ptr<facebook::react::Instance> weakReactInstance,
    std::string &&eventName,
    folly::dynamic &&args);

void SendEvent(
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    std::wstring_view &&eventName,
    winrt::Microsoft::ReactNative::JSValueObject &&args) noexcept;

void SendEvent(
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    std::wstring_view &&eventName,
    winrt::Microsoft::ReactNative::JSValueArray &&args) noexcept;

winrt::Microsoft::ReactNative::JSValue ToJSValue(folly::dynamic &value) noexcept;

folly::dynamic ToDynamic(const winrt::Microsoft::ReactNative::JSValue &value) noexcept;

} // namespace Microsoft::React::Modules
