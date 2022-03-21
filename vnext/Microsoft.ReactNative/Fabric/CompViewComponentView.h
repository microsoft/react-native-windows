
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include "ComponentView.h"

namespace Microsoft::ReactNative {

struct CompBaseComponentView;
struct CompContext;

// TODO replace with proper FocusManager - does it need to be per rootview?
CompBaseComponentView *GetFocusedComponent() noexcept;
void SetFocusedComponent(CompBaseComponentView *value) noexcept;

struct CompBaseComponentView : public IComponentView {
  virtual const winrt::Windows::UI::Composition::Visual Visual() const noexcept = 0;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  const facebook::react::SharedViewEventEmitter &GetEventEmitter() const noexcept;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  void parent(IComponentView *parent) noexcept override;
  IComponentView *parent() const noexcept override;

  void CompContext(const std::shared_ptr<Microsoft::ReactNative::CompContext> &compContext) noexcept;
  winrt::Windows::UI::Composition::Compositor Compositor() noexcept;
  winrt::Windows::UI::Composition::CompositionGraphicsDevice CompositionGraphicsDevice() noexcept;
  facebook::react::Tag Tag() const noexcept;
  void Tag(facebook::react::Tag) noexcept;

  virtual facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept = 0;
  virtual bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept;
  virtual int64_t SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept;
  RECT getClientRect() const noexcept override;

  void ensureBorderVisual() noexcept;
  void indexOffsetForBorder(uint32_t &index) const noexcept;
  void updateBorderProps(
      const facebook::react::ViewProps &oldViewProps,
      const facebook::react::ViewProps &newViewProps) noexcept;
  void updateBorderLayoutMetrics() noexcept;

 protected:
  std::shared_ptr<Microsoft::ReactNative::CompContext> m_compContext;
  facebook::react::Tag m_tag;
  facebook::react::SharedViewEventEmitter m_eventEmitter;
  std::vector<const IComponentView *> m_children;
  IComponentView *m_parent{nullptr};
  facebook::react::LayoutMetrics m_layoutMetrics;

  winrt::Windows::UI::Composition::SpriteVisual m_borderVisual{nullptr};
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
  RECT getClientRect() const noexcept override;

  facebook::react::SharedProps props() noexcept override;

  facebook::react::Tag hitTest(facebook::react::Point pt, facebook::react::Point &localPt) const noexcept override;
  bool ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept override;

  virtual const winrt::Windows::UI::Composition::Visual Visual() const noexcept;

 private:
  void ensureVisual() noexcept;
  bool shouldBeControl() const noexcept;

  facebook::react::SharedViewProps m_props;
  winrt::Windows::UI::Composition::Visual m_visual{nullptr};
};

} // namespace Microsoft::ReactNative
