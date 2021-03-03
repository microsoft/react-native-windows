// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CoalescingEventEmitter.h"
#include "CoalescingEventEmitter.g.cpp"
#include "DynamicWriter.h"

namespace winrt::Microsoft::ReactNative::implementation {

/*static*/ ReactPropertyId<CoalescingEventEmitter> CoalescingEventEmitter::Property() noexcept {
  static ReactPropertyId<CoalescingEventEmitter> propId{L"ReactNative.CoalescingEventEmitter", L"Emitter"};
  return propId;
}

/*static*/ winrt::Microsoft::ReactNative::CoalescingEventEmitter CoalescingEventEmitter::FromContext(
    IReactContext const &context) noexcept {
  return context.Properties()
      .Get(CoalescingEventEmitter::Property().Handle())
      .as<winrt::Microsoft::ReactNative::CoalescingEventEmitter>();
}

/*static*/ winrt::Microsoft::ReactNative::CoalescingEventEmitter CoalescingEventEmitter::FromContext(
    Mso::React::IReactContext const &context) noexcept {
  return context.Properties()
      .Get(CoalescingEventEmitter::Property().Handle())
      .as<winrt::Microsoft::ReactNative::CoalescingEventEmitter>();
}

CoalescingEventEmitter::CoalescingEventEmitter(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept
    : m_context(std::move(context)) {}

void CoalescingEventEmitter::EmitJSEvent(int64_t coalesceKey, JSValueArgWriter const &paramsArgWriter) noexcept {
  std::unique_lock lock(m_mutex);

  for (auto &evt : m_eventQueue) {
    if (evt.coalesceKey && *evt.coalesceKey == coalesceKey) {
      evt.expired = true;
    }
  }

  auto paramsWriter = winrt::make_self<DynamicWriter>();
  paramsArgWriter(*paramsWriter);

  CoalescingEvent newEvent;
  newEvent.coalesceKey = coalesceKey;
  newEvent.args = paramsWriter->TakeValue();

  m_eventQueue.push_back(std::move(newEvent));
  EnsureRenderCallbackRegistered();
}

void CoalescingEventEmitter::EmitDurableJSEvent(JSValueArgWriter const &paramsArgWriter) noexcept {
  std::unique_lock lock(m_mutex);

  auto paramsWriter = winrt::make_self<DynamicWriter>();
  paramsArgWriter(*paramsWriter);

  CoalescingEvent newEvent;
  newEvent.args = paramsWriter->TakeValue();

  m_eventQueue.push_back(std::move(newEvent));
  EnsureRenderCallbackRegistered();
}

void CoalescingEventEmitter::EnsureRenderCallbackRegistered() noexcept {
  if (!m_renderingRevoker) {
    m_renderingRevoker = xaml::Media::CompositionTarget::Rendering(
        winrt::auto_revoke, [weakThis{get_weak()}](auto const &, auto const &) {
          if (auto strongThis = weakThis.get()) {
            strongThis->OnRendering();
          }
        });
  }
}

void CoalescingEventEmitter::OnRendering() noexcept {
  auto jsDispatcher = m_context->Properties().Get(ReactDispatcherHelper::JSDispatcherProperty()).as<IReactDispatcher>();

  // Submit the batch of requests to the JS queue
  jsDispatcher.Post([weakThis{get_weak()}]() noexcept {
    auto strongThis = weakThis.get();
    if (!strongThis) {
      return;
    }

    while (!strongThis->m_eventQueue.empty()) {
      auto &evt = strongThis->m_eventQueue.front();
      if (!evt.expired) {
        strongThis->m_context->CallJSFunction("RCTEventEmitter", "receiveEvent", std::move(evt.args));
      }

      strongThis->m_eventQueue.pop_front();
    }
  });

  // Don't leave the callback continuously registered as it can waste power.
  // See https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.media.compositiontarget.rendering?view=winrt-19041
  m_renderingRevoker.revoke();
}

} // namespace winrt::Microsoft::ReactNative::implementation
