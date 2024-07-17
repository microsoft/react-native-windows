#include "pch.h"
#include "ReactNativeAppBuilder.h"
#include "ReactNativeAppBuilder.g.cpp"
#include "ReactNativeHost.h"
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
}

ReactNativeAppBuilder::~ReactNativeAppBuilder()
{
  m_desktopChildSiteBridge.Close();
  m_desktopChildSiteBridge = nullptr;

  // Destroy all Composition objects
  m_compositor.Close();
  m_compositor = nullptr;
}

winrt::Microsoft::UI::Content::DesktopChildSiteBridge ReactNativeAppBuilder::DesktopChildSiteBridge() {
  return m_desktopChildSiteBridge;
}

winrt::Microsoft::ReactNative::ReactNativeIsland ReactNativeAppBuilder::ReactNativeIsland() {
  return m_reactNativeIsland;
}

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
  m_compositor = compositor;

  return *this;
}

winrt::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetAppWindow(
    winrt::Microsoft::UI::Windowing::AppWindow const &appWindow) {
  m_appWindow = appWindow;

  return *this;
}

winrt::Microsoft::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetReactViewOptions(
    winrt::Microsoft::ReactNative::ReactViewOptions const &reactViewOptions) {
  m_reactViewOptions = reactViewOptions;

  return *this;
}

winrt::ReactNative::ReactNativeHost ReactNativeAppBuilder::Start() {
  if (m_compositor == nullptr)
  {
    // Create a Compositor for all Content
    m_compositor = winrt::Microsoft::UI::Composition::Compositor();
  }

  /*
  if (m_appWindow == nullptr)
  {
    m_appWindow = winrt::Microsoft::UI::Windowing::AppWindow::Create();
    m_appWindow.Title(L"Sample-App-Fabric");
    m_appWindow.Resize({1000, 1000});
    m_appWindow.Show();
  }
  */

  auto hwnd{winrt::UI::GetWindowFromWindowId(m_appWindow.Id())};

  winrt::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      m_host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

  winrt::ReactNative::Composition::CompositionUIService::SetCompositor(m_host.InstanceSettings(), m_compositor);

  // Start the react-native instance, which will create a JavaScript runtime and load the applications bundle.
  m_host.ReloadInstance();

  // Create a RootView which will present a react-native component
  m_reactNativeIsland = winrt::Microsoft::ReactNative::ReactNativeIsland(m_compositor);
  m_reactNativeIsland.ReactViewHost(
      winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(m_host, m_reactViewOptions));

  // DesktopChildSiteBridge create a ContentSite that can host the RootView ContentIsland
  m_desktopChildSiteBridge =
      winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(m_compositor, m_appWindow.Id());
  m_desktopChildSiteBridge.Connect(m_reactNativeIsland.Island());

  m_desktopChildSiteBridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

  return m_host;
}

} // namespace winrt::Microsoft::ReactNative::implementation
