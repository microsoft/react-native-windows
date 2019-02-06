// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace folly {
  struct dynamic;
}

namespace react { namespace uwp {

winrt::Windows::UI::Color ColorFrom(const folly::dynamic& d);
winrt::Windows::UI::Xaml::Media::Brush BrushFrom(const folly::dynamic& d);
winrt::Windows::UI::Xaml::Media::SolidColorBrush SolidColorBrushFrom(const folly::dynamic& d);
winrt::Windows::UI::Xaml::VerticalAlignment VerticalAlignmentFrom(const folly::dynamic& d);
winrt::Windows::UI::Xaml::HorizontalAlignment HorizontalAlignmentFrom(const folly::dynamic& d);

std::wstring asWStr(const folly::dynamic& d);
winrt::hstring asHstring(const folly::dynamic& d);
folly::dynamic HstringToDynamic(winrt::hstring hstr);

winrt::Windows::Foundation::TimeSpan TimeSpanFromMs(double ms);

} }
