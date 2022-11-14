
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include "CompositionHelpers.h"

namespace Microsoft::ReactNative {

struct CompositionBaseComponentView;
struct CompContext;

// TODO replace with proper FocusManager - does it need to be per rootview?
CompositionBaseComponentView *GetFocusedComponent() noexcept;
void SetFocusedComponent(CompositionBaseComponentView *value) noexcept;

struct CompositionBaseComponentView : public IComponentView {
  static constexpr size_t SpecialBorderLayerCount = 8;

  CompositionBaseComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag);

  virtual winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept = 0;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  const facebook::react::SharedViewEventEmitter &GetEventEmitter() const noexcept;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  void parent(IComponentView *parent) noexcept override;
  IComponentView *parent() const noexcept override;
  void onFocusLost() noexcept override;
  void onFocusGained() noexcept override;

  facebook::react::Tag tag() const noexcept override;

  virtual bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept;
  virtual int64_t SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept;
  RECT getClientRect() const noexcept override;

  void indexOffsetForBorder(uint32_t &index) const noexcept;
  void updateBorderProps(
      const facebook::react::ViewProps &oldViewProps,
      const facebook::react::ViewProps &newViewProps) noexcept;
  void updateBorderLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::ViewProps &viewProps) noexcept;

  virtual void OnRenderingDeviceLost() noexcept;

  comp::CompositionPropertySet EnsureCenterPointPropertySet() noexcept;
  void EnsureTransformMatrixFacade() noexcept;

 protected:
  std::array<winrt::Microsoft::ReactNative::Composition::SpriteVisual, SpecialBorderLayerCount>
  FindSpecialBorderLayers() const noexcept;
  bool TryUpdateSpecialBorderLayers(
      std::array<winrt::Microsoft::ReactNative::Composition::SpriteVisual, SpecialBorderLayerCount> &spBorderVisuals,
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::ViewProps &viewProps) noexcept;
  void UpdateSpecialBorderLayers(
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::ViewProps &viewProps) noexcept;
  void UpdateCenterPropertySet() noexcept;

  winrt::Microsoft::ReactNative::Composition::ICompositionContext m_compContext;
  comp::CompositionPropertySet m_centerPropSet{nullptr};
  const facebook::react::Tag m_tag;
  facebook::react::SharedViewEventEmitter m_eventEmitter;
  std::vector<const IComponentView *> m_children;
  IComponentView *m_parent{nullptr};
  facebook::react::LayoutMetrics m_layoutMetrics;
  bool m_needsBorderUpdate{false};
  bool m_hasTransformMatrixFacade{false};
  uint8_t m_numBorderVisuals{0};
};

struct CompositionViewComponentView : public CompositionBaseComponentView {
  using Super = CompositionBaseComponentView;
  CompositionViewComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag);

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
  facebook::react::SharedTouchEventEmitter touchEventEmitter() noexcept override;

  facebook::react::Props::Shared props() noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept override;

  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

 private:
  facebook::react::SharedViewProps m_props;
  winrt::Microsoft::ReactNative::Composition::SpriteVisual m_visual{nullptr};
};

} // namespace Microsoft::ReactNative
