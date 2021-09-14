// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactDispatcher.h"
#include "ReactDispatcherHelper.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative::implementation {

// Implements IDispatchQueue2 on top of a custom IReactDispatcher provided by the application
struct WrappedReactDispatcher : public Mso::UnknownObject<Mso::React::IDispatchQueue2> {
  WrappedReactDispatcher(const winrt::Microsoft::ReactNative::IReactDispatcher &dispatcher) noexcept
      : m_dispatcher(dispatcher) {}

  void Post(Mso::DispatchTask &&task) const noexcept override {
    m_dispatcher.Post(task);
  }

  void InvokeElsePost(Mso::DispatchTask &&task) const noexcept override {
    if (m_dispatcher.HasThreadAccess()) {
      task();
    } else {
      Post(std::move(task));
    }
  }

 private:
  winrt::Microsoft::ReactNative::IReactDispatcher m_dispatcher;
};

ReactDispatcher::ReactDispatcher(Mso::DispatchQueue &&queue) noexcept : m_queue{std::move(queue)} {}

bool ReactDispatcher::HasThreadAccess() noexcept {
  return m_queue.HasThreadAccess();
}

void ReactDispatcher::Post(ReactDispatcherCallback const &callback) noexcept {
  return m_queue.Post([callback]() noexcept { callback(); });
}

void ReactDispatcher::Post(Mso::DispatchTask &&task) const noexcept {
  m_queue.Post(std::move(task));
}

void ReactDispatcher::InvokeElsePost(Mso::DispatchTask &&task) const noexcept {
  m_queue.InvokeElsePost(std::move(task));
}

/*static*/ IReactDispatcher ReactDispatcher::CreateSerialDispatcher() noexcept {
  return make<ReactDispatcher>(Mso::DispatchQueue{});
}

/*static*/ Mso::CntPtr<Mso::React::IDispatchQueue2> ReactDispatcher::GetUIDispatchQueue2(
    IReactPropertyBag const &properties) noexcept {
  auto iReactDispatcher = GetUIDispatcher(properties);

  if (!iReactDispatcher)
    return nullptr;

  if (auto simpleDispatcher = iReactDispatcher.try_as<IDispatchQueue2>())
    return simpleDispatcher.get();

  return Mso::Make<WrappedReactDispatcher>(iReactDispatcher);
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
