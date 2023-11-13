// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "JSExecutorFactoryDelegate.h"
#include "ReactPropertyBag.h"

namespace facebook::react {
class JSExecutorFactory;
}

namespace Microsoft::JSI::JSExecutorFactorySettings {

void SetJSExecutorFactoryDelegate(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    facebook::react::JSExecutorFactoryDelegate const &value) noexcept;
const facebook::react::JSExecutorFactoryDelegate GetJSExecutorFactoryDelegate(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

} // namespace Microsoft::JSI::JSExecutorFactorySettings
