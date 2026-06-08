// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Windows.h>

#include <MddBootstrap.h>
#include <WindowsAppSDK-VersionInfo.h>

#include <CppRuntimeOptions.h>
#include "Modules/TestModule.h"
#include "TestReactNativeHostHolder.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace msrn = winrt::Microsoft::ReactNative;

namespace Microsoft::React::Test {

namespace {
void RunTest(std::wstring_view jsBundle) {
  TestModule::Reset();

  winrt::handle instanceLoadedEvent{CreateEvent(nullptr, TRUE, FALSE, nullptr)};
  bool instanceFailed{false};

  auto holder = TestReactNativeHostHolder(
      jsBundle, [&instanceLoadedEvent, &instanceFailed](msrn::ReactNativeHost const &host) noexcept {
        host.InstanceSettings().InstanceLoaded(
            [&instanceLoadedEvent, &instanceFailed](auto const &, msrn::InstanceLoadedEventArgs args) noexcept {
              instanceFailed = args.Failed();
              SetEvent(instanceLoadedEvent.get());
            });
      });

  // First, wait for instance to load
  WaitForSingleObject(instanceLoadedEvent.get(), INFINITE);
  if (instanceFailed) {
    auto err = holder.GetLastError();
    auto msg = L"InstanceLoaded reported failure: " + (err.empty() ? L"(no error captured)" : err);
    Assert::Fail(msg.c_str());
  }

  auto status = TestModule::AwaitCompletion();
  Assert::IsTrue(status == TestStatus::Passed, L"Test did not pass (JS did not call markTestPassed within timeout)");
}
} // namespace

TEST_CLASS (RNTesterHeadlessTests) {
  TEST_CLASS_INITIALIZE(Initialize) {
    // https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/win32/mddbootstrap/nf-mddbootstrap-mddbootstrapinitialize2
    winrt::uninit_apartment();
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    if (FAILED(MddBootstrapInitialize2(
            Microsoft::WindowsAppSDK::Release::MajorMinor,
            Microsoft::WindowsAppSDK::Release::VersionTag,
            {Microsoft::WindowsAppSDK::Runtime::Version::UInt64},
            MddBootstrapInitializeOptions::MddBootstrapInitializeOptions_None))) {
      throw std::exception("Could not initialize Windows App runtime");
    }

    SetRuntimeOptionBool("WebSocket.ResourceV2", true);
  }

  TEST_CLASS_CLEANUP(Cleanup) {
    MddBootstrapShutdown();
  }

  TEST_METHOD(Dummy) {
    RunTest(L"IntegrationTests/DummyTest");
  }

  TEST_METHOD(Fetch) {
    RunTest(L"IntegrationTests/FetchTest");
  }

  TEST_METHOD(XHR) {
    RunTest(L"IntegrationTests/XHRTest");
  }

  TEST_METHOD(WebSocket) {
    RunTest(L"IntegrationTests/WebSocketTest");
  }

  TEST_METHOD(WebSocketBlob) {
    RunTest(L"IntegrationTests/WebSocketBlobTest");
  }

  TEST_METHOD(WebSocketArrayBuffer) {
    RunTest(L"IntegrationTests/WebSocketArrayBufferTest");
  }
};

} // namespace Microsoft::React::Test
