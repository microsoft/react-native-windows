#include "pch.h"
#include "ReactNativeApplication.h"
#include "ReactNativeApplication.g.cpp"
#include "ReactNativeIsland.h"

namespace winrt::ReactNative
{
  using namespace winrt::Microsoft::ReactNative;
} // namespace winrt::ReactNative

namespace winrt::UI
{
  using namespace winrt::Microsoft::UI;
} // namespace winrt::UI

namespace winrt::Microsoft::ReactNative::implementation
{
  winrt::ReactNative::ReactNativeHost ReactNativeApplication::CreateReactNativeHost(
      winrt::ReactNative::ReactApplicationInstanceSettings const &settings,
      winrt::UI::Windowing::AppWindow const &window,
      winrt::UI::Composition::Compositor const &compositor)
  {
    auto host{winrt::Microsoft::ReactNative::ReactNativeHost()};

#if BUNDLE
    host.InstanceSettings().JavaScriptBundleFile(L"index.windows");
    host.InstanceSettings().BundleRootPath(settings.bundleRootPath.c_str());
    host.InstanceSettings().UseFastRefresh(false);
#else
    host.InstanceSettings().JavaScriptBundleFile(L"index");
    host.InstanceSettings().UseFastRefresh(true);
#endif

#if _DEBUG
    host.InstanceSettings().UseDirectDebugger(true);
    host.InstanceSettings().UseDeveloperSupport(true);
#else
    host.InstanceSettings().UseDirectDebugger(false);
    host.InstanceSettings().UseDeveloperSupport(false);
#endif

    auto hwnd{winrt::UI::GetWindowFromWindowId(window.Id())};

    winrt::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
        host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

    winrt::ReactNative::Composition::CompositionUIService::SetCompositor(host.InstanceSettings(), compositor);

    return host;
}

  winrt::ReactNative::ReactNativeIsland ReactNativeApplication::CreateReactNativeIsland(
    winrt::UI::Composition::Compositor const &compositor,
    winrt::UI::Windowing::AppWindow const &window,
    winrt::ReactNative::ReactNativeHost const &host,
    hstring const &mainComponentName)
  {
    return winrt::make<ReactNativeIsland>(compositor, window, host, mainComponentName);
  }
} // namespace winrt::Microsoft::ReactNative::implementation
