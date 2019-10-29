#include "pch.h"
#include "WinUI2.h"

winrt::Windows::UI::Xaml::DependencyObject WinUI2::CreateColorPicker() {
  return winrt::Microsoft::UI::Xaml::Controls::ColorPicker();
}
