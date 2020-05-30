// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// In this file we override xaml::Application::Current() to return nullptr
// instead of crashing when Application is not found.
#pragma once
#include <winrt/base.h>

#ifndef USE_WINUI3

#ifndef WINRT_Windows_UI_Xaml_2_H
#include <winrt/impl/Windows.UI.Xaml.2.h>

namespace winrt::impl {

template <>
WINRT_IMPL_AUTO(Windows::UI::Xaml::Application)
inline consume_Windows_UI_Xaml_IApplicationStatics<Windows::UI::Xaml::IApplicationStatics>::Current() const {
  using D = Windows::UI::Xaml::IApplicationStatics; // for the WINRT_IMPL_SHIM macro
  void *value{};
  WINRT_IMPL_SHIM(Windows::UI::Xaml::IApplicationStatics)->get_Current(&value);
  return Windows::UI::Xaml::Application{value, take_ownership_from_abi};
}

} // namespace winrt::impl

#endif // WINRT_Windows_UI_Xaml_2_H

#else // USE_WINUI3

#ifndef WINRT_Microsoft_UI_Xaml_2_H
#include <winrt/impl/Microsoft.UI.Xaml.2.h>

namespace winrt::impl {

template <>
WINRT_IMPL_AUTO(Microsoft::UI::Xaml::Application)
inline consume_Microsoft_UI_Xaml_IApplicationStatics<Microsoft::UI::Xaml::IApplicationStatics>::Current() const {
  using D = Microsoft::UI::Xaml::IApplicationStatics; // for the WINRT_IMPL_SHIM macro
  void *value{};
  WINRT_IMPL_SHIM(Microsoft::UI::Xaml::IApplicationStatics)->get_Current(&value);
  return Microsoft::UI::Xaml::Application{value, take_ownership_from_abi};
}

} // namespace winrt::impl

#endif // WINRT_Microsoft_UI_Xaml_2_H

#endif // USE_WINUI3
