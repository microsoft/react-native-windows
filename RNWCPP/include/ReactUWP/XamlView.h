// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>

namespace winrt {
  using namespace Windows::UI::Xaml;
  using namespace Windows::Foundation;
}

namespace react { namespace uwp {

typedef winrt::DependencyObject XamlView;

inline int64_t GetTag(XamlView view)
{
  return view.GetValue(winrt::FrameworkElement::TagProperty()).as<winrt::IPropertyValue>().GetInt64();
}

} }
