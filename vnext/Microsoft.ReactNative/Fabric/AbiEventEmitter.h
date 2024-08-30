// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewProps.g.h"

#include "EventEmitter.g.h"
#include <react/renderer/core/EventEmitter.h>
#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative::implementation {

struct EventEmitter : EventEmitterT<EventEmitter> {
  EventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter);

  void DispatchEvent(winrt::hstring eventName, const winrt::Microsoft::ReactNative::JSValueArgWriter &args);

 private:
  facebook::react::EventEmitter::Shared const m_eventEmitter;
};

} // namespace winrt::Microsoft::ReactNative::implementation
