#pragma once
#include "CoreAppPage.g.h"
#include <winrt/Windows.UI.Xaml.Markup.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct CoreAppPage : CoreAppPageT<CoreAppPage> {
  CoreAppPage();
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct CoreAppPage : CoreAppPageT<CoreAppPage, implementation::CoreAppPage> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
