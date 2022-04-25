
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <d2d1_1.h>
#include <dwrite.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/components/text/ParagraphProps.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompViewComponentView.h"
#include <Fabric/ComponentView.h>

namespace Microsoft::ReactNative {

struct CompParagraphComponentView : CompBaseComponentView {
  CompParagraphComponentView();

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

  const winrt::Windows::UI::Composition::Visual Visual() const noexcept override;

 private:
  void ensureVisual() noexcept;
  void updateVisualBrush() noexcept;
  void DrawText() noexcept;
  void updateTextAlignment(const std::optional<facebook::react::TextAlignment> &fbAlignment) noexcept;

  std::shared_ptr<facebook::react::ParagraphProps const> m_props;
  winrt::Windows::UI::Composition::SpriteVisual m_visual{nullptr};
  winrt::com_ptr<::IDWriteTextLayout> m_textLayout{nullptr};
  facebook::react::AttributedStringBox m_attributedStringBox;
  facebook::react::ParagraphAttributes m_paragraphAttributes;

  bool m_requireRedraw{true};
  winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop> m_drawingSurfaceInterop{nullptr};
  winrt::com_ptr<winrt::Windows::UI::Composition::ICompositionGraphicsDevice> _compositionGraphicsDevice{nullptr};
  winrt::event_token m_renderDeviceReplacedToken;
};

} // namespace Microsoft::ReactNative
