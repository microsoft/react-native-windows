// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSI_JSIAPI
#define MICROSOFT_REACTNATIVE_JSI_JSIAPI

#include "../ReactContext.h"
#include "JsiAbiApi.h"
#include "ReactPromise.h"

namespace winrt::Microsoft::ReactNative {

// Try to get JSI Runtime for the current JS dispatcher thread.
// If it is not found, then create it based on context JSI runtime and store it in the context.Properties().
// The function returns nullptr if the current context does not have JSI runtime.
// It makes sure that the JSI runtime holder is removed when the instance is unloaded.
facebook::jsi::Runtime *TryGetOrCreateContextRuntime(ReactContext const &context) noexcept;

// Calls TryGetOrCreateContextRuntime to get JSI runtime.
// It crashes when TryGetOrCreateContextRuntime returns null.
// Note: deprecated in favor of TryGetOrCreateContextRuntime.
[[deprecated]] facebook::jsi::Runtime &GetOrCreateContextRuntime(ReactContext const &context) noexcept;

// Call provided lambda with the facebook::jsi::Runtime& parameter.
// For example: ExecuteJsi(context, [](facebook::jsi::Runtime& runtime){...})
// The code is executed synchronously if it is already in JSDispatcher, or asynchronously otherwise.
template <class TCodeWithRuntime>
void ExecuteJsi(ReactContext const &context, TCodeWithRuntime const &code, ReactPromise<void> *callStatus = nullptr) {
  ReactDispatcher jsDispatcher = context.JSDispatcher();
  auto callCode = [](ReactContext const &context, TCodeWithRuntime const &code, ReactPromise<void> *callStatus) {
    facebook::jsi::Runtime *runtime = TryGetOrCreateContextRuntime(context);
    if (runtime) {
      code(*runtime);
    }

    // Report status of the call
    if (callStatus) {
      if (runtime) {
        callStatus->Resolve();
      } else {
        callStatus->Reject("No JSI runtime");
      }
    }
  };

  if (jsDispatcher.HasThreadAccess()) {
    // Execute immediately if we are in JS thread.
    callCode(context, code, callStatus);
  } else {
    // Otherwise, schedule work in JS thread.
    jsDispatcher.Post([callCode,
                       context,
                       code,
                       callStatus = callStatus ? std::make_unique<ReactPromise<void>>(*callStatus)
                                               : std::unique_ptr<ReactPromise<void>>(nullptr)]() noexcept {
      callCode(context, code, callStatus.get());
    });
  }
}

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSI_JSIAPI
