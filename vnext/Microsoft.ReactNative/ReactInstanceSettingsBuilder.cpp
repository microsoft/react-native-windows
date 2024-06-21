#include "pch.h"
#include "ReactInstanceSettingsBuilder.h"
#include "ReactInstanceSettingsBuilder.g.cpp"
#include "ReactInstanceSettings.h"

namespace winrt::ReactNative {
using namespace winrt::Microsoft::ReactNative;
}

namespace winrt::UI {
using namespace winrt::Microsoft::UI;
}

namespace winrt::Microsoft::ReactNative::implementation {
ReactInstanceSettingsBuilder::ReactInstanceSettingsBuilder() {
  m_reactInstanceSettings = winrt::make<winrt::ReactNative::implementation::ReactInstanceSettings>();
}

winrt::ReactNative::ReactInstanceSettings ReactInstanceSettingsBuilder::ReactInstanceSettings() {
  return m_reactInstanceSettings;
}

winrt::ReactNative::ReactInstanceSettingsBuilder ReactInstanceSettingsBuilder::UseDirectDebugger(bool const& state) {
  m_reactInstanceSettings.UseDirectDebugger(state);

  return *this;
}

winrt::ReactNative::ReactInstanceSettingsBuilder ReactInstanceSettingsBuilder::UseDeveloperSupport(bool const& state) {
  m_reactInstanceSettings.UseDeveloperSupport(state);

  return *this;
}

winrt::ReactNative::ReactInstanceSettingsBuilder ReactInstanceSettingsBuilder::BundleRootPath(hstring const &path) {
  m_reactInstanceSettings.BundleRootPath(path.c_str());

  return *this;
}

winrt::ReactNative::ReactInstanceSettingsBuilder ReactInstanceSettingsBuilder::JavaScriptBundleFile(hstring const &file) {
  m_reactInstanceSettings.JavaScriptBundleFile(file.c_str());

  return *this;
}

winrt::ReactNative::ReactInstanceSettingsBuilder ReactInstanceSettingsBuilder::UseFastRefresh(bool const &state) {
  m_reactInstanceSettings.UseFastRefresh(state);

  return *this;
}
} // namespace winrt::Microsoft::ReactNative::implementation
