
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TooltipService.h"

#include <react/renderer/textlayoutmanager/WindowsTextLayoutManager.h>

#include <CompositionSwitcher.Experimental.interop.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include "TextDrawing.h"
#include "dwmapi.h"

namespace winrt::Microsoft::ReactNative {

constexpr PCWSTR c_tooltipWindowClassName = L"RN_TOOLTIP";
constexpr auto TooltipDataProperty = L"TooltipData";
constexpr float tooltipFontSize = 12;
constexpr float tooltipMaxHeight = 1000;
constexpr float tooltipMaxWidth = 320;
constexpr float tooltipHorizontalPadding = 8;
constexpr float tooltipTopPadding = 5;
constexpr float tooltipBottomPadding = 7;
constexpr float toolTipBorderThickness = 1;
constexpr int toolTipPlacementMargin = 12;
constexpr int toolTipAnimationTimeMs = 200;
constexpr int toolTipTimeToShowMs = 1000;

struct TooltipData {
  TooltipData(const winrt::Microsoft::ReactNative::ComponentView &view) : view(view) {}

  static TooltipData *GetFromWindow(HWND hwnd) {
    auto data = reinterpret_cast<TooltipData *>(GetProp(hwnd, TooltipDataProperty));
    return data;
  }

  int width;
  int height;
  ::Microsoft::ReactNative::ReactTaggedView view;
  winrt::com_ptr<::IDWriteTextLayout> textLayout;
  facebook::react::AttributedStringBox attributedString;
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext compositionContext;
};

facebook::react::AttributedStringBox CreateTooltipAttributedString(const std::string &tooltip) noexcept {
  auto attributedString = facebook::react::AttributedString{};
  auto fragment = facebook::react::AttributedString::Fragment{};
  fragment.string = tooltip;
  fragment.textAttributes.fontSize = tooltipFontSize;
  fragment.textAttributes.fontSizeMultiplier =
      static_cast<float>(winrt::Windows::UI::ViewManagement::UISettings().TextScaleFactor());
  attributedString.appendFragment(std::move(fragment));
  return facebook::react::AttributedStringBox{attributedString};
}

LRESULT CALLBACK TooltipWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
  switch (message) {
    case WM_DESTROY: {
      delete TooltipData::GetFromWindow(hwnd);
      SetProp(hwnd, TooltipDataProperty, 0);
      return 0;
    }
    case WM_PRINTCLIENT:
    case WM_PAINT: {
      HDC hdc;
      PAINTSTRUCT ps;

      if (message != WM_PRINTCLIENT)
        hdc = BeginPaint(hwnd, &ps);
      else
        hdc = (HDC)wparam;
      auto data = TooltipData::GetFromWindow(hwnd);

      if (auto view = data->view.view()) {
        auto scaleFactor = view.LayoutMetrics().PointScaleFactor;

        auto ccInterop = data->compositionContext
                             .as<::Microsoft::ReactNative::Composition::Experimental::ICompositionContextInterop>();
        winrt::com_ptr<ID2D1Factory1> factory;
        ccInterop->D2DFactory(factory.put());

        winrt::com_ptr<ID2D1DCRenderTarget> renderTarget;

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
            0,
            0,
            D2D1_RENDER_TARGET_USAGE_NONE,
            D2D1_FEATURE_LEVEL_DEFAULT);
        winrt::check_hresult(factory->CreateDCRenderTarget(&props, renderTarget.put()));
        RECT rc;
        GetClientRect(hwnd, &rc);
        winrt::check_hresult(renderTarget->BindDC(hdc, &rc));
        auto theme = view.as<winrt::Microsoft::ReactNative::Composition::ComponentView>().Theme();
        auto selfTheme = winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(theme);

        renderTarget->BeginDraw();
        renderTarget->Clear(selfTheme->D2DPlatformColor("ToolTipBackground"));

        auto textAttributes = facebook::react::TextAttributes{};
        facebook::react::Color fgColor;
        fgColor.m_platformColor.push_back("ToolTipForeground");
        textAttributes.foregroundColor = fgColor;

        winrt::Microsoft::ReactNative::Composition::RenderText(
            *renderTarget,
            *data->textLayout,
            data->attributedString.getValue(),
            textAttributes,
            {std::round(tooltipHorizontalPadding * scaleFactor), std::round(tooltipTopPadding * scaleFactor)},
            scaleFactor,
            *selfTheme);

        auto hr = renderTarget->EndDraw();
      }

      if (message != WM_PRINTCLIENT)
        EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_NCCREATE: {
      auto cs = reinterpret_cast<CREATESTRUCT *>(lparam);
      auto data = static_cast<TooltipData *>(cs->lpCreateParams);
      WINRT_ASSERT(data);
      SetProp(hwnd, TooltipDataProperty, reinterpret_cast<HANDLE>(data));
      break;
    }
  }

  return DefWindowProc(hwnd, message, wparam, lparam);
}

void RegisterTooltipWndClass() noexcept {
  static bool registered = false;
  if (registered) {
    return;
  }

  HINSTANCE hInstance =
      GetModuleHandle(NULL); // returns a handle to the file used to create the calling process (.exe file)

  WNDCLASSEX wcex = {}; // contains window class information
  wcex.cbSize = sizeof(wcex); // size of windows class (bytes)
  wcex.style = CS_HREDRAW | CS_VREDRAW; // class style (redraw window on size adjustment)
  wcex.lpfnWndProc = &TooltipWndProc; // pointer to windows procedure
  wcex.cbClsExtra = DLGWINDOWEXTRA; // extra bytes to allocate
  wcex.cbWndExtra = sizeof(TooltipData *); // extra bytes to allocate
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // handle to class cursor
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // background color
  wcex.lpszClassName = c_tooltipWindowClassName; // specify resource name
  ATOM classId = RegisterClassEx(&wcex); // register new windows class
  WINRT_VERIFY(classId); // 0 = fail
  winrt::check_win32(!classId);

  registered = true;
}

// Clamp tooltip position so it stays within the nearest monitor's work area.
// Flips the tooltip below the cursor if it would go above the work area.
void ClampTooltipToMonitor(POINT cursorScreenPt, int tooltipWidth, int tooltipHeight, float scaleFactor, int &x, int &y) noexcept {
  HMONITOR hMonitor = MonitorFromPoint(cursorScreenPt, MONITOR_DEFAULTTONEAREST);
  if (!hMonitor)
    return;

  MONITORINFO mi = {};
  mi.cbSize = sizeof(mi);
  if (!GetMonitorInfo(hMonitor, &mi))
    return;

  const RECT &workArea = mi.rcWork;

  // Clamp horizontally
  if (x + tooltipWidth > workArea.right) {
    x = workArea.right - tooltipWidth;
  }
  if (x < workArea.left) {
    x = workArea.left;
  }

  // If tooltip goes above the work area, flip it below the cursor
  if (y < workArea.top) {
    y = static_cast<int>(cursorScreenPt.y + (toolTipPlacementMargin * scaleFactor));
  }

  // If tooltip goes below the work area (after flip), clamp to bottom
  if (y + tooltipHeight > workArea.bottom) {
    y = workArea.bottom - tooltipHeight;
  }
}

TooltipTracker::TooltipTracker(
    const winrt::Microsoft::ReactNative::ComponentView &view,
    const winrt::Microsoft::ReactNative::ReactPropertyBag &properties,
    TooltipService *outer)
    : m_view(view), m_properties(properties), m_outer(outer) {
  view.PointerEntered({this, &TooltipTracker::OnPointerEntered});
  view.PointerExited({this, &TooltipTracker::OnPointerExited});
  view.PointerMoved({this, &TooltipTracker::OnPointerMoved});
  view.Unmounted({this, &TooltipTracker::OnUnmounted});
}

TooltipTracker::~TooltipTracker() {
  DestroyTimer();
  DestroyTooltip();
}

facebook::react::Tag TooltipTracker::Tag() const noexcept {
  return m_view.Tag();
}

void TooltipTracker::OnPointerEntered(
    const winrt::Windows::Foundation::IInspectable &sender,
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  if (args.Pointer().PointerDeviceType() !=
          winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse &&
      args.Pointer().PointerDeviceType() != winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Pen)
    return;

  auto pp = args.GetCurrentPoint(-1);
  m_pos = pp.Position();

  m_timer = winrt::Microsoft::ReactNative::Timer::Create(m_properties.Handle());
  m_timer.Interval(std::chrono::milliseconds(toolTipTimeToShowMs));
  m_timer.Tick({this, &TooltipTracker::OnTick});
  m_timer.Start();
}

void TooltipTracker::OnPointerMoved(
    const winrt::Windows::Foundation::IInspectable &sender,
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  if (args.Pointer().PointerDeviceType() !=
          winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse &&
      args.Pointer().PointerDeviceType() != winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Pen)
    return;

  auto pp = args.GetCurrentPoint(-1);
  m_pos = pp.Position();
}

void TooltipTracker::OnTick(
    const winrt::Windows::Foundation::IInspectable &,
    const winrt::Windows::Foundation::IInspectable &) noexcept {
  ShowTooltip(m_view.view());
}

void TooltipTracker::OnPointerExited(
    const winrt::Windows::Foundation::IInspectable &sender,
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  if (args.Pointer().PointerDeviceType() !=
          winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse &&
      args.Pointer().PointerDeviceType() != winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Pen)
    return;
  DestroyTimer();
  DestroyTooltip();
}

void TooltipTracker::DismissActiveTooltip() noexcept {
  DestroyTimer();
  DestroyTooltip();
}

void TooltipTracker::OnUnmounted(
    const winrt::Windows::Foundation::IInspectable &,
    const winrt::Microsoft::ReactNative::ComponentView &) noexcept {
  DestroyTimer();
  DestroyTooltip();
}

void TooltipTracker::ShowTooltip(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
  DestroyTimer();

  if (!m_hwndTip) {
    auto viewCompView = view.as<winrt::Microsoft::ReactNative::Composition::ViewComponentView>();
    auto selfView =
        winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::ViewComponentView>(viewCompView);
    auto parentHwnd = selfView->GetHwndForParenting();
    auto tooltipData = std::make_unique<TooltipData>(view);
    tooltipData->attributedString = CreateTooltipAttributedString(*selfView->viewProps()->tooltip);

    tooltipData->compositionContext = selfView->CompositionContext();
    tooltipData->view = view;

    auto scaleFactor = view.LayoutMetrics().PointScaleFactor;
    facebook::react::LayoutConstraints layoutConstraints;
    layoutConstraints.layoutDirection = facebook::react::LayoutDirection::Undefined;
    layoutConstraints.maximumSize.height = tooltipMaxHeight * scaleFactor;
    layoutConstraints.maximumSize.width = tooltipMaxWidth * scaleFactor;
    layoutConstraints.minimumSize.height = 0;
    layoutConstraints.minimumSize.width = 0;

    facebook::react::WindowsTextLayoutManager::GetTextLayout(
        tooltipData->attributedString, {} /*paragraphAttributes*/, layoutConstraints, tooltipData->textLayout);

    if (tooltipData->textLayout) {
      DWRITE_TEXT_METRICS tm;
      winrt::check_hresult(tooltipData->textLayout->GetMetrics(&tm));

      tooltipData->width =
          static_cast<int>((tm.width + tooltipHorizontalPadding + tooltipHorizontalPadding) * scaleFactor);
      tooltipData->height = static_cast<int>((tm.height + tooltipTopPadding + tooltipBottomPadding) * scaleFactor);

      // Convert island-local DIP coordinates to screen pixel coordinates.
      // Use LocalToScreen which properly handles both ContentIsland and HWND hosting.
      auto screenPt = selfView->LocalToScreen({m_pos.X, m_pos.Y});
      POINT pt = {static_cast<LONG>(screenPt.X), static_cast<LONG>(screenPt.Y)};

      // Calculate initial desired tooltip position and clamp to monitor work area
      int tooltipX = pt.x - tooltipData->width / 2;
      int tooltipY = static_cast<int>(pt.y - tooltipData->height - (toolTipPlacementMargin * scaleFactor));
      ClampTooltipToMonitor(pt, tooltipData->width, tooltipData->height, scaleFactor, tooltipX, tooltipY);

      RegisterTooltipWndClass();
      HINSTANCE hInstance = GetModuleHandle(NULL);
      m_hwndTip = CreateWindow(
          c_tooltipWindowClassName,
          L"Tooltip",
          WS_POPUP,
          tooltipX,
          tooltipY,
          tooltipData->width,
          tooltipData->height,
          parentHwnd,
          NULL,
          hInstance,
          tooltipData.get());

      DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUNDSMALL;
      UINT borderThickness = 0;
      DwmSetWindowAttribute(m_hwndTip, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

      tooltipData.release();
      AnimateWindow(m_hwndTip, toolTipAnimationTimeMs, AW_BLEND);
    }
  }
}

void TooltipTracker::DestroyTooltip() noexcept {
  if (m_hwndTip) {
    AnimateWindow(m_hwndTip, toolTipAnimationTimeMs, AW_BLEND | AW_HIDE);
    DestroyWindow(m_hwndTip);
    m_hwndTip = nullptr;
  }
}

void TooltipTracker::DestroyTimer() noexcept {
  if (m_timer) {
    m_timer.Stop();
    m_timer = nullptr;
  }
}

TooltipService::TooltipService(const winrt::Microsoft::ReactNative::ReactPropertyBag &properties)
    : m_properties(properties) {}

void TooltipService::StartTracking(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
  m_trackers.push_back(std::make_shared<TooltipTracker>(view, m_properties, this));
}

void TooltipService::StopTracking(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
  for (auto it = m_trackers.begin(); it != m_trackers.end();) {
    if ((*it)->Tag() == view.Tag())
      it = m_trackers.erase(it);
    else
      ++it;
  }
}

void TooltipService::DismissAllTooltips() noexcept {
  for (auto &tracker : m_trackers) {
    tracker->DismissActiveTooltip();
  }
}

static const ReactPropertyId<winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<TooltipService>>>
    &TooltipServicePropertyId() noexcept {
  static const ReactPropertyId<winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<TooltipService>>> prop{
      L"ReactNative", L"TooltipService"};
  return prop;
}

std::shared_ptr<TooltipService> TooltipService::GetCurrent(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &properties) noexcept {
  return *properties.GetOrCreate(TooltipServicePropertyId(), [properties]() -> std::shared_ptr<TooltipService> {
    return std::make_shared<TooltipService>(properties);
  });
}

} // namespace winrt::Microsoft::ReactNative
