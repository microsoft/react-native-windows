#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "DevMenuControl.g.h"

namespace winrt::Microsoft::ReactNative::implementation {
    struct DevMenuControl : DevMenuControlT<DevMenuControl>
    {
        DevMenuControl();
    };
}

namespace winrt::Microsoft::ReactNative::factory_implementation {
    struct DevMenuControl : DevMenuControlT<DevMenuControl, implementation::DevMenuControl>
    {
    };
}
