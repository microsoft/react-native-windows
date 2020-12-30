// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <JSI/JsiAbiApi.h>
#include <JSI/JsiApiContext.h>
#include <ReactCommon/TurboModule.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>

namespace winrt::Microsoft::ReactNative {

// CallInvoker on top of JSDispatcher.
struct AbiCallInvoker : facebook::react::CallInvoker {
  AbiCallInvoker(IReactDispatcher const &jsDispatcher) : m_jsDispatcher(jsDispatcher) {}

  void invokeAsync(std::function<void()> &&func) override {
    m_jsDispatcher.Post([func = std::move(func)]() { func(); });
  }

  void invokeSync(std::function<void()> &&func) override {
    if (m_jsDispatcher.HasThreadAccess()) {
      func();
    } else {
      std::mutex mutex;
      std::condition_variable cv;
      bool completed{false};
      auto lock = std::unique_lock{mutex};
      m_jsDispatcher.Post([&func, &mutex, &cv, &completed]() {
        func();
        auto lock = std::unique_lock{mutex};
        completed = true;
        cv.notify_all();
      });
      cv.wait(lock, [&] { return completed; });
    }
    func = nullptr;
  }

 private:
  IReactDispatcher m_jsDispatcher{nullptr};
};

// ABI-safe wrapper for Turbo module
template <typename TTurboModule>
struct AbiTurboModule : implements<AbiTurboModule<TTurboModule>, IJsiHostObject> {
  AbiTurboModule() {}
  void Initialize(IReactContext const &context) {
    m_callInvoker = std::make_shared<AbiCallInvoker>(context.JSDispatcher());
    auto turboModule = std::make_shared<TTurboModule>(m_callInvoker);
    m_module = winrt::make<JsiHostObjectWrapper>(std::move(turboModule));
  }

  JsiValueRef GetProperty(JsiRuntime const &runtime, JsiPropertyIdRef const &name) {
    return m_module.GetProperty(runtime, name);
  }

  void SetProperty(JsiRuntime const &runtime, JsiPropertyIdRef const &name, JsiValueRef const &value) {
    m_module.SetProperty(runtime, name, value);
  }

  Windows::Foundation::Collections::IVector<JsiPropertyIdRef> GetPropertyIds(JsiRuntime const &runtime) {
    return m_module.GetPropertyIds(runtime);
  }

 private:
  IJsiHostObject m_module{nullptr};
  std::shared_ptr<facebook::react::CallInvoker> m_callInvoker;
};

// Create a module provider for TModule type.
template <
    typename TTurboModule,
    std::enable_if_t<std::is_base_of_v<::facebook::react::TurboModule, TTurboModule>, int> = 0>
inline ReactModuleProvider MakeJsiTurboModuleProvider() noexcept {
  return [](IReactModuleBuilder const &moduleBuilder) noexcept->winrt::Windows::Foundation::IInspectable {
    auto abiTurboModule = winrt::make_self<AbiTurboModule<TTurboModule>>();
    moduleBuilder.AddInitializer([abiTurboModule](IReactContext const &context) mutable {
      ReactContext ctx{context};
      GetOrCreateContextRuntime(ctx);
      abiTurboModule->Initialize(context);
      abiTurboModule = nullptr;
    });
    return abiTurboModule.as<winrt::Windows::Foundation::IInspectable>();
  };
}

template <typename TTurboModule>
void AddTurboModuleProvider(IReactPackageBuilder const &packageBuilder, std::wstring_view moduleName) {
  auto experimental = packageBuilder.as<IReactPackageBuilderExperimental>();
  experimental.AddTurboModule(moduleName, MakeJsiTurboModuleProvider<TTurboModule>());
}

} // namespace winrt::Microsoft::ReactNative
