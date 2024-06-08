// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CompositionHwndHost.h"
#include "CompositionHwndHost.g.cpp"

#include <Fabric/Composition/CompositionUIService.h>
#include <QuirkSettings.h>
#include <ReactHost/MsoUtils.h>
#include <Utils/Helpers.h>
#include <dispatchQueue/dispatchQueue.h>
#include <windowsx.h>
#include <winrt/Windows.UI.Core.h>
#include "CompositionContextHelper.h"
#include "ReactNativeHost.h"

#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include "CompositionRootAutomationProvider.h"
#include "ReactNativeIsland.h"

#if USE_WINUI3
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.interop.h>
#endif

WINUSERAPI UINT WINAPI GetDpiForWindow(_In_ HWND hwnd);

namespace winrt::Microsoft::ReactNative::implementation {

CompositionHwndHost::CompositionHwndHost() noexcept {}

void CompositionHwndHost::Initialize(uint64_t hwnd) noexcept {
  m_hwnd = (HWND)hwnd;

  auto compositionContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          ReactViewHost().ReactNativeHost().InstanceSettings().Properties());
#if USE_WINUI3
  if (auto liftedCompositor =
          winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerCompositor(
              compositionContext)) {
    m_compRootView = winrt::Microsoft::ReactNative::ReactNativeIsland(liftedCompositor);

    auto bridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(
        liftedCompositor, winrt::Microsoft::UI::GetWindowIdFromWindow(m_hwnd));

    auto island = m_compRootView.Island();

    bridge.Connect(island);
    bridge.Show();

    m_compRootView.ScaleFactor(ScaleFactor());
    bridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);
  } else {
    m_compRootView = winrt::Microsoft::ReactNative::ReactNativeIsland();
    m_compRootView.as<winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCompositionRootView>()
        .SetWindow(reinterpret_cast<uint64_t>(m_hwnd));

#endif
    auto compositor =
        winrt::Microsoft::ReactNative::Composition::Experimental::SystemCompositionContextHelper::InnerCompositor(
            compositionContext);
    auto interop = compositor.as<ABI::Windows::UI::Composition::Desktop::ICompositorDesktopInterop>();
    winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget target{nullptr};
    check_hresult(interop->CreateDesktopWindowTarget(
        m_hwnd,
        false,
        reinterpret_cast<ABI::Windows::UI::Composition::Desktop::IDesktopWindowTarget **>(put_abi(target))));

    auto root = compositor.CreateContainerVisual();
    root.RelativeSizeAdjustment({1.0f, 1.0f});
    root.Offset({0, 0, 0});
    root.Comment(L"Root Visual");
    target.Root(root);

    m_compRootView.as<winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCompositionRootView>()
        .InternalRootVisual(
            winrt::Microsoft::ReactNative::Composition::Experimental::SystemCompositionContextHelper::CreateVisual(
                target.Root()));

#if USE_WINUI3
  }
#endif

  m_compRootView.ReactViewHost(std::move(m_reactViewHost));
  m_compRootView.ScaleFactor(ScaleFactor());
  UpdateSize();
}

float CompositionHwndHost::ScaleFactor() noexcept {
  return GetDpiForWindow(m_hwnd) / 96.0f;
}

void CompositionHwndHost::UpdateSize() noexcept {
  RECT rc;
  if (GetClientRect(m_hwnd, &rc)) {
    if (m_height != (rc.bottom - rc.top) || m_width != (rc.right - rc.left)) {
      m_height = rc.bottom - rc.top;
      m_width = rc.right - rc.left;
      winrt::Windows::Foundation::Size size{
          static_cast<float>(m_width / ScaleFactor()), static_cast<float>(m_height / ScaleFactor())};
      // Do not relayout when minimized
      if (!IsIconic(m_hwnd)) {
        winrt::Microsoft::ReactNative::LayoutConstraints constraints;
        constraints.MinimumSize = constraints.MaximumSize = size;
        m_compRootView.Arrange(constraints, {0, 0});
      }
    }
  }
}

LRESULT CompositionHwndHost::TranslateMessage(int msg, uint64_t wParam, int64_t lParam) noexcept {
  if (!m_hwnd || !m_compRootView)
    return 0;

  switch (msg) {
    case WM_WINDOWPOSCHANGED: {
      UpdateSize();
      return 0;
    }
  }

  if (m_compRootView) {
#if USE_WINUI3
    if (!m_compRootView.Island()) // When using Island hosting we dont need to forward window messages
#endif
      return static_cast<LRESULT>(
          m_compRootView.as<winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCompositionRootView>()
              .SendMessage(msg, wParam, lParam));
  }
  return 0;
}

winrt::Microsoft::ReactNative::FocusNavigationResult CompositionHwndHost::NavigateFocus(
    const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept {
  return m_compRootView.NavigateFocus(request);
}

ReactNative::IReactViewHost CompositionHwndHost::ReactViewHost() const noexcept {
  return m_reactViewHost ? m_reactViewHost : m_compRootView.ReactViewHost();
}

void CompositionHwndHost::ReactViewHost(ReactNative::IReactViewHost const &value) noexcept {
  if (m_compRootView) {
    m_compRootView.ReactViewHost(value);
  } else {
    m_reactViewHost = value;
  }
}

IInspectable CompositionHwndHost::UiaProvider() noexcept {
  auto compRootView = winrt::get_self<implementation::ReactNativeIsland>(m_compRootView);
  auto provider = compRootView->GetUiaProvider();
  auto pRootProvider = static_cast<CompositionRootAutomationProvider *>(provider.as<IRawElementProviderSimple>().get());
  if (pRootProvider != nullptr) {
    pRootProvider->SetHwnd(m_hwnd);
  }
  return provider;
}

} // namespace winrt::Microsoft::ReactNative::implementation
