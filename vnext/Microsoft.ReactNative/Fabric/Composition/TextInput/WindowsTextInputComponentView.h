
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Composition.WindowsTextInputComponentView.g.h"
#include <ReactContext.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <richedit.h>
#include <textserv.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include "../ComponentView.h"
#include "../CompositionHelpers.h"
#include "../CompositionViewComponentView.h"
#include "WindowsTextInputProps.h"
#include "WindowsTextInputShadowNode.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {
struct CompTextHost;

struct WindowsTextInputComponentView : WindowsTextInputComponentViewT<WindowsTextInputComponentView, ComponentView> {
  friend CompTextHost;

  using Super = WindowsTextInputComponentViewT<WindowsTextInputComponentView, ComponentView>;
  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
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
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::SharedViewProps viewProps() noexcept override;
  void HandleCommand(winrt::hstring commandName, const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept
      override;
  facebook::react::Tag hitTest(
      facebook::react::Point pt,
      facebook::react::Point &localPt,
      bool ignorePointerEvents = false) const noexcept override;
  void OnRenderingDeviceLost() noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;
  void onFocusLost() noexcept override;
  void onFocusGained() noexcept override;
  bool focusable() const noexcept override;
  std::string DefaultControlType() const noexcept override;
  std::string DefaultAccessibleName() const noexcept override;
  std::string DefaultHelpText() const noexcept override;
  void onThemeChanged() noexcept override;
  void OnPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void OnPointerMoved(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;

  void OnKeyDown(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void OnKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void OnCharacterReceived(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept
      override;

  std::optional<std::string> getAcccessiblityValue() noexcept override;
  void setAcccessiblityValue(std::string &&value) noexcept override;
  bool getAcccessiblityIsReadOnly() noexcept override;

  WindowsTextInputComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

 private:
  struct DrawBlock {
    DrawBlock(WindowsTextInputComponentView &view);
    ~DrawBlock();
    WindowsTextInputComponentView &m_view;
  };

  facebook::react::AttributedString getAttributedString() const;
  void ensureVisual() noexcept;
  void ensureDrawingSurface() noexcept;
  void DrawText() noexcept;
  void ShowCaret(bool show) noexcept;
  winrt::com_ptr<::IDWriteTextLayout> CreatePlaceholderLayout();
  void UpdateCharFormat() noexcept;
  void UpdateParaFormat() noexcept;
  void UpdateText(const std::string &str) noexcept;
  void OnTextUpdated() noexcept;
  void OnSelectionChanged(LONG start, LONG end) noexcept;
  std::string GetTextFromRichEdit() const noexcept;
  void updateCursorColor(
      const facebook::react::SharedColor &cursorColor,
      const facebook::react::SharedColor &foregroundColor) noexcept;
  bool ShouldSubmit(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept;

  winrt::Windows::UI::Composition::CompositionSurfaceBrush m_brush{nullptr};
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::ICaretVisual m_caretVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::IDrawingSurfaceBrush m_drawingSurface{nullptr};

  // Used by ITextHost impl
  CHARFORMAT2W m_cf;
  PARAFORMAT2 m_pf;

  winrt::com_ptr<ITextHost> m_textHost;
  winrt::com_ptr<ITextServices2> m_textServices;
  unsigned int m_imgWidth{0}, m_imgHeight{0};
  std::shared_ptr<facebook::react::WindowsTextInputProps const> m_props;
  std::shared_ptr<facebook::react::WindowsTextInputShadowNode::ConcreteState const> m_state;
  RECT m_rcClient;
  int64_t m_mostRecentEventCount{0};
  int m_nativeEventCount{0};
  bool m_comingFromJS{false};
  bool m_comingFromState{false};
  int m_cDrawBlock{0};
  bool m_needsRedraw{false};
  bool m_drawing{false};
  bool m_clearTextOnSubmit{false};
  bool m_multiline{false};
  std::vector<facebook::react::CompWindowsTextInputSubmitKeyEventsStruct> m_submitKeyEvents;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
