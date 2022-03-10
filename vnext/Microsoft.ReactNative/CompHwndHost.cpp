// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CompHwndHost.h"
#include "CompHwndHost.g.cpp"

#include <QuirkSettings.h>
#include <ReactHost/MsoUtils.h>
#include <Utils/Helpers.h>
#include <dispatchQueue/dispatchQueue.h>
#include <windowsx.h>
#include <winrt/Windows.UI.Core.h>
#include "ReactNativeHost.h"

namespace winrt::Microsoft::ReactNative::implementation {

winrt::Windows::UI::Composition::Visual CompHwndHost::RootVisual() const noexcept {
  return m_target.Root();
}

winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget CompHwndHost::Target() const noexcept {
  assert(m_target);
  return m_target;
}

void CompHwndHost::EnsureDispatcherQueue() {
  namespace abi = ABI::Windows::System;

  if (m_dispatcherQueueController == nullptr) {
    DispatcherQueueOptions options{
        sizeof(DispatcherQueueOptions), /* dwSize */
        DQTYPE_THREAD_CURRENT, /* threadType */
        DQTAT_COM_ASTA /* apartmentType */
    };

    Windows::System::DispatcherQueueController controller{nullptr};
    check_hresult(CreateDispatcherQueueController(
        options, reinterpret_cast<abi::IDispatcherQueueController **>(put_abi(controller))));
    m_dispatcherQueueController = controller;
  }
}

void CompHwndHost::CreateDesktopWindowTarget(HWND window) {
  namespace abi = ABI::Windows::UI::Composition::Desktop;

  auto interop = m_compRootView.Compositor().as<abi::ICompositorDesktopInterop>();
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget target{nullptr};
  check_hresult(interop->CreateDesktopWindowTarget(
      window, false, reinterpret_cast<abi::IDesktopWindowTarget **>(put_abi(target))));
  m_target = target;
}

void CompHwndHost::CreateCompositionRoot() {
  auto root = m_compRootView.Compositor().CreateContainerVisual();
  root.RelativeSizeAdjustment({1.0f, 1.0f});
  root.Offset({0, 0, 0});
  m_target.Root(root);
}

CompHwndHost::CompHwndHost() noexcept {}

void CompHwndHost::Initialize(uint64_t hwnd) noexcept {
  m_hwnd = (HWND)hwnd;

  EnsureDispatcherQueue();
  m_compRootView = winrt::Microsoft::ReactNative::CompRootView();

  assert(m_dispatcherQueueController);
  m_compRootView.Compositor(winrt::Windows::UI::Composition::Compositor());

  CreateDesktopWindowTarget(m_hwnd);
  CreateCompositionRoot();

  EnsureTarget();

  m_compRootView.InitialProps(std::move(m_initialPropsWriter));
  m_compRootView.ComponentName(std::move(m_componentName));
  m_compRootView.ReactNativeHost(std::move(m_reactNativeHost));

  m_compRootView.ScaleFactor(GetDpiForWindow(m_hwnd) / 96.0);
  m_compRootView.RootVisual(RootVisual());

  UpdateSize();
}

void CompHwndHost::UpdateSize() noexcept {
  RECT rc;
  if (GetClientRect(m_hwnd, &rc)) {
    winrt::Windows::Foundation::Size size{
        static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top)};
    m_compRootView.Size(size);
    m_compRootView.Measure(size);
    m_compRootView.Arrange(size);
  }
}

LRESULT CompHwndHost::TranslateMessage(int msg, WPARAM wParam, LPARAM lParam) noexcept {
  if (!m_hwnd || !m_compRootView)
    return 0;

  switch (msg) {
    case WM_MOUSEWHEEL: {
      POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
      ::ScreenToClient(m_hwnd, &pt);
      int32_t delta = GET_WHEEL_DELTA_WPARAM(wParam);
      m_compRootView.OnScrollWheel({static_cast<float>(pt.x), static_cast<float>(pt.y)}, delta);
      return 0;
    }
    case WM_LBUTTONDOWN: {
      m_compRootView.OnMouseDown({static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam))});
      return 0;
    }
    case WM_LBUTTONUP: {
      m_compRootView.OnMouseUp({static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam))});
      return 0;
    }
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
  return 0;
}

ReactNative::ReactNativeHost CompHwndHost::ReactNativeHost() noexcept {
  return m_compRootView ? m_compRootView.ReactNativeHost() : m_reactNativeHost;
}

void CompHwndHost::ReactNativeHost(ReactNative::ReactNativeHost const &value) noexcept {
  if (m_compRootView) {
    m_compRootView.ReactNativeHost(value);
  } else {
    m_reactNativeHost = value;
  }
  EnsureTarget();
}

void CompHwndHost::EnsureTarget() noexcept {
  if (!ReactNativeHost() || !m_hwnd) {
    return;
  }

  winrt::Microsoft::ReactNative::ReactPropertyBag propBag(ReactNativeHost().InstanceSettings().Properties());
  propBag.Set(
      winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget>(
          L"CompCoreDispatcher"),
      Target());
}

winrt::hstring CompHwndHost::ComponentName() noexcept {
  return m_compRootView ? m_compRootView.ComponentName() : m_componentName;
}

void CompHwndHost::ComponentName(winrt::hstring const &value) noexcept {
  if (m_compRootView)
    m_compRootView.ComponentName(value);
  else
    m_componentName = value;
}

ReactNative::JSValueArgWriter CompHwndHost::InitialProps() noexcept {
  return m_compRootView ? m_compRootView.InitialProps() : m_initialPropsWriter;
}

void CompHwndHost::InitialProps(ReactNative::JSValueArgWriter const &value) noexcept {
  if (m_compRootView)
    m_compRootView.InitialProps(value);
  else
    m_initialPropsWriter = value;
}

} // namespace winrt::Microsoft::ReactNative::implementation
