// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Text.h>

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Text.h>
namespace text = winrt::Microsoft::UI::Text;
namespace winrt::Microsoft::UI::Text {
using FontStyle = winrt::Windows::UI::Text::FontStyle;
}
#else
namespace text = winrt::Windows::UI::Text;
#endif //  USE_WINUI3
