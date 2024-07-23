#include "pch.h"
#include "ReactNativeAppBuilder.h"
#include "ReactNativeAppBuilder.g.cpp"
#include "ReactNativeHost.h"
#include "ReactNativeWin32App.h"
#include "winrt/Microsoft.UI.Composition.h"
#include "winrt/Microsoft.UI.Windowing.h"
#include "winrt/microsoft.UI.Interop.h"

namespace winrt::ReactNative {
using namespace winrt::Microsoft::ReactNative;
}

namespace winrt::UI {
using namespace winrt::Microsoft::UI;
}

namespace winrt::Microsoft::ReactNative::implementation {
ReactNativeAppBuilder::ReactNativeAppBuilder() {
  m_host = winrt::make<winrt::ReactNative::implementation::ReactNativeHost>();

  m_reactNativeWin32App = winrt::make<implementation::ReactNativeWin32App>();
}

ReactNativeAppBuilder::~ReactNativeAppBuilder() {}

winrt::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::AddPackageProviders(
    winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::IReactPackageProvider> const
        &packageProviders) {
  for (auto const &provider : packageProviders) {
    m_host.PackageProviders().Append(provider);
  }

  return *this;
}

winrt::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetReactInstanceSettings(
    winrt::Microsoft::ReactNative::ReactInstanceSettings const &settings) {
  m_host.InstanceSettings(settings);

  return *this;
}

winrt::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetCompositor(
    winrt::Microsoft::UI::Composition::Compositor const &compositor) {
  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->Compositor(compositor);
  return *this;
}

winrt::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetAppWindow(
    winrt::Microsoft::UI::Windowing::AppWindow const &appWindow) {
  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->AppWindow(appWindow);

  return *this;
}

winrt::Microsoft::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetReactViewOptions(
    winrt::Microsoft::ReactNative::ReactViewOptions const &reactViewOptions) {
  m_reactViewOptions = reactViewOptions;

  return *this;
}

winrt::ReactNative::ReactNativeWin32App ReactNativeAppBuilder::Build() {
  if (m_reactNativeWin32App.Compositor() == nullptr) {
    m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->Compositor(nullptr);
  }

  if (m_reactNativeWin32App.AppWindow() == nullptr) {
    auto appWindow = winrt::Microsoft::UI::Windowing::AppWindow::Create();
    appWindow.Title(L"Sample-App-Fabric");
    appWindow.Resize({1000, 1000});
    appWindow.Show();

    m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->AppWindow(appWindow);
  }

  auto hwnd{winrt::UI::GetWindowFromWindowId(m_reactNativeWin32App.AppWindow().Id())};

  winrt::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      m_host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

  winrt::ReactNative::Composition::CompositionUIService::SetCompositor(
      m_host.InstanceSettings(), m_reactNativeWin32App.Compositor());

  // Start the react-native instance, which will create a JavaScript runtime and load the applications bundle.
  m_host.ReloadInstance();

  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->ReactNativeHost(std::move(m_host));

  // Create a RootView which will present a react-native component
  auto reactNativeIsland = winrt::Microsoft::ReactNative::ReactNativeIsland(m_reactNativeWin32App.Compositor());
  reactNativeIsland.ReactViewHost(winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(
      m_reactNativeWin32App.ReactNativeHost(), m_reactViewOptions));

  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->ReactNativeIsland(reactNativeIsland);

  // DesktopChildSiteBridge create a ContentSite that can host the RootView ContentIsland
  auto desktopChildSiteBridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(
      m_reactNativeWin32App.Compositor(), m_reactNativeWin32App.AppWindow().Id());

  desktopChildSiteBridge.Connect(m_reactNativeWin32App.ReactNativeIsland().Island());

  desktopChildSiteBridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->DesktopChildSiteBridge(desktopChildSiteBridge);

  return m_reactNativeWin32App;
}

} // namespace winrt::Microsoft::ReactNative::implementation
