
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <functional/functor.h>
#include <inspectable.h>
#include <react/renderer/componentregistry/ComponentDescriptorProvider.h>
#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/LayoutMetrics.h>

#include <Fabric/Composition/Theme.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Microsoft.ReactNative.h>

#include "ComponentView.g.h"
#include "CreateComponentViewArgs.g.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {
struct RootComponentView;
}

namespace winrt::Microsoft::ReactNative::implementation {

enum class ClipState : std::uint_fast8_t {
  NoClip = 0,
  PartialClip = 1,
  FullyClipped = 2,
};

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

struct CreateComponentViewArgs : public CreateComponentViewArgsT<CreateComponentViewArgs> {
  CreateComponentViewArgs(const winrt::Microsoft::ReactNative::IReactContext &reactContext, facebook::react::Tag tag);

  facebook::react::Tag Tag() const noexcept;

  winrt::Microsoft::ReactNative::IReactContext ReactContext() const noexcept;

 private:
  const facebook::react::Tag m_tag;
  const winrt::Microsoft::ReactNative::IReactContext m_reactContext;
};

struct ComponentView : public ComponentViewT<ComponentView> {
  ComponentView(winrt::Microsoft::ReactNative::CreateComponentViewArgs const &args);

  ComponentView(
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      bool customComponent);

  virtual std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept;
  virtual void updateProps(
      facebook::react::Props::Shared const &props,
      facebook::react::Props::Shared const &oldProps) noexcept;
  virtual void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept;
  virtual void updateState(
      facebook::react::State::Shared const &state,
      facebook::react::State::Shared const &oldState) noexcept;
  virtual void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept;
  virtual void prepareForRecycle() noexcept;
  virtual facebook::react::Props::Shared props() noexcept;
  virtual winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *rootComponentView() noexcept;
  virtual void parent(const winrt::Microsoft::ReactNative::ComponentView &parent) noexcept;
  virtual winrt::Microsoft::ReactNative::ComponentView Parent() const noexcept;
  virtual winrt::IVectorView<winrt::Microsoft::ReactNative::ComponentView> Children() const noexcept;
  virtual void theme(winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme) noexcept;
  virtual winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme() const noexcept;
  virtual void onThemeChanged() noexcept;
  // Run fn on all children of this node until fn returns true
  // returns true if the fn ever returned true
  bool runOnChildren(bool forward, Mso::Functor<bool(ComponentView &)> &fn) noexcept;
  virtual RECT getClientRect() const noexcept;
  // The offset from this elements parent to its children (accounts for things like scroll position)
  virtual facebook::react::Point getClientOffset() const noexcept;
  virtual void onFocusLost() noexcept;
  virtual void onFocusGained() noexcept;

  virtual bool focusable() const noexcept;
  virtual facebook::react::SharedViewEventEmitter eventEmitterAtPoint(facebook::react::Point pt) noexcept;
  virtual facebook::react::SharedViewEventEmitter eventEmitter() noexcept;
  virtual facebook::react::Tag Tag() const noexcept;
  // By default, hitTests according the pointerEvents prop on the Component.
  // If ignorePointerEvents = true, all Components are treated as valid targets
  virtual facebook::react::Tag
  hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents = false) const noexcept;
  virtual winrt::IInspectable EnsureUiaProvider() noexcept;
  virtual std::optional<std::string> getAcccessiblityValue() noexcept;
  virtual void setAcccessiblityValue(std::string &&value) noexcept;
  virtual bool getAcccessiblityIsReadOnly() noexcept;
  winrt::Microsoft::ReactNative::IReactContext ReactContext() const noexcept;

  // Notify up the tree to bring the rect into view by scrolling as needed
  virtual void StartBringIntoView(BringIntoViewOptions &&args) noexcept;

  virtual const winrt::Microsoft::ReactNative::IComponentProps userProps(
      facebook::react::Props::Shared const &props) noexcept;

  // Publicaly overridable APIs
  virtual void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept;
  virtual void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept;
  virtual void HandleCommand(
      winrt::hstring commandName,
      const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept;
  virtual void UpdateProps(
      const winrt::Microsoft::ReactNative::IComponentProps &props,
      const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept;
  virtual void UpdateState(const winrt::Microsoft::ReactNative::IComponentState &state) noexcept;
  virtual void UpdateLayoutMetrics(const LayoutMetrics &metrics, const LayoutMetrics &oldMetrics) noexcept;
  virtual void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept;
  virtual void OnPointerEntered(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  virtual void OnPointerExited(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  virtual void OnPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  virtual void OnPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  virtual void OnPointerMoved(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  virtual void OnPointerWheelChanged(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  virtual void OnPointerCaptureLost() noexcept;
  virtual void OnKeyDown(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
  virtual void OnKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
  virtual void OnCharacterReceived(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept;

 protected:
  const bool m_customComponent; // Is a user custom component, and so needs to call external override functions
  const facebook::react::Tag m_tag;
  winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *m_rootView{nullptr};
  mutable winrt::Microsoft::ReactNative::Composition::implementation::Theme *m_theme{nullptr};
  const winrt::Microsoft::ReactNative::ReactContext m_reactContext;
  winrt::Microsoft::ReactNative::ComponentView m_parent{nullptr};
  winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::ComponentView> m_children{
      winrt::single_threaded_vector<winrt::Microsoft::ReactNative::ComponentView>()};
};

// Run fn on all nodes of the component view tree starting from this one until fn returns true
// returns true if the fn ever returned true
bool walkTree(
    const winrt::Microsoft::ReactNative::ComponentView &view,
    bool forward,
    Mso::Functor<bool(const winrt::Microsoft::ReactNative::ComponentView &)> &fn) noexcept;

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ComponentView : ComponentViewT<ComponentView, implementation::ComponentView> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
