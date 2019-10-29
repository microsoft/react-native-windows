#include "pch.h"
#include "Microsoft.React.WinUI.h"
#if __has_include("Microsoft.React.WinUI.g.cpp")
#include "Microsoft.React.WinUI.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Microsoft::React;

namespace winrt::Microsoft::React::implementation {
int32_t WinUI::MyProperty() {
  throw hresult_not_implemented();
}

void WinUI::MyProperty(int32_t /*value*/) {
  throw hresult_not_implemented();
}
} // namespace winrt::Microsoft::React::implementation
