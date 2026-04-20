// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <windows.h>
#include <MddBootstrap.h>
#include <WindowsAppSDK-VersionInfo.h>

#include "Modules/TestModule.h"
#include "TestReactNativeHostHolder.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace msrn = winrt::Microsoft::ReactNative;

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
