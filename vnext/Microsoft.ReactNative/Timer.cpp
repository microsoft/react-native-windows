// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Timer.h"
#include "Timer.g.cpp"

#include <CppWinRTIncludes.h>
#include "ReactCoreInjection.h"
#include <winrt/Microsoft.UI.Dispatching.h>

namespace winrt::Microsoft::ReactNative::implementation {

// Implementation of ITimer based on Windows' DispatcherQueue
struct ReactTimer : winrt::implements<ReactTimer, ITimer> {
  ReactTimer(const IReactPropertyBag &properties) {
    const auto queue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    m_dispatcherQueueTimer = queue.CreateTimer();
  }

  winrt::Windows::Foundation::TimeSpan Interval() noexcept {
    return m_dispatcherQueueTimer.Interval();
  }

  void Interval(winrt::Windows::Foundation::TimeSpan value) noexcept {
    m_dispatcherQueueTimer.Interval(value);
  }

  void Start() noexcept {
    return m_dispatcherQueueTimer.Start();
  }

  void Stop() noexcept {
    return m_dispatcherQueueTimer.Stop();
  }

  winrt::event_token Tick(const winrt::Windows::Foundation::EventHandler<IInspectable> &handler) {
    return m_dispatcherQueueTimer.Tick([handler](auto sender, auto args) { handler(sender, args); });
  }

  void Tick(winrt::event_token const &token) {
    m_dispatcherQueueTimer.Tick(token);
  }

 private:
  winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer m_dispatcherQueueTimer{nullptr};
};

Timer::Timer() noexcept {}

ITimer Timer::Create(const IReactPropertyBag &properties) {
  auto dispatcher = properties.Get(ReactDispatcherHelper::UIDispatcherProperty()).try_as<IReactDispatcher>();
  if (!dispatcher.HasThreadAccess()) {
    throw winrt::hresult_wrong_thread();
  }

  if (auto customTimerFactory = implementation::ReactCoreInjection::GetTimerFactory(properties)) {
    return customTimerFactory();
  } else {
    return winrt::make<ReactTimer>(properties);
  }
}

} // namespace winrt::Microsoft::ReactNative::implementation
