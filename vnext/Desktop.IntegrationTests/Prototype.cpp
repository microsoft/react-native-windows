// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Microsoft.ReactNative/ReactHost/React.h>
#include <Microsoft.ReactNative/IReactDispatcher.h>
#include <winrt/Microsoft.UI.Dispatching.h>

#include <windows.h>
#include <MddBootstrap.h>
#include <WindowsAppSDK-VersionInfo.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>

#include "..\codegen\NativeDeviceInfoSpec.g.h"

#include "Modules/TestModule.h"
#include "MockCompositionContext.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace msrn = winrt::Microsoft::ReactNative;

// Work around crash in DeviceInfo when running outside of XAML environment
REACT_MODULE(DeviceInfo)
struct DeviceInfo {
  using ModuleSpec = ::Microsoft::ReactNativeSpecs::DeviceInfoSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
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

struct TestReactPackageProvider : winrt::implements<TestReactPackageProvider, msrn::IReactPackageProvider> {
  void CreatePackage(msrn::IReactPackageBuilder const &packageBuilder) noexcept {
    packageBuilder.AddTurboModule(L"DeviceInfo", winrt::Microsoft::ReactNative::MakeModuleProvider<DeviceInfo>());
    winrt::Microsoft::ReactNative::TryAddAttributedModule(packageBuilder, L"TestModule", true);
  }
};

struct TestUIDispatcher : public winrt::implements<TestUIDispatcher, msrn::IReactDispatcher>
{
  TestUIDispatcher(winrt::Microsoft::UI::Dispatching::DispatcherQueue const& dispatcherQueue)
    : m_dispatcherQueue{dispatcherQueue}
  {
    m_dispatcherQueue.TryEnqueue([self = get_strong()]() noexcept
    {
      self->m_threadId = GetCurrentThreadId();
    });
  }

  bool HasThreadAccess()
  {
    return m_threadId == GetCurrentThreadId();
  }

  void Post(msrn::ReactDispatcherCallback const& callback)
  {
    m_dispatcherQueue.TryEnqueue([callback]() noexcept
    {
      callback();
    });
  }

private:
  winrt::Microsoft::UI::Dispatching::DispatcherQueue m_dispatcherQueue;
  DWORD m_threadId{0};
};

msrn::ReactPropertyId<winrt::hstring> PlatformNameOverrideProperty() noexcept {
  static msrn::ReactPropertyId<winrt::hstring> prop{L"ReactNative.Injection", L"PlatformNameOverride"};
  return prop;
}

struct TestReactNativeHostHolder {
  struct Options {
    bool LoadInstance = true;
  };

  TestReactNativeHostHolder(
      std::wstring_view jsBundle,
      Mso::Functor<void(winrt::Microsoft::ReactNative::ReactNativeHost const &)> &&hostInitializer,
      Options &&options = {}) noexcept;
  ~TestReactNativeHostHolder() noexcept;

  winrt::Microsoft::ReactNative::ReactNativeHost const &Host() const noexcept;
  std::wstring GetLastError() const noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  winrt::Microsoft::UI::Dispatching::DispatcherQueueController m_queueController{nullptr};
  msrn::IReactDispatcher m_uiDispatcher{nullptr};
  mutable std::mutex m_errorMutex;
  std::wstring m_lastError;
};

#pragma region TRNHH impl

TestReactNativeHostHolder::TestReactNativeHostHolder(
    std::wstring_view jsBundle,
    Mso::Functor<void(winrt::Microsoft::ReactNative::ReactNativeHost const &)> &&hostInitializer,
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
            winrt::make<MockComposition::MockCompositionContext>());

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

#pragma endregion TRNHH impl

TEST_CLASS (Prototype) {

  // The test DLL's embedded manifest (resource ID 2) registers WinRT activatable classes,
  // but testhost.exe doesn't know about them. We manually activate the DLL's manifest
  // so that RoGetActivationFactory can find our classes.
  static inline HANDLE s_actCtx{INVALID_HANDLE_VALUE};
  static inline ULONG_PTR s_actCtxCookie{0};

  TEST_CLASS_INITIALIZE(Initialize)
  {
    HMODULE thisModule{};
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&Initialize),
        &thisModule);

    ACTCTXW actCtx{};
    actCtx.cbSize = sizeof(actCtx);
    actCtx.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;
    actCtx.hModule = thisModule;
    actCtx.lpResourceName = MAKEINTRESOURCEW(2);
    s_actCtx = CreateActCtxW(&actCtx);
    if (s_actCtx != INVALID_HANDLE_VALUE) {
      ActivateActCtx(s_actCtx, &s_actCtxCookie);
    }

    // https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/win32/mddbootstrap/nf-mddbootstrap-mddbootstrapinitialize2
    winrt::uninit_apartment();
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    if (FAILED(MddBootstrapInitialize2(
        Microsoft::WindowsAppSDK::Release::MajorMinor,
        Microsoft::WindowsAppSDK::Release::VersionTag,
        { Microsoft::WindowsAppSDK::Runtime::Version::UInt64 },
            MddBootstrapInitializeOptions::MddBootstrapInitializeOptions_None)))
    {
      throw std::exception("Could not initialize Windows App runtime");
    }
  }

  TEST_CLASS_CLEANUP(Cleanup)
  {
    MddBootstrapShutdown();

    if (s_actCtxCookie) {
      DeactivateActCtx(0, s_actCtxCookie);
      s_actCtxCookie = 0;
    }
    if (s_actCtx != INVALID_HANDLE_VALUE) {
      ReleaseActCtx(s_actCtx);
      s_actCtx = INVALID_HANDLE_VALUE;
    }
  }

  TEST_METHOD(Proto2)
  {
    Microsoft::React::Test::TestModule::Reset();

    winrt::handle instanceLoadedEvent{CreateEvent(nullptr, TRUE, FALSE, nullptr)};
    bool instanceFailed{false};

    auto holder = TestReactNativeHostHolder(L"IntegrationTests/DummyTest",
        [&instanceLoadedEvent, &instanceFailed](msrn::ReactNativeHost const &host) noexcept {
      host.InstanceSettings().InstanceLoaded(
          [&instanceLoadedEvent, &instanceFailed](
              auto const &, winrt::Microsoft::ReactNative::InstanceLoadedEventArgs args) noexcept {
            instanceFailed = args.Failed();
            SetEvent(instanceLoadedEvent.get());
          });
    });

    // First, wait for instance to load
    WaitForSingleObject(instanceLoadedEvent.get(), 5000);
    if (instanceFailed) {
      auto err = holder.GetLastError();
      auto msg = L"InstanceLoaded reported failure: " + (err.empty() ? L"(no error captured)" : err);
      Assert::Fail(msg.c_str());
    }

    auto status = Microsoft::React::Test::TestModule::AwaitCompletion(5000);
    Assert::IsTrue(
        status == Microsoft::React::Test::TestStatus::Passed,
        L"Test did not pass (JS did not call markTestPassed within timeout)");
  }

};
