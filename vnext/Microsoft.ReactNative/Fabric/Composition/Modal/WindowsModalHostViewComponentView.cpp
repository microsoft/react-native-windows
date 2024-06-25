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
  auto host =
      winrt::Microsoft::ReactNative::implementation::ReactNativeHost::GetReactNativeHost(m_reactContext.Properties());

  // return if hwnd already exists
  if (!host || m_hwnd) {
    return;
  }

  RegisterWndClass(); // creates and register a windows class
  /*
  m_compositionHwndHost = winrt::Microsoft::ReactNative::CompositionHwndHost(); // creates a new RNW hwndHost
  winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
  // this is the name of the component registered in Javascript via AppRegistry.registerComponent('ModuleName', () =>
  ModuleName);
  // IE, Bootstrap is the app name registered, do we need to do the same for Modal in javascript? How do we do that?
  viewOptions.ComponentName(L"Bootstrap");
  m_compositionHwndHost.ReactViewHost(winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(host,
  viewOptions)); // creates a new ReactViewHost. What is the difference between CompositionHwndHost and ViewHost?

  winrt::impl::abi<winrt::Microsoft::ReactNative::ICompositionHwndHost>::type *pHost{nullptr};
  winrt::com_ptr<::IUnknown> spunk;
  m_compositionHwndHost.as(spunk);
  */

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

  // create new root visual
  // auto compositor =
  // winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerCompositor(m_compositionContext);
  winrt::Windows::UI::Composition::Compositor compositor;
  rootVisual = compositor.CreateContainerVisual();
  rootVisual.RelativeSizeAdjustment({1.0f, 1.0f});
  rootVisual.Offset({0, 0, 0});
  rootVisual.Comment(L"Modal Root Visual");

  for (unsigned i = 0; i != 3; ++i) {
    winrt::Windows::UI::Composition::SpriteVisual visual = compositor.CreateSpriteVisual();
    visual.Size({300.0f, 200.0f});
    visual.Offset({50 + 20.0f * i, 50 + 20.0f * i, 0.0f});
    visual.Comment(L"Visual" + i);
    rootVisual.Children().InsertAtTop(visual);
  }

  // Create a child visual with a solid color brush
  auto childVisual = compositor.CreateSpriteVisual();
  childVisual.Size({100.0f, 100.0f}); // Set the size of the visual
  auto brush = compositor.CreateColorBrush(winrt::Windows::UI::Colors::Red()); // Set the color to red
  childVisual.Brush(brush);

  // Add the child visual to the root visual
  rootVisual.Children().InsertAtTop(childVisual);

  // trying new stuff
  // auto compositor2 =
  // winrt::Microsoft::ReactNative::Composition::CompositionUIService::GetCompositor(ReactContext().Properties());
  // winrt::Windows::UI::Composition::Compositor compositor;

  // try to link it to the hwnd
  auto interop = compositor.try_as<ABI::Windows::UI::Composition::Desktop::ICompositorDesktopInterop>();
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget m_target{nullptr};
  check_hresult(interop->CreateDesktopWindowTarget(
      m_hwnd,
      false,
      reinterpret_cast<ABI::Windows::UI::Composition::Desktop::IDesktopWindowTarget **>(put_abi(m_target))));
  m_target.Root(rootVisual);

  winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      host.InstanceSettings().Properties(), reinterpret_cast<uint64_t>(m_hwnd));

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
  // Set parent: This line sets the parent of the childComponentView to *this (the current instance of
  // WindowsModalHostComponentView). The get_self function is used to obtain the implementation object from the WinRT
  // projection object.
  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(childComponentView)->parent(*this);
  // Adjust index for borders: This line calls the indexOffsetForBorder method with the index parameter. This method
  // likely adjusts the index to account for any special border handling or offsets required when inserting the child.
  indexOffsetForBorder(index);
  // Ensure visual setup: This line calls the ensureVisual method. This method probably ensures that the visual tree for
  // the component view is properly set up and initialized.
  ensureVisual();
  // This ensures that the subsequent code only runs if the childComponentView can be treated as a ComponentView
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
