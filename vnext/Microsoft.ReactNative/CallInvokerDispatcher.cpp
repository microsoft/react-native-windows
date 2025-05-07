// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "CallInvokerDispatcher.h"

namespace Microsoft::ReactNative {

  CallInvokerDispatcher::CallInvokerDispatcher(std::shared_ptr<facebook::react::CallInvoker> &&callInvoker) noexcept
  : m_callInvoker(callInvoker)
  {}

  bool CallInvokerDispatcher::HasThreadAccess() noexcept {
    return m_threadId == std::this_thread::get_id();
  }

  void CallInvokerDispatcher::Post(winrt::Microsoft::ReactNative::ReactDispatcherCallback const &callback) noexcept {
    m_callInvoker->invokeAsync([this, callback = std::move(callback)](facebook::jsi::Runtime&) {
      m_threadId = std::this_thread::get_id();
      callback();
      m_threadId = std::thread::id{};
    });
  }

}
