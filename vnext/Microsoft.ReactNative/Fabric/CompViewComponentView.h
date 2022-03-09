
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include "ComponentView.h"

namespace Microsoft::ReactNative {

struct CompBaseComponentView : public IComponentView {
  virtual const winrt::Windows::UI::Composition::Visual Visual() const noexcept = 0;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  const facebook::react::SharedViewEventEmitter &GetEventEmitter() const noexcept;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;

  void Compositor(const winrt::Windows::UI::Composition::Compositor &compositor) noexcept;
  facebook::react::Tag Tag() const noexcept;
  void Tag(facebook::react::Tag) noexcept;

  virtual facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept = 0;
  virtual bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept;

 protected:
  winrt::Windows::UI::Composition::Compositor m_compositor;
  facebook::react::Tag m_tag;
  facebook::react::SharedViewEventEmitter m_eventEmitter;
  std::vector<const IComponentView *> m_children;
};

struct CompViewComponentView : public CompBaseComponentView {
  CompViewComponentView();

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

  virtual const winrt::Windows::UI::Composition::Visual Visual() const noexcept;

 private:
  void ensureVisual() noexcept;
  bool shouldBeControl() const noexcept;

  facebook::react::SharedViewProps m_props;
  facebook::react::LayoutMetrics m_layoutMetrics;
  winrt::Windows::UI::Composition::Visual m_visual{nullptr};
};

} // namespace Microsoft::ReactNative
