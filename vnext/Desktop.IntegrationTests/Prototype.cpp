//TODO: copyright

#include <CppUnitTest.h>

#include <Microsoft.ReactNative/ReactHost/React.h>
#include <Microsoft.ReactNative/IReactDispatcher.h>
#include <winrt/Microsoft.UI.Dispatching.h>

#include <windows.h>
#include <MddBootstrap.h>
#include <WindowsAppSDK-VersionInfo.h>
#include <winrt/base.h>

#include <future>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace msrn = winrt::Microsoft::ReactNative;

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
  void WaitForInstanceCreated();

 private:
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  winrt::Microsoft::UI::Dispatching::DispatcherQueueController m_queueController{nullptr};
  winrt::handle m_instanceCreatedEvent{CreateEvent(nullptr, TRUE, FALSE, nullptr)};
  msrn::IReactDispatcher m_uiDispatcher{nullptr};
};

#pragma region TRNHH impl

TestReactNativeHostHolder::TestReactNativeHostHolder(
    std::wstring_view jsBundle,
    Mso::Functor<void(winrt::Microsoft::ReactNative::ReactNativeHost const &)> &&hostInitializer,
    Options &&options) noexcept {
  m_host = winrt::Microsoft::ReactNative::ReactNativeHost{};

  // Subscribe to InstanceCreated event BEFORE launching async work
  m_host.InstanceSettings().InstanceCreated([this](auto&&, auto&&) {
    SetEvent(m_instanceCreatedEvent.get());
  });

  m_queueController = winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnDedicatedThread();

  // Create UI dispatcher for headless tests using the queue controller's dispatcher
  m_uiDispatcher = winrt::make<TestUIDispatcher>(m_queueController.DispatcherQueue());
  m_queueController.DispatcherQueue().TryEnqueue([this,
                                                  jsBundle = std::wstring{jsBundle},
                                                  hostInitializer = std::move(hostInitializer),
                                                  options = std::move(options)]() noexcept {
    //// bundle is assumed to be co-located with the test binary
    //wchar_t testBinaryPath[MAX_PATH];
    //TestCheck(GetModuleFileNameW(NULL, testBinaryPath, MAX_PATH) < MAX_PATH);
    //testBinaryPath[std::wstring_view{testBinaryPath}.rfind(L"\\")] = 0;

    //m_host.InstanceSettings().BundleRootPath(testBinaryPath);
    //m_host.InstanceSettings().JavaScriptBundleFile(jsBundle);
    //m_host.InstanceSettings().UseDeveloperSupport(false);
    //m_host.InstanceSettings().UseFastRefresh(false);
    //m_host.InstanceSettings().UseLiveReload(false);
    //m_host.InstanceSettings().EnableDeveloperMenu(false);
    //m_host.PackageProviders().Append(winrt::make<TestReactPackageProvider>());

    auto settings = m_host.InstanceSettings();
    settings.Properties().Set(msrn::ReactDispatcherHelper::UIDispatcherProperty(), m_uiDispatcher);
    settings.Properties().Set(PlatformNameOverrideProperty().Handle(), winrt::box_value(L"windows"));
    settings.UseFastRefresh(true);

    // Set on instance creation or load
    //settings.JavaScriptBundleFile(L"IntegrationTests/IntegrationTestsApp");
    //settings.JavaScriptBundleFile(L"IntegrationTests/DummyTest");

    // To properly enable fabric you need to set a compositor.
    // Since the UTs are ui-less we can force fabric by setting a CompositionContext with a null compositor
    //winrt::Microsoft::ReactNative::ReactPropertyBag(m_host.InstanceSettings().Properties())
    //winrt::Microsoft::ReactNative::ReactPropertyBag(settings.Properties())
    //    .Set(
    //        winrt::Microsoft::ReactNative::ReactPropertyId<
    //            winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext>{
    //            L"ReactNative.Composition", L"CompositionContext"},
    //        winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::CreateContext(
    //            nullptr));

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

void TestReactNativeHostHolder::WaitForInstanceCreated() {
  WaitForSingleObject(m_instanceCreatedEvent.get(), INFINITE);
}

#pragma endregion TRNHH impl

TEST_CLASS (Prototype) {

  TEST_CLASS_INITIALIZE(Initialize)
  {
    // See
    // https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/win32/mddbootstrap/nf-mddbootstrap-mddbootstrapinitialize2
    winrt::uninit_apartment();
    winrt::init_apartment(winrt::apartment_type::multi_threaded);


    // Use the version constants from WindowsAppSDK-VersionInfo.h
    //const UINT32 majorMinor = WINDOWSAPPSDK_RELEASE_MAJORMINOR;
    //PCWSTR versionTag = WINDOWSAPPSDK_RELEASE_CHANNEL_W; // e.g. LTS, Stable, etc.
    //PCWSTR minVersion = WINDOWSAPPSDK_RELEASE_VERSION_TAG_W; // e.g. "1.5.240829000"

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
  }

  TEST_METHOD(Proto2)
  {
    auto holder = TestReactNativeHostHolder(L"TurboModuleTests", [](msrn::ReactNativeHost const &host) noexcept {
      host.InstanceSettings().JavaScriptBundleFile(L"IntegrationTests/DummyTest");

      host.InstanceSettings().InstanceLoaded(
        [](auto const &, winrt::Microsoft::ReactNative::IInstanceLoadedEventArgs args) noexcept {
          //if (args.Failed()) {
          //  TestEventService::LogEvent("InstanceLoaded::Failed", nullptr);
          //} else {
          //  TestEventService::LogEvent("InstanceLoaded::Success", nullptr);
          //}

          //    const int rootTag = 101;
          //folly::dynamic params = folly::dynamic::array(
          //        std::move("DummyTest"),
          //        folly::dynamic::object("initialProps", folly::dynamic::object())("rootTag", rootTag));

          const int rootTag = 101;
          auto jsArgs = msrn::JSValueArray
          {
            "DummyTest", // appName
            msrn::JSValueObject
            {
              { "initialProps", msrn::JSValueObject{} },
              { "rootTag", rootTag }
            }
          };

            args.Context().CallJSFunction(
                L"AppRegistry",
                L"runApplication",
                msrn::MakeJSValueArgWriter(jsArgs));

          //TODO: await TestModule

            args.Context().CallJSFunction(
            L"AppRegistry",
            L"unmountApplicationComponentAtRootTag",
            msrn::MakeJSValueArgWriter(msrn::JSValueArray{rootTag})
          );
      });
    });

    // Wait for React Native instance to be fully initialized
    holder.WaitForInstanceCreated();

    Assert::IsTrue(true);
  }

  TEST_METHOD(Proto1)
  {
    bool succeeded = true;

    {
      auto queueController = winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnDedicatedThread();
      msrn::IReactDispatcher dispatcher = winrt::make<TestUIDispatcher>(queueController.DispatcherQueue());

      msrn::ReactNativeHost host{};

      auto settings = host.InstanceSettings();
      settings.Properties().Set(msrn::ReactDispatcherHelper::UIDispatcherProperty(), dispatcher);
      settings.Properties().Set(PlatformNameOverrideProperty().Handle(), winrt::box_value(L"windows"));
      settings.UseFastRefresh(true);
      settings.JavaScriptBundleFile(L"IntegrationTests/IntegrationTestsApp");


      auto action = host.ReloadInstance();

      action.get();

      queueController.ShutdownQueueAsync().get();
    }

    Assert::IsTrue(succeeded);
  }

};
