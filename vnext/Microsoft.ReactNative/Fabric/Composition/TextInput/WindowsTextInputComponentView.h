
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

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

namespace Microsoft::ReactNative {

struct CompTextHost;

struct WindowsTextInputComponentView : CompositionBaseComponentView {
  friend CompTextHost;

  using Super = CompositionBaseComponentView;
  WindowsTextInputComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept
      override;
  void mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::Props::Shared props() noexcept override;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  int64_t SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  void parent(IComponentView *parent) noexcept override;
  void OnRenderingDeviceLost() noexcept override;
  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;
  void onFocusLost() noexcept override;
  void onFocusGained() noexcept override;
  facebook::react::SharedTouchEventEmitter touchEventEmitter() noexcept override;

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
  void UpdateCharFormat() noexcept;
  void UpdateParaFormat() noexcept;
  void UpdateText(const std::string &str) noexcept;
  void OnTextUpdated() noexcept;
  void OnSelectionChanged(LONG start, LONG end) noexcept;
  std::string GetTextFromRichEdit() const noexcept;

  winrt::Windows::UI::Composition::CompositionSurfaceBrush m_brush{nullptr};
  winrt::Microsoft::ReactNative::Composition::SpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::ICaretVisual m_caretVisual{nullptr};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  winrt::Microsoft::ReactNative::Composition::ICompositionDrawingSurface m_drawingSurface{nullptr};

  // Used by ITextHost impl
  CHARFORMAT2W m_cf;
  PARAFORMAT2 m_pf;

  winrt::com_ptr<ITextHost> m_textHost;
  winrt::com_ptr<ITextServices2> m_textServices;
  unsigned int m_imgWidth{0}, m_imgHeight{0};
  std::shared_ptr<facebook::react::WindowsTextInputProps const> m_props;
  std::shared_ptr<facebook::react::WindowsTextInputShadowNode::ConcreteState const> m_state;
  int m_mostRecentEventCount{0};
  int m_nativeEventCount{0};
  bool m_comingFromJS{false};
  bool m_comingFromState{false};
  int m_cDrawBlock{0};
  bool m_needsRedraw{false};
  bool m_drawing{false};
};

} // namespace Microsoft::ReactNative
