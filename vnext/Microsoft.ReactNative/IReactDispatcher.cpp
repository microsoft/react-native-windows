// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactDispatcher.h"
#include "ReactDispatcherHelper.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative::implementation {

ReactDispatcher::ReactDispatcher(Mso::DispatchQueue &&queue) noexcept : m_queue{std::move(queue)} {}

bool ReactDispatcher::HasThreadAccess() noexcept {
  return m_queue.HasThreadAccess();
}

void ReactDispatcher::Post(ReactDispatcherCallback const &callback) noexcept {
  return m_queue.Post([callback]() noexcept { callback(); });
}

/*static*/ IReactDispatcher ReactDispatcher::CreateSerialDispatcher() noexcept {
  return make<ReactDispatcher>(Mso::DispatchQueue{});
}

/*static*/ Mso::DispatchQueue ReactDispatcher::GetUIDispatchQueue(IReactPropertyBag const &properties) noexcept {
  return GetUIDispatcher(properties).as<ReactDispatcher>()->m_queue;
}

/*static*/ IReactDispatcher ReactDispatcher::UIThreadDispatcher() noexcept {
  static thread_local weak_ref<IReactDispatcher> *tlsWeakDispatcher{nullptr};
  IReactDispatcher dispatcher;
  auto queue = Mso::DispatchQueue::GetCurrentUIThreadQueue();
  if (queue) {
    queue.InvokeElsePost([&queue, &dispatcher ]() noexcept {
      auto tlsGuard{queue.LockLocalValue(&tlsWeakDispatcher)};
      dispatcher = tlsWeakDispatcher->get();
      if (!dispatcher) {
        dispatcher = winrt::make<ReactDispatcher>(std::move(queue));
        *tlsWeakDispatcher = dispatcher;
      }
    });
  }

  return dispatcher;
}

/*static*/ ReactPropertyId<IReactDispatcher> ReactDispatcher::UIDispatcherProperty() noexcept {
  static ReactPropertyId<IReactDispatcher> uiThreadDispatcherProperty{L"ReactNative.Dispatcher", L"UIDispatcher"};
  return uiThreadDispatcherProperty;
}

/*static*/ IReactDispatcher ReactDispatcher::GetUIDispatcher(IReactPropertyBag const &properties) noexcept {
  return ReactPropertyBag{properties}.Get(UIDispatcherProperty());
}

/*static*/ void ReactDispatcher::SetUIThreadDispatcher(IReactPropertyBag const &properties) noexcept {
  ReactPropertyBag{properties}.Set(UIDispatcherProperty(), UIThreadDispatcher());
}

} // namespace winrt::Microsoft::ReactNative::implementation
