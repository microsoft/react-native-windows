
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

struct CompositionBaseComponentView : public IComponentView,
                                      public std::enable_shared_from_this<CompositionBaseComponentView> {
  static constexpr size_t SpecialBorderLayerCount = 8;

  CompositionBaseComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag);

  virtual winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept = 0;
  // Visual that should be parented to this ComponentView's parent
  virtual winrt::Microsoft::ReactNative::Composition::IVisual OuterVisual() const noexcept;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  const facebook::react::SharedViewEventEmitter &GetEventEmitter() const noexcept;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  RootComponentView *rootComponentView() noexcept override;
  const std::vector<IComponentView *> &children() const noexcept override;
  void parent(IComponentView *parent) noexcept override;
  IComponentView *parent() const noexcept override;
  bool runOnChildren(bool forward, Mso::Functor<bool(IComponentView &)> &fn) noexcept override;
  void onFocusLost() noexcept override;
  void onFocusGained() noexcept override;
  bool focusable() const noexcept override;
  std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept
      override;
  facebook::react::SharedTouchEventEmitter touchEventEmitter() noexcept override;
  facebook::react::SharedTouchEventEmitter touchEventEmitterAtPoint(facebook::react::Point pt) noexcept override;
  facebook::react::Tag tag() const noexcept override;
  int64_t sendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept override;

  virtual bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept;
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

  winrt::IInspectable EnsureUiaProvider() noexcept override;

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

  winrt::IInspectable m_uiaProvider{nullptr};
  winrt::Microsoft::ReactNative::Composition::ICompositionContext m_compContext;
  comp::CompositionPropertySet m_centerPropSet{nullptr};
  const facebook::react::Tag m_tag;
  facebook::react::SharedViewEventEmitter m_eventEmitter;
  std::vector<IComponentView *> m_children;
  IComponentView *m_parent{nullptr};
  facebook::react::LayoutMetrics m_layoutMetrics;
  bool m_needsBorderUpdate{false};
  bool m_hasTransformMatrixFacade{false};
  bool m_enableFocusVisual{false};
  uint8_t m_numBorderVisuals{0};

 private:
  void showFocusVisual(bool show) noexcept;
  winrt::Microsoft::ReactNative::Composition::IFocusVisual m_focusVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::IVisual m_outerVisual{nullptr};
};

struct CompositionViewComponentView : public CompositionBaseComponentView {
  using Super = CompositionBaseComponentView;

  [[nodiscard]] static std::shared_ptr<CompositionViewComponentView> Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag) noexcept;

  void mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
  void unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept override;
  void prepareForRecycle() noexcept override;
  bool focusable() const noexcept override;

  facebook::react::Props::Shared props() noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept override;

  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

 protected:
  CompositionViewComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag);

 private:
  facebook::react::SharedViewProps m_props;
  winrt::Microsoft::ReactNative::Composition::SpriteVisual m_visual{nullptr};
};

} // namespace Microsoft::ReactNative
