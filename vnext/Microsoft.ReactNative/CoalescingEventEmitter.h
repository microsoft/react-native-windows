// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CoalescingEventEmitter.g.h"
#include "JSValue.h"
#include "ReactHost/React.h"
#include "ReactPropertyBag.h"
#include "winrt/Microsoft.ReactNative.h"

#include <deque>
#include <mutex>

namespace winrt::Microsoft::ReactNative::implementation {

struct CoalescingEvent
{
  std::optional<int64_t> coalesceKey;
  bool expired{false};
  folly::dynamic args;
};

struct CoalescingEventEmitter : CoalescingEventEmitterT<CoalescingEventEmitter> {
 public:
  static ReactPropertyId<CoalescingEventEmitter> Property() noexcept;
  static winrt::Microsoft::ReactNative::CoalescingEventEmitter FromContext(IReactContext const & context) noexcept;
  static winrt::Microsoft::ReactNative::CoalescingEventEmitter FromContext(Mso::React::IReactContext const & context) noexcept;

  CoalescingEventEmitter(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;

  void EmitJSEvent(int64_t coalesceKey, JSValueArgWriter const & paramsArgWriter) noexcept;
  void EmitDurableJSEvent(JSValueArgWriter const & paramsArgWriter) noexcept;

 private:
  void EnsureRenderCallbackRegistered() noexcept;
  void OnRendering() noexcept;

  Mso::CntPtr<Mso::React::IReactContext> m_context;
  std::deque<CoalescingEvent> m_eventQueue;
  std::mutex m_mutex;
  xaml::Media::CompositionTarget::Rendering_revoker m_renderingRevoker;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct CoalescingEventEmitter : CoalescingEventEmitterT<CoalescingEventEmitter, implementation::CoalescingEventEmitter> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
