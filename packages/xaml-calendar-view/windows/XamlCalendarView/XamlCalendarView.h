#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/NativeXamlCalendarViewDataTypes.g.h")
  #include "codegen/NativeXamlCalendarViewDataTypes.g.h"
#endif

#include "codegen/react/components/CalendarView/CalendarView.g.h"

#include "NativeModules.h"

namespace winrt::XamlCalendarView
{

REACT_MODULE(XamlCalendarView)
struct XamlCalendarView
{
  //using ModuleSpec = XamlCalendarViewCodegen::XamlCalendarViewSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_SYNC_METHOD(multiply)
  double multiply(double a, double b) noexcept;

private:
  React::ReactContext m_context;
};

void RegisterCalendarViewNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept;

} // namespace winrt::XamlCalendarView


