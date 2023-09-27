
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
#include "CompositionHelpers.h"
#include "CompositionViewComponentView.h"

namespace Microsoft::ReactNative {

struct ParagraphComponentView : CompositionBaseComponentView {
  using Super = CompositionBaseComponentView;

  [[nodiscard]] static std::shared_ptr<ParagraphComponentView> Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag) noexcept;

  void mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
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
  facebook::react::Props::Shared props() noexcept override;
  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents)
      const noexcept override;
  void OnRenderingDeviceLost() noexcept override;
  facebook::react::SharedViewEventEmitter eventEmitterAtPoint(facebook::react::Point pt) noexcept override;

  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;
  virtual std::string DefaultControlType() const noexcept override;
  virtual std::string DefaultAccessibleName() const noexcept override;

 private:
  ParagraphComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag);

  void ensureVisual() noexcept;
  void updateVisualBrush() noexcept;
  void DrawText() noexcept;
  void updateTextAlignment(const std::optional<facebook::react::TextAlignment> &fbAlignment) noexcept;

  std::shared_ptr<facebook::react::ParagraphProps const> m_props;
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
  winrt::com_ptr<::IDWriteTextLayout> m_textLayout;
  facebook::react::AttributedStringBox m_attributedStringBox;
  facebook::react::ParagraphAttributes m_paragraphAttributes;

  bool m_requireRedraw{true};
  winrt::Microsoft::ReactNative::Composition::IDrawingSurfaceBrush m_drawingSurface;
};

} // namespace Microsoft::ReactNative
