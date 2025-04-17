
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

  ParagraphComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

 private:
  void updateVisualBrush() noexcept;
  void DrawText() noexcept;
  void updateTextAlignment(const std::optional<facebook::react::TextAlignment> &fbAlignment) noexcept;

  winrt::com_ptr<::IDWriteTextLayout> m_textLayout;
  facebook::react::AttributedStringBox m_attributedStringBox;
  facebook::react::ParagraphAttributes m_paragraphAttributes;

  bool m_requireRedraw{true}; 
  winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush m_drawingSurface;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
