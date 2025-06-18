
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ScrollViewComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)

#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.ViewManagement.Core.h>

#include <AutoDraw.h>
#include <Fabric/DWriteHelpers.h>
#include <unicode.h>
#include "CompositionDynamicAutomationProvider.h"
#include "JSValueReader.h"
#include "RootComponentView.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

constexpr float c_scrollerLineDelta = 16.0f;

enum class ScrollbarHitRegion : int {
  Unknown = -1,
  ArrowFirst = 0,
  PageUp = 1,
  Thumb = 2,
  PageDown = 3,
  ArrowLast = 4,
};

struct ScrollBarComponent {
  ScrollBarComponent(
      const winrt::Microsoft::ReactNative::Composition::ScrollViewComponentView &outer,
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      bool vertical)
      : m_wkOuter(outer), m_compContext(compContext), m_reactContext(reactContext), m_vertical(vertical) {
    m_rootVisual = m_compContext.CreateSpriteVisual();
    m_trackVisual = m_compContext.CreateRoundedRectangleVisual();
    m_thumbVisual = m_compContext.CreateRoundedRectangleVisual();
    m_arrowVisualFirst = m_compContext.CreateSpriteVisual();
    m_arrowVisualLast = m_compContext.CreateSpriteVisual();

    m_rootVisual.InsertAt(m_trackVisual, 0);
    m_rootVisual.InsertAt(m_arrowVisualFirst, 1);
    m_rootVisual.InsertAt(m_arrowVisualLast, 2);
    m_rootVisual.InsertAt(m_thumbVisual, 3);

    m_trackVisual.AnimationClass(winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBar);
    m_arrowVisualFirst.AnimationClass(
        winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBar);
    m_arrowVisualLast.AnimationClass(
        winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBar);
    m_thumbVisual.AnimationClass(
        vertical ? winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBarThumbVertical
                 : winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBarThumbHorizontal);

    updateShy(true);
    onScaleChanged();
    UpdateColorForScrollBarRegions();
  }

  void UpdateColorForScrollBarRegions() noexcept {
    updateHighlight(ScrollbarHitRegion::ArrowFirst);
    updateHighlight(ScrollbarHitRegion::ArrowLast);
    updateHighlight(ScrollbarHitRegion::Thumb);

    if (auto outer = m_wkOuter.get()) {
      m_trackVisual.Brush(
          winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(outer.Theme())
              ->InternalPlatformBrush(L"ScrollBarTrackFill"));
    }
  }

  void ContentSize(winrt::Windows::Foundation::Size contentSize) noexcept {
    if (m_contentSize == contentSize) {
      return;
    }
    m_contentSize = contentSize;
    updateThumb();
    updateVisibility(m_visible);
  }

  void updateTrack() noexcept {
    if (m_vertical) {
      m_trackVisual.Size({m_arrowSize, std::max(m_size.Height - (m_trackMargin * 2), 0.0f)});
      m_trackVisual.Offset({-m_arrowSize, m_trackMargin, 0.0f}, {1.0f, 0.0f, 0.0f});
    } else {
      m_trackVisual.Size({std::max(m_size.Width - (m_trackMargin * 2), 0.0f), m_arrowSize});
      m_trackVisual.Offset({m_trackMargin, -m_arrowSize, 0.0f}, {0.0f, 1.0f, 0.0f});
    }
  }

  void updateLayoutMetrics(facebook::react::LayoutMetrics const &layoutMetrics) noexcept {
    m_size = {
        layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
        layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor};

    if (m_scaleFactor != layoutMetrics.pointScaleFactor) {
      m_scaleFactor = layoutMetrics.pointScaleFactor;
      onScaleChanged();
    }

    updateTrack();
    updateThumb();
    updateVisibility(m_visible);
  }

  void updateVisibility(bool visible) noexcept {
    if ((m_size.Width <= 0.0f && m_size.Height <= 0.0f) ||
        (m_contentSize.Width <= 0.0f && m_contentSize.Height <= 0.0f)) {
      m_rootVisual.IsVisible(false);
      return;
    }

    if (!visible) {
      m_visible = false;
      m_rootVisual.IsVisible(visible);
      return;
    }

    bool newVisibility = false;
    if (m_vertical) {
      newVisibility = (m_contentSize.Height > m_size.Height);
    } else {
      newVisibility = (m_contentSize.Width > m_size.Width);
    }

    m_visible = newVisibility;
    m_rootVisual.IsVisible(m_visible);
  }

  void updateRootAndArrowVisualOffsets() noexcept {
    if (m_vertical) {
      m_rootVisual.RelativeSizeWithOffset({m_arrowSize, 0.0f}, {0.0f, 1.0f});
      m_rootVisual.Offset({-(m_arrowSize + m_trackEdgeMargin), 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
      m_arrowVisualFirst.Offset({0.0f, m_arrowMargin, 0.0f});
      m_arrowVisualLast.Offset({0.0f, -(m_arrowSize + m_arrowMargin), 0.0f}, {0.0f, 1.0f, 0.0f});
    } else {
      m_rootVisual.RelativeSizeWithOffset({0.0f, m_arrowSize}, {1.0f, 0.0f});
      m_rootVisual.Offset({0.0f, -(m_arrowSize + m_trackEdgeMargin), 0.0f}, {0.0f, 1.0f, 0.0f});
      m_arrowVisualFirst.Offset({m_arrowMargin, 0.0f, 0.0f});
      m_arrowVisualLast.Offset({-(m_arrowSize + m_arrowMargin), 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
    }
  }

  void onScaleChanged() noexcept {
    m_arrowSize = 12 * m_scaleFactor; // From Xaml resource: ScrollBarSize
    m_thumbWidth = 6 * m_scaleFactor; // From Xaml resource: ScrollBarThumbStrokeThickness
    m_thumbShyWidth = 2 * m_scaleFactor;
    m_arrowMargin = 4 * m_scaleFactor; // From Xaml resource: ScrollBar(Vertical|Horizontal)(Increase|Decrease)Margin
    m_trackEdgeMargin = 1 * m_scaleFactor; // From Xaml resource: ScrollViewerScrollBarMargin
    m_trackMargin =
        (2 + 1) * m_scaleFactor; // From Xaml template VerticalPanningThumb.Margin + ScrollViewerScrollBarMargin
    m_minThumbSize = static_cast<int>(
        30 * m_scaleFactor); // From Xaml resource: ScrollBarVerticalThumbMinHeight / ScrollBarHorizontalThumbMinWidth

    m_thumbVisual.CornerRadius({m_thumbWidth / 2.0f, m_thumbWidth / 2.0f});
    m_trackVisual.CornerRadius({m_arrowSize / 2.0f, m_arrowSize / 2.0f});

    m_arrowVisualFirst.Size({m_arrowSize, m_arrowSize});
    m_arrowVisualLast.Size({m_arrowSize, m_arrowSize});
    m_arrowFirstDrawingSurface = nullptr; // Reset arrow textures when scale changes
    m_arrowLastDrawingSurface = nullptr;
    updateRootAndArrowVisualOffsets();
  }

  void ContentOffset(const winrt::Windows::Foundation::Numerics::float3 &offset) noexcept {
    m_offset = offset;
    updateThumb();
  }

  ScrollbarHitRegion HitTest(winrt::Windows::Foundation::Point pt) noexcept {
    pt = {pt.X * m_scaleFactor, pt.Y * m_scaleFactor};
    if (m_vertical) {
      if (pt.X < m_size.Width - (m_arrowSize + m_trackEdgeMargin)) {
        return ScrollbarHitRegion::Unknown;
      }

      if (pt.Y < (m_arrowSize + m_arrowMargin)) {
        return ScrollbarHitRegion::ArrowFirst;
      }

      if (pt.Y < (m_arrowSize + m_arrowMargin) + m_thumbPos) {
        return ScrollbarHitRegion::PageUp;
      }

      if (pt.Y < (m_arrowSize + m_arrowMargin) + m_thumbPos + m_thumbSize) {
        return ScrollbarHitRegion::Thumb;
      }

      if (pt.Y < m_size.Height - (m_arrowSize + m_arrowMargin)) {
        return ScrollbarHitRegion::PageDown;
      }

      return ScrollbarHitRegion::ArrowLast;
    } else {
      if (pt.Y < m_size.Height - (m_arrowSize + m_trackEdgeMargin)) {
        return ScrollbarHitRegion::Unknown;
      }

      if (pt.X < (m_arrowSize + m_arrowMargin)) {
        return ScrollbarHitRegion::ArrowFirst;
      }

      if (pt.X < (m_arrowSize + m_arrowMargin) + m_thumbPos) {
        return ScrollbarHitRegion::PageUp;
      }

      if (pt.X < (m_arrowSize + m_arrowMargin) + m_thumbPos + m_thumbSize) {
        return ScrollbarHitRegion::Thumb;
      }

      if (pt.X < m_size.Width - (m_arrowSize + m_arrowMargin)) {
        return ScrollbarHitRegion::PageDown;
      }

      return ScrollbarHitRegion::ArrowLast;
    }

    return ScrollbarHitRegion::Unknown;
  }

  int getViewportSize() const noexcept {
    return static_cast<int>(m_vertical ? m_size.Height : m_size.Width);
  }

  int calulateMaxThumbLength() const noexcept {
    return std::max(static_cast<int>(getViewportSize() - (2 * (m_arrowSize + m_arrowMargin))), 0);
  }

  int getScrollRange() const noexcept {
    return static_cast<int>(m_vertical ? m_contentSize.Height : m_contentSize.Width);
  }

  int getComputedPixelThumbSize() const noexcept {
    auto maxThumbLength = calulateMaxThumbLength();
    auto scrollRange = getScrollRange();
    auto viewportSize = getViewportSize();
    return std::max(::MulDiv(std::min(scrollRange, viewportSize), maxThumbLength, scrollRange), 0);
  }

  float scrollOffsetFromThumbPos(int thumbPos) const noexcept {
    auto maxThumbLength = calulateMaxThumbLength();
    auto scrollRange = getScrollRange();

    const int computedPixelThumbSize = getComputedPixelThumbSize();

    int thumbCorrection = 0;
    if (!(m_minThumbSize <= computedPixelThumbSize || maxThumbLength < 2 * m_minThumbSize)) {
      thumbCorrection = m_minThumbSize - computedPixelThumbSize;
    }
    return std::clamp(
        static_cast<float>(::MulDiv(thumbPos, scrollRange, (maxThumbLength - thumbCorrection))),
        0.0f,
        static_cast<float>(scrollRange));
  }

  void updateThumb() noexcept {
    auto maxThumbLength = calulateMaxThumbLength();
    auto scrollRange = getScrollRange();
    auto scrollOffset = static_cast<int>(m_vertical ? m_offset.y : m_offset.x);

    const int computedPixelThumbSize = getComputedPixelThumbSize();

    m_thumbSize = 0;
    int thumbCorrection = 0;

    if (m_minThumbSize <= computedPixelThumbSize || maxThumbLength < 2 * m_minThumbSize)
      m_thumbSize = computedPixelThumbSize;
    else {
      thumbCorrection = m_minThumbSize - computedPixelThumbSize;
      m_thumbSize = m_minThumbSize;
    }

    // Position is relative to available area for thumb
    m_thumbPos = (scrollRange > 0 ? ::MulDiv(scrollOffset, maxThumbLength - thumbCorrection, scrollRange) : 0);

    auto thumbOffset = (m_arrowSize - m_thumbWidth) / 2.0f;
    auto shyOffset = m_shy ? (m_thumbWidth - m_thumbShyWidth) : 0.0f;
    auto thumbScale = m_shy ? (m_thumbShyWidth / m_thumbWidth) : 1.0f;

    if (m_vertical) {
      m_thumbVisual.Size({m_thumbWidth, static_cast<float>(m_thumbSize)});
      m_thumbVisual.Offset(
          {-m_arrowSize + thumbOffset + shyOffset, m_arrowSize + m_arrowMargin + static_cast<float>(m_thumbPos), 0.0f},
          {1.0f, 0.0f, 0.0f});
      m_thumbVisual.Scale({thumbScale, 1.0f, 1.0f});
    } else {
      m_thumbVisual.Size({static_cast<float>(m_thumbSize), m_thumbWidth});
      m_thumbVisual.Offset(
          {m_arrowSize + m_arrowMargin + static_cast<float>(m_thumbPos), -m_arrowSize + thumbOffset + shyOffset, 0.0f},
          {0.0f, 1.0f, 0.0f});
      m_thumbVisual.Scale({1.0f, thumbScale, 1.0f});
    }
  }

  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual Visual() const noexcept {
    return m_rootVisual;
  }

  void OnPointerReleased(const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) {
    if (auto outer = m_wkOuter.get()) {
      if (!m_visible)
        return;
      auto pt = args.GetCurrentPoint(outer.Tag());
      if (m_nTrackInputOffset != -1 &&
          pt.PointerDeviceType() == winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse &&
          pt.Properties().PointerUpdateKind() ==
              winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::LeftButtonReleased) {
        handleMoveThumb(args);
        stopTrackingThumb();
        outer.ReleasePointerCapture(args.Pointer());

        auto reg = HitTest(pt.Position());
        updateShy(reg == ScrollbarHitRegion::Unknown);
      }
    }
  }

  void stopTrackingThumb() noexcept {
    m_nTrackInputOffset = -1;
    m_thumbVisual.AnimationClass(
        m_vertical
            ? winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBarThumbVertical
            : winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::ScrollBarThumbHorizontal);
  }

  void handleMoveThumb(const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) {
    if (auto outer = m_wkOuter.get()) {
      auto pt = args.GetCurrentPoint(outer.Tag());
      auto pos = pt.Position();

      auto newTrackingPosition = static_cast<int>((m_vertical ? pos.Y : pos.X) * m_scaleFactor) - m_nTrackInputOffset;
      winrt::get_self<ScrollViewComponentView>(outer)->scrollTo(
          m_vertical ? winrt::Windows::Foundation::Numerics::
                           float3{m_offset.x, scrollOffsetFromThumbPos(newTrackingPosition), m_offset.z}
                     : winrt::Windows::Foundation::Numerics::
                           float3{scrollOffsetFromThumbPos(newTrackingPosition), m_offset.y, m_offset.z},
          false);
    }
    args.Handled(true);
  }

  void OnPointerPressed(const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) {
    if (auto outer = m_wkOuter.get()) {
      if (!m_visible)
        return;
      auto pt = args.GetCurrentPoint(outer.Tag());
      if (pt.PointerDeviceType() == winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse) {
        auto pos = pt.Position();
        auto reg = HitTest(pos);

        switch (reg) {
          case ScrollbarHitRegion::ArrowFirst:
            if (m_vertical) {
              winrt::get_self<ScrollViewComponentView>(outer)->lineUp(false);
            } else {
              winrt::get_self<ScrollViewComponentView>(outer)->lineLeft(false);
            }
            args.Handled(true);
            break;
          case ScrollbarHitRegion::ArrowLast:
            if (m_vertical) {
              winrt::get_self<ScrollViewComponentView>(outer)->lineDown(false);
            } else {
              winrt::get_self<ScrollViewComponentView>(outer)->lineRight(false);
            }
            args.Handled(true);
            break;
          case ScrollbarHitRegion::PageUp:
            if (m_vertical) {
              winrt::get_self<ScrollViewComponentView>(outer)->pageUp(false);
            }
            args.Handled(true);
            break;
          case ScrollbarHitRegion::PageDown:
            if (m_vertical) {
              winrt::get_self<ScrollViewComponentView>(outer)->pageDown(false);
            }
            args.Handled(true);
            break;
          case ScrollbarHitRegion::Thumb: {
            outer.CapturePointer(args.Pointer());
            m_nTrackInputOffset = static_cast<int>((m_vertical ? pos.Y : pos.X) * m_scaleFactor) - m_thumbPos;
            m_thumbVisual.AnimationClass(
                winrt::Microsoft::ReactNative::Composition::Experimental::AnimationClass::None);
            handleMoveThumb(args);
          }
        }
      }
    }
  }

  void OnPointerMoved(const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) {
    if (auto outer = m_wkOuter.get()) {
      if (!m_visible)
        return;
      auto pt = args.GetCurrentPoint(outer.Tag());
      if (pt.PointerDeviceType() == winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse) {
        if (m_nTrackInputOffset != -1) {
          handleMoveThumb(args);
        } else {
          auto pos = pt.Position();
          auto reg = HitTest(pos);
          updateShy(reg == ScrollbarHitRegion::Unknown);
          setHighlightedRegion(reg);
        }
      }
    }
  }

  void OnPointerCaptureLost() {
    if (!m_visible)
      return;

    stopTrackingThumb();
    updateShy(true);
  }

  void updateShy(bool shy) {
    if (shy == m_shy)
      return;

    m_shy = shy;

    m_trackVisual.Opacity(m_shy ? 0.0f : 1.0f);
    m_arrowVisualFirst.Opacity(m_shy ? 0.0f : 1.0f);
    m_arrowVisualLast.Opacity(m_shy ? 0.0f : 1.0f);

    updateThumb();
  }

  void setHighlightedRegion(ScrollbarHitRegion region) noexcept {
    if (m_highlightedRegion == region)
      return;

    auto oldRegion = m_highlightedRegion;
    m_highlightedRegion = region;
    updateHighlight(oldRegion);
    updateHighlight(m_highlightedRegion);
  }

  // Renders the text into our composition surface
  void drawArrow(ScrollbarHitRegion region, bool disabled, bool hovered) noexcept {
    if (auto outer = m_wkOuter.get()) {
      auto &drawingSurface =
          (region == ScrollbarHitRegion::ArrowFirst) ? m_arrowFirstDrawingSurface : m_arrowLastDrawingSurface;
      if (!drawingSurface) {
        drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
            {m_arrowSize, m_arrowSize},
            winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
      }

      if (winrt::get_self<ScrollViewComponentView>(outer)->theme()->IsEmpty()) {
        return;
      }

      winrt::com_ptr<IDWriteTextFormat> spTextFormat;
      winrt::check_hresult(::Microsoft::ReactNative::DWriteFactory()->CreateTextFormat(
          L"Segoe Fluent Icons",
          nullptr, // Font collection (nullptr sets it to use the system font collection).
          DWRITE_FONT_WEIGHT_REGULAR,
          DWRITE_FONT_STYLE_NORMAL,
          DWRITE_FONT_STRETCH_NORMAL,
          8, //  Xaml resource: ScrollBarButtonArrowIconFontSize
          L"",
          spTextFormat.put()));

      winrt::check_hresult(spTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));

      winrt::com_ptr<IDWriteTextLayout> spTextLayout;
      winrt::check_hresult(::Microsoft::ReactNative::DWriteFactory()->CreateTextLayout(
          m_vertical ? ((region == ScrollbarHitRegion::ArrowFirst) ? L"\uEDDB" : L"\uEDDC")
                     : ((region == ScrollbarHitRegion::ArrowFirst) ? L"\uEDD9" : L"\uEDDA"),
          1, // The length of the string.
          spTextFormat.get(), // The text format to apply to the string (contains font information, etc).
          (m_arrowSize / m_scaleFactor), // The width of the layout box.
          (m_arrowSize / m_scaleFactor), // The height of the layout box.
          spTextLayout.put() // The IDWriteTextLayout interface pointer.
          ));

      POINT offset;
      {
        ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(drawingSurface, m_scaleFactor, &offset);
        if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
          d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
          assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);

          // Create a solid color brush for the text. A more sophisticated application might want
          // to cache and reuse a brush across all text elements instead, taking care to recreate
          // it in the event of device removed.
          winrt::com_ptr<ID2D1SolidColorBrush> brush;

          D2D1::ColorF color{0};
          if (disabled) {
            color = winrt::get_self<ScrollViewComponentView>(outer)->theme()->D2DPlatformColor(
                "ScrollBarButtonArrowForegroundDisabled");
          } else if (hovered) {
            color = winrt::get_self<ScrollViewComponentView>(outer)->theme()->D2DPlatformColor(
                "ScrollBarButtonArrowForegroundPointerOver");
          } else {
            color = winrt::get_self<ScrollViewComponentView>(outer)->theme()->D2DPlatformColor(
                "ScrollBarButtonArrowForeground");
          }
          winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(color, brush.put()));

          {
            DWRITE_TEXT_METRICS dtm{};
            winrt::check_hresult(spTextLayout->GetMetrics(&dtm));
            offset.y += static_cast<int>((m_arrowSize - dtm.height) / 2.0f);
          }

          // Draw the line of text at the specified offset, which corresponds to the top-left
          // corner of our drawing surface. Notice we don't call BeginDraw on the D2D device
          // context; this has already been done for us by the composition API.
          d2dDeviceContext->DrawTextLayout(
              D2D1::Point2F(
                  static_cast<FLOAT>((offset.x) / m_scaleFactor), static_cast<FLOAT>((offset.y) / m_scaleFactor)),
              spTextLayout.get(),
              brush.get(),
              D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        }
      }
      if (drawingSurface) {
        drawingSurface.HorizontalAlignmentRatio(0.0f);
        drawingSurface.VerticalAlignmentRatio(0.0f);
        drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);
      }

      auto &arrowVisual = (region == ScrollbarHitRegion::ArrowFirst) ? m_arrowVisualFirst : m_arrowVisualLast;
      arrowVisual.Brush(drawingSurface);
    }
  }

  void updateHighlight(ScrollbarHitRegion region) noexcept {
    if (auto outer = m_wkOuter.get()) {
      switch (region) {
        case ScrollbarHitRegion::ArrowFirst:
        case ScrollbarHitRegion::ArrowLast: {
          auto disabled = !std::static_pointer_cast<const facebook::react::ScrollViewProps>(
                               winrt::get_self<ScrollViewComponentView>(outer)->viewProps())
                               ->scrollEnabled;
          drawArrow(region, disabled, m_highlightedRegion == region);
        }
        case ScrollbarHitRegion::Thumb: {
          if (!std::static_pointer_cast<const facebook::react::ScrollViewProps>(
                   winrt::get_self<ScrollViewComponentView>(outer)->viewProps())
                   ->scrollEnabled) {
            m_thumbVisual.Brush(
                winrt::get_self<Theme>(outer.Theme())->InternalPlatformBrush(L"ScrollBarThumbFillDisabled"));
          } else if (m_highlightedRegion == region) {
            m_thumbVisual.Brush(
                winrt::get_self<Theme>(outer.Theme())->InternalPlatformBrush(L"ScrollBarThumbFillPointerOver"));
          } else {
            m_thumbVisual.Brush(winrt::get_self<Theme>(outer.Theme())->InternalPlatformBrush(L"ScrollBarThumbFill"));
          }
        }
      }
    }
  }

 private:
  winrt::weak_ref<winrt::Microsoft::ReactNative::Composition::ScrollViewComponentView> m_wkOuter;
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext m_compContext;
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
  const bool m_vertical;
  bool m_visible{true};
  bool m_shy{false};
  int m_thumbSize{0};
  float m_arrowSize{0};
  float m_arrowMargin{0}; // margin on outside end of arrow buttons
  float m_trackEdgeMargin{0}; // margin between track and edge of component
  float m_trackMargin{0}; // margin of track background to ends of scrollbar
  float m_thumbWidth{0};
  float m_thumbShyWidth{0};
  int m_minThumbSize{0};
  float m_scaleFactor{1};
  int m_thumbPos{0};
  int m_nTrackInputOffset{-1};
  ScrollbarHitRegion m_highlightedRegion{ScrollbarHitRegion::Unknown};
  winrt::Windows::Foundation::Numerics::float3 m_offset{0};
  winrt::Windows::Foundation::Size m_contentSize{0, 0};
  winrt::Windows::Foundation::Size m_size{0, 0};
  winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual m_rootVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IRoundedRectangleVisual m_thumbVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual m_arrowVisualFirst{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual m_arrowVisualLast{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush m_arrowFirstDrawingSurface{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush m_arrowLastDrawingSurface{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IRoundedRectangleVisual m_trackVisual{nullptr};
};

winrt::Microsoft::ReactNative::ComponentView ScrollViewComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<ScrollViewComponentView>(compContext, tag, reactContext);
}

facebook::react::SharedViewProps ScrollViewComponentView::defaultProps() noexcept {
  static auto const defaultViewProps = std::make_shared<facebook::react::ScrollViewProps const>();
  return defaultViewProps;
}

ScrollViewComponentView::ScrollViewComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(
          ScrollViewComponentView::defaultProps(),
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background) {
  // m_element.Content(m_contentPanel);

  /*
    m_scrollViewerViewChangingRevoker =
        m_element.ViewChanging(winrt::auto_revoke, [this](const auto &sender, const auto &args) {
          const auto scrollViewerNotNull = sender.as<xaml::Controls::ScrollViewer>();

          facebook::react::ScrollViewMetrics scrollMetrics;
          scrollMetrics.containerSize.height = static_cast<facebook::react::Float>(m_element.ActualHeight());
          scrollMetrics.containerSize.width = static_cast<facebook::react::Float>(m_element.ActualWidth());
          scrollMetrics.contentOffset.x = static_cast<facebook::react::Float>(args.NextView().HorizontalOffset());
          scrollMetrics.contentOffset.y = static_cast<facebook::react::Float>(args.NextView().VerticalOffset());
          scrollMetrics.zoomScale = args.NextView().ZoomFactor();
          scrollMetrics.contentSize.height = static_cast<facebook::react::Float>(m_contentPanel.ActualHeight());
          scrollMetrics.contentSize.width = static_cast<facebook::react::Float>(m_contentPanel.ActualWidth());

          // If we are transitioning to inertial scrolling.
          if (m_isScrolling && !m_isScrollingFromInertia && args.IsInertial()) {
            m_isScrollingFromInertia = true;

            if (m_eventEmitter) {
              std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                  ->onScrollEndDrag(scrollMetrics);
              std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                  ->onMomentumScrollBegin(scrollMetrics);
            }
          }

          if (m_eventEmitter) {
            std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                ->onScroll(scrollMetrics);
          }
        });

    m_scrollViewerDirectManipulationStartedRevoker =
        m_element.DirectManipulationStarted(winrt::auto_revoke, [this](const auto &sender, const auto &) {
          m_isScrolling = true;


          //if (m_dismissKeyboardOnDrag && m_SIPEventHandler) {
          //  m_SIPEventHandler->TryHide();
          //}


          facebook::react::ScrollViewMetrics scrollMetrics;
          scrollMetrics.containerSize.height = static_cast<facebook::react::Float>(m_element.ActualHeight());
          scrollMetrics.containerSize.width = static_cast<facebook::react::Float>(m_element.ActualWidth());
          scrollMetrics.contentOffset.x = static_cast<facebook::react::Float>(m_element.HorizontalOffset());
          scrollMetrics.contentOffset.y = static_cast<facebook::react::Float>(m_element.VerticalOffset());
          scrollMetrics.zoomScale = m_element.ZoomFactor();
          scrollMetrics.contentSize.height = static_cast<facebook::react::Float>(m_contentPanel.ActualHeight());
          scrollMetrics.contentSize.width = static_cast<facebook::react::Float>(m_contentPanel.ActualWidth());

          const auto scrollViewer = sender.as<xaml::Controls::ScrollViewer>();
          if (m_eventEmitter) {
            std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                ->onScrollBeginDrag(scrollMetrics);
          }
        });

    m_scrollViewerDirectManipulationCompletedRevoker =
        m_element.DirectManipulationCompleted(winrt::auto_revoke, [this](const auto &sender, const auto &args) {
          const auto scrollViewer = sender.as<xaml::Controls::ScrollViewer>();

          facebook::react::ScrollViewMetrics scrollMetrics;
          scrollMetrics.containerSize.height = static_cast<facebook::react::Float>(m_element.ActualHeight());
          scrollMetrics.containerSize.width = static_cast<facebook::react::Float>(m_element.ActualWidth());
          scrollMetrics.contentOffset.x = static_cast<facebook::react::Float>(m_element.HorizontalOffset());
          scrollMetrics.contentOffset.y = static_cast<facebook::react::Float>(m_element.VerticalOffset());
          scrollMetrics.zoomScale = m_element.ZoomFactor();
          scrollMetrics.contentSize.height = static_cast<facebook::react::Float>(m_contentPanel.ActualHeight());
          scrollMetrics.contentSize.width = static_cast<facebook::react::Float>(m_contentPanel.ActualWidth());

          if (m_eventEmitter) {
            if (m_isScrollingFromInertia) {
              std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                  ->onMomentumScrollEnd(scrollMetrics);
            } else {
              std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                  ->onScrollEndDrag(scrollMetrics);
            }
          }

          m_isScrolling = false;
          m_isScrollingFromInertia = false;
        });
        */
}

void ScrollViewComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  // Call ComponentView::UnmountChildComponentView instead of base to handle out own Visual hosting
  ComponentView::MountChildComponentView(childComponentView, index);

  ensureVisual();
  m_scrollVisual.InsertAt(
      childComponentView.as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()->OuterVisual(),
      index);
}

void ScrollViewComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  // Call ComponentView::UnmountChildComponentView instead of base to handle out own Visual hosting
  ComponentView::UnmountChildComponentView(childComponentView, index);
  m_scrollVisual.Remove(childComponentView.as<ComponentView>()->OuterVisual());
}

void ScrollViewComponentView::updateBackgroundColor(const facebook::react::SharedColor &color) noexcept {
  if (color) {
    m_scrollVisual.Brush(theme()->Brush(*color));
  } else {
    m_scrollVisual.Brush(m_compContext.CreateColorBrush({0, 0, 0, 0}));
  }
}

void ScrollViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(props);
  const auto &oldViewProps =
      *std::static_pointer_cast<const facebook::react::ScrollViewProps>(oldProps ? oldProps : viewProps());

  ensureVisual();

  if (!oldProps || oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    updateBackgroundColor(newViewProps.backgroundColor);
  }

  // update BaseComponentView props
  base_type::updateProps(props, oldProps);

  // Update the color only after updating the m_props in BaseComponentView
  // to avoid scrollbarcomponents reading outdated scrollEnabled value.
  if (!oldProps || oldViewProps.scrollEnabled != newViewProps.scrollEnabled) {
    m_scrollVisual.ScrollEnabled(newViewProps.scrollEnabled);
    m_horizontalScrollbarComponent->UpdateColorForScrollBarRegions();
    m_verticalScrollbarComponent->UpdateColorForScrollBarRegions();
  }

  if (!oldProps || oldViewProps.horizontal != newViewProps.horizontal) {
    m_scrollVisual.Horizontal(newViewProps.horizontal);
  }

  if (!oldProps || oldViewProps.showsHorizontalScrollIndicator != newViewProps.showsHorizontalScrollIndicator) {
    updateShowsHorizontalScrollIndicator(newViewProps.showsHorizontalScrollIndicator);
  }

  if (!oldProps || oldViewProps.showsVerticalScrollIndicator != newViewProps.showsVerticalScrollIndicator) {
    updateShowsVerticalScrollIndicator(newViewProps.showsVerticalScrollIndicator);
  }

  if (!oldProps || oldViewProps.decelerationRate != newViewProps.decelerationRate) {
    updateDecelerationRate(newViewProps.decelerationRate);
  }

  if (!oldProps || oldViewProps.scrollEventThrottle != newViewProps.scrollEventThrottle) {
    // Zero means "send value only once per significant logical event".
    // Prop value is in milliseconds.
    auto throttleInSeconds = newViewProps.scrollEventThrottle / 1000.0;
    auto msPerFrame = 1.0 / 60.0;
    if (throttleInSeconds < 0) {
      m_scrollEventThrottle = INFINITY;
    } else if (throttleInSeconds <= msPerFrame) {
      m_scrollEventThrottle = 0;
    } else {
      m_scrollEventThrottle = throttleInSeconds;
    }
  }

  if (oldViewProps.maximumZoomScale != newViewProps.maximumZoomScale) {
    m_scrollVisual.SetMaximumZoomScale(newViewProps.maximumZoomScale);
  }

  if (oldViewProps.minimumZoomScale != newViewProps.minimumZoomScale) {
    m_scrollVisual.SetMinimumZoomScale(newViewProps.minimumZoomScale);
  }

  if (oldViewProps.zoomScale != newViewProps.zoomScale) {
    m_scrollVisual.Scale({newViewProps.zoomScale, newViewProps.zoomScale, newViewProps.zoomScale});
  }

  if (!oldProps || oldViewProps.snapToStart != newViewProps.snapToStart) {
    m_scrollVisual.SnapToStart(newViewProps.snapToStart);
  }

  if (!oldProps || oldViewProps.snapToOffsets != newViewProps.snapToOffsets) {
    // Convert from std::vector<facebook::react::Float> to winrt::IVectorView<float>
    const auto snapToOffsets = winrt::single_threaded_vector<float>();
    for (const auto &offset : newViewProps.snapToOffsets) {
      snapToOffsets.Append(static_cast<float>(offset));
    }
    m_scrollVisual.SnapToOffsets(snapToOffsets.GetView());
  }
}

void ScrollViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  m_state = std::static_pointer_cast<facebook::react::ScrollViewShadowNode::ConcreteState const>(state);
  m_contentSize = m_state->getData().getContentSize();
  updateContentVisualSize();
}

void ScrollViewComponentView::updateStateWithContentOffset() noexcept {
  if (!m_state) {
    return;
  }

  auto scrollPosition = m_scrollVisual.ScrollPosition();
  m_verticalScrollbarComponent->ContentOffset(scrollPosition);
  m_horizontalScrollbarComponent->ContentOffset(scrollPosition);

  m_state->updateState([scrollPosition](const facebook::react::ScrollViewShadowNode::ConcreteState::Data &data) {
    auto newData = data;
    newData.contentOffset = {scrollPosition.x, scrollPosition.y};
    return std::make_shared<facebook::react::ScrollViewShadowNode::ConcreteState::Data const>(newData);
  });
}

void ScrollViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  ensureVisual();
  if (oldLayoutMetrics != layoutMetrics) {
    m_verticalScrollbarComponent->updateLayoutMetrics(layoutMetrics);
    m_horizontalScrollbarComponent->updateLayoutMetrics(layoutMetrics);
    base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
    m_scrollVisual.Size(
        {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
         layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
    updateContentVisualSize();
  }
}

void ScrollViewComponentView::updateContentVisualSize() noexcept {
  winrt::Windows::Foundation::Size contentSize = {
      std::max(m_contentSize.width, m_layoutMetrics.frame.size.width) * m_layoutMetrics.pointScaleFactor,
      std::max(m_contentSize.height, m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor};
  m_verticalScrollbarComponent->ContentSize(contentSize);
  m_horizontalScrollbarComponent->ContentSize(contentSize);
  m_scrollVisual.ContentSize(contentSize);
}

void ScrollViewComponentView::prepareForRecycle() noexcept {}

/*
ScrollViewComponentView::ScrollInteractionTrackerOwner::ScrollInteractionTrackerOwner(
    ScrollViewComponentView *outer)
    : m_outer(outer) {}

void ScrollViewComponentView::ScrollInteractionTrackerOwner::CustomAnimationStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerCustomAnimationStateEnteredArgs args) noexcept {}
void ScrollViewComponentView::ScrollInteractionTrackerOwner::IdleStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerIdleStateEnteredArgs args) noexcept {}
void ScrollViewComponentView::ScrollInteractionTrackerOwner::InertiaStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerInertiaStateEnteredArgs args) noexcept {}
void ScrollViewComponentView::ScrollInteractionTrackerOwner::InteractingStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerInteractingStateEnteredArgs args) noexcept {}
void ScrollViewComponentView::ScrollInteractionTrackerOwner::RequestIgnored(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerRequestIgnoredArgs args) noexcept {}

void ScrollViewComponentView::ScrollInteractionTrackerOwner::ValuesChanged(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerValuesChangedArgs args) noexcept {

  auto eventEmitter = m_outer->GetEventEmitter();
  if (eventEmitter) {
    facebook::react::ScrollViewMetrics scrollMetrics;
    scrollMetrics.containerSize.height = m_outer->Visual().Size().y / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.containerSize.width = m_outer->Visual().Size().x / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.contentOffset.x = args.Position().x / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.contentOffset.y = args.Position().y / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.zoomScale = m_outer->ContentVisual().Scale().x;
    scrollMetrics.contentSize.height = m_outer->ContentVisual().Size().y / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.contentSize.width = m_outer->ContentVisual().Size().x / m_outer->m_layoutMetrics.pointScaleFactor;
    std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(eventEmitter)->onScroll(scrollMetrics);
  }
}
*/
/*
void ScrollViewComponentView::OnPointerDown(const winrt::Windows::UI::Input::PointerPoint &pp) noexcept {
  m_visualInteractionSource.TryRedirectForManipulation(pp);
}
*/

void ScrollViewComponentView::onThemeChanged() noexcept {
  updateBackgroundColor(std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->backgroundColor);
  m_verticalScrollbarComponent->UpdateColorForScrollBarRegions();
  m_horizontalScrollbarComponent->UpdateColorForScrollBarRegions();
  Super::onThemeChanged();
}

void ScrollViewComponentView::OnPointerWheelChanged(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  auto ppp = args.GetCurrentPoint(-1).Properties();
  auto delta = static_cast<float>(ppp.MouseWheelDelta());
  if (ppp.IsHorizontalMouseWheel()) {
    if (delta > 0) {
      if (scrollLeft(delta * m_layoutMetrics.pointScaleFactor, true)) {
        args.Handled(true);
      }
    } else if (delta < 0) {
      if (scrollRight(-delta * m_layoutMetrics.pointScaleFactor, true)) {
        args.Handled(true);
      }
    }
  } else {
    if (delta > 0) {
      if (scrollUp(delta * m_layoutMetrics.pointScaleFactor, true)) {
        args.Handled(true);
      }
    } else if (delta < 0) {
      if (scrollDown(-delta * m_layoutMetrics.pointScaleFactor, true)) {
        args.Handled(true);
      }
    }
  }
  Super::OnPointerWheelChanged(args);
}

void ScrollViewComponentView::OnPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_verticalScrollbarComponent->OnPointerPressed(args);
  m_horizontalScrollbarComponent->OnPointerPressed(args);
  Super::OnPointerPressed(args);

  if (!args.Handled()) {
    auto f = args.Pointer();
    auto g = f.PointerDeviceType();
    m_scrollVisual.OnPointerPressed(args);
  }
}

void ScrollViewComponentView::OnPointerReleased(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_verticalScrollbarComponent->OnPointerReleased(args);
  m_horizontalScrollbarComponent->OnPointerReleased(args);
  Super::OnPointerReleased(args);
}

void ScrollViewComponentView::OnPointerMoved(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_verticalScrollbarComponent->OnPointerMoved(args);
  m_horizontalScrollbarComponent->OnPointerMoved(args);
  Super::OnPointerMoved(args);
}

void ScrollViewComponentView::OnPointerCaptureLost() noexcept {
  m_verticalScrollbarComponent->OnPointerCaptureLost();
  m_horizontalScrollbarComponent->OnPointerCaptureLost();
  Super::OnPointerCaptureLost();
}

void ScrollViewComponentView::OnKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  switch (args.Key()) {
    case winrt::Windows::System::VirtualKey::End:
      args.Handled(scrollToEnd(true));
      break;
    case winrt::Windows::System::VirtualKey::Home:
      args.Handled(scrollToStart(true));
      break;
    case winrt::Windows::System::VirtualKey::PageDown:
      args.Handled(pageDown(true));
      break;
    case winrt::Windows::System::VirtualKey::PageUp:
      args.Handled(pageUp(true));
      break;
    case winrt::Windows::System::VirtualKey::Up:
      args.Handled(lineUp(true));
      break;
    case winrt::Windows::System::VirtualKey::Down:
      args.Handled(lineDown(true));
      break;
    case winrt::Windows::System::VirtualKey::Left:
      args.Handled(lineLeft(true));
      break;
    case winrt::Windows::System::VirtualKey::Right:
      args.Handled(lineRight(true));
      break;
  }

  base_type::OnKeyDown(args);
}

bool ScrollViewComponentView::scrollToEnd(bool animate) noexcept {
  if (!std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->scrollEnabled) {
    return false;
  }

  if ((((m_contentSize.height - m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor) -
       m_scrollVisual.ScrollPosition().y) < 1.0f) {
    return false;
  }

  auto x = (m_contentSize.width - m_layoutMetrics.frame.size.width) * m_layoutMetrics.pointScaleFactor;
  auto y = (m_contentSize.height - m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor;
  // Ensure at least one scroll event will fire
  m_allowNextScrollNoMatterWhat = true;
  m_scrollVisual.TryUpdatePosition({static_cast<float>(x), static_cast<float>(y), 0.0f}, animate);
  return true;
}

bool ScrollViewComponentView::scrollToStart(bool animate) noexcept {
  if (!std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->scrollEnabled) {
    return false;
  }

  m_scrollVisual.TryUpdatePosition({0.0f, 0.0f, 0.0f}, animate);
  return true;
}

bool ScrollViewComponentView::pageUp(bool animate) noexcept {
  if (std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->horizontal) {
    return scrollLeft(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor, animate);
  }
  return scrollUp(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::pageDown(bool animate) noexcept {
  if (std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->horizontal) {
    return scrollRight(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor, animate);
  }
  return scrollDown(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::lineUp(bool animate) noexcept {
  return scrollUp(c_scrollerLineDelta * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::lineDown(bool animate) noexcept {
  return scrollDown(c_scrollerLineDelta * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::lineLeft(bool animate) noexcept {
  return scrollLeft(c_scrollerLineDelta * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::lineRight(bool animate) noexcept {
  return scrollRight(c_scrollerLineDelta * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::scrollDown(float delta, bool animate) noexcept {
  if (!std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->scrollEnabled) {
    return false;
  }

  if (((m_contentSize.height - m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor) -
          m_scrollVisual.ScrollPosition().y <
      1.0f) {
    return false;
  }

  m_scrollVisual.ScrollBy({0, delta, 0}, animate);
  return true;
}

bool ScrollViewComponentView::scrollUp(float delta, bool animate) noexcept {
  if (!std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->scrollEnabled) {
    return false;
  }

  if (m_scrollVisual.ScrollPosition().y <= 0.0f) {
    return false;
  }

  m_scrollVisual.ScrollBy({0, -delta, 0}, animate);
  return true;
}

bool ScrollViewComponentView::scrollLeft(float delta, bool animate) noexcept {
  if (!std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->scrollEnabled) {
    return false;
  }

  if (m_scrollVisual.ScrollPosition().x <= 0.0f) {
    return false;
  }

  m_scrollVisual.ScrollBy({-delta, 0, 0}, animate);
  return true;
}

bool ScrollViewComponentView::scrollRight(float delta, bool animate) noexcept {
  if (!std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->scrollEnabled) {
    return false;
  }

  if (((m_contentSize.width - m_layoutMetrics.frame.size.width) * m_layoutMetrics.pointScaleFactor) -
          m_scrollVisual.ScrollPosition().x <
      1.0f) {
    return false;
  }

  m_scrollVisual.ScrollBy({delta, 0, 0}, animate);
  return true;
}

void ScrollViewComponentView::HandleCommand(const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept {
  Super::HandleCommand(args);
  if (args.Handled())
    return;

  auto commandName = args.CommandName();
  if (commandName == L"scrollTo") {
    double x, y;
    bool animate;
    winrt::Microsoft::ReactNative::ReadArgs(args.CommandArgs(), x, y, animate);
    scrollTo(
        {static_cast<float>(x) * m_layoutMetrics.pointScaleFactor,
         static_cast<float>(y) * m_layoutMetrics.pointScaleFactor,
         0.0f},
        animate);
  } else if (commandName == L"flashScrollIndicators") {
    // No-op for now
  } else if (commandName == L"scrollToEnd") {
    bool animate;
    winrt::Microsoft::ReactNative::ReadArgs(args.CommandArgs(), animate);
    scrollToEnd(animate);
  } else if (commandName == L"zoomToRect") {
    // No-op for now
  }
}

void ScrollViewComponentView::scrollTo(winrt::Windows::Foundation::Numerics::float3 offset, bool animate) noexcept {
  if (!std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->scrollEnabled) {
    return;
  }

  m_scrollVisual.TryUpdatePosition(offset, animate);
}

void ScrollViewComponentView::StartBringIntoView(
    winrt::Microsoft::ReactNative::implementation::BringIntoViewOptions &&options) noexcept {
  RECT rc{getClientRect()};

  if (!options.TargetRect) {
    Super::StartBringIntoView(std::move(options));
    return;
  }

  bool needsScroll = false;
  float scrollToVertical = m_scrollVisual.ScrollPosition().y;
  float scrollToHorizontal = m_scrollVisual.ScrollPosition().x;
  float viewerHeight = m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor;
  float viewerWidth = m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor;
  float targetBottom = options.TargetRect->origin.y + options.TargetRect->size.height;
  float targetRight = options.TargetRect->origin.x + options.TargetRect->size.width;

  if (isnan(options.VerticalAlignmentRatio)) {
    // Scroll Down
    if (targetBottom > (m_scrollVisual.ScrollPosition().y + viewerHeight) &&
        options.TargetRect->origin.y > m_scrollVisual.ScrollPosition().y) {
      needsScroll = true;
      if (options.TargetRect->size.height > viewerHeight) {
        scrollToVertical = options.TargetRect->origin.y + options.VerticalOffset * m_layoutMetrics.pointScaleFactor;
      } else {
        scrollToVertical = (targetBottom - viewerHeight) + options.VerticalOffset * m_layoutMetrics.pointScaleFactor;
      }
      // Scroll Up
    } else if (
        options.TargetRect->origin.y < m_scrollVisual.ScrollPosition().y &&
        targetBottom < (m_scrollVisual.ScrollPosition().y + viewerHeight)) {
      needsScroll = true;
      if (options.TargetRect->size.height > viewerHeight) {
        scrollToVertical = targetBottom - viewerHeight - options.VerticalOffset * m_layoutMetrics.pointScaleFactor;
      } else {
        scrollToVertical = options.TargetRect->origin.y - options.VerticalOffset * m_layoutMetrics.pointScaleFactor;
      }
    }
  } else {
    needsScroll = true;
    scrollToVertical = options.TargetRect->getMidY() - (viewerHeight * options.VerticalAlignmentRatio);
  }

  if (isnan(options.HorizontalAlignmentRatio)) {
    // Scroll Right
    if (targetRight > (m_scrollVisual.ScrollPosition().x + viewerWidth) &&
        options.TargetRect->origin.x > m_scrollVisual.ScrollPosition().x) {
      needsScroll = true;
      if (options.TargetRect->size.width > viewerWidth) {
        scrollToHorizontal = options.TargetRect->origin.x + options.HorizontalOffset * m_layoutMetrics.pointScaleFactor;
      } else {
        scrollToHorizontal = (targetRight - viewerWidth) + options.HorizontalOffset * m_layoutMetrics.pointScaleFactor;
      }
      // Scroll Left
    } else if (
        options.TargetRect->origin.x < m_scrollVisual.ScrollPosition().x &&
        targetRight < (m_scrollVisual.ScrollPosition().x + viewerWidth)) {
      needsScroll = true;
      if (options.TargetRect->size.width > viewerWidth) {
        scrollToHorizontal = targetRight - viewerWidth - options.HorizontalOffset * m_layoutMetrics.pointScaleFactor;
      } else {
        scrollToHorizontal = options.TargetRect->origin.x - options.HorizontalOffset * m_layoutMetrics.pointScaleFactor;
      }
    }
  } else {
    needsScroll = true;
    scrollToHorizontal = options.TargetRect->getMidX() - (viewerWidth * options.HorizontalAlignmentRatio);
  }

  if (needsScroll && std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->scrollEnabled) {
    m_scrollVisual.TryUpdatePosition(
        {static_cast<float>(scrollToHorizontal), static_cast<float>(scrollToVertical), 0.0f}, options.AnimationDesired);
  }

  if (m_parent) {
    options.TargetRect->origin.y += m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor;
    options.TargetRect->origin.x += m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor;

    options.TargetRect->origin.y -= m_scrollVisual.ScrollPosition().y;
    options.TargetRect->origin.x -= m_scrollVisual.ScrollPosition().x;

    facebook::react::Rect viewerRect = {
        {m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor,
         m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor},
        {m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
         m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor}};

    options.TargetRect = facebook::react::Rect::intersect(viewerRect, options.TargetRect.value());

    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->StartBringIntoView(
        std::move(options));
  }
}

facebook::react::ScrollViewEventEmitter::Metrics ScrollViewComponentView::getScrollMetrics(
    facebook::react::SharedViewEventEmitter const &eventEmitter,
    winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs const &args) noexcept {
  facebook::react::ScrollViewEventEmitter::Metrics scrollMetrics{};
  if (eventEmitter) {
    scrollMetrics.containerSize.height = m_layoutMetrics.frame.size.height;
    scrollMetrics.containerSize.width = m_layoutMetrics.frame.size.width;
    scrollMetrics.contentOffset.x = args.Position().x / m_layoutMetrics.pointScaleFactor;
    scrollMetrics.contentOffset.y = args.Position().y / m_layoutMetrics.pointScaleFactor;
    scrollMetrics.zoomScale = 1.0;
    scrollMetrics.contentSize.height = std::max(m_contentSize.height, m_layoutMetrics.frame.size.height);
    scrollMetrics.contentSize.width = std::max(m_contentSize.width, m_layoutMetrics.frame.size.width);
  }

  return scrollMetrics;
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ScrollViewComponentView::createVisual() noexcept {
  auto visual = m_compContext.CreateSpriteVisual();
  m_scrollVisual = m_compContext.CreateScrollerVisual();
  m_verticalScrollbarComponent = std::make_shared<ScrollBarComponent>(*this, m_compContext, m_reactContext, true);
  m_horizontalScrollbarComponent = std::make_shared<ScrollBarComponent>(*this, m_compContext, m_reactContext, false);
  visual.InsertAt(m_scrollVisual, 0);
  visual.InsertAt(m_verticalScrollbarComponent->Visual(), 1);
  visual.InsertAt(m_horizontalScrollbarComponent->Visual(), 2);
  m_scrollPositionChangedRevoker = m_scrollVisual.ScrollPositionChanged(
      winrt::auto_revoke,
      [this](
          winrt::IInspectable const & /*sender*/,
          winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs const &args) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_lastScrollEventTime).count();

        // Dismiss keyboard if mode is "on-drag"
        auto keyboardDismissMode =
            std::static_pointer_cast<const facebook::react::ScrollViewProps>(viewProps())->keyboardDismissMode;
        if (keyboardDismissMode == facebook::react::ScrollViewKeyboardDismissMode::OnDrag) {
          auto coreInputView = winrt::Windows::UI::ViewManagement::Core::CoreInputView::GetForCurrentView();
          if (coreInputView) {
            coreInputView.TryHide();
          }
        }

        if (m_allowNextScrollNoMatterWhat ||
            (m_scrollEventThrottle < std::max(std::chrono::duration<double>(0.017).count(), elapsed))) {
          updateStateWithContentOffset();
          auto eventEmitter = GetEventEmitter();
          if (eventEmitter) {
            auto scrollMetrics = getScrollMetrics(eventEmitter, args);
            std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(eventEmitter)
                ->onScroll(scrollMetrics);
            m_lastScrollEventTime = now;
            m_allowNextScrollNoMatterWhat = false;
          }
        }
      });

  m_scrollBeginDragRevoker = m_scrollVisual.ScrollBeginDrag(
      winrt::auto_revoke,
      [this](
          winrt::IInspectable const & /*sender*/,
          winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs const &args) {
        m_allowNextScrollNoMatterWhat = true; // Ensure next scroll event is recorded, regardless of throttle
        updateStateWithContentOffset();
        auto eventEmitter = GetEventEmitter();
        if (eventEmitter) {
          auto scrollMetrics = getScrollMetrics(eventEmitter, args);
          std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(eventEmitter)
              ->onScrollBeginDrag(scrollMetrics);
        }
      });

  m_scrollEndDragRevoker = m_scrollVisual.ScrollEndDrag(
      winrt::auto_revoke,
      [this](
          winrt::IInspectable const & /*sender*/,
          winrt::Microsoft::ReactNative::Composition::Experimental::IScrollPositionChangedArgs const &args) {
        updateStateWithContentOffset();
        auto eventEmitter = GetEventEmitter();
        if (eventEmitter) {
          auto scrollMetrics = getScrollMetrics(eventEmitter, args);
          std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(eventEmitter)
              ->onScrollEndDrag(scrollMetrics);
        }
      });

  return visual;
}

facebook::react::Tag ScrollViewComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  facebook::react::Point ptViewport{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Point ptContent{
      ptViewport.x + m_scrollVisual.ScrollPosition().x / m_layoutMetrics.pointScaleFactor,
      ptViewport.y + m_scrollVisual.ScrollPosition().y / m_layoutMetrics.pointScaleFactor};

  facebook::react::Tag targetTag = -1;

  if ((ignorePointerEvents || viewProps()->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       viewProps()->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptViewport.x >= 0 && ptViewport.x <= m_layoutMetrics.frame.size.width && ptViewport.y >= 0 &&
      ptViewport.y <= m_layoutMetrics.frame.size.height) {
    if ((ignorePointerEvents || viewProps()->pointerEvents == facebook::react::PointerEventsMode::Auto ||
         viewProps()->pointerEvents == facebook::react::PointerEventsMode::BoxNone) &&
        anyHitTestHelper(targetTag, ptContent, localPt))
      return targetTag;

    if (ignorePointerEvents || viewProps()->pointerEvents == facebook::react::PointerEventsMode::Auto ||
        viewProps()->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) {
      return this->Tag();
    }
  }

  return -1;
}

facebook::react::Point ScrollViewComponentView::getClientOffset() const noexcept {
  facebook::react::Point parentOffset{0};
  if (m_parent) {
    parentOffset =
        winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->getClientOffset();
  }

  return {
      m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor - m_scrollVisual.ScrollPosition().x +
          parentOffset.x,
      m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor - m_scrollVisual.ScrollPosition().y +
          parentOffset.y};
}

std::string ScrollViewComponentView::DefaultControlType() const noexcept {
  return "pane";
}

winrt::com_ptr<ComponentView> ScrollViewComponentView::focusVisualRoot(
    const facebook::react::Rect &focusRect) noexcept {
  return get_strong();
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual
ScrollViewComponentView::visualToHostFocus() noexcept {
  return m_scrollVisual;
}

int ScrollViewComponentView::getScrollPositionX() noexcept {
  return int((m_scrollVisual.ScrollPosition().x / m_horizontalScrollbarComponent->getScrollRange()) * 100);
}

int ScrollViewComponentView::getScrollPositionY() noexcept {
  return int((m_scrollVisual.ScrollPosition().y / m_verticalScrollbarComponent->getScrollRange()) * 100);
}

double ScrollViewComponentView::getVerticalSize() noexcept {
  return std::min((m_layoutMetrics.frame.size.height / m_contentSize.height * 100.0), 100.0);
}

double ScrollViewComponentView::getHorizontalSize() noexcept {
  return std::min((m_layoutMetrics.frame.size.width / m_contentSize.width * 100.0), 100.0);
}

void ScrollViewComponentView::updateShowsHorizontalScrollIndicator(bool value) noexcept {
  if (value) {
    m_horizontalScrollbarComponent->updateVisibility(true);
  } else {
    m_horizontalScrollbarComponent->updateVisibility(false);
  }
}

void ScrollViewComponentView::updateShowsVerticalScrollIndicator(bool value) noexcept {
  if (value) {
    m_verticalScrollbarComponent->updateVisibility(true);
  } else {
    m_verticalScrollbarComponent->updateVisibility(false);
  }
}

void ScrollViewComponentView::updateDecelerationRate(float value) noexcept {
  m_scrollVisual.SetDecelerationRate({value, value, value});
}
} // namespace winrt::Microsoft::ReactNative::Composition::implementation
