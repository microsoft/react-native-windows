// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactOptions.h"

using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Microsoft::ReactNative::Bridge;

namespace winrt::Microsoft::ReactNative {

template <class TItem, template <class> class TWinRTVector>
std::vector<TItem> ToStdVector(TWinRTVector<TItem> const &vector) noexcept {
  std::vector<TItem> result;
  result.resize(vector.Size());
  vector.GetMany(0, result);
  return result;
}

ReactOptionsData::ReactOptionsData(
    Microsoft::ReactNative::IReactOptions const &options) noexcept
    : PackageProviders{single_threaded_vector(
          ToStdVector(options.PackageProviders()))},
      MainComponentName{options.MainComponentName()},
      UseDeveloperSupport{options.UseDeveloperSupport()},
      JavaScriptMainModuleName{options.JavaScriptMainModuleName()},
      JavaScriptBundleFile{options.JavaScriptBundleFile()} {}

ReactOptionsData ReactOptionsData::DeepCopy() const noexcept {
  ReactOptionsData dataCopy{*this};
  dataCopy.PackageProviders =
      single_threaded_vector(ToStdVector(PackageProviders));
  return dataCopy;
}

ReactOptions::ReactOptions(ReactOptionsData &&data) noexcept
    : m_data{std::move(data)} {}

IVectorView<IReactPackageProvider> ReactOptions::PackageProviders() noexcept {
  return m_data.PackageProviders.GetView();
}

hstring ReactOptions::MainComponentName() noexcept {
  return m_data.MainComponentName;
}

bool ReactOptions::UseDeveloperSupport() noexcept {
  return m_data.UseDeveloperSupport;
}

hstring ReactOptions::JavaScriptMainModuleName() noexcept {
  return m_data.JavaScriptMainModuleName;
}

hstring ReactOptions::JavaScriptBundleFile() noexcept {
  return m_data.JavaScriptBundleFile;
}

} // namespace winrt::Microsoft::ReactNative
