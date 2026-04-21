// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TestReactNativeHostHolder.h"

#include "TestReactPackageProvider.h"
#include "TestUIDispatcher.h"

#include <Microsoft.ReactNative/IReactDispatcher.h>
#include <ReactPropertyBag.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Windows.Foundation.Collections.h>

#include "TestCompositionContext.h"

namespace Microsoft::React::Test {

msrn::ReactPropertyId<winrt::hstring> PlatformNameOverrideProperty() noexcept {
  static msrn::ReactPropertyId<winrt::hstring> prop{L"ReactNative.Injection", L"PlatformNameOverride"};
  return prop;
}

TestReactNativeHostHolder::TestReactNativeHostHolder(
    std::wstring_view jsBundle,
    std::function<void(winrt::Microsoft::ReactNative::ReactNativeHost const &)> hostInitializer,
    Options &&options) noexcept {
  m_host = winrt::Microsoft::ReactNative::ReactNativeHost{};
  m_queueController = winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnDedicatedThread();
  m_uiDispatcher = winrt::make<TestUIDispatcher>(m_queueController.DispatcherQueue());

  m_queueController.DispatcherQueue().TryEnqueue([this,
                                                  jsBundle = std::wstring{jsBundle},
                                                  hostInitializer = std::move(hostInitializer),
                                                  options = std::move(options)]() noexcept {
    auto settings = m_host.InstanceSettings();
    settings.JavaScriptBundleFile(jsBundle);
    settings.Properties().Set(msrn::ReactDispatcherHelper::UIDispatcherProperty(), m_uiDispatcher);
    settings.Properties().Set(PlatformNameOverrideProperty().Handle(), winrt::box_value(L"windows"));
    settings.UseDeveloperSupport(false);
    settings.UseFastRefresh(true);
    settings.UseLiveReload(false);
    settings.EnableDeveloperMenu(false);
    settings.PackageProviders().Append(winrt::make<TestReactPackageProvider>());

    // Capture errors for diagnostics
    settings.NativeLogger([this](msrn::LogLevel level, winrt::hstring const &message) {
      if (static_cast<int>(level) >= static_cast<int>(msrn::LogLevel::Error)) {
        std::lock_guard lock(m_errorMutex);
        if (m_lastError.empty()) {
          m_lastError = message;
        }
      }
    });

    // Enable Fabric by setting a stub CompositionContext.
    // In a headless test we have no real compositor, but this is sufficient
    // for the runtime to choose the Fabric code path.
    winrt::Microsoft::ReactNative::ReactPropertyBag(settings.Properties())
        .Set(
            winrt::Microsoft::ReactNative::ReactPropertyId<
                winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext>{
                L"ReactNative.Composition", L"CompositionContext"},
            winrt::make<TestCompositionContext>());

    hostInitializer(m_host);

    if (options.LoadInstance) {
      m_host.LoadInstance();
    }
  });
}

TestReactNativeHostHolder::~TestReactNativeHostHolder() noexcept {
  m_host.UnloadInstance().get();
  m_queueController.ShutdownQueueAsync().get();
}

winrt::Microsoft::ReactNative::ReactNativeHost const &TestReactNativeHostHolder::Host() const noexcept {
  return m_host;
}

std::wstring TestReactNativeHostHolder::GetLastError() const noexcept {
  std::lock_guard lock(m_errorMutex);
  return m_lastError;
}

} // namespace Microsoft::React::Test
