// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AbiEventEmitter.h"
#include "JsiWriter.h"

namespace winrt::Microsoft::ReactNative::implementation {

EventEmitter::EventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) : m_eventEmitter(eventEmitter) {}

void EventEmitter::DispatchEvent(
    winrt::hstring eventName,
    const winrt::Microsoft::ReactNative::JSValueArgWriter &args) {
  m_eventEmitter->dispatchEvent(winrt::to_string(eventName), [args](facebook::jsi::Runtime &runtime) {
    auto writer = winrt::make<JsiWriter>(runtime);
    args(writer);
    return winrt::get_self<JsiWriter>(writer)->MoveResult();
  });
}

} // namespace winrt::Microsoft::ReactNative::implementation
