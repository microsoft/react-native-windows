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
#include <windows.ui.composition.interop.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.interop.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Composition.h>
#include "IReactContext.h"
#include "ReactHost/ReactInstanceWin.h"
#include "ReactNativeHost.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {
WindowsModalHostComponentView::WindowsModalHostComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(
          WindowsModalHostComponentView::defaultProps(),
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background) {
  m_reactContext = reactContext; // save react context
  m_compositionContext = compContext; // save composition context
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
const int MODAL_DEFAULT_WIDTH = 500;
const int MODAL_DEFAULT_HEIGHT = 500;

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
  auto roothwnd = reinterpret_cast<HWND>(
      winrt::Microsoft::ReactNative::ReactCoreInjection::GetTopLevelWindowId(m_reactContext.Properties().Handle()));

  m_hwnd = CreateWindow(
      c_modalWindowClassName,
      L"React-Native Modal",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      MODAL_DEFAULT_WIDTH,
      MODAL_DEFAULT_HEIGHT,
      roothwnd, // parent
      nullptr,
      hInstance,
      spunk.get());

  // Check if window creation succeeded
  if (!m_hwnd) {
    throw std::exception("Failed to create new hwnd for Modal: " + GetLastError());
  }

  // set the top-level windows as the new hwnd
  winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(m_hwnd));

  // get current compositor - handles the creation/manipulation of visual objects
  auto compositionContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          m_reactContext.Properties().Handle());
  auto compositor =
      winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerCompositor(
          compositionContext);

  // create a react native island - code taken from CompositionHwndHost
  auto bridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(
      compositor, winrt::Microsoft::UI::GetWindowIdFromWindow(m_hwnd));
  m_reactNativeIsland = winrt::Microsoft::ReactNative::ReactNativeIsland(compositor);
  auto contentIsland = m_reactNativeIsland.Island();
  bridge.Connect(contentIsland);
  bridge.Show();
  // ReactNativeIsland creates a new rootVisual for us
  m_rootVisual = m_reactNativeIsland.RootVisual().try_as<winrt::Microsoft::UI::Composition::ContainerVisual>();

  // set ScaleFactor
  m_reactNativeIsland.ScaleFactor(GetDpiForWindow(m_hwnd) / 96.0f);

  // set layout contraints
  winrt::Microsoft::ReactNative::LayoutConstraints constraints;
  constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::LeftToRight;
  constraints.MaximumSize = constraints.MinimumSize = {500 / ScaleFactor(m_hwnd), 500 / ScaleFactor(m_hwnd)};
  m_reactNativeIsland.Arrange(constraints, {0, 0});
  bridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

  // Add CompositionEventHandler
  m_reactNativeIsland.AddFragmentCompositionEventHandler(m_reactContext.Handle(), *this);

  spunk.detach();
}

void WindowsModalHostComponentView::ShowOnUIThread() {
  if (m_hwnd) {
    ShowWindow(m_hwnd, SW_NORMAL);
    BringWindowToTop(m_hwnd);
    SetFocus(m_hwnd);
  }
}

void WindowsModalHostComponentView::HideOnUIThread() noexcept {
  if (m_hwnd) {
    SendMessage(m_hwnd, WM_CLOSE, 0, 0);
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
    case WM_COMMAND: {
      break;
    }
    case WM_NCCREATE: { // called before WM_CREATE, lparam should be identical to members of CreateWindowEx
      auto createStruct = reinterpret_cast<CREATESTRUCT *>(lparam); // CreateStruct
      data = static_cast<::IUnknown *>(createStruct->lpCreateParams);
      SetProp(hwnd, CompHostProperty, data); // adds new properties to window
      break;
    }
    case WM_CREATE: { // called after window is created but before visible
      break;
    }
    case WM_CLOSE: {
      // Just hide the window instead of destroying it
      ::ShowWindow(hwnd, SW_HIDE);
      return 0;
    }
    case WM_DESTROY: { // called when we want to destroy the window
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

// childComponentView - reference to the child component view
// index - the position in which the childComponentView should be mounted
void WindowsModalHostComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  EnsureModalCreated();
  // Skip calling ViewComponentView::MountChildComponentView since we override adding it to the rootVisual below
  ComponentView::MountChildComponentView(childComponentView, index);

  // Handle index offset and ensure visual
  indexOffsetForBorder(index);
  ensureVisual();

  if (auto compositionChild = childComponentView.try_as<ComponentView>()) {
    auto containerChildren = m_rootVisual.Children();
    auto compVisual =
        winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerVisual(
            compositionChild->OuterVisual());

    if (index == 0) {
      containerChildren.InsertAtBottom(compVisual);
    } else {
      auto insertAfter = containerChildren.First();
      for (uint32_t i = 1; i < index; i++) {
        insertAfter.MoveNext();
      }
      containerChildren.InsertAbove(compVisual, insertAfter.Current());
    }
  }
}

void WindowsModalHostComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  ComponentView::UnmountChildComponentView(childComponentView, index);
  HideOnUIThread();
}

void WindowsModalHostComponentView::HandleCommand(
    const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept {
  Super::HandleCommand(args);
}

void WindowsModalHostComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldModalProps =
      *std::static_pointer_cast<const facebook::react::ModalHostViewProps>(oldProps ? oldProps : viewProps());
  const auto &newModalProps = *std::static_pointer_cast<const facebook::react::ModalHostViewProps>(props);

  if (newModalProps.visible) {
    EnsureModalCreated();
    ShowOnUIThread();
  }

  base_type::updateProps(props, oldProps);
}

void WindowsModalHostComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

void WindowsModalHostComponentView::FinalizeUpdates(
    winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  Super::FinalizeUpdates(updateMask);
}

void WindowsModalHostComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

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
  facebook::react::LayoutMetrics layoutMetrics = facebook::react::LayoutMetrics();
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
