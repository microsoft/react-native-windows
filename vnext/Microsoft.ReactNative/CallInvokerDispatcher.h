// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Microsoft.ReactNative.h>
#include <ReactCommon/CallInvoker.h>

namespace Microsoft::ReactNative {

struct CallInvokerDispatcher : winrt::implements<CallInvokerDispatcher, winrt::Microsoft::ReactNative::IReactDispatcher> {
  CallInvokerDispatcher(std::shared_ptr<facebook::react::CallInvoker> &&callInvoker) noexcept;

  bool HasThreadAccess() noexcept;
  void Post(winrt::Microsoft::ReactNative::ReactDispatcherCallback const &callback) noexcept;

 private:
   std::atomic<std::thread::id> m_threadId{};
   std::shared_ptr<facebook::react::CallInvoker> m_callInvoker;
};

}
