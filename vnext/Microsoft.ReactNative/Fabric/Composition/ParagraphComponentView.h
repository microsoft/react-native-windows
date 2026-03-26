
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Composition.ParagraphComponentView.g.h"
#include <Fabric/ComponentView.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/components/text/ParagraphProps.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include <chrono>
#include "CompositionHelpers.h"
#include "CompositionViewComponentView.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ParagraphComponentView : ParagraphComponentViewT<ParagraphComponentView, ViewComponentView> {
  using Super = ParagraphComponentViewT<ParagraphComponentView, ViewComponentView>;

  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept override;
  void OnRenderingDeviceLost() noexcept override;
  void onThemeChanged() noexcept override;
  facebook::react::SharedViewEventEmitter eventEmitterAtPoint(facebook::react::Point pt) noexcept override;

  virtual std::string DefaultControlType() const noexcept override;
  virtual std::string DefaultAccessibleName() const noexcept override;
  static facebook::react::SharedViewProps defaultProps() noexcept;
  const facebook::react::ParagraphProps &paragraphProps() const noexcept;

  facebook::react::Tag hitTest(
      facebook::react::Point pt,
      facebook::react::Point &localPt,
      bool ignorePointerEvents = false) const noexcept override;

  // Returns true when text is selectable
  bool focusable() const noexcept override;

  // Returns I-beam cursor for selectable text
  std::pair<facebook::react::Cursor, HCURSOR> cursor() const noexcept override;

  // Called when losing focus, when another text starts selection, or when clicking outside text bounds.
  void ClearSelection() noexcept;

  // Text selection pointer event handlers
  void OnPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerMoved(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerCaptureLost() noexcept override;
  void onLostFocus(const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept override;

  // Keyboard event handler for copy
  void OnKeyDown(const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;

  ParagraphComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

 private:
  void updateVisualBrush() noexcept;
  void DrawText() noexcept;
  void DrawSelectionHighlight(
      ID2D1RenderTarget &renderTarget,
      float offsetX,
      float offsetY,
      float pointScaleFactor) noexcept;
  void updateTextAlignment(const std::optional<facebook::react::TextAlignment> &fbAlignment) noexcept;
  bool IsTextSelectableAtPoint(facebook::react::Point pt) noexcept;
  std::optional<int32_t> GetTextPositionAtPoint(facebook::react::Point pt) noexcept;
  std::optional<int32_t> GetClampedTextPosition(facebook::react::Point pt) noexcept;
  std::string GetSelectedText() const noexcept;

  // Copies currently selected text to the system clipboard
  void CopySelectionToClipboard() noexcept;

  // Selects the word at the given character position
  void SelectWordAtPosition(int32_t charPosition) noexcept;
  std::pair<int32_t, int32_t> GetWordBoundariesAtPosition(int32_t charPosition) noexcept;

  // Shows a context menu with Copy/Select All options on right-click
  void ShowContextMenu() noexcept;

  // m_selectionStart <= m_selectionEnd
  void SetSelection(int32_t start, int32_t end) noexcept;

  winrt::com_ptr<::IDWriteTextLayout> m_textLayout;
  facebook::react::AttributedStringBox m_attributedStringBox;
  facebook::react::ParagraphAttributes m_paragraphAttributes;

  bool m_requireRedraw{true};
  winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush m_drawingSurface;

  std::optional<int32_t> m_selectionStart;
  std::optional<int32_t> m_selectionEnd;
  bool m_isSelecting{false};

  // Double click + drag selection
  bool m_isWordSelecting{false};
  int32_t m_wordAnchorStart{0};
  int32_t m_wordAnchorEnd{0};

  // Double-click detection
  std::chrono::steady_clock::time_point m_lastClickTime{};
  std::optional<int32_t> m_lastClickPosition;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
