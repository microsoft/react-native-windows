// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <functional/functor.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.System.h>
#include <string_view>

namespace ReactNativeIntegrationTests {

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

 private:
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  winrt::Windows::System::DispatcherQueueController m_queueController{nullptr};
};

} // namespace ReactNativeIntegrationTests
