// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSI_JSIAPI
#define MICROSOFT_REACTNATIVE_JSI_JSIAPI
#include "../ReactContext.h"
#include "JsiAbiApi.h"

namespace winrt::Microsoft::ReactNative {

// Call provided lambda with the facebook::jsi::Runtime& parameter.
// For example: ExecuteJsi(context, [](facebook::jsi::Runtime& runtime){...})
// The code is executed synchronously if it is already in JSDispatcher, or asynchronously otherwise.
template <class TCodeWithRuntime>
void ExecuteJsi(ReactContext const &context, TCodeWithRuntime const &code) {
  ReactDispatcher jsDispatcher = context.JSDispatcher();
  if (jsDispatcher.HasThreadAccess()) {
    // Execute immediately if we are in JS thread.
    code(*JsiAbiRuntime::GetOrCreate(context.Handle().JSRuntime().as<JsiRuntime>()));
  } else {
    // Otherwise, schedule work in JS thread.
    jsDispatcher.Post([ context, code ]() noexcept {
      code(*JsiAbiRuntime::GetOrCreate(context.Handle().JSRuntime().as<JsiRuntime>()));
    });
  }
}

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSI_JSIAPI
