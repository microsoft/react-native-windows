// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestReactNativeHostHolder.h"

namespace ReactNativeIntegrationTests {

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::System;

TestReactNativeHostHolder::TestReactNativeHostHolder(
    std::wstring_view jsBundle,
    Mso::Functor<void(ReactNativeHost const &)> &&hostInitializer) noexcept {
  m_host = ReactNativeHost{};
  m_queueController = DispatcherQueueController::CreateOnDedicatedThread();
  m_queueController.DispatcherQueue().TryEnqueue(
      [this, jsBundle = std::wstring{jsBundle}, hostInitializer = std::move(hostInitializer)]() noexcept {
        // bundle is assumed to be co-located with the test binary
        wchar_t testBinaryPath[MAX_PATH];
        TestCheck(GetModuleFileNameW(NULL, testBinaryPath, MAX_PATH) < MAX_PATH);
        testBinaryPath[std::wstring_view{testBinaryPath}.rfind(L"\\")] = 0;

        m_host.InstanceSettings().BundleRootPath(testBinaryPath);
        m_host.InstanceSettings().JavaScriptBundleFile(jsBundle);
        m_host.InstanceSettings().UseDeveloperSupport(false);
        m_host.InstanceSettings().UseWebDebugger(false);
        m_host.InstanceSettings().UseFastRefresh(false);
        m_host.InstanceSettings().UseLiveReload(false);
        m_host.InstanceSettings().EnableDeveloperMenu(false);

        hostInitializer(m_host);

        m_host.LoadInstance();
      });
}

TestReactNativeHostHolder::~TestReactNativeHostHolder() noexcept {
  m_host.UnloadInstance().get();
  m_queueController.ShutdownQueueAsync().get();
}

} // namespace ReactNativeIntegrationTests
