// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactDispatcher.h"
#include "ReactDispatcherHelper.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative {

ReactDispatcher::ReactDispatcher(Mso::DispatchQueue &&queue) noexcept : m_queue{std::move(queue)} {}

bool ReactDispatcher::HasThreadAccess() noexcept {
  return m_queue.HasThreadAccess();
}

void ReactDispatcher::Post(ReactDispatcherCallback const &callback) noexcept {
  return m_queue.Post([callback]() noexcept { callback(); });
}

/*static*/ Mso::DispatchQueue ReactDispatcher::GetUIDispatchQueue(IReactPropertyBag const &properties) noexcept {
  return GetUIThreadDispatcher(properties).as<ReactDispatcher>()->m_queue;
}

/*static*/ IReactDispatcher ReactDispatcher::UIThreadDispatcher() noexcept {
  return make<ReactDispatcher>(Mso::DispatchQueue::MakeCurrentThreadUIQueue());
}

/*static*/ ReactPropertyId<IReactDispatcher> ReactDispatcher::UIThreadDispatcherProperty() noexcept {
  static ReactPropertyId<IReactDispatcher> uiThreadDispatcherProperty{L"ReactNative", L"UIThreadDispatcher"};
  return uiThreadDispatcherProperty;
}

/*static*/ IReactDispatcher ReactDispatcher::GetUIThreadDispatcher(IReactPropertyBag const &properties) noexcept {
  return ReactPropertyBag{properties}.Get(UIThreadDispatcherProperty());
}

/*static*/ void ReactDispatcher::SetUIThreadDispatcher(IReactPropertyBag const &properties) noexcept {
  ReactPropertyBag{properties}.Set(UIThreadDispatcherProperty(), UIThreadDispatcher());
}

} // namespace winrt::Microsoft::ReactNative
