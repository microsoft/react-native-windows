// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "BatchingEventEmitter.h"
#include "DynamicWriter.h"
#include "JSValueWriter.h"

namespace winrt::Microsoft::ReactNative {

BatchingEventEmitter::BatchingEventEmitter(Mso::CntPtr<const Mso::React::IReactContext> &&context) noexcept
    : m_context(std::move(context)) {
  m_uiDispatcher = m_context->Properties().Get(ReactDispatcherHelper::UIDispatcherProperty()).as<IReactDispatcher>();
}

void BatchingEventEmitter::EmitJSEvent(int64_t tag, winrt::hstring &&eventName, JSValue &&eventObject) noexcept {
  return EmitJSEvent(L"receiveEvent", tag, std::move(eventName), std::move(eventObject));
}

void BatchingEventEmitter::EmitJSEvent(
    winrt::hstring &&emitterMethod,
    int64_t tag,
    winrt::hstring &&eventName,
    JSValue &&eventObject) noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());

  implementation::BatchedEvent newEvent{std::move(emitterMethod), tag, std::move(eventName), std::move(eventObject)};

  size_t queueSizeAfterInsert;

  {
    std::scoped_lock lock(m_eventQueueMutex);

    m_eventQueue.push_back(std::move(newEvent));
    queueSizeAfterInsert = m_eventQueue.size();
  }

  // Once a frame is presented, send a task to JS to emit all elements in the
  // queue. We know the task is pending on the UI thread or JS thread if we
  // have a non-zero size queue, so we want to only trigger pumping when the
  // first element is added to an empty queue.
  if (queueSizeAfterInsert == 1) {
    m_renderingRevoker = xaml::Media::CompositionTarget::Rendering(
        winrt::auto_revoke, [weakThis{weak_from_this()}](auto const &, auto const &) {
          if (auto strongThis = weakThis.lock()) {
            strongThis->OnFrameUI();
          }
        });
  }
}

void BatchingEventEmitter::EmitCoalescingJSEvent(
    int64_t tag,
    winrt::hstring &&eventName,
    JSValue &&eventObject) noexcept {
  return EmitCoalescingJSEvent(L"receiveEvent", tag, std::move(eventName), std::move(eventObject));
}

void BatchingEventEmitter::EmitCoalescingJSEvent(
    winrt::hstring &&emitterMethod,
    int64_t tag,
    winrt::hstring &&eventName,
    JSValue &&eventObject) noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());

  {
    std::scoped_lock lock(m_eventQueueMutex);
    auto endIter = std::remove_if(m_eventQueue.begin(), m_eventQueue.end(), [&](const auto &evt) noexcept {
      return evt.eventName == eventName && evt.tag == tag;
    });

    m_eventQueue.erase(endIter, m_eventQueue.end());
  }

  EmitJSEvent(std::move(emitterMethod), tag, std::move(eventName), std::move(eventObject));
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

    auto paramsWriter = winrt::make_self<DynamicWriter>();
    paramsWriter->WriteArrayBegin();
    WriteValue(*paramsWriter, evt.tag);
    WriteValue(*paramsWriter, evt.eventName);
    WriteValue(*paramsWriter, evt.eventObject);
    paramsWriter->WriteArrayEnd();

    m_context->CallJSFunction("RCTEventEmitter", winrt::to_string(evt.emitterMethod), paramsWriter->TakeValue());
    currentBatch.pop_front();
  }
}

} // namespace winrt::Microsoft::ReactNative
