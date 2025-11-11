// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestReactNativeHostHolder.h"
#include "..\codegen\NativeDeviceInfoSpec.g.h"
#include <NativeModules.h>

#ifdef USE_FABRIC
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#endif

namespace ReactNativeIntegrationTests {

using namespace winrt;

// Work around crash in DeviceInfo when running outside of XAML environment
REACT_MODULE(DeviceInfo)
struct DeviceInfo {
  using ModuleSpec = ::Microsoft::ReactNativeSpecs::DeviceInfoSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept {
    m_context = reactContext;
  }

  REACT_GET_CONSTANTS(GetConstants)
  ::Microsoft::ReactNativeSpecs::DeviceInfoSpec_DeviceInfoConstants GetConstants() noexcept {
    ::Microsoft::ReactNativeSpecs::DeviceInfoSpec_DeviceInfoConstants constants;
    ::Microsoft::ReactNativeSpecs::DeviceInfoSpec_DisplayMetrics screenDisplayMetrics;
    screenDisplayMetrics.fontScale = 1;
    screenDisplayMetrics.height = 1024;
    screenDisplayMetrics.width = 1024;
    screenDisplayMetrics.scale = 1;
    constants.Dimensions.screen = screenDisplayMetrics;
    constants.Dimensions.window = screenDisplayMetrics;
    return constants;
  }

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

struct TestReactPackageProvider
    : winrt::implements<TestReactPackageProvider, winrt::Microsoft::ReactNative::IReactPackageProvider> {
 public: // IReactPackageProvider
  void CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
    packageBuilder.AddTurboModule(L"DeviceInfo", winrt::Microsoft::ReactNative::MakeModuleProvider<DeviceInfo>());
  }
};

TestReactNativeHostHolder::TestReactNativeHostHolder(
    std::wstring_view jsBundle,
    Mso::Functor<void(winrt::Microsoft::ReactNative::ReactNativeHost const &)> &&hostInitializer,
    Options &&options) noexcept {
  m_host = winrt::Microsoft::ReactNative::ReactNativeHost{};
#ifdef USE_WINUI3
  m_queueController = winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnDedicatedThread();
#else
  m_queueController = winrt::Windows::System::DispatcherQueueController::CreateOnDedicatedThread();
#endif
  m_queueController.DispatcherQueue().TryEnqueue([this,
                                                  jsBundle = std::wstring{jsBundle},
                                                  hostInitializer = std::move(hostInitializer),
                                                  options = std::move(options)]() noexcept {
    // bundle is assumed to be co-located with the test binary
    wchar_t testBinaryPath[MAX_PATH];
    TestCheck(GetModuleFileNameW(NULL, testBinaryPath, MAX_PATH) < MAX_PATH);
    testBinaryPath[std::wstring_view{testBinaryPath}.rfind(L"\\")] = 0;

    m_host.InstanceSettings().BundleRootPath(testBinaryPath);
    m_host.InstanceSettings().JavaScriptBundleFile(jsBundle);
    m_host.InstanceSettings().UseDeveloperSupport(false);
    m_host.InstanceSettings().UseFastRefresh(false);
    m_host.InstanceSettings().UseLiveReload(false);
    m_host.InstanceSettings().EnableDeveloperMenu(false);
    m_host.PackageProviders().Append(winrt::make<TestReactPackageProvider>());

#if USE_FABRIC
    // To properly enable fabric you need to set a compositor.
    // Since the UTs are ui-less we can force fabric by setting a CompositionContext with a null compositor
    winrt::Microsoft::ReactNative::ReactPropertyBag(m_host.InstanceSettings().Properties())
        .Set(
            winrt::Microsoft::ReactNative::ReactPropertyId<
                winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext>{
                L"ReactNative.Composition", L"CompositionContext"},
            winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::CreateContext(
                nullptr));
#endif

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

} // namespace ReactNativeIntegrationTests
