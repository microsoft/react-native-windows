// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactHost/MsoReactContext.h>
#include <ReactPropertyBag.h>
#include "ReactHost/React.h"
#include "winrt/Microsoft.ReactNative.h"

#include "CallInvoker.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct CallInvoker : CallInvokerT<CallInvoker> {
  CallInvoker(
      Mso::React::IReactContext &reactContext,
      std::shared_ptr<facebook::react::CallInvoker> callInvoker) noexcept;

  void InvokeAsync(CallFunc func) noexcept;
  void InvokeSync(CallFunc func) noexcept;

  static winrt::Microsoft::ReactNative::CallInvoker FromProperties(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &properties) noexcept;
  static void SetProperties(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &properties,
      winrt::Microsoft::ReactNative::CallInvoker invoker) noexcept;

 private:
  Mso::CntPtr<Mso::React::IReactContext> m_context;
  std::shared_ptr<facebook::react::CallInvoker> m_callInvoker;
};

} // namespace winrt::Microsoft::ReactNative::implementation
