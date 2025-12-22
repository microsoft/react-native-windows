// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ParagraphComponentView.h"

#include <react/renderer/textlayoutmanager/WindowsTextLayoutManager.h>

#include <AutoDraw.h>
#include <Fabric/ReactTaggedView.h>
#include <Utils/ValueUtils.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/ParagraphState.h>
#include <unicode.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "CompositionDynamicAutomationProvider.h"
#include "CompositionHelpers.h"
#include "RootComponentView.h"
#include "TextDrawing.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

// Automatically restores the original DPI of a render target
struct DpiRestorer {
  ID2D1RenderTarget *renderTarget = nullptr;
  float originalDpiX = 0.0f;
  float originalDpiY = 0.0f;

  void operator()(ID2D1RenderTarget *) const noexcept {
    if (renderTarget) {
      renderTarget->SetDpi(originalDpiX, originalDpiY);
    }
  }
};

inline std::unique_ptr<ID2D1RenderTarget, DpiRestorer>
MakeDpiGuard(ID2D1RenderTarget &renderTarget, float newDpiX, float newDpiY) noexcept {
  float originalDpiX, originalDpiY;
  renderTarget.GetDpi(&originalDpiX, &originalDpiY);
  renderTarget.SetDpi(newDpiX, newDpiY);

  return std::unique_ptr<ID2D1RenderTarget, DpiRestorer>(
      &renderTarget, DpiRestorer{&renderTarget, originalDpiX, originalDpiY});
}

ParagraphComponentView::ParagraphComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(
          ParagraphComponentView::defaultProps(),
          compContext,
          tag,
          reactContext,
          // Disable Background (text draws its own) and FocusVisual (selection highlight is the focus indicator)
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background & ~ComponentViewFeatures::FocusVisual) {}

void ParagraphComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  base_type::MountChildComponentView(childComponentView, index);
}

void ParagraphComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  base_type::UnmountChildComponentView(childComponentView, index);
}

void ParagraphComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps =
      *std::static_pointer_cast<const facebook::react::ParagraphProps>(oldProps ? oldProps : viewProps());
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(props);

  ensureVisual();

  if (oldViewProps.textAttributes.foregroundColor != newViewProps.textAttributes.foregroundColor) {
    m_requireRedraw = true;
  }

  if (oldViewProps.textAttributes.opacity != newViewProps.textAttributes.opacity) {
    m_requireRedraw = true;
  }

  if (oldViewProps.textAttributes.alignment != newViewProps.textAttributes.alignment) {
    updateTextAlignment(newViewProps.textAttributes.alignment);
  }

  if (oldViewProps.paragraphAttributes.ellipsizeMode != newViewProps.paragraphAttributes.ellipsizeMode) {
    m_textLayout = nullptr;
  }

  if (oldViewProps.paragraphAttributes.adjustsFontSizeToFit != newViewProps.paragraphAttributes.adjustsFontSizeToFit) {
    m_textLayout = nullptr;
  }

  // Clear selection if text becomes non-selectable
  if (oldViewProps.isSelectable != newViewProps.isSelectable) {
    if (!newViewProps.isSelectable) {
      ClearSelection();
    }
    m_requireRedraw = true;
  }

  Super::updateProps(props, oldProps);
}

void ParagraphComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  const auto &newState = *std::static_pointer_cast<facebook::react::ParagraphShadowNode::ConcreteState const>(state);

  m_attributedStringBox = facebook::react::AttributedStringBox(newState.getData().attributedString);
  m_paragraphAttributes = facebook::react::ParagraphAttributes(newState.getData().paragraphAttributes);

  m_textLayout = nullptr;
}

void ParagraphComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);

  if (layoutMetrics.pointScaleFactor != oldLayoutMetrics.pointScaleFactor) {
    m_textLayout = nullptr;
  }
}

void ParagraphComponentView::FinalizeUpdates(
    winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  ensureVisual();
  updateVisualBrush();
  Super::FinalizeUpdates(updateMask);
}

facebook::react::SharedViewEventEmitter ParagraphComponentView::eventEmitterAtPoint(
    facebook::react::Point pt) noexcept {
  if (m_attributedStringBox.getValue().getFragments().size() && m_textLayout) {
    BOOL isTrailingHit = false;
    BOOL isInside = false;
    DWRITE_HIT_TEST_METRICS metrics;
    winrt::check_hresult(m_textLayout->HitTestPoint(pt.x, pt.y, &isTrailingHit, &isInside, &metrics));
    if (isInside) {
      uint32_t textPosition = metrics.textPosition;

      for (auto fragment : m_attributedStringBox.getValue().getFragments()) {
        if (textPosition < fragment.string.length()) {
          return std::static_pointer_cast<const facebook::react::ViewEventEmitter>(
              fragment.parentShadowView.eventEmitter);
        }
        textPosition -= static_cast<uint32_t>(fragment.string.length());
      }
    }
  }

  return m_eventEmitter;
}

void ParagraphComponentView::updateTextAlignment(
    const std::optional<facebook::react::TextAlignment> &fbAlignment) noexcept {
  // Reset text layout to force recreation with new alignment
  m_textLayout = nullptr;
}

bool ParagraphComponentView::IsTextSelectableAtPoint(facebook::react::Point pt) noexcept {
  // paragraph-level selectable prop is enabled
  const auto &props = paragraphProps();
  if (!props.isSelectable) {
    return false;
  }

  // Finds which text fragment was hit
  if (m_attributedStringBox.getValue().getFragments().size() && m_textLayout) {
    BOOL isTrailingHit = false;
    BOOL isInside = false;
    DWRITE_HIT_TEST_METRICS metrics;
    winrt::check_hresult(m_textLayout->HitTestPoint(pt.x, pt.y, &isTrailingHit, &isInside, &metrics));

    if (isInside) {
      uint32_t textPosition = metrics.textPosition;

      // Finds which fragment contains this text position
      for (auto fragment : m_attributedStringBox.getValue().getFragments()) {
        if (textPosition < fragment.string.length()) {
          return true;
        }
        textPosition -= static_cast<uint32_t>(fragment.string.length());
      }
    }
  }

  return false;
}

std::optional<int32_t> ParagraphComponentView::GetTextPositionAtPoint(facebook::react::Point pt) noexcept {
  if (!m_textLayout) {
    return std::nullopt;
  }

  BOOL isTrailingHit = FALSE;
  BOOL isInside = FALSE;
  DWRITE_HIT_TEST_METRICS metrics = {};

  // Convert screen coordinates to character position
  HRESULT hr = m_textLayout->HitTestPoint(pt.x, pt.y, &isTrailingHit, &isInside, &metrics);
  if (FAILED(hr) || !isInside) {
    return std::nullopt;
  }

  // Calculates the actual character position
  // If isTrailingHit is true, the point is closer to the trailing edge of the character,
  // so we should return the next character position (for cursor positioning)
  return static_cast<int32_t>(metrics.textPosition + isTrailingHit);
}

std::optional<int32_t> ParagraphComponentView::GetClampedTextPosition(facebook::react::Point pt) noexcept {
  if (!m_textLayout) {
    return std::nullopt;
  }

  const std::string fullText = m_attributedStringBox.getValue().getString();
  if (fullText.empty()) {
    return std::nullopt;
  }

  DWRITE_TEXT_METRICS textMetrics;
  if (FAILED(m_textLayout->GetMetrics(&textMetrics))) {
    return std::nullopt;
  }

  // Clamp the point to the text bounds for hit testing
  const float clampedX = std::max(0.0f, std::min(pt.x, textMetrics.width));
  const float clampedY = std::max(0.0f, std::min(pt.y, textMetrics.height));

  BOOL isTrailingHit = FALSE;
  BOOL isInside = FALSE;
  DWRITE_HIT_TEST_METRICS metrics = {};

  HRESULT hr = m_textLayout->HitTestPoint(clampedX, clampedY, &isTrailingHit, &isInside, &metrics);
  if (FAILED(hr)) {
    return std::nullopt;
  }

  int32_t result = static_cast<int32_t>(metrics.textPosition);
  if (pt.x > textMetrics.width) {
    // Dragging right - go to end of character
    result = static_cast<int32_t>(metrics.textPosition + metrics.length);
  } else if (pt.x < 0) {
    // Dragging left - go to start of character
    result = static_cast<int32_t>(metrics.textPosition);
  } else if (isTrailingHit) {
    // Inside bounds, trailing hit
    result += 1;
  }

  // Vertical clamping
  if (pt.y > textMetrics.height) {
    // Dragging below - select to end of text
    result = static_cast<int32_t>(fullText.length());
  } else if (pt.y < 0) {
    // Dragging above - select to start of text
    result = 0;
  }

  return result;
}

void ParagraphComponentView::OnRenderingDeviceLost() noexcept {
  DrawText();
}

void ParagraphComponentView::updateVisualBrush() noexcept {
  bool requireNewBrush{false};

  // TODO
  // updateTextAlignment(paragraphProps.textAttributes.alignment);
  if (!m_textLayout) {
    facebook::react::LayoutConstraints constraints;
    constraints.maximumSize.width =
        m_layoutMetrics.frame.size.width - m_layoutMetrics.contentInsets.left - m_layoutMetrics.contentInsets.right;
    constraints.maximumSize.height =
        m_layoutMetrics.frame.size.height - m_layoutMetrics.contentInsets.top - m_layoutMetrics.contentInsets.bottom;

    facebook::react::WindowsTextLayoutManager::GetTextLayout(
        m_attributedStringBox, m_paragraphAttributes, constraints, m_textLayout);

    // Apply text alignment after creating the text layout
    if (m_textLayout) {
      const auto &props = paragraphProps();
      DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
      if (props.textAttributes.alignment) {
        switch (*props.textAttributes.alignment) {
          case facebook::react::TextAlignment::Center:
            alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
            break;
          case facebook::react::TextAlignment::Justified:
            alignment = DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
            break;
          case facebook::react::TextAlignment::Left:
            alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
            break;
          case facebook::react::TextAlignment::Right:
            alignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
            break;
          case facebook::react::TextAlignment::Natural:
            alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
            break;
          default:
            alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
            break;
        }
      }
      winrt::check_hresult(m_textLayout->SetTextAlignment(alignment));
    }

    requireNewBrush = true;
  }

  if (requireNewBrush || !m_drawingSurface) {
    if (!m_textLayout) { // Empty Text element
      m_drawingSurface = nullptr;
    } else {
      // Create the surface just big enough to hold the formatted text block.
      DWRITE_TEXT_METRICS metrics;
      winrt::check_hresult(m_textLayout->GetMetrics(&metrics));

      if (metrics.width == 0 || metrics.height == 0) {
        m_drawingSurface = nullptr;
        return;
      }

      winrt::Windows::Foundation::Size surfaceSize = {
          m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
          m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor};
      m_drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
          surfaceSize,
          winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
          winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
    }

    DrawText();

    // The surfaceBrush's size is based on the size the text takes up, which maybe smaller than the total visual
    // So we need to align the brush within the visual to match the text alignment.
    float horizAlignment{0.f};

    /*
    const auto &props = paragraphProps()
    if (props.textAttributes.alignment) {
      switch (*props.textAttributes.alignment) {
        case facebook::react::TextAlignment::Center:
          horizAlignment = 0.5f;
          break;
        case facebook::react::TextAlignment::Justified:
          horizAlignment = 0.5f;
          break;
        case facebook::react::TextAlignment::Left:
          horizAlignment = 0.f;
          break;
        case facebook::react::TextAlignment::Right:
          horizAlignment = 1.f;
          break;
        // TODO use LTR values
        case facebook::react::TextAlignment::Natural:
          horizAlignment = 0.f;
          break;
        default:
          assert(false);
      }
    }
    */
    // TODO Using brush alignment to align the text makes it blurry...
    if (m_drawingSurface) {
      m_drawingSurface.HorizontalAlignmentRatio(horizAlignment);
      m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);
    }
    Visual().as<Experimental::ISpriteVisual>().Brush(m_drawingSurface);
  }

  if (m_requireRedraw) {
    DrawText();
  }
}

facebook::react::SharedViewProps ParagraphComponentView::defaultProps() noexcept {
  static auto const defaultProps = std::make_shared<facebook::react::ParagraphProps const>();
  return defaultProps;
}

const facebook::react::ParagraphProps &ParagraphComponentView::paragraphProps() const noexcept {
  return *std::static_pointer_cast<const facebook::react::ParagraphProps>(viewProps());
}

void ParagraphComponentView::onThemeChanged() noexcept {
  DrawText();
  Super::onThemeChanged();
}

// Renders the text into our composition surface
void ParagraphComponentView::DrawSelectionHighlight(
    ID2D1RenderTarget &renderTarget,
    float offsetX,
    float offsetY,
    float pointScaleFactor) noexcept {
  if (!m_selectionStart || !m_selectionEnd || !m_textLayout) {
    return;
  }

  // During drag, selection may not be normalized yet, using min/max for rendering
  const int32_t selStart = std::min(*m_selectionStart, *m_selectionEnd);
  const int32_t selEnd = std::max(*m_selectionStart, *m_selectionEnd);
  if (selEnd <= selStart) {
    return;
  }

  // Scale offset to match text layout coordinates (same as RenderText)
  const float scaledOffsetX = offsetX / pointScaleFactor;
  const float scaledOffsetY = offsetY / pointScaleFactor;

  // Set DPI to match text rendering
  const float dpi = pointScaleFactor * 96.0f;
  std::unique_ptr<ID2D1RenderTarget, DpiRestorer> dpiGuard = MakeDpiGuard(renderTarget, dpi, dpi);

  // Get the hit test metrics for the selected text range
  UINT32 actualCount = 0;
  HRESULT hr = m_textLayout->HitTestTextRange(
      static_cast<UINT32>(selStart),
      static_cast<UINT32>(selEnd - selStart),
      scaledOffsetX,
      scaledOffsetY,
      nullptr,
      0,
      &actualCount);

  if (actualCount == 0) {
    return;
  }

  std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualCount);
  hr = m_textLayout->HitTestTextRange(
      static_cast<UINT32>(selStart),
      static_cast<UINT32>(selEnd - selStart),
      scaledOffsetX,
      scaledOffsetY,
      hitTestMetrics.data(),
      actualCount,
      &actualCount);

  if (FAILED(hr)) {
    return;
  }

  // TODO: use prop selectionColor if provided
  winrt::com_ptr<ID2D1SolidColorBrush> selectionBrush;
  const D2D1_COLOR_F selectionColor = theme()->D2DPlatformColor("Highlight@40");
  hr = renderTarget.CreateSolidColorBrush(selectionColor, selectionBrush.put());

  if (FAILED(hr)) {
    return;
  }

  // Draw rectangles for each hit test metric
  for (UINT32 i = 0; i < actualCount; i++) {
    const auto &metric = hitTestMetrics[i];
    const D2D1_RECT_F rect = {metric.left, metric.top, metric.left + metric.width, metric.top + metric.height};
    renderTarget.FillRectangle(&rect, selectionBrush.get());
  }
}

void ParagraphComponentView::DrawText() noexcept {
  if (!m_drawingSurface || theme()->IsEmpty())
    return;

  if (m_layoutMetrics.frame.size.width == 0 || m_layoutMetrics.frame.size.height == 0) {
    m_requireRedraw = false;
    return;
  }

  POINT offset;
  {
    ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(
        m_drawingSurface, m_layoutMetrics.pointScaleFactor, &offset);
    if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
      d2dDeviceContext->Clear(
          viewProps()->backgroundColor ? theme()->D2DColor(*viewProps()->backgroundColor)
                                       : D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
      const auto &props = paragraphProps();

      // Calculate text offset
      const float textOffsetX = static_cast<float>(offset.x) + m_layoutMetrics.contentInsets.left;
      const float textOffsetY = static_cast<float>(offset.y) + m_layoutMetrics.contentInsets.top;

      // Draw selection highlight behind text
      DrawSelectionHighlight(*d2dDeviceContext, textOffsetX, textOffsetY, m_layoutMetrics.pointScaleFactor);

      RenderText(
          *d2dDeviceContext,
          *m_textLayout,
          m_attributedStringBox.getValue(),
          props.textAttributes,
          {textOffsetX, textOffsetY},
          m_layoutMetrics.pointScaleFactor,
          *theme());

      if (!isnan(props.opacity)) {
        Visual().Opacity(props.opacity);
      }
    }
    m_requireRedraw = false;
  }
}

void ParagraphComponentView::ClearSelection() noexcept {
  const bool hadSelection = (m_selectionStart || m_selectionEnd || m_isSelecting);
  m_selectionStart = std::nullopt;
  m_selectionEnd = std::nullopt;
  m_isSelecting = false;
  if (hadSelection) {
    // Clears selection highlight
    DrawText();
  }
}

void ParagraphComponentView::OnPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  // Only handle selection if text is selectable
  const auto &props = paragraphProps();
  if (!props.isSelectable) {
    Super::OnPointerPressed(args);
    return;
  }

  auto pp = args.GetCurrentPoint(-1);

  // Ignores right-click
  if (pp.Properties().PointerUpdateKind() ==
      winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::RightButtonPressed) {
    args.Handled(true);
    return;
  }

  auto position = pp.Position();

  facebook::react::Point localPt{
      position.X - m_layoutMetrics.frame.origin.x, position.Y - m_layoutMetrics.frame.origin.y};

  std::optional<int32_t> charPosition = GetTextPositionAtPoint(localPt);

  if (charPosition) {
    if (auto root = rootComponentView()) {
      root->ClearCurrentTextSelection();
    }

    // Check for double-click
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastClick = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastClickTime);
    const UINT doubleClickTime = GetDoubleClickTime();
    const bool isDoubleClick = (timeSinceLastClick.count() < static_cast<long long>(doubleClickTime)) &&
        m_lastClickPosition && (std::abs(*charPosition - *m_lastClickPosition) <= 1);

    // Update last click tracking
    m_lastClickTime = now;
    m_lastClickPosition = charPosition;

    if (isDoubleClick) {
      SelectWordAtPosition(*charPosition);
      m_isSelecting = false;
    } else {
      // Single-click: start drag selection
      m_selectionStart = charPosition;
      m_selectionEnd = charPosition;
      m_isSelecting = true;

      // Tracks selection even when the mouse moves outside the component bounds
      CapturePointer(args.Pointer());
    }

    if (auto root = rootComponentView()) {
      root->SetViewWithTextSelection(*get_strong());
    }

    // Focuses so we receive onLostFocus when clicking elsewhere
    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(*get_strong(), winrt::Microsoft::ReactNative::FocusNavigationDirection::None);
    }

    args.Handled(true);
  } else {
    ClearSelection();
    m_lastClickPosition = std::nullopt;
    Super::OnPointerPressed(args);
  }
}

void ParagraphComponentView::OnPointerMoved(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  // Only track movement if we're actively selecting
  if (!m_isSelecting) {
    Super::OnPointerMoved(args);
    return;
  }

  auto pp = args.GetCurrentPoint(static_cast<int32_t>(Tag()));
  auto position = pp.Position();

  facebook::react::Point localPt{position.X, position.Y};
  std::optional<int32_t> charPosition = GetClampedTextPosition(localPt);

  if (charPosition && charPosition != m_selectionEnd) {
    m_selectionEnd = charPosition;
    DrawText();
    args.Handled(true);
  }
}

void ParagraphComponentView::OnPointerReleased(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  // Check for right-click to show context menu
  auto pp = args.GetCurrentPoint(-1);
  if (pp.Properties().PointerUpdateKind() ==
      winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::RightButtonReleased) {
    const auto &props = paragraphProps();
    if (props.isSelectable) {
      ShowContextMenu();
      args.Handled(true);
      return;
    }
  }

  if (!m_isSelecting) {
    Super::OnPointerReleased(args);
    return;
  }

  m_isSelecting = false;

  ReleasePointerCapture(args.Pointer());

  if (!m_selectionStart || !m_selectionEnd || *m_selectionStart == *m_selectionEnd) {
    m_selectionStart = std::nullopt;
    m_selectionEnd = std::nullopt;
  } else {
    SetSelection(*m_selectionStart, *m_selectionEnd);
  }

  args.Handled(true);
}

void ParagraphComponentView::onLostFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  ClearSelection();

  Super::onLostFocus(args);
}

void ParagraphComponentView::OnPointerCaptureLost() noexcept {
  // Pointer capture was lost stop any active selection drag
  if (m_isSelecting) {
    m_isSelecting = false;

    if (!m_selectionStart || !m_selectionEnd || *m_selectionStart == *m_selectionEnd) {
      m_selectionStart = std::nullopt;
      m_selectionEnd = std::nullopt;
    } else {
      SetSelection(*m_selectionStart, *m_selectionEnd);
    }
  }

  Super::OnPointerCaptureLost();
}

std::string ParagraphComponentView::GetSelectedText() const noexcept {
  if (!m_selectionStart || !m_selectionEnd) {
    return "";
  }

  const int32_t selStart = std::min(*m_selectionStart, *m_selectionEnd);
  const int32_t selEnd = std::max(*m_selectionStart, *m_selectionEnd);

  if (selEnd <= selStart) {
    return "";
  }

  const std::string fullText = m_attributedStringBox.getValue().getString();

  if (selStart >= static_cast<int32_t>(fullText.length())) {
    return "";
  }

  const int32_t clampedEnd = std::min(selEnd, static_cast<int32_t>(fullText.length()));
  return fullText.substr(static_cast<size_t>(selStart), static_cast<size_t>(clampedEnd - selStart));
}

void ParagraphComponentView::CopySelectionToClipboard() noexcept {
  const std::string selectedText = GetSelectedText();
  if (selectedText.empty()) {
    return;
  }

  // Convert UTF-8 to wide string for Windows clipboard
  const std::wstring wideText = ::Microsoft::Common::Unicode::Utf8ToUtf16(selectedText);

  winrt::Windows::ApplicationModel::DataTransfer::DataPackage dataPackage;
  dataPackage.SetText(wideText);
  winrt::Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(dataPackage);
}

void ParagraphComponentView::SelectWordAtPosition(int32_t charPosition) noexcept {
  const std::string fullText = m_attributedStringBox.getValue().getString();
  if (fullText.empty() || charPosition < 0 || charPosition >= static_cast<int32_t>(fullText.length())) {
    return;
  }

  // Finds word boundaries
  int32_t wordStart = charPosition;
  int32_t wordEnd = charPosition;

  while (wordStart > 0 && std::isalnum(static_cast<unsigned char>(fullText[wordStart - 1]))) {
    wordStart--;
  }

  while (wordEnd < static_cast<int32_t>(fullText.length()) &&
         std::isalnum(static_cast<unsigned char>(fullText[wordEnd]))) {
    wordEnd++;
  }

  if (wordEnd > wordStart) {
    SetSelection(wordStart, wordEnd);
    DrawText();
  }
}

void ParagraphComponentView::SetSelection(int32_t start, int32_t end) noexcept {
  m_selectionStart = std::min(start, end);
  m_selectionEnd = std::max(start, end);
}

void ParagraphComponentView::ShowContextMenu() noexcept {
  HMENU menu = CreatePopupMenu();
  if (!menu) {
    return;
  }

  const bool hasSelection = (m_selectionStart && m_selectionEnd && *m_selectionStart != *m_selectionEnd);
  const std::string fullText = m_attributedStringBox.getValue().getString();
  const bool hasText = !fullText.empty();

  // Add menu items (1 = Copy, 2 = Select All)
  AppendMenuW(menu, MF_STRING | (hasSelection ? 0 : MF_GRAYED), 1, L"Copy");
  AppendMenuW(menu, MF_STRING | (hasText ? 0 : MF_GRAYED), 2, L"Select All");

  // Get cursor position for menu placement
  POINT cursorPos;
  GetCursorPos(&cursorPos);

  const HWND hwnd = GetActiveWindow();

  const int cmd = TrackPopupMenu(
      menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_NONOTIFY, cursorPos.x, cursorPos.y, 0, hwnd, NULL);

  if (cmd == 1) {
    // Copy
    CopySelectionToClipboard();
  } else if (cmd == 2) {
    // Select All
    SetSelection(0, static_cast<int32_t>(fullText.length()));
    DrawText();
  }

  DestroyMenu(menu);
}

void ParagraphComponentView::OnKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  const bool isCtrlDown =
      (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Control) &
       winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down;

  // Handle Ctrl+C for copy
  if (isCtrlDown && args.Key() == winrt::Windows::System::VirtualKey::C) {
    if (m_selectionStart && m_selectionEnd && *m_selectionStart != *m_selectionEnd) {
      CopySelectionToClipboard();
      args.Handled(true);
      return;
    }
  }

  // Handle Ctrl+A for select all
  if (isCtrlDown && args.Key() == winrt::Windows::System::VirtualKey::A) {
    const std::string fullText = m_attributedStringBox.getValue().getString();
    if (!fullText.empty()) {
      if (auto root = rootComponentView()) {
        root->ClearCurrentTextSelection();
      }

      SetSelection(0, static_cast<int32_t>(fullText.length()));

      if (auto root = rootComponentView()) {
        root->SetViewWithTextSelection(*get_strong());
      }

      DrawText();
      args.Handled(true);
      return;
    }
  }

  Super::OnKeyDown(args);
}

std::string ParagraphComponentView::DefaultControlType() const noexcept {
  return "text";
}

std::string ParagraphComponentView::DefaultAccessibleName() const noexcept {
  return m_attributedStringBox.getValue().getString();
}

bool ParagraphComponentView::focusable() const noexcept {
  // Text is focusable when it's selectable or when explicitly marked as focusable via props
  return paragraphProps().isSelectable || viewProps()->focusable;
}

std::pair<facebook::react::Cursor, HCURSOR> ParagraphComponentView::cursor() const noexcept {
  // Returns I-beam cursor for selectable text
  if (paragraphProps().isSelectable) {
    return {facebook::react::Cursor::Text, nullptr};
  }
  return Super::cursor();
}

winrt::Microsoft::ReactNative::ComponentView ParagraphComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<ParagraphComponentView>(compContext, tag, reactContext);
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
