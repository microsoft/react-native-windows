// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSI_JSIAPI
#define MICROSOFT_REACTNATIVE_JSI_JSIAPI

#include "../ReactContext.h"
#include "JsiAbiApi.h"

namespace winrt::Microsoft::ReactNative {

// Get JSI Runtime from the current JS dispatcher thread.
// If it is not found, then create it and store it in the context.Properties().
// Make sure that the JSI runtime holder is removed when the instance is unloaded.
facebook::jsi::Runtime &GetOrCreateContextRuntime(ReactContext const &context) noexcept;

// Call provided lambda with the facebook::jsi::Runtime& parameter.
// For example: ExecuteJsi(context, [](facebook::jsi::Runtime& runtime){...})
// The code is executed synchronously if it is already in JSDispatcher, or asynchronously otherwise.
template <class TCodeWithRuntime>
void ExecuteJsi(ReactContext const &context, TCodeWithRuntime const &code) {
  ReactDispatcher jsDispatcher = context.JSDispatcher();
  if (jsDispatcher.HasThreadAccess()) {
    // Execute immediately if we are in JS thread.
    code(GetOrCreateContextRuntime(context));
  } else {
    // Otherwise, schedule work in JS thread.
    jsDispatcher.Post([context, code]() noexcept { code(GetOrCreateContextRuntime(context)); });
  }
}

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSI_JSIAPI
