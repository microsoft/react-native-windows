// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "JSValue.h"
#include "ReactHost/React.h"
#include "ReactPropertyBag.h"
#include "winrt/Microsoft.ReactNative.h"

#include <deque>
#include <mutex>

namespace winrt::Microsoft::ReactNative::implementation {
struct BatchedEvent {
  int64_t tag{};
  winrt::hstring eventName;
  JSValueObject eventObject;
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative {

//! Emits queued events from native to JS in queued batches (at most once per-native frame). Events within a batch may
//! be coalesced. The batch is finished at the time the JS thread starts to process it. I.e. it is possible for a batch
//! to last for multiple frames if the JS thread is blocked. This is by-design as it allows our coalescing strategy to
//! account for long operations on the JS thread.
struct BatchingEventEmitter : winrt::implements<BatchingEventEmitter, winrt::Windows::Foundation::IInspectable> {
 public:
  BatchingEventEmitter(Mso::CntPtr<const Mso::React::IReactContext> &&context) noexcept;

  //! Queues an event to be fired via RTCEventEmitter.receiveEvent().
  void EmitJSEvent(int64_t tag, winrt::hstring &&eventName, JSValueObject &&eventObject) noexcept;

  //! Queues an event to be fired via RTCEventEmitter.receiveEvent(). Existing events in the batch with the same name
  //! and tag will be removed.
  void EmitCoalescingJSEvent(int64_t tag, winrt::hstring &&eventName, JSValueObject &&eventObject) noexcept;

 private:
  void EnsureQueuePumping() noexcept;
  void OnRendering() noexcept;

  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  std::deque<implementation::BatchedEvent> m_eventQueue;
  std::mutex m_eventQueueMutex;
  xaml::Media::CompositionTarget::Rendering_revoker m_renderingRevoker;
};

} // namespace winrt::Microsoft::ReactNative
