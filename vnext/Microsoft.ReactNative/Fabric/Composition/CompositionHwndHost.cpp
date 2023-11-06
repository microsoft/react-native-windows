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

#include "CompositionRootAutomationProvider.h"
#include "CompositionRootView.h"

WINUSERAPI UINT WINAPI GetDpiForWindow(_In_ HWND hwnd);

namespace winrt::Microsoft::ReactNative::implementation {

winrt::Windows::UI::Composition::Visual CompositionHwndHost::RootVisual() const noexcept {
  return m_target.Root();
}

winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget CompositionHwndHost::Target() const noexcept {
  assert(m_target);
  return m_target;
}

void CompositionHwndHost::CreateDesktopWindowTarget(HWND window) {
  namespace abi = ABI::Windows::UI::Composition::Desktop;

  auto interop = Compositor().as<abi::ICompositorDesktopInterop>();
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget target{nullptr};
  check_hresult(interop->CreateDesktopWindowTarget(
      window, false, reinterpret_cast<abi::IDesktopWindowTarget **>(put_abi(target))));
  m_target = target;
}

void CompositionHwndHost::CreateCompositionRoot() {
  auto root = Compositor().CreateContainerVisual();
  root.RelativeSizeAdjustment({1.0f, 1.0f});
  root.Offset({0, 0, 0});
  m_target.Root(root);
}

CompositionHwndHost::CompositionHwndHost() noexcept {}

void CompositionHwndHost::Initialize(uint64_t hwnd) noexcept {
  m_hwnd = (HWND)hwnd;

  m_compRootView = winrt::Microsoft::ReactNative::CompositionRootView();

  CreateDesktopWindowTarget(m_hwnd);
  CreateCompositionRoot();

  m_compRootView.ReactViewHost(std::move(m_reactViewHost));

  m_compRootView.ScaleFactor(ScaleFactor());
  m_compRootView.RootVisual(
      winrt::Microsoft::ReactNative::Composition::implementation::WindowsCompositionContextHelper::CreateVisual(
          RootVisual()));

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
      m_compRootView.Size(size);
      m_compRootView.Measure(size);
      m_compRootView.Arrange(size);
    }
  }
}

LRESULT CompositionHwndHost::TranslateMessage(int msg, uint64_t wParam, int64_t lParam) noexcept {
  if (!m_hwnd || !m_compRootView)
    return 0;

  switch (msg) {
    case WM_WINDOWPOSCHANGED: {
      UpdateSize();
      /*
      winrt::Windows::Foundation::Size size{
          static_cast<float>(reinterpret_cast<const WINDOWPOS *>(lParam)->cx),
          static_cast<float>(reinterpret_cast<const WINDOWPOS *>(lParam)->cy)};
      m_compRootView.Size(size);
      m_compRootView.Measure(size);
      m_compRootView.Arrange(size);
      */
      return 0;
    }
  }

  if (m_compRootView) {
    return static_cast<LRESULT>(m_compRootView.SendMessage(msg, wParam, lParam));
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

winrt::Windows::UI::Composition::Compositor CompositionHwndHost::Compositor() const noexcept {
  auto compositionContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          m_reactViewHost.ReactNativeHost().InstanceSettings().Properties());

  return winrt::Microsoft::ReactNative::Composition::implementation::WindowsCompositionContextHelper::InnerCompositor(
      compositionContext);
}

IInspectable CompositionHwndHost::UiaProvider() noexcept {
  auto compRootView = winrt::get_self<implementation::CompositionRootView>(m_compRootView);
  auto provider = compRootView->GetUiaProvider();
  auto pRootProvider = static_cast<CompositionRootAutomationProvider *>(provider.as<IRawElementProviderSimple>().get());
  if (pRootProvider != nullptr) {
    pRootProvider->SetHwnd(m_hwnd);
  }
  return provider;
}

} // namespace winrt::Microsoft::ReactNative::implementation
