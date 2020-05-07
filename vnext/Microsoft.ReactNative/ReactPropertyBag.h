// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ReactPropertyBag.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactPropertyBag : ReactPropertyBagT<ReactPropertyBag> {
  ReactPropertyBag() = default;

  static Microsoft::ReactNative::IReactPropertyNamespace GlobalNamespace() noexcept;
  static Microsoft::ReactNative::IReactPropertyNamespace GetNamespace(hstring const &namespaceName) noexcept;
  static Microsoft::ReactNative::IReactPropertyName GetName(
      Microsoft::ReactNative::IReactPropertyNamespace const &ns,
      hstring const &localName) noexcept;
  Windows::Foundation::IInspectable GetProperty(
      Microsoft::ReactNative::IReactPropertyName const &propertyName) noexcept;
  Windows::Foundation::IInspectable GetOrCreateProperty(
      Microsoft::ReactNative::IReactPropertyName const &propertyName,
      Microsoft::ReactNative::ReactCreatePropertyValue const &createValue) noexcept;
  Windows::Foundation::IInspectable SetProperty(
      Microsoft::ReactNative::IReactPropertyName const &propertyName,
      Windows::Foundation::IInspectable const &value) noexcept;
  Windows::Foundation::IInspectable RemoveProperty(
      Microsoft::ReactNative::IReactPropertyName const &propertyName) noexcept;

 private:
  std::mutex m_mutex;
  std::map<Microsoft::ReactNative::IReactPropertyName, Windows::Foundation::IInspectable> m_entries;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactPropertyBag : ReactPropertyBagT<ReactPropertyBag, implementation::ReactPropertyBag> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
