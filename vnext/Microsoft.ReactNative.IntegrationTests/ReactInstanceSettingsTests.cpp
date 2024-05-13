// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Microsoft.ReactNative.h>

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Dispatching.h>
#else
#include <winrt/Windows.System.h>
#endif

using namespace winrt;
using namespace Microsoft::ReactNative;

#ifdef USE_WINUI3
using namespace Microsoft::UI::Dispatching;
#else
using namespace Windows::System;
#endif

namespace ReactNativeIntegrationTests {

TEST_CLASS (ReactInstanceSettingsTests) {
  TEST_METHOD(DefaultUIDispatcher_NonUIThread) {
    // In case if current thread has no ThreadDispatcher, then the
    // ReactInstanceSettings::UIDispatcher is not initialized.
    ReactInstanceSettings settings;
    TestCheck(settings);
    TestCheck(!settings.UIDispatcher());
    // UIDispatcher() is a shortcut for getting property value.
    TestCheck(!settings.Properties().Get(ReactDispatcherHelper::UIDispatcherProperty()));
  }

  TEST_METHOD(DefaultUIDispatcher_UIThread) {
    // ReactInstanceSettings::UIDispatcher is set to a non-null value if it is
    // creates from a UI thread. We simulate the UI thread with DispatcherQueueController.
    auto queueController = DispatcherQueueController::CreateOnDedicatedThread();
    auto uiDispatcher = queueController.DispatcherQueue();
    TestCheck(uiDispatcher);
    uiDispatcher.TryEnqueue([]() noexcept {
      ReactInstanceSettings settings;
      TestCheck(settings);
      TestCheck(settings.UIDispatcher());
      // UIDispatcher() is a shortcut for getting property value.
      TestCheck(settings.Properties().Get(ReactDispatcherHelper::UIDispatcherProperty()));
      TestCheckEqual(ReactDispatcherHelper::UIThreadDispatcher(), settings.UIDispatcher());
      TestCheckEqual(
          ReactDispatcherHelper::UIThreadDispatcher(),
          settings.Properties().Get(ReactDispatcherHelper::UIDispatcherProperty()));
    });
    queueController.ShutdownQueueAsync().get();
  }
};

} // namespace ReactNativeIntegrationTests
