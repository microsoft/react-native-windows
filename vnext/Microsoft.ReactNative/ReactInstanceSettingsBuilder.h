#pragma once
#include "ReactInstanceSettingsBuilder.g.h"

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactInstanceSettingsBuilder : ReactInstanceSettingsBuilderT<ReactInstanceSettingsBuilder> {
  ReactInstanceSettingsBuilder();

  winrt::Microsoft::ReactNative::ReactInstanceSettings ReactInstanceSettings();
  winrt::Microsoft::ReactNative::ReactInstanceSettingsBuilder SetBundleRootPath(hstring const &path);
  winrt::Microsoft::ReactNative::ReactInstanceSettingsBuilder SetJavaScriptBundleFile(hstring const &file);
  winrt::Microsoft::ReactNative::ReactInstanceSettingsBuilder SetUseFastRefresh(bool const &state);

 private:
  winrt::Microsoft::ReactNative::ReactInstanceSettings m_reactInstanceSettings{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::implementation
namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactInstanceSettingsBuilder
    : ReactInstanceSettingsBuilderT<ReactInstanceSettingsBuilder, implementation::ReactInstanceSettingsBuilder> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
