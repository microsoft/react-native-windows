#pragma once

#include <Microsoft.React.WinUI.g.h>

namespace winrt::Microsoft::React::implementation {
struct WinUI : WinUIT<WinUI> {
  WinUI() = default;

  int32_t MyProperty();
  void MyProperty(int32_t value);
};
} // namespace winrt::Microsoft::React::implementation

namespace winrt::Microsoft::React::factory_implementation {
struct WinUI : WinUIT<WinUI, implementation::WinUI> {};
} // namespace winrt::WinUI::factory_implementation
