
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include "CompHelpers.h"

namespace Microsoft::ReactNative {

struct CompBaseComponentView;
struct CompContext;

// TODO replace with proper FocusManager - does it need to be per rootview?
CompBaseComponentView *GetFocusedComponent() noexcept;
void SetFocusedComponent(CompBaseComponentView *value) noexcept;

struct CompBaseComponentView : public IComponentView {
  CompBaseComponentView(const winrt::com_ptr<Composition::ICompositionContext> &compContext, facebook::react::Tag tag);

  virtual winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept = 0;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  const facebook::react::SharedViewEventEmitter &GetEventEmitter() const noexcept;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  void parent(IComponentView *parent) noexcept override;
  IComponentView *parent() const noexcept override;

  facebook::react::Tag Tag() const noexcept;

  virtual facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept = 0;
  virtual bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept;
  virtual int64_t SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept;
  RECT getClientRect() const noexcept override;

  void ensureBorderVisual() noexcept;
  void indexOffsetForBorder(uint32_t &index) const noexcept;
  void updateBorderProps(
      const facebook::react::ViewProps &oldViewProps,
      const facebook::react::ViewProps &newViewProps) noexcept;
  void updateBorderLayoutMetrics(const facebook::react::ViewProps &viewProps) noexcept;

  virtual void OnRenderingDeviceLost() noexcept;

 protected:
  const winrt::com_ptr<Composition::ICompositionContext> m_compContext;
  const facebook::react::Tag m_tag;
  facebook::react::SharedViewEventEmitter m_eventEmitter;
  std::vector<const IComponentView *> m_children;
  IComponentView *m_parent{nullptr};
  facebook::react::LayoutMetrics m_layoutMetrics;

  /*
    winrt::Windows::UI::Composition::ShapeVisual m_borderVisual{nullptr};
    winrt::Windows::UI::Composition::CompositionSpriteShape m_borderShape{nullptr};
    winrt::Windows::UI::Composition::CompositionRoundedRectangleGeometry m_borderGeometry{nullptr};
    */
};

struct CompViewComponentView : public CompBaseComponentView {
  using Super = CompBaseComponentView;
  CompViewComponentView(const winrt::com_ptr<Composition::ICompositionContext> &compContext, facebook::react::Tag tag);

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

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept override;

  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

 private:
  facebook::react::SharedViewProps m_props;
  winrt::Microsoft::ReactNative::Composition::SpriteVisual m_visual{nullptr};
  // winrt::Windows::UI::Composition::Visual m_visual{nullptr};
};

} // namespace Microsoft::ReactNative
