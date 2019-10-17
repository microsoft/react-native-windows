// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactOptionsBuilder.h"
#include "ReactOptionsBuilder.g.cpp"
#include "ReactOptions.h"

using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Microsoft::ReactNative::Bridge;

namespace winrt::Microsoft::ReactNative::implementation {

ReactOptionsBuilder::ReactOptionsBuilder(
    Microsoft::ReactNative::IReactOptions const &options) noexcept
    : m_data{options} {}

IVector<IReactPackageProvider>
ReactOptionsBuilder::PackageProviders() noexcept {
  return m_data.PackageProviders;
}

hstring ReactOptionsBuilder::MainComponentName() noexcept {
  return m_data.MainComponentName;
}

void ReactOptionsBuilder::MainComponentName(hstring const &value) noexcept {
  m_data.MainComponentName = value;
}

bool ReactOptionsBuilder::UseDeveloperSupport() noexcept {
  return m_data.UseDeveloperSupport;
}

void ReactOptionsBuilder::UseDeveloperSupport(bool value) noexcept {
  m_data.UseDeveloperSupport = value;
}

hstring ReactOptionsBuilder::JavaScriptMainModuleName() noexcept {
  return m_data.JavaScriptMainModuleName;
}

void ReactOptionsBuilder::JavaScriptMainModuleName(
    hstring const &value) noexcept {
  m_data.JavaScriptMainModuleName = value;
}

hstring ReactOptionsBuilder::JavaScriptBundleFile() noexcept {
  return m_data.JavaScriptBundleFile;
}

void ReactOptionsBuilder::JavaScriptBundleFile(hstring const &value) noexcept {
  m_data.JavaScriptBundleFile = value;
}

IReactOptions ReactOptionsBuilder::CreateOptions() noexcept {
  return make<ReactOptions>(m_data.DeepCopy());
}

} // namespace winrt::Microsoft::ReactNative::implementation
