// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CppWinRTIncludes.h"

namespace Microsoft::ReactNative {

using XamlView = xaml::DependencyObject;
constexpr int64_t InvalidTag = -1;

inline int64_t GetTag(XamlView view) {
  auto tagValue = view.ReadLocalValue(xaml::FrameworkElement::TagProperty());
  if (tagValue != xaml::DependencyProperty::UnsetValue()) {
    if (auto tagValueInt = tagValue.try_as<winrt::IPropertyValue>()) {
      if (tagValueInt.Type() == winrt::PropertyType::Int64) {
        return tagValueInt.GetInt64();
      }
    }
  }

  return InvalidTag;
}

inline void SetTag(XamlView view, int64_t tag) {
  view.SetValue(xaml::FrameworkElement::TagProperty(), winrt::PropertyValue::CreateInt64(tag));
}

inline void SetTag(XamlView view, winrt::IInspectable tag) {
  SetTag(view, tag.as<winrt::IPropertyValue>().GetInt64());
}

inline bool IsValidTag(winrt::IPropertyValue value) {
  assert(value);
  return (value.Type() == winrt::PropertyType::Int64);
}

inline int64_t GetTag(winrt::IPropertyValue value) {
  assert(value);
  return value.GetInt64();
}

inline winrt::IPropertyValue GetTagAsPropertyValue(XamlView view) {
  assert(view);
  return view.GetValue(xaml::FrameworkElement::TagProperty()).try_as<winrt::IPropertyValue>();
}

xaml::XamlRoot TryGetXamlRoot(const XamlView &view);
comp::Compositor GetCompositor(const XamlView &view = nullptr);
void SetCompositor(const comp::Compositor &compositor);

} // namespace Microsoft::ReactNative
