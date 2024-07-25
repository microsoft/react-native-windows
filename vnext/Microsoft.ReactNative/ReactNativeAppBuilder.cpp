#include "pch.h"
#include "ReactNativeAppBuilder.h"
#include "ReactNativeAppBuilder.g.cpp"
#include "ReactNativeHost.h"
#include "ReactNativeWin32App.h"
#include "IReactDispatcher.h"
#include "winrt/Microsoft.UI.Composition.h"
#include "winrt/Microsoft.UI.Windowing.h"
#include "winrt/microsoft.UI.Interop.h"

// Scaling factor for the window's content based on the DPI of the display where the window is located.
float ScaleFactor(HWND hwnd) noexcept {
  return GetDpiForWindow(hwnd) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

namespace winrt::ReactNative {
using namespace winrt::Microsoft::ReactNative;
}

namespace winrt::UI {
using namespace winrt::Microsoft::UI;
}

namespace winrt::Microsoft::ReactNative::implementation {
ReactNativeAppBuilder::ReactNativeAppBuilder() {
  m_reactNativeWin32App = winrt::make<implementation::ReactNativeWin32App>();
}

ReactNativeAppBuilder::~ReactNativeAppBuilder() {}

winrt::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::AddPackageProviders(
    winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::IReactPackageProvider> const
        &packageProviders) {
  for (auto const &provider : packageProviders) {
    m_reactNativeWin32App.ReactNativeHost().PackageProviders().Append(provider);
  }

  return *this;
}

winrt::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetReactInstanceSettings(
    winrt::Microsoft::ReactNative::ReactInstanceSettings const &settings) {
  m_reactNativeWin32App.ReactNativeHost().InstanceSettings(settings);

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
    // Create a DispatcherQueue for this thread.  This is needed for Composition, Content, and
    // Input APIs.
    auto dispatcherQueueController =
        winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread();

    m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->DispatchQueueController(
        dispatcherQueueController);

    // Create the compositor on behalf of the App Developer
    auto compositor = winrt::Microsoft::UI::Composition::Compositor();
    m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->Compositor(compositor);
  }

  // Create the AppWindow if the developer doesn't provide one
  if (m_reactNativeWin32App.AppWindow() == nullptr) {
    auto appWindow = winrt::Microsoft::UI::Windowing::AppWindow::Create();
    appWindow.Title(L"Sample-App-Fabric");
    appWindow.Resize({1000, 1000});
    appWindow.Show();

    m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->AppWindow(appWindow);
  }

  // Currently set the property to use current thread dispatcher as a default UI dispatcher.
  m_reactNativeWin32App.ReactNativeHost().InstanceSettings().Properties().Set(
      ReactDispatcherHelper::UIDispatcherProperty(), ReactDispatcherHelper::UIThreadDispatcher());
  
  auto hwnd{winrt::UI::GetWindowFromWindowId(m_reactNativeWin32App.AppWindow().Id())};

  winrt::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      m_reactNativeWin32App.ReactNativeHost().InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

  winrt::ReactNative::Composition::CompositionUIService::SetCompositor(
      m_reactNativeWin32App.ReactNativeHost().InstanceSettings(), m_reactNativeWin32App.Compositor());

  // Start the react-native instance, which will create a JavaScript runtime and load the applications bundle.
  m_reactNativeWin32App.ReactNativeHost().ReloadInstance();

  // Create a RootView which will present a react-native component
  auto reactNativeIsland = winrt::Microsoft::ReactNative::ReactNativeIsland(m_reactNativeWin32App.Compositor());
  reactNativeIsland.ReactViewHost(winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(
      m_reactNativeWin32App.ReactNativeHost(), m_reactViewOptions));

  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->ReactNativeIsland(std::move(reactNativeIsland));

  // DesktopChildSiteBridge create a ContentSite that can host the RootView ContentIsland
  auto desktopChildSiteBridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(
      m_reactNativeWin32App.Compositor(), m_reactNativeWin32App.AppWindow().Id());

  desktopChildSiteBridge.Connect(m_reactNativeWin32App.ReactNativeIsland().Island());

  desktopChildSiteBridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

  auto scaleFactor = ScaleFactor(hwnd);
  m_reactNativeWin32App.ReactNativeIsland().ScaleFactor(scaleFactor);

  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->DesktopChildSiteBridge(
      std::move(desktopChildSiteBridge));

  return m_reactNativeWin32App;
}

} // namespace winrt::Microsoft::ReactNative::implementation
