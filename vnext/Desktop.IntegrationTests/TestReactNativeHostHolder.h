// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <unknwn.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Dispatching.h>

#include <functional>
#include <mutex>
#include <string>

namespace Microsoft::React::Test {

namespace msrn = winrt::Microsoft::ReactNative;

struct TestReactNativeHostHolder {
  struct Options {
    bool LoadInstance = true;
  };

  TestReactNativeHostHolder(
      std::wstring_view jsBundle,
      std::function<void(winrt::Microsoft::ReactNative::ReactNativeHost const &)> hostInitializer,
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

} // namespace Microsoft::React::Test
