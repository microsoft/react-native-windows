// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsModalHostViewComponentView.h"

#include <AutoDraw.h>
#include <Fabric/DWriteHelpers.h>
#include "../CompositionDynamicAutomationProvider.h"
#include "Unicode.h"

#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <winrt/Windows.UI.Composition.h>
#include "IReactContext.h"
#include "ReactHost/ReactInstanceWin.h"
#include "ReactNativeHost.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {
WindowsModalHostComponentView::WindowsModalHostComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background,
          false) {
  m_props = std::make_shared<facebook::react::ModalHostViewProps const>();
  m_context = reactContext; // save context
  m_visual = compContext.CreateSpriteVisual();
}

winrt::Microsoft::ReactNative::ComponentView WindowsModalHostComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<WindowsModalHostComponentView>(compContext, tag, reactContext);
}

// constants for creating a new windows (code mostly taken from LogBox)
constexpr PCWSTR c_modalWindowClassName = L"MS_REACTNATIVE_MODAL";
constexpr auto CompHostProperty = L"CompHost";
const int MODAL_DEFAULT_WIDTH = 500;
const int MODAL_DEFAULT_HEIGHT = 500;

// creates a new modal window
void WindowsModalHostComponentView::ShowOnUIThread() {
  auto host =
      winrt::Microsoft::ReactNative::implementation::ReactNativeHost::GetReactNativeHost(m_context.Properties());
  if (!host) {
    return;
  }

  RegisterWndClass(); // creates and register a windows class

  if (!m_hwnd) {
    auto CompositionHwndHost = winrt::Microsoft::ReactNative::CompositionHwndHost();
    winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
    viewOptions.ComponentName(L"Modal");
    CompositionHwndHost.ReactViewHost(
        winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));
    HINSTANCE hInstance = GetModuleHandle(NULL);
    winrt::impl::abi<winrt::Microsoft::ReactNative::ICompositionHwndHost>::type *pHost{nullptr};
    winrt::com_ptr<::IUnknown> spunk;
    CompositionHwndHost.as(spunk);

    // get the root hwnd
    auto roothwnd = reinterpret_cast<HWND>(
        winrt::Microsoft::ReactNative::ReactCoreInjection::GetTopLevelWindowId(m_context.Properties().Handle()));

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

    spunk.detach();
  }

  ShowWindow(m_hwnd, SW_NORMAL);
  BringWindowToTop(m_hwnd);
  SetFocus(m_hwnd);
}

void WindowsModalHostComponentView::HideOnUIThread() noexcept {
  if (m_hwnd) {
    ::ShowWindow(m_hwnd, SW_HIDE);
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
    case WM_NCCREATE: { // sent before WM_CREATE, lparam should be identical to members of CreateWindowEx
      auto createStruct = reinterpret_cast<CREATESTRUCT *>(lparam); // CreateStruct
      data = static_cast<::IUnknown *>(createStruct->lpCreateParams);
      SetProp(hwnd, CompHostProperty, data); // adds new properties to window
      break;
    }
    case WM_CREATE: { // recieves after window is created but before visible
      // host.Initialize((uint64_t)hwnd); cause Modal to throw a not registered error
      break;
    }
    case WM_CLOSE: {
      // Just hide the window instead of destroying it
      ::ShowWindow(hwnd, SW_HIDE);
      return 0;
    }
    case WM_DESTROY: { // called when we want to destroy the window
      data->Release();
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

void WindowsModalHostComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  // Disabled due to partial Modal implementation. Tracking re-enablement with task list here:
  // https://github.com/microsoft/react-native-windows/issues/11157 assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void WindowsModalHostComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  // Disabled due to partial Modal implementation.Tracking re-enablement with task list here : https : //
  // github.com/microsoft/react-native-windows/issues/11157 assert(false);
  base_type::UnmountChildComponentView(childComponentView, index);
}

void WindowsModalHostComponentView::HandleCommand(
    winrt::hstring commandName,
    const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept {
  Super::HandleCommand(commandName, args);
}

void WindowsModalHostComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldModalProps = *std::static_pointer_cast<const facebook::react::ModalHostViewProps>(m_props);
  const auto &newModalProps = *std::static_pointer_cast<const facebook::react::ModalHostViewProps>(props);

  // currently Modal only gets Destroyed by closing the window
  if (newModalProps.visible) {
    ShowOnUIThread();
  }

  // update BaseComponentView props
  updateTransformProps(oldModalProps, newModalProps, m_visual);
  Super::updateProps(props, oldProps);
  m_props = std::static_pointer_cast<facebook::react::ModalHostViewProps const>(props);
}

void WindowsModalHostComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  // Temparary placeholder for Modal, draws on main hwnd
  if (m_layoutMetrics.frame.size != layoutMetrics.frame.size ||
      m_layoutMetrics.pointScaleFactor != layoutMetrics.pointScaleFactor || m_layoutMetrics.frame.size.width == 0) {
    // Always make visual a min size, so that even if its laid out at zero size, its clear an unimplemented view was
    // rendered
    float width = std::max(m_layoutMetrics.frame.size.width, 200.0f);
    float height = std::max(m_layoutMetrics.frame.size.width, 50.0f);

    winrt::Windows::Foundation::Size surfaceSize = {
        width * m_layoutMetrics.pointScaleFactor, height * m_layoutMetrics.pointScaleFactor};
    auto drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
        surfaceSize,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    drawingSurface.HorizontalAlignmentRatio(0.f);
    drawingSurface.VerticalAlignmentRatio(0.f);
    drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::None);
    m_visual.Brush(drawingSurface);
    m_visual.Size(surfaceSize);
    m_visual.Offset({
        layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
        layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
        0.0f,
    });

    POINT offset;
    {
      ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(drawingSurface, &offset);
      if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
        d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Blue, 0.3f));
        assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);
        const auto dpi = m_layoutMetrics.pointScaleFactor * 96.0f;
        float oldDpiX, oldDpiY;
        d2dDeviceContext->GetDpi(&oldDpiX, &oldDpiY);
        d2dDeviceContext->SetDpi(dpi, dpi);

        float offsetX = static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor);
        float offsetY = static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor);

        winrt::com_ptr<IDWriteTextFormat> spTextFormat;
        winrt::check_hresult(::Microsoft::ReactNative::DWriteFactory()->CreateTextFormat(
            L"Segoe UI",
            nullptr, // Font collection (nullptr sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            12,
            L"",
            spTextFormat.put()));

        winrt::com_ptr<ID2D1SolidColorBrush> textBrush;
        winrt::check_hresult(
            d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.put()));

        const D2D1_RECT_F rect = {
            static_cast<float>(offset.x), static_cast<float>(offset.y), width + offset.x, height + offset.y};

        auto label = ::Microsoft::Common::Unicode::Utf8ToUtf16(std::string("This is a Modal"));
        d2dDeviceContext->DrawText(
            label.c_str(),
            static_cast<UINT32>(label.length()),
            spTextFormat.get(),
            rect,
            textBrush.get(),
            D2D1_DRAW_TEXT_OPTIONS_NONE,
            DWRITE_MEASURING_MODE_NATURAL);
      }
    }
  }

  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
}

void WindowsModalHostComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void WindowsModalHostComponentView::prepareForRecycle() noexcept {}

facebook::react::SharedViewProps WindowsModalHostComponentView::viewProps() noexcept {
  return m_props;
}

winrt::Microsoft::ReactNative::Composition::IVisual WindowsModalHostComponentView::Visual() const noexcept {
  return m_visual;
}

winrt::Microsoft::ReactNative::Composition::IVisual WindowsModalHostComponentView::OuterVisual() const noexcept {
  return m_visual;
}

facebook::react::Tag WindowsModalHostComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
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
