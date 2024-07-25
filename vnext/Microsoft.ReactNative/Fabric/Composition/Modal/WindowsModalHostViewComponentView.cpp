// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsModalHostViewComponentView.h"

#include <AutoDraw.h>
#include <Fabric/DWriteHelpers.h>
#include "../CompositionDynamicAutomationProvider.h"
#include "Unicode.h"

#include <DispatcherQueue.h>
#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
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
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background,
          false) {
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

// creates a new modal window
void WindowsModalHostComponentView::EnsureModalCreated() {
  auto host = winrt::Microsoft::ReactNative::implementation::ReactNativeHost::GetReactNativeHost(m_reactContext.Properties());

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
   winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(m_hwnd));

  // create new compositor - handles the creation/manipulation of visual objects
  auto compositor = winrt::Windows::UI::Composition::Compositor();

  // link the compositor to the hwnd
  auto interop = compositor.try_as<ABI::Windows::UI::Composition::Desktop::ICompositorDesktopInterop>(); // ICompositorDesktopInterop provides methods that allow composition visuals to be rendered to HWND
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget m_target{nullptr}; // DesktopWindowTarget allow the render of composition visual to a specific hwnd.
  // params m_hwnd - hwnd the visuals to be rendered at
  //        true - if the window should be topmost
  //        IDesktopWindowTarget - get raw pointer to m_target interface and cast it as a IDesktopWindowTarget
  check_hresult(interop->CreateDesktopWindowTarget(
      m_hwnd,
      true,
      reinterpret_cast<ABI::Windows::UI::Composition::Desktop::IDesktopWindowTarget **>(put_abi(m_target))));

  // make container/root visual (every visual will be added to this tree)
  rootVisual = compositor.CreateContainerVisual(); 
  rootVisual.RelativeSizeAdjustment({1.0f, 1.0f});
  rootVisual.Offset({0, 0, 0});
  rootVisual.Comment(L"Modal Root Visual");

  // set the root visual of DesktopWindowTarget
  m_target.Root(rootVisual);

  // Create a SpriteVisual (a rectangle filled with a solid color)
  winrt::Windows::UI::Composition::SpriteVisual spriteVisual = compositor.CreateSpriteVisual();
  spriteVisual.Size({100.0f, 100.0f});
  spriteVisual.Offset({50.0f, 50.0f, 0.0f});
  spriteVisual.Brush(compositor.CreateColorBrush(winrt::Windows::UI::Colors::Red()));

  // Animate the position of the sprite visual - just to see if it end up anywhere
  auto animation = compositor.CreateVector3KeyFrameAnimation();
  animation.InsertKeyFrame(0.0f, {0.0f, 0.0f, 0.0f});
  animation.InsertKeyFrame(1.0f, {200.0f, 200.0f, 0.0f});
  animation.Duration(std::chrono::seconds(2));

  // Add the spirit visual to the root visual
  rootVisual.Children().InsertAtTop(spriteVisual);

  // Start the animation
  spriteVisual.StartAnimation(L"Offset", animation);

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
    LPARAM lParam = 0;
    WPARAM wParam = 0;
    SendMessage(m_hwnd, WM_CLOSE, wParam, lParam);
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
      // host.Initialize((uint64_t)hwnd);
      break;
    }
    case WM_CLOSE: {
      // Just hide the window instead of destroying it
      ::ShowWindow(hwnd, SW_HIDE);
      return 0;
    }
    case WM_DESTROY: { // called when we want to destroy the window
      // data->Release();
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
  // TODO: these are getting added to the roothwnd but we want them in the modal hwnd? What is the best way to do this?
  m_children.InsertAt(index, childComponentView); // insert childComponent into m_children
  // Sets the parent of the childComponentView to *this (the current instance of WindowsModalHostComponentView)
  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(childComponentView)->parent(*this);
  indexOffsetForBorder(index);
  ensureVisual();
  if (auto compositionChild = childComponentView.try_as<ComponentView>()) {
    Visual().InsertAt(compositionChild->OuterVisual(), index);
  }
}

void WindowsModalHostComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  // base_type::UnmountChildComponentView(childComponentView, index);
  HideOnUIThread();
}

void WindowsModalHostComponentView::HandleCommand(
    winrt::hstring commandName,
    const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept {
  Super::HandleCommand(commandName, args);
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

  // Temporary placeholder for Modal, draws on main hwnd
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
    drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);
    Visual().as<Experimental::ISpriteVisual>().Brush(drawingSurface);
    Visual().Size(surfaceSize);
    Visual().Offset({
        layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
        layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
        0.0f,
    });

    POINT offset;
    {
      ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(
          drawingSurface, m_layoutMetrics.pointScaleFactor, &offset);
      if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
        d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Blue, 0.3f));
        assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);

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
