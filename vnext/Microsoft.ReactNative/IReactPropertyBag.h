// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ReactPropertyBagHelper.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactPropertyBag : implements<ReactPropertyBag, IReactPropertyBag> {
  ReactPropertyBag() = default;

  IInspectable Get(IReactPropertyName const &name) noexcept;
  IInspectable GetOrCreate(IReactPropertyName const &name, ReactCreatePropertyValue const &createValue) noexcept;
  IInspectable Set(IReactPropertyName const &name, IInspectable const &value) noexcept;

 private:
  std::mutex m_mutex;
  std::map<IReactPropertyName, IInspectable> m_entries;
};

struct ReactPropertyBagHelper {
  ReactPropertyBagHelper() = default;

  static IReactPropertyNamespace GlobalNamespace() noexcept;
  static IReactPropertyNamespace GetNamespace(hstring const &namespaceName) noexcept;
  static IReactPropertyName GetName(IReactPropertyNamespace const &ns, hstring const &localName) noexcept;
  static IReactPropertyBag CreatePropertyBag() noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactPropertyBagHelper
    : ReactPropertyBagHelperT<ReactPropertyBagHelper, implementation::ReactPropertyBagHelper> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
