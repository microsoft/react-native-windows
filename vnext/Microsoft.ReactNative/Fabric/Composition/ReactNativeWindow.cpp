// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "ReactNativeWindow.h"
#include "ReactNativeWindow.g.cpp"

#include <winrt/Microsoft.UI.Content.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.interop.h>


namespace winrt::Microsoft::ReactNative::implementation {

void ReactNativeWindow::UpdateRootViewSizeToAppWindow(
    winrt::Microsoft::ReactNative::ReactNativeIsland const &rootView,
    winrt::Microsoft::UI::Windowing::AppWindow const &window) noexcept {

  if (!rootView) {
    return;
  }

  if (m_resizePolicy != winrt::Microsoft::ReactNative::ContentSizePolicy::ResizeContentToParentWindow) {
    return;
  }

  auto scaleFactor = GetDpiForWindow(winrt::Microsoft::UI::GetWindowFromWindowId(window.Id())) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
  winrt::Windows::Foundation::Size size{
      window.ClientSize().Width / scaleFactor, window.ClientSize().Height / scaleFactor};
  // Do not relayout when minimized
  auto presenter = window.Presenter().try_as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>();
  if (!presenter || presenter.State() != winrt::Microsoft::UI::Windowing::OverlappedPresenterState::Minimized) {
    winrt::Microsoft::ReactNative::LayoutConstraints constraints;
    constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::Undefined;
    constraints.MaximumSize = constraints.MinimumSize = size;
    rootView.Arrange(constraints, {0, 0});
  }
}

ReactNativeWindow::ReactNativeWindow() noexcept
{
}

winrt::Microsoft::ReactNative::ReactNativeWindow ReactNativeWindow::CreateFromCompositor(const winrt::Microsoft::UI::Composition::Compositor& compositor) noexcept
{
  auto reactNativeWindow = make_self<ReactNativeWindow>();
  reactNativeWindow->m_compositor = compositor;
  reactNativeWindow->AppWindow(winrt::Microsoft::UI::Windowing::AppWindow::Create());
  return *reactNativeWindow;
}

winrt::Microsoft::ReactNative::ReactNativeWindow ReactNativeWindow::CreateFromWindow(const winrt::Microsoft::UI::Windowing::AppWindow& appWindow, const winrt::Microsoft::UI::Composition::Compositor& compositor) noexcept
{
  auto reactNativeWindow = make_self<ReactNativeWindow>();
  reactNativeWindow->m_compositor = compositor;
  reactNativeWindow->AppWindow(appWindow);
  return *reactNativeWindow;
}

winrt::Microsoft::ReactNative::ReactNativeWindow ReactNativeWindow::CreateFromWindowAndIsland(const winrt::Microsoft::UI::Windowing::AppWindow& appWindow, const winrt::Microsoft::ReactNative::ReactNativeIsland& island) noexcept
{
  auto reactNativeWindow = make_self<ReactNativeWindow>();
  reactNativeWindow->AppWindow(appWindow);
  reactNativeWindow->ReactNativeIsland(island);
  return *reactNativeWindow;
}

winrt::Microsoft::ReactNative::ReactNativeWindow ReactNativeWindow::CreateFromContentSiteBridgeAndIsland(const winrt::Microsoft::UI::Content::IContentSiteBridge& bridge, const winrt::Microsoft::ReactNative::ReactNativeIsland& island)
{
  auto reactNativeWindow = make_self<ReactNativeWindow>();
  reactNativeWindow->m_bridge = bridge;

  if (auto desktopSiteBridge = bridge.try_as<winrt::Microsoft::UI::Content::DesktopSiteBridge>())
  {
    reactNativeWindow->AppWindow(winrt::Microsoft::UI::Windowing::AppWindow::GetFromWindowId(desktopSiteBridge.WindowId()));
  }
  else if (auto desktopPopupSiteBridge = bridge.as<winrt::Microsoft::UI::Content::DesktopPopupSiteBridge>())
  {
    reactNativeWindow->AppWindow(winrt::Microsoft::UI::Windowing::AppWindow::GetFromWindowId(desktopPopupSiteBridge.WindowId()));
  }
  else
  {
    throw winrt::hresult_not_implemented(L"Unsupported ContentSiteBridge type");
  }

  reactNativeWindow->ReactNativeIsland(island);
  return *reactNativeWindow;
}

ReactNativeWindow::~ReactNativeWindow() noexcept {
  Close();
}

winrt::Microsoft::UI::Windowing::AppWindow ReactNativeWindow::AppWindow() noexcept {

  if (!m_appWindow) {
    AppWindow(winrt::Microsoft::UI::Windowing::AppWindow::Create());
  }

  return m_appWindow;
}

void ReactNativeWindow::AppWindow(const winrt::Microsoft::UI::Windowing::AppWindow &appWindow) noexcept {

  assert(!m_appWindow);

  m_appWindow = appWindow;

  if (!m_appWindow)
    return;

  // Update the size of the RootView when the AppWindow changes size
  m_appWindowChangedToken = m_appWindow.Changed([this](winrt::Microsoft::UI::Windowing::AppWindow const& window,
      winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const& args)
  {
    AppWindow_Changed(window, args);
  });

  UpdateRootViewSizeToAppWindow(m_island, m_appWindow);
}

void ReactNativeWindow::AppWindow_Changed(winrt::Microsoft::UI::Windowing::AppWindow const& window,
  winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const& args) noexcept
{
  if (args.DidSizeChange() || args.DidVisibilityChange())
  {
    UpdateRootViewSizeToAppWindow(m_island, m_appWindow);
  }
}


winrt::Microsoft::ReactNative::ReactNativeIsland ReactNativeWindow::ReactNativeIsland() noexcept {

  if (!m_island) {
    ReactNativeIsland(winrt::Microsoft::ReactNative::ReactNativeIsland(Compositor()));
  }

  return m_island;
}

void ReactNativeWindow::Connect(winrt::Microsoft::UI::Content::ContentIsland contentIsland) noexcept
{
  if (auto desktopSiteBridge = ContentSiteBridge().try_as<winrt::Microsoft::UI::Content::DesktopChildSiteBridge>())
    desktopSiteBridge.Connect(contentIsland);
  else
  {
    // Only support DesktopChildSiteBridge and DesktopPopupSiteBridge currently
    auto popupSiteBridge = m_bridge.as<winrt::Microsoft::UI::Content::DesktopPopupSiteBridge>();
    popupSiteBridge.Connect(contentIsland);
  }
}


void ReactNativeWindow::ReactNativeIsland(const winrt::Microsoft::ReactNative::ReactNativeIsland &value) noexcept {

  if (m_island && m_bridge) {
    Connect(nullptr);
  }

  m_island = value;

  if (m_island) {
      m_island.as<winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCompositionRootView>()
        .SetWindow(reinterpret_cast<uint64_t>(winrt::Microsoft::UI::GetWindowFromWindowId(m_appWindow.Id())));
      Connect(m_island.Island());
      UpdateIslandScaleFactor();
      EnsureResizePolicyState();
  }
}

winrt::Microsoft::UI::Composition::Compositor ReactNativeWindow::Compositor() noexcept
{
  if (!m_compositor)
    m_compositor = ReactNativeIsland().Compositor();

  return m_compositor;
}

winrt::Microsoft::UI::Content::IContentSiteBridge ReactNativeWindow::ContentSiteBridge() noexcept {

  if (!m_bridge) {
    auto desktopChildSiteBridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(Compositor(), m_appWindow.Id());
    desktopChildSiteBridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);
    m_bridge = desktopChildSiteBridge;

    UpdateIslandScaleFactor();

    desktopChildSiteBridge.Show();
  }

  return m_bridge;
}


void ReactNativeWindow::UpdateIslandScaleFactor() noexcept {
  if (m_island) {
    auto scaleFactor = GetDpiForWindow(winrt::Microsoft::UI::GetWindowFromWindowId(m_appWindow.Id())) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
    m_island.ScaleFactor(scaleFactor);
  }
}

void ReactNativeWindow::ResizePolicy(winrt::Microsoft::ReactNative::ContentSizePolicy value) noexcept {

  if (m_resizePolicy == value) {
    return;
  }

  m_sizeChangedRevoker.revoke();

  m_resizePolicy = value;

  EnsureResizePolicyState();
}

void ReactNativeWindow::EnsureResizePolicyState() noexcept
{
  if (!m_island)
    return;

  if (m_resizePolicy == winrt::Microsoft::ReactNative::ContentSizePolicy::ResizeParentWindowToContent)
  {
    m_sizeChangedRevoker = ReactNativeIsland().SizeChanged(winrt::auto_revoke,
      [compositor = Compositor(), appWindow = m_appWindow](
        auto /*sender*/, const winrt::Microsoft::ReactNative::RootViewSizeChangedEventArgs& args)
    {
      auto async = compositor.RequestCommitAsync();
      async.Completed([appWindow, size = args.Size()](
        auto /*asyncInfo*/, winrt::Windows::Foundation::AsyncStatus /*asyncStatus*/)
      {
        appWindow.ResizeClient({ static_cast<int>(size.Width), static_cast<int>(size.Height) });
      });
    });
    ReactNativeIsland().Arrange(m_layoutConstraints, { 0, 0 });
  }

  if (m_island && m_resizePolicy == winrt::Microsoft::ReactNative::ContentSizePolicy::ResizeContentToParentWindow)
    UpdateRootViewSizeToAppWindow(m_island, m_appWindow);

}

winrt::Microsoft::ReactNative::ContentSizePolicy ReactNativeWindow::ResizePolicy() const noexcept {
  return m_resizePolicy;
}

winrt::Microsoft::ReactNative::LayoutConstraints ReactNativeWindow::LayoutConstraints() const noexcept {
  return m_layoutConstraints;
}

void ReactNativeWindow::LayoutConstraints(winrt::Microsoft::ReactNative::LayoutConstraints const &value) noexcept {
  m_layoutConstraints = value;
  ReactNativeIsland().Arrange(m_layoutConstraints, {0, 0});
}

void ReactNativeWindow::Close() noexcept {
  if (m_bridge)
    m_bridge.Close();

  if (m_appWindow) {
    m_appWindow.Changed(m_appWindowChangedToken);
    m_appWindow.Destroy();
  }
}


} // namespace winrt::Microsoft::ReactNative::implementation
