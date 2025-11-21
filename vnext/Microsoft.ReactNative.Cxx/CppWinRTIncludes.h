// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "NamespaceRedirect.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

#include <winrt/Microsoft.UI.Xaml.h>

#define XAML_CPPWINRT_NAMESPACE winrt::Microsoft::UI::Xaml
namespace xaml = winrt::Microsoft::UI::Xaml;
namespace comp = winrt::Microsoft::UI::Composition;
namespace ui = winrt::Microsoft::UI;
namespace winrt {
using Colors = winrt::Microsoft::UI::Colors;
} // namespace winrt

namespace winrt::Microsoft::UI::Xaml {
using IUIElement7 = UIElement;
using IUIElement9 = UIElement;
using IUIElement10 = UIElement;
} // namespace winrt::Microsoft::UI::Xaml

namespace winrt {
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
} // namespace winrt

#define _QUOTE(x) L#x
#define QUOTE(x) _QUOTE(x)
#define XAML_NAMESPACE_STR QUOTE(XAML_NAMESPACE)
