#include "pch.h"

#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif

#include "XamlCalendarView.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::XamlCalendarView::implementation
{

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept
{
  AddAttributedModules(packageBuilder, true);
  RegisterCalendarViewNativeComponent(packageBuilder);
}

} // namespace winrt::XamlCalendarView::implementation
