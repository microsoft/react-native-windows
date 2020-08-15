#include "pch.h"
#include "DevMenuControl.h"
#if __has_include("DevMenuControl.g.cpp")
#include "DevMenuControl.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Microsoft::ReactNative::implementation
{
    DevMenuControl::DevMenuControl()
    {
        InitializeComponent();
    }
}
