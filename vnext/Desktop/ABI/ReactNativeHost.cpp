#include <pch.h>
#include "ReactNativeHost.h"

#include "Microsoft.ReactNative.ReactNativeHost.g.cpp"

using namespace winrt::Windows::Foundation::Collections;

namespace winrt::Microsoft::ReactNative::implementation {

ReactNativeHost::ReactNativeHost() noexcept {
  // TODO: implement
}

IVector<IReactPackageProvider> ReactNativeHost::PackageProviders() noexcept {
  if (!m_packageProviders) {
    m_packageProviders = single_threaded_vector<IReactPackageProvider>();
  }

  return m_packageProviders;
}

void ReactNativeHost::PackageProviders(IVector<IReactPackageProvider> const &value) noexcept {
  m_packageProviders = value;
}

ReactNative::ReactInstanceSettings ReactNativeHost::InstanceSettings() noexcept {
  if (!m_instanceSettings) {
    // TODO: implement
    // m_instanceSettings = make<ReactInstanceSettings>();
  }

  return m_instanceSettings;
}

void ReactNativeHost::InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept {
  m_instanceSettings = value;
}

void ReactNativeHost::ReloadInstance() noexcept {
  // TODO: implement
}

void ReactNativeHost::OnSuspend() noexcept {
  // TODO: implement
}

void ReactNativeHost::OnEnteredBackground() noexcept {
  // TODO: implement
}

void ReactNativeHost::OnLeavingBackground() noexcept {
  // TODO: implement
}

void ReactNativeHost::OnResume(OnResumeAction const & /*action*/) noexcept {
  // TODO: implement
}

void ReactNativeHost::OnBackPressed() noexcept {
  // TODO: implement
}

} // namespace winrt::Microsoft::ReactNative::implementation
