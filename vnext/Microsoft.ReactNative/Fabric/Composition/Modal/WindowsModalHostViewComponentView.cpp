// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsModalHostViewComponentView.h"

#include <AutoDraw.h>
#include <Fabric/DWriteHelpers.h>
#include "../CompositionDynamicAutomationProvider.h"
#include "Unicode.h"

#include <DispatcherQueue.h>
#include <Fabric/ComponentView.h>
#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <Fabric/Composition/ReactNativeIsland.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.interop.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Composition.h>
#include "IReactContext.h"
#include "ReactHost/ReactInstanceWin.h"
#include "ReactNativeHost.h"
#include "WindowsModalHostViewShadowNode.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {
WindowsModalHostComponentView::WindowsModalHostComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(compContext, tag, reactContext) {}

WindowsModalHostComponentView::~WindowsModalHostComponentView() {
  // dispatch onDismiss event
  auto emitter = std::static_pointer_cast<const facebook::react::ModalHostViewEventEmitter>(m_eventEmitter);
  facebook::react::ModalHostViewEventEmitter::OnDismiss onDismissArgs;
  emitter->onDismiss(onDismissArgs);

  // reset the topWindowID
  if (m_prevWindowID) {
    auto host =
        winrt::Microsoft::ReactNative::implementation::ReactNativeHost::GetReactNativeHost(m_reactContext.Properties());
    winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
        host.InstanceSettings().Properties(), m_prevWindowID);
    m_prevWindowID = 0;
  }

  // enable input to parent
  EnableWindow(m_parentHwnd, true);

  // Check if the window handle (m_hwnd) exists and destroy it if necessary
  if (m_hwnd) {
    // Close/Destroy the modal window
    SendMessage(m_hwnd, WM_DESTROY, 0, 0);
    m_hwnd = nullptr;
  }
}

winrt::Microsoft::ReactNative::ComponentView WindowsModalHostComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<WindowsModalHostComponentView>(compContext, tag, reactContext);
}

// constants for creating a new windows
constexpr PCWSTR c_modalWindowClassName = L"MS_REACTNATIVE_MODAL";
constexpr auto CompHostProperty = L"CompHost";
const int MODAL_MIN_WIDTH = 50;
const int MODAL_MIN_HEIGHT = 50;

float ScaleFactor(HWND hwnd) noexcept {
  return GetDpiForWindow(hwnd) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

// creates a new modal window
void WindowsModalHostComponentView::EnsureModalCreated() {
  auto host =
      winrt::Microsoft::ReactNative::implementation::ReactNativeHost::GetReactNativeHost(m_reactContext.Properties());

  // return if hwnd already exists
  if (!host || m_hwnd) {
    return;
  }

  RegisterWndClass();

  HINSTANCE hInstance = GetModuleHandle(NULL);
  winrt::com_ptr<::IUnknown> spunk;

  // get the root hwnd
  m_prevWindowID =
      winrt::Microsoft::ReactNative::ReactCoreInjection::GetTopLevelWindowId(m_reactContext.Properties().Handle());

  m_parentHwnd = GetHwndForParenting();

  auto windowsStyle = m_showTitleBar ? WS_OVERLAPPEDWINDOW : WS_POPUP;

  m_hwnd = CreateWindow(
      c_modalWindowClassName,
      L"React-Native Modal",
      windowsStyle,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      MODAL_MIN_WIDTH,
      MODAL_MIN_HEIGHT,
      m_parentHwnd, // parent
      nullptr,
      hInstance,
      spunk.get());

  // Check if window creation succeeded
  if (!m_hwnd) {
    throw std::exception("Failed to create new hwnd for Modal: " + GetLastError());
  }

  // Disable user sizing of the hwnd
  ::SetWindowLong(m_hwnd, GWL_STYLE, GetWindowLong(m_hwnd, GWL_STYLE) & ~WS_SIZEBOX);

  // set the top-level windows as the new hwnd
  winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(m_hwnd));

  // get current compositor - handles the creation/manipulation of visual objects
  auto compositionContext = CompositionContext();
  auto compositor =
      winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerCompositor(
          compositionContext);

  // create a react native island - code taken from CompositionHwndHost
  auto bridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(
      compositor, winrt::Microsoft::UI::GetWindowIdFromWindow(m_hwnd));
  m_reactNativeIsland = winrt::Microsoft::ReactNative::ReactNativeIsland(compositor, m_reactContext.Handle(), *this);
  auto contentIsland = m_reactNativeIsland.Island();
  bridge.Connect(contentIsland);
  bridge.Show();

  // set ScaleFactor
  ScaleFactor(m_hwnd);

  // set layout contraints
  winrt::Microsoft::ReactNative::LayoutConstraints constraints;
  constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::Undefined;

  RECT rc;
  GetClientRect(m_parentHwnd, &rc);
  // Maximum size is set to size of parent hwnd
  constraints.MaximumSize = {(rc.right - rc.left) * ScaleFactor(m_hwnd), (rc.bottom - rc.top) / ScaleFactor(m_hwnd)};
  constraints.MinimumSize = {MODAL_MIN_WIDTH * ScaleFactor(m_hwnd), MODAL_MIN_HEIGHT * ScaleFactor(m_hwnd)};
  m_reactNativeIsland.Arrange(constraints, {0, 0});
  bridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

  spunk.detach();
}

void WindowsModalHostComponentView::ShowOnUIThread() {
  if (m_hwnd && !IsWindowVisible(m_hwnd)) {
    ShowWindow(m_hwnd, SW_NORMAL);
    BringWindowToTop(m_hwnd);
    SetFocus(m_hwnd);

    // disable input to parent
    EnableWindow(m_parentHwnd, false);

    // dispatch onShow event
    auto emitter = std::static_pointer_cast<const facebook::react::ModalHostViewEventEmitter>(m_eventEmitter);
    facebook::react::ModalHostViewEventEmitter::OnShow onShowArgs;
    emitter->onShow(onShowArgs);
  }
}

void WindowsModalHostComponentView::HideOnUIThread() noexcept {
  if (m_hwnd) {
    SendMessage(m_hwnd, WM_CLOSE, 0, 0);
  }

  // dispatch onDismiss event
  auto emitter = std::static_pointer_cast<const facebook::react::ModalHostViewEventEmitter>(m_eventEmitter);
  facebook::react::ModalHostViewEventEmitter::OnDismiss onDismissArgs;
  emitter->onDismiss(onDismissArgs);

  // enable input to parent
  EnableWindow(m_parentHwnd, true);

  // reset the topWindowID
  if (m_prevWindowID) {
    auto host =
        winrt::Microsoft::ReactNative::implementation::ReactNativeHost::GetReactNativeHost(m_reactContext.Properties());
    winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
        host.InstanceSettings().Properties(), m_prevWindowID);
    m_prevWindowID = 0;
  }
}

// Windows Procedure - callback function used for handling all messages (generated by NTUser or manual calls to
// SendMessage)
LRESULT CALLBACK ModalBoxWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
  auto data = reinterpret_cast<::IUnknown *>(GetProp(
      hwnd,
      CompHostProperty)); // gets data handle from the property list of specified window (ie the window we want to make)
  winrt::Microsoft::ReactNative::CompositionHwndHost host{nullptr};

  if (data) {
    winrt::check_hresult(data->QueryInterface(
        winrt::guid_of<winrt::Microsoft::ReactNative::CompositionHwndHost>(),
        winrt::put_abi(host))); // look into the data for a CompositionHwndHost and store it in host
    auto result = static_cast<LRESULT>(host.TranslateMessage(message, wparam, lparam));
    if (result) {
      return result;
    }
  }

  switch (message) {
    case WM_NCCREATE: { // called before WM_CREATE, lparam should be identical to members of CreateWindowEx
      auto createStruct = reinterpret_cast<CREATESTRUCT *>(lparam); // CreateStruct
      data = static_cast<::IUnknown *>(createStruct->lpCreateParams);
      SetProp(hwnd, CompHostProperty, data); // adds new properties to window
      break;
    }
    case WM_CLOSE: {
      // Just hide the window instead of destroying it
      ::ShowWindow(hwnd, SW_HIDE);
      return 0;
    }
    case WM_DESTROY: { // called when we want to destroy the window
      ::ShowWindow(hwnd, SW_HIDE);
      if (data) {
        data->Release();
      }
      SetProp(hwnd, CompHostProperty, nullptr);
      break;
    }
  }

  return DefWindowProc(hwnd, message, wparam, lparam);
}

// Creates and Register a new window class
void WindowsModalHostComponentView::RegisterWndClass() noexcept {
  static bool registered = false;
  if (registered) {
    return;
  }

  HINSTANCE hInstance =
      GetModuleHandle(NULL); // returns a handle to the file used to create the calling process (.exe file)

  WNDCLASSEX wcex = {}; // contains window class information
  wcex.cbSize = sizeof(wcex); // size of windows class (bytes)
  wcex.style = CS_HREDRAW | CS_VREDRAW; // class style (redraw window on size adjustment)
  wcex.lpfnWndProc = &ModalBoxWndProc; // pointer to windows procedure
  wcex.cbClsExtra = DLGWINDOWEXTRA; // extra bytes to allocate
  wcex.cbWndExtra =
      sizeof(winrt::impl::abi<winrt::Microsoft::ReactNative::ICompositionHwndHost>::type *); // extra bytes to allocate
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // handle to class cursor
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // background color
  wcex.lpszClassName = c_modalWindowClassName; // specify resource name
  ATOM classId = RegisterClassEx(&wcex); // register new windows class
  WINRT_VERIFY(classId); // 0 = fail
  winrt::check_win32(!classId);

  registered = true;
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual
WindowsModalHostComponentView::VisualToMountChildrenInto() noexcept {
  return m_reactNativeIsland
      .as<winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCompositionRootView>()
      .InternalRootVisual();
}

// childComponentView - reference to the child component view
// index - the position in which the childComponentView should be mounted
void WindowsModalHostComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  EnsureModalCreated();
  base_type::MountChildComponentView(childComponentView, index);
}

void WindowsModalHostComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  base_type::UnmountChildComponentView(childComponentView, index);
}

void WindowsModalHostComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
  if (m_hwnd) {
    EnsureModalCreated();
    AdjustWindowSize();
    ShowOnUIThread();
  }
}

void WindowsModalHostComponentView::AdjustWindowSize() noexcept {
  if (m_layoutMetrics.overflowInset.right == 0 && m_layoutMetrics.overflowInset.bottom == 0) {
    return;
  }

  // Modal's size is based on it's children, use the overflow to calculate the width/height
  float xPos = (-m_layoutMetrics.overflowInset.right * (m_layoutMetrics.pointScaleFactor));
  float yPos = (-m_layoutMetrics.overflowInset.bottom * (m_layoutMetrics.pointScaleFactor));
  RECT rc;
  GetClientRect(m_hwnd, &rc);
  RECT rect = {0, 0, (int)xPos, (int)yPos};

  if (m_showTitleBar) {
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE); // Adjust for title bar and borders
  }

  // set the layoutMetrics
  m_layoutMetrics.frame.size = {(float)rect.right - rect.left, (float)rect.bottom - rect.top};
  m_layoutMetrics.overflowInset.right = 0;
  m_layoutMetrics.overflowInset.bottom = 0;

  // get Modal's position based on parent
  RECT parentRC;
  GetWindowRect(m_parentHwnd, &parentRC);
  float xCor = (parentRC.left + parentRC.right - m_layoutMetrics.frame.size.width) / 2; // midpointx - width / 2
  float yCor = (parentRC.top + parentRC.bottom - m_layoutMetrics.frame.size.height) / 2; // midpointy - height / 2

  // Adjust window position and size
  MoveWindow(m_hwnd, (int)xCor, (int)yCor, (int)(rect.right - rect.left), (int)(rect.bottom - rect.top), true);

  // Let RNWIsland know that Modal's size has changed
  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(m_reactNativeIsland)
      ->NotifySizeChanged();
};

void WindowsModalHostComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldModalProps =
      *std::static_pointer_cast<const facebook::react::ModalHostViewProps>(oldProps ? oldProps : viewProps());
  const auto &newModalProps = *std::static_pointer_cast<const facebook::react::ModalHostViewProps>(props);
  newModalProps.visible ? m_isVisible = true : m_isVisible = false;
  if (!m_isVisible) {
    HideOnUIThread();
  }
  base_type::updateProps(props, oldProps);
}

facebook::react::SharedViewProps WindowsModalHostComponentView::defaultProps() noexcept {
  static auto const defaultProps = std::make_shared<facebook::react::ModalHostViewProps const>();
  return defaultProps;
}
const facebook::react::ModalHostViewProps &WindowsModalHostComponentView::modalHostViewProps() const noexcept {
  return *std::static_pointer_cast<const facebook::react::ModalHostViewProps>(viewProps());
}

facebook::react::Tag WindowsModalHostComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  if ((ignorePointerEvents || viewProps()->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       viewProps()->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return Tag();
  }

  return -1;
}

bool WindowsModalHostComponentView::focusable() const noexcept {
  return false;
}

std::string WindowsModalHostComponentView::DefaultControlType() const noexcept {
  return "modal";
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
