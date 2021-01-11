// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Xaml.Controls.h>
namespace winrt::Microsoft::UI::Xaml::Controls {
using IControl7 = Control;
} // namespace winrt::Microsoft::UI::Xaml::Controls
#else
#include <winrt/Windows.UI.Xaml.Controls.h>
#endif //  USE_WINUI3
