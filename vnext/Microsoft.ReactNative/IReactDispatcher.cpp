// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactDispatcher.h"
#include "ReactDispatcherHelper.g.cpp"

#include <glog/logging.h>

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative::implementation {

ReactDispatcher::ReactDispatcher(Mso::DispatchQueue &&queue) noexcept : m_queue{std::move(queue)} {}

bool ReactDispatcher::HasThreadAccess() noexcept {
  return m_queue.HasThreadAccess();
}

ReactDispatcherCallback CreateLoggingCallback(ReactDispatcherCallback const &callback) {
  return [callback]() {
    try {
      callback();
    } catch (winrt::hresult_error const &error) {
      std::stringstream errorCode;
      errorCode << "0x" << std::hex << error.code();
      LOG(ERROR) << "HRESULT " << errorCode.str() << ": " << winrt::to_string(error.message());
      throw;
    }
  };
}

void ReactDispatcher::Post(ReactDispatcherCallback const &callback) noexcept {
  return m_queue.Post([callback = CreateLoggingCallback(callback)]() noexcept { callback(); });
}

/*static*/ IReactDispatcher ReactDispatcher::CreateSerialDispatcher() noexcept {
  return make<ReactDispatcher>(Mso::DispatchQueue{});
}

/*static*/ Mso::DispatchQueue ReactDispatcher::GetUIDispatchQueue(IReactPropertyBag const &properties) noexcept {
  return GetUIDispatcher(properties).as<ReactDispatcher>()->m_queue;
}

/*static*/ IReactDispatcher ReactDispatcher::UIThreadDispatcher() noexcept {
  static thread_local weak_ref<IReactDispatcher> *tlsWeakDispatcher{nullptr};
  IReactDispatcher dispatcher{nullptr};
  auto queue = Mso::DispatchQueue::GetCurrentUIThreadQueue();
  if (queue && queue.HasThreadAccess()) {
    queue.InvokeElsePost([&queue, &dispatcher]() noexcept {
      // This code runs synchronously, but we want it to be run the queue context to
      // access the queue local value where we store the weak_ref to the dispatcher.
      // The queue local values are destroyed along with the queue.
      // To access queue local value we temporary swap it with the thread local value.
      // It must be a TLS value to ensure proper indexing of the queue local value entry.
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

/*static*/ IReactPropertyName ReactDispatcher::UIDispatcherProperty() noexcept {
  static IReactPropertyName uiThreadDispatcherProperty{ReactPropertyBagHelper::GetName(
      ReactPropertyBagHelper::GetNamespace(L"ReactNative.Dispatcher"), L"UIDispatcher")};
  return uiThreadDispatcherProperty;
}

/*static*/ IReactDispatcher ReactDispatcher::GetUIDispatcher(IReactPropertyBag const &properties) noexcept {
  return properties.Get(UIDispatcherProperty()).try_as<IReactDispatcher>();
}

/*static*/ void ReactDispatcher::SetUIThreadDispatcher(IReactPropertyBag const &properties) noexcept {
  properties.Set(UIDispatcherProperty(), UIThreadDispatcher());
}

/*static*/ IReactPropertyName ReactDispatcher::JSDispatcherProperty() noexcept {
  static IReactPropertyName jsThreadDispatcherProperty{ReactPropertyBagHelper::GetName(
      ReactPropertyBagHelper::GetNamespace(L"ReactNative.Dispatcher"), L"JSDispatcher")};
  return jsThreadDispatcherProperty;
}

} // namespace winrt::Microsoft::ReactNative::implementation
