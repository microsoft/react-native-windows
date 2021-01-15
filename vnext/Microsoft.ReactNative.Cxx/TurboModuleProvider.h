// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <JSI/JsiAbiApi.h>
#include <JSI/JsiApiContext.h>
#include <ReactCommon/TurboModule.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>

namespace winrt::Microsoft::ReactNative {

// Creates CallInvoker based on JSDispatcher.
std::shared_ptr<facebook::react::CallInvoker> MakeAbiCallInvoker(IReactDispatcher const &jsDispatcher) noexcept;

template <
    typename TTurboModule,
    std::enable_if_t<std::is_base_of_v<::facebook::react::TurboModule, TTurboModule>, int> = 0>
void AddTurboModuleProvider(IReactPackageBuilder const &packageBuilder, std::wstring_view moduleName) {
  auto experimental = packageBuilder.as<IReactPackageBuilderExperimental>();
  experimental.AddTurboModule(
      moduleName, [](IReactModuleBuilder const &moduleBuilder) noexcept -> winrt::Windows::Foundation::IInspectable {
        IJsiHostObject abiTurboModule{nullptr};
        // We expect the initializer to be called immediately for TurboModules
        moduleBuilder.AddInitializer([&abiTurboModule](IReactContext const &context) mutable {
          GetOrCreateContextRuntime(ReactContext{context}); // Ensure the JSI runtime is created.
          auto callInvoker = MakeAbiCallInvoker(context.JSDispatcher());
          auto turboModule = std::make_shared<TTurboModule>(callInvoker);
          abiTurboModule = winrt::make<JsiHostObjectWrapper>(std::move(turboModule));
        });
        return abiTurboModule.as<winrt::Windows::Foundation::IInspectable>();
      });
}

} // namespace winrt::Microsoft::ReactNative
