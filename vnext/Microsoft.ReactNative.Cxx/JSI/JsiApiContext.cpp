// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiApiContext.h"

#if RNW_NEW_ARCH
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#endif

// Use __ImageBase to get current DLL handle.
// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace winrt::Microsoft::ReactNative {

namespace Details {
// Try to get JSI Runtime for the ReactContext
// If it is not found, then create it based on context JSI runtime and store it in the context.Properties().
// The function returns nullptr if the current context does not have JSI runtime.
// It makes sure that the JSI runtime holder is removed when the instance is unloaded.
JsiAbiRuntime *TryGetOrCreateContextRuntime(ReactContext const &context, JsiRuntime const &abiJsiRuntime) noexcept {
  // See if the JSI runtime was previously created.
  JsiAbiRuntime *runtime = JsiAbiRuntime::GetFromJsiRuntime(abiJsiRuntime);
  if (!runtime) {
    // Create the runtime
    std::unique_ptr<JsiAbiRuntime> runtimeHolder = std::make_unique<JsiAbiRuntime>(abiJsiRuntime);
    runtime = runtimeHolder.get();

    // We want to keep the JSI runtime while current instance is alive.
    // The JSI runtime object must be local to our DLL.
    // We create a property name based on the current DLL handle.
    HMODULE currentDllHandle = reinterpret_cast<HMODULE>(&__ImageBase);
    std::wstring jsiRuntimeLocalName = L"jsiRuntime_" + std::to_wstring(reinterpret_cast<uintptr_t>(currentDllHandle));
    using ValueType = ReactNonAbiValue<std::unique_ptr<JsiAbiRuntime>>;
    ReactPropertyId<ValueType> jsiRuntimeProperty{L"ReactNative.InstanceData", jsiRuntimeLocalName.c_str()};
    ValueType runtimeValue{std::in_place, std::move(runtimeHolder)};
    context.Properties().Set(jsiRuntimeProperty, runtimeValue);

    // We remove the JSI runtime from properties when React instance is destroyed.
    auto destroyInstanceNotificationId{
        ReactNotificationId<InstanceDestroyedEventArgs>{L"ReactNative.InstanceSettings", L"InstanceDestroyed"}};
    context.Notifications().Subscribe(
        destroyInstanceNotificationId,
        [context, jsiRuntimeProperty](
            winrt::Windows::Foundation::IInspectable const & /*sender*/,
            ReactNotificationArgs<InstanceDestroyedEventArgs> const &args) noexcept {
          context.Properties().Remove(jsiRuntimeProperty);
          args.Subscription().Unsubscribe(); // Unsubscribe after we handle the notification.
        });
  }

  return runtime;
}

} // namespace Details

facebook::jsi::Runtime *TryGetOrCreateContextRuntime(
    ReactContext const &context,
    winrt::Windows::Foundation::IInspectable const &runtimeHandle) noexcept {
  if (!runtimeHandle) {
    return nullptr;
  }

  // The JSI runtime is not available if we do Web debugging when JS is running in web browser.
  JsiRuntime abiJsiRuntime = runtimeHandle.as<JsiRuntime>();
  if (!abiJsiRuntime) {
    return nullptr;
  }

  return Details::TryGetOrCreateContextRuntime(context, abiJsiRuntime);
}

// Note: deprecated in favor of TryGetOrCreateContextRuntime with Handle parameter
facebook::jsi::Runtime *TryGetOrCreateContextRuntime(ReactContext const &context) noexcept {
#ifdef DEBUG
#ifdef RNW_NEW_ARCH
  // TODO move this check into ReactContext.JSRuntime().
  VerifyElseCrashSz(
      !context.Properties().Get(winrt::Microsoft::ReactNative::ReactPropertyId<
                                winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext>{
          L"ReactNative.Composition", L"CompositionContext"}),
      "ExecuteJsi/TryGetOrCreateContextRuntime not supported on new arch, use ReactContext.CallInvoker instead.");

  ReactDispatcher jsDispatcher = context.JSDispatcher();
  VerifyElseCrashSz(jsDispatcher.HasThreadAccess(), "Must be in JS thread");
#endif
#endif

  if (auto runtimeHandle = context.Handle().JSRuntime()) {
    return TryGetOrCreateContextRuntime(context, runtimeHandle);
  }

  return nullptr;
}

// Calls TryGetOrCreateContextRuntime to get JSI runtime.
// It crashes when TryGetOrCreateContextRuntime returns null.
[[deprecated]] facebook::jsi::Runtime &GetOrCreateContextRuntime(
    ReactContext const &context,
    winrt::Windows::Foundation::IInspectable const &runtimeHandle) noexcept {
  facebook::jsi::Runtime *runtime = TryGetOrCreateContextRuntime(context, runtimeHandle);
  VerifyElseCrashSz(runtime, "JSI runtime is not available");
  return *runtime;
}

// Calls TryGetOrCreateContextRuntime to get JSI runtime.
// It crashes when TryGetOrCreateContextRuntime returns null.
// Note: deprecated in favor of TryGetOrCreateContextRuntime.
[[deprecated]] facebook::jsi::Runtime &GetOrCreateContextRuntime(ReactContext const &context) noexcept {
  facebook::jsi::Runtime *runtime = TryGetOrCreateContextRuntime(context);
  VerifyElseCrashSz(runtime, "JSI runtime is not available");
  return *runtime;
}

} // namespace winrt::Microsoft::ReactNative
