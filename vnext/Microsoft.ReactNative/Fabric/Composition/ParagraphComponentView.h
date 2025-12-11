
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

constexpr D2D1_COLOR_F kDefaultSelectionColor = {0.0f, 0.47f, 0.84f, 0.3f};

/// Clears any active text selection in the application.
/// Called by CompositionEventHandler when pointer is pressed anywhere,
/// allowing the target ParagraphComponentView to re-establish selection if needed.
/// This ensures only one text component has selection at a time.
void ClearCurrentTextSelection() noexcept;

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

  // Returns true when text is selectable
  bool focusable() const noexcept override;

  // Returns I-beam cursor for selectable text
  std::pair<facebook::react::Cursor, HCURSOR> cursor() const noexcept override;

  /// @brief Clears the current text selection and redraws the component.
  /// Called when losing focus, when another text starts selection, or when clicking outside text bounds.
  void ClearSelection() noexcept;

  // Text selection pointer event handlers
  void OnPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerMoved(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
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
  bool isTextSelectableAtPoint(facebook::react::Point pt) noexcept;

  // Text selection support
  // Returns character position at the given point, or -1 if outside text bounds
  // Similar pattern to TextInput's hit testing but using IDWriteTextLayout directly
  int32_t getTextPositionAtPoint(facebook::react::Point pt) noexcept;

  // Returns the currently selected text, or empty string if no selection
  std::string getSelectedText() const noexcept;

  // Copies currently selected text to the system clipboard
  void copySelectionToClipboard() noexcept;

  // Finds word boundaries at the given character position and sets selection
  void selectWordAtPosition(int32_t charPosition) noexcept;

  // Shows a context menu with Copy/Select All options on right-click
  void ShowContextMenu() noexcept;

  winrt::com_ptr<::IDWriteTextLayout> m_textLayout;
  facebook::react::AttributedStringBox m_attributedStringBox;
  facebook::react::ParagraphAttributes m_paragraphAttributes;

  bool m_requireRedraw{true};
  winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush m_drawingSurface;

  // Selection state - character indices
  // -1 means no selection
  int32_t m_selectionStart{-1};
  int32_t m_selectionEnd{-1};
  // True while pointer is pressed and dragging
  bool m_isSelecting{false};

  // Double-click detection
  std::chrono::steady_clock::time_point m_lastClickTime{};
  int32_t m_lastClickPosition{-1};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
