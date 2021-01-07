// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CppWinRTIncludes.h"

namespace Microsoft::ReactNative {

using XamlView = xaml::DependencyObject;

inline winrt::IPropertyValue GetTagAsPropertyValue(xaml::FrameworkElement fe) {
  assert(fe);
  return fe.GetValue(xaml::FrameworkElement::TagProperty()).try_as<winrt::IPropertyValue>();
}

inline int64_t GetTag(XamlView view) {
  if (auto fe = view.try_as<xaml::FrameworkElement>()) {
    return GetTagAsPropertyValue(fe).GetInt64();
  }
  return 0;
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

xaml::XamlRoot TryGetXamlRoot(const XamlView &view);
comp::Compositor GetCompositor(const XamlView &view);
void SetCompositor(const comp::Compositor &compositor);
comp::Compositor GetCompositor();

} // namespace Microsoft::ReactNative
