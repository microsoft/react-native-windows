
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/components/text/ParagraphProps.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompHelpers.h"
#include "CompViewComponentView.h"

namespace Microsoft::ReactNative {

struct CompParagraphComponentView : CompBaseComponentView {
  using Super = CompBaseComponentView;
  CompParagraphComponentView(
      const winrt::com_ptr<Composition::ICompositionContext> &compContext,
      facebook::react::Tag tag);

  std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept
      override;
  void mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  facebook::react::SharedProps props() noexcept override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  void OnRenderingDeviceLost() noexcept override;

  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

 private:
  void ensureVisual() noexcept;
  void updateVisualBrush() noexcept;
  void DrawText() noexcept;
  void updateTextAlignment(const std::optional<facebook::react::TextAlignment> &fbAlignment) noexcept;

  std::shared_ptr<facebook::react::ParagraphProps const> m_props;
  winrt::Microsoft::ReactNative::Composition::SpriteVisual m_visual{nullptr};
  winrt::com_ptr<::IDWriteTextLayout> m_textLayout;
  facebook::react::AttributedStringBox m_attributedStringBox;
  facebook::react::ParagraphAttributes m_paragraphAttributes;

  bool m_requireRedraw{true};
  winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> m_drawingSurfaceInterop;
};

} // namespace Microsoft::ReactNative
