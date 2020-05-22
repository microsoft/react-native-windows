#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include <winrt/Microsoft.ReactNative.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::<%= name %>::implementation {
  MainPage::MainPage() {
    InitializeComponent();
    ReactRootView().ReactNativeHost();
  }

  winrt::Microsoft::ReactNative::ReactRootView MainPage::ReactRootView() {
    return m_reactRootView;
  }

  void MainPage::ReactRootView(const winrt::Microsoft::ReactNative::ReactRootView &reactRootView) {
    m_reactRootView = reactRootView;
  }
} // namespace winrt::<%=name%>::implementation
