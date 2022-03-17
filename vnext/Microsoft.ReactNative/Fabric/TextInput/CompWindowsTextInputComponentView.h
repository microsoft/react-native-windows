
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactContext.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <richedit.h>
#include <textserv.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include "../CompViewComponentView.h"
#include "../ComponentView.h"
#include "WindowsTextInputProps.h"
#include "WindowsTextInputShadowNode.h"

namespace Microsoft::ReactNative {

struct CompTextHost;

struct CompWindowsTextInputComponentView : CompBaseComponentView {
  friend CompTextHost;

  using Super = CompBaseComponentView;
  CompWindowsTextInputComponentView(winrt::Microsoft::ReactNative::ReactContext const &reactContext);

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
  facebook::react::SharedProps props() noexcept override;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  int64_t SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  void parent(IComponentView *parent) noexcept override;

  const winrt::Windows::UI::Composition::Visual Visual() const noexcept override;

 private:
  struct DrawBlock {
    DrawBlock(CompWindowsTextInputComponentView &view);
    ~DrawBlock();
    CompWindowsTextInputComponentView &m_view;
  };

  facebook::react::AttributedString getAttributedString() const;
  void ensureVisual() noexcept;
  void ensureDrawingSurface() noexcept;
  void DrawImage() noexcept;
  void UpdateCharFormat() noexcept;
  void UpdateParaFormat() noexcept;
  void UpdateText(const std::string &str) noexcept;
  void OnTextUpdated() noexcept;
  void OnSelectionChanged(LONG start, LONG end) noexcept;
  std::string GetTextFromRichEdit() const noexcept;

  winrt::Windows::UI::Composition::CompositionSurfaceBrush m_brush{nullptr};
  winrt::Windows::UI::Composition::SpriteVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop> m_drawingSurfaceInterop{nullptr};

  // Used by ITextHost impl
  CHARFORMAT2W m_cf;
  PARAFORMAT2 m_pf;

  winrt::com_ptr<ITextHost> m_textHost;
  winrt::com_ptr<ITextServices2> m_textServices;
  winrt::event_token m_renderDeviceReplacedToken;
  unsigned int m_imgWidth{0}, m_imgHeight{0};
  std::shared_ptr<facebook::react::WindowsTextInputProps const> m_props;
  std::shared_ptr<facebook::react::WindowsTextInputShadowNode::ConcreteState const> m_state;
  int m_mostRecentEventCount{0};
  int m_nativeEventCount{0};
  bool m_comingFromJS{false};
  bool m_comingFromState{false};
  int m_cDrawBlock{0};
  bool m_needsRedraw{false};
};

} // namespace Microsoft::ReactNative
