#pragma once

#include "ReactPackageProvider.g.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::XamlCalendarView::implementation
{

struct ReactPackageProvider : ReactPackageProviderT<ReactPackageProvider>
{
  ReactPackageProvider() = default;

  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept;
};

} // namespace winrt::XamlCalendarView::implementation

namespace winrt::XamlCalendarView::factory_implementation
{

struct ReactPackageProvider : ReactPackageProviderT<ReactPackageProvider, implementation::ReactPackageProvider> {};

} // namespace winrt::XamlCalendarView::factory_implementation
