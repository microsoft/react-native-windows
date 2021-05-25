// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "BatchingEventEmitter.h"

namespace winrt::Microsoft::ReactNative {

BatchingEventEmitter::BatchingEventEmitter(Mso::CntPtr<const Mso::React::IReactContext> &&context) noexcept
    : m_context(std::move(context)) {
  m_uiDispatcher = m_context->Properties().Get(ReactDispatcherHelper::UIDispatcherProperty()).as<IReactDispatcher>();
}

void BatchingEventEmitter::DispatchEvent(
    int64_t tag,
    const std::string &eventName,
    folly::dynamic &&eventData) noexcept {
  folly::dynamic params = folly::dynamic::array(tag, eventName, std::move(eventData));
  return EmitJSEvent("RCTEventEmitter", "receiveEvent", std::move(params));
}

void BatchingEventEmitter::EmitJSEvent(
    const std::string &eventEmitterName,
    const std::string &emitterMethod,
    folly::dynamic &&params) noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());

  implementation::BatchedEvent newEvent{eventEmitterName, emitterMethod, "", 0, std::move(params)};
  bool isFirstEventInBatch = false;

  {
    std::scoped_lock lock(m_eventQueueMutex);

    isFirstEventInBatch = m_eventQueue.size() == 0;
    m_eventQueue.push_back(std::move(newEvent));
  }

  if (isFirstEventInBatch) {
    RegisterFrameCallback();
  }
} // namespace winrt::Microsoft::ReactNative

void BatchingEventEmitter::DispatchCoalescingEvent(
    int64_t tag,
    const std::string &eventName,
    folly::dynamic &&eventData) noexcept {
  folly::dynamic params = folly::dynamic::array(tag, eventName, std::move(eventData));
  EmitCoalescingJSEvent("RCTEventEmitter", "receiveEvent", eventName, tag, std::move(params));
}

void BatchingEventEmitter::EmitCoalescingJSEvent(
    const std::string &eventEmitterName,
    const std::string &emitterMethod,
    const std::string &eventName,
    int64_t coalescingKey,
    folly::dynamic &&params) noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());

  implementation::BatchedEvent newEvent{eventEmitterName, emitterMethod, eventName, coalescingKey, std::move(params)};
  bool isFirstEventInBatch = false;

  {
    std::scoped_lock lock(m_eventQueueMutex);

    isFirstEventInBatch = m_eventQueue.size() == 0;

    auto endIter = std::remove_if(m_eventQueue.begin(), m_eventQueue.end(), [&](const auto &evt) noexcept {
      const auto shouldCoalesce = evt.eventEmitterName == eventEmitterName && evt.emitterMethod == emitterMethod &&
          evt.eventName == eventName && evt.coalescingKey == coalescingKey;
      return shouldCoalesce;
    });

    m_eventQueue.erase(endIter, m_eventQueue.end());
    m_eventQueue.push_back(std::move(newEvent));
  }

  if (isFirstEventInBatch) {
    RegisterFrameCallback();
  }
}

void BatchingEventEmitter::RegisterFrameCallback() noexcept {
  VerifyElseCrash(!m_renderingRevoker);

  m_renderingRevoker = xaml::Media::CompositionTarget::Rendering(
      winrt::auto_revoke, [weakThis{weak_from_this()}](auto const &, auto const &) {
        if (auto strongThis = weakThis.lock()) {
          strongThis->OnFrameUI();
        }
      });
}

void BatchingEventEmitter::OnFrameUI() noexcept {
  auto jsDispatcher = m_context->Properties().Get(ReactDispatcherHelper::JSDispatcherProperty()).as<IReactDispatcher>();

  jsDispatcher.Post([weakThis{weak_from_this()}]() noexcept {
    if (auto strongThis = weakThis.lock()) {
      strongThis->OnFrameJS();
    }
  });

  // Don't leave the callback continuously registered as it can waste power.
  // See https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.media.compositiontarget.rendering?view=winrt-19041
  m_renderingRevoker.revoke();
}

void BatchingEventEmitter::OnFrameJS() noexcept {
  std::deque<implementation::BatchedEvent> currentBatch;

  {
    std::scoped_lock lock(m_eventQueueMutex);
    currentBatch.swap(m_eventQueue);
  }

  while (!currentBatch.empty()) {
    auto &evt = currentBatch.front();
    m_context->CallJSFunction(std::move(evt.eventEmitterName), std::move(evt.emitterMethod), std::move(evt.params));
    currentBatch.pop_front();
  }
}

} // namespace winrt::Microsoft::ReactNative
