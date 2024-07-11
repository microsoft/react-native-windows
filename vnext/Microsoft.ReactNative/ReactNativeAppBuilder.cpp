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

winrt::ReactNative::ReactNativeHost ReactNativeAppBuilder::Start() {
  auto hwnd{winrt::UI::GetWindowFromWindowId(m_appWindow.Id())};

  winrt::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      m_host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

  winrt::ReactNative::Composition::CompositionUIService::SetCompositor(m_host.InstanceSettings(), m_compositor);

  // Start the react-native instance, which will create a JavaScript runtime and load the applications bundle.
  m_host.ReloadInstance();

  return m_host;
}
} // namespace winrt::Microsoft::ReactNative::implementation
