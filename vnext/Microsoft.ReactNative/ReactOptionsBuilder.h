// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ReactOptionsBuilder.g.h"
#include "ReactOptions.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactOptionsBuilder : ReactOptionsBuilderT<ReactOptionsBuilder> {
  ReactOptionsBuilder() = default;

  ReactOptionsBuilder(
      Microsoft::ReactNative::IReactOptions const &options) noexcept;

  Windows::Foundation::Collections::IVector<
      Microsoft::ReactNative::Bridge::IReactPackageProvider>
  PackageProviders() noexcept;

  hstring MainComponentName() noexcept;
  void MainComponentName(hstring const &value) noexcept;

  bool UseDeveloperSupport() noexcept;
  void UseDeveloperSupport(bool value) noexcept;

  hstring JavaScriptMainModuleName() noexcept;
  void JavaScriptMainModuleName(hstring const &value) noexcept;

  hstring JavaScriptBundleFile() noexcept;
  void JavaScriptBundleFile(hstring const &value) noexcept;

  Microsoft::ReactNative::IReactOptions CreateOptions() noexcept;

 private:
  ReactOptionsData m_data;
};


} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactOptionsBuilder : ReactOptionsBuilderT<
                                 ReactOptionsBuilder,
                                 implementation::ReactOptionsBuilder> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
