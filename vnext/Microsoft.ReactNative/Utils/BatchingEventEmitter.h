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
  std::string eventEmitterName;
  std::string emitterMethod;
  std::string eventName;
  int64_t coalescingKey;
  folly::dynamic params;
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative {

//! Emits events from native to JS in queued batches (at most once per-native frame). Events within a batch may be
//! coalesced. The batch is finished at the time the JS thread starts to process it. I.e. it is possible for a batch to
//! last for multiple frames if the JS thread is blocked. This is by-design as it allows our coalescing strategy to
//! account for long operations on the JS thread.
struct BatchingEventEmitter : public std::enable_shared_from_this<BatchingEventEmitter> {
 public:
  BatchingEventEmitter(Mso::CntPtr<const Mso::React::IReactContext> &&context) noexcept;

  //! Dispatches an event from a view manager.
  void DispatchEvent(int64_t tag, const std::string &eventName, folly::dynamic &&eventData) noexcept;
  //! Queues an event to be fired.
  void
  EmitJSEvent(const std::string &eventEmitterName, const std::string &emitterMethod, folly::dynamic &&params) noexcept;

  //! Dispatches an event from a view manager. Existing events in the batch with the same name and tag will be removed.
  void DispatchCoalescingEvent(int64_t tag, const std::string &eventName, folly::dynamic &&eventData) noexcept;
  //! Queues an event to be fired. Existing events in the batch with the same name and tag will be removed.
  void EmitCoalescingJSEvent(
      const std::string &eventEmitterName,
      const std::string &emitterMethod,
      const std::string &eventName,
      int64_t coalescingKey,
      folly::dynamic &&params) noexcept;

 private:
  void RegisterFrameCallback() noexcept;
  void OnFrameUI() noexcept;
  void OnFrameJS() noexcept;

  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  std::deque<implementation::BatchedEvent> m_eventQueue;
  std::mutex m_eventQueueMutex;
  xaml::Media::CompositionTarget::Rendering_revoker m_renderingRevoker;
  IReactDispatcher m_uiDispatcher;
};

} // namespace winrt::Microsoft::ReactNative
