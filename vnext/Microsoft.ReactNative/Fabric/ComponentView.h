
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <functional/functor.h>
#include <inspectable.h>
#include <react/renderer/componentregistry/ComponentDescriptorProvider.h>
#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/LayoutMetrics.h>

#include <winrt/Microsoft.ReactNative.Composition.Input.h>

namespace Microsoft::ReactNative {

enum class RNComponentViewUpdateMask : std::uint_fast8_t {
  None = 0,
  Props = 1 << 0,
  EventEmitter = 1 << 1,
  State = 1 << 2,
  LayoutMetrics = 1 << 3,

  All = Props | EventEmitter | State | LayoutMetrics
};

DEFINE_ENUM_FLAG_OPERATORS(RNComponentViewUpdateMask);

struct RootComponentView;

struct BringIntoViewOptions {
  bool AnimationDesired{false};
  // NaN will bring the element fully into view aligned to the nearest edge of the viewport
  float HorizontalAlignmentRatio{std::numeric_limits<float>::quiet_NaN()};
  float HorizontalOffset{0};
  std::optional<facebook::react::Rect> TargetRect;
  // NaN will bring the element fully into view aligned to the nearest edge of the viewport
  float VerticalAlignmentRatio{std::numeric_limits<float>::quiet_NaN()};
  float VerticalOffset{0};
};

struct IComponentView {
  virtual std::vector<facebook::react::ComponentDescriptorProvider>
  supplementalComponentDescriptorProviders() noexcept = 0;
  virtual void mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept = 0;
  virtual void unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept = 0;
  virtual void updateProps(
      facebook::react::Props::Shared const &props,
      facebook::react::Props::Shared const &oldProps) noexcept = 0;
  virtual void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept = 0;
  virtual void updateState(
      facebook::react::State::Shared const &state,
      facebook::react::State::Shared const &oldState) noexcept = 0;
  virtual void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept = 0;
  virtual void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept = 0;
  virtual void prepareForRecycle() noexcept = 0;
  virtual facebook::react::Props::Shared props() noexcept = 0;
  virtual void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept = 0;
  virtual RootComponentView *rootComponentView() noexcept = 0;
  virtual void parent(IComponentView *parent) noexcept = 0;
  virtual IComponentView *parent() const noexcept = 0;
  virtual const std::vector<IComponentView *> &children() const noexcept = 0;
  // Run fn on all children of this node until fn returns true
  // returns true if the fn ever returned true
  virtual bool runOnChildren(bool forward, Mso::Functor<bool(IComponentView &)> &fn) noexcept = 0;
  virtual RECT getClientRect() const noexcept = 0;
  virtual void onFocusLost() noexcept = 0;
  virtual void onFocusGained() noexcept = 0;
  virtual void onPointerEntered(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept = 0;
  virtual void onPointerExited(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept = 0;
  virtual void onPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept = 0;
  virtual void onPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept = 0;
  virtual void onPointerMoved(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept = 0;
  virtual void onPointerWheelChanged(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept = 0;
  virtual void onKeyDown(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept = 0;
  virtual void onKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept = 0;
  virtual bool focusable() const noexcept = 0;
  virtual facebook::react::SharedViewEventEmitter eventEmitterAtPoint(facebook::react::Point pt) noexcept = 0;
  virtual facebook::react::SharedViewEventEmitter eventEmitter() noexcept = 0;
  virtual facebook::react::Tag tag() const noexcept = 0;
  // By default, hitTests according the pointerEvents prop on the Component.
  // If ignorePointerEvents = true, all Components are treated as valid targets
  virtual facebook::react::Tag hitTest(
      facebook::react::Point pt,
      facebook::react::Point &localPt,
      bool ignorePointerEvents = false) const noexcept = 0;
  virtual int64_t sendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept = 0;
  virtual winrt::IInspectable EnsureUiaProvider() noexcept = 0;
  // Notify up the tree to bring the rect into view by scrolling as needed
  virtual void StartBringIntoView(BringIntoViewOptions &&args) noexcept = 0;
};

// Run fn on all nodes of the component view tree starting from this one until fn returns true
// returns true if the fn ever returned true
bool walkTree(IComponentView &view, bool forward, Mso::Functor<bool(IComponentView &)> &fn) noexcept;

} // namespace Microsoft::ReactNative
