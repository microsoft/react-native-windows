
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <functional/functor.h>
#include <inspectable.h>
#include <react/renderer/componentregistry/ComponentDescriptorProvider.h>
#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/LayoutMetrics.h>

#include <ComponentView.Experimental.interop.h>
#include <Fabric/Composition/ReactCompositionViewComponentBuilder.h>
#include <Fabric/Composition/Theme.h>
#include <uiautomationcore.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Microsoft.ReactNative.h>

#include "ComponentView.g.h"
#include "LayoutMetricsChangedArgs.g.h"
#include "MountChildComponentViewArgs.g.h"
#include "UnmountChildComponentViewArgs.g.h"

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
  float HorizontalOffset{20};
  std::optional<facebook::react::Rect> TargetRect;
  // NaN will bring the element fully into view aligned to the nearest edge of the viewport
  float VerticalAlignmentRatio{std::numeric_limits<float>::quiet_NaN()};
  float VerticalOffset{20};
};

struct LayoutMetricsChangedArgs : public LayoutMetricsChangedArgsT<LayoutMetricsChangedArgs> {
  LayoutMetricsChangedArgs(const LayoutMetrics &newLayoutMetrics, const LayoutMetrics &oldLayoutMetrics);

  LayoutMetrics OldLayoutMetrics() const noexcept;
  LayoutMetrics NewLayoutMetrics() const noexcept;

 private:
  LayoutMetrics m_old;
  LayoutMetrics m_new;
};

struct MountChildComponentViewArgs : public MountChildComponentViewArgsT<MountChildComponentViewArgs> {
  MountChildComponentViewArgs(const winrt::Microsoft::ReactNative::ComponentView &child, uint32_t index);

  winrt::Microsoft::ReactNative::ComponentView Child() const noexcept;
  uint32_t Index() const noexcept;

 private:
  winrt::Microsoft::ReactNative::ComponentView m_child;
  uint32_t m_index;
};

struct UnmountChildComponentViewArgs : public UnmountChildComponentViewArgsT<UnmountChildComponentViewArgs> {
  UnmountChildComponentViewArgs(const winrt::Microsoft::ReactNative::ComponentView &child, uint32_t index);

  winrt::Microsoft::ReactNative::ComponentView Child() const noexcept;
  uint32_t Index() const noexcept;

 private:
  winrt::Microsoft::ReactNative::ComponentView m_child;
  uint32_t m_index;
};

struct ComponentView
    : public ComponentViewT<ComponentView, ::Microsoft::ReactNative::Composition::Experimental::IComponentViewInterop> {
  ComponentView(
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder *builder);

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
  virtual winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *rootComponentView()
      const noexcept;
  virtual void parent(const winrt::Microsoft::ReactNative::ComponentView &parent) noexcept;
  virtual winrt::Microsoft::ReactNative::ComponentView Parent() const noexcept;
  virtual winrt::Windows::Foundation::Collections::IVectorView<winrt::Microsoft::ReactNative::ComponentView> Children()
      const noexcept;
  virtual void theme(winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme) noexcept;
  virtual winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme() const noexcept;
  virtual void onThemeChanged() noexcept;
  // Run fn on all children of this node until fn returns true
  // returns true if the fn ever returned true
  bool runOnChildren(bool forward, Mso::Functor<bool(ComponentView &)> &fn) noexcept;
  virtual RECT getClientRect() const noexcept;
  winrt::Windows::Foundation::Point ScreenToLocal(winrt::Windows::Foundation::Point pt) noexcept;
  winrt::Windows::Foundation::Point LocalToScreen(winrt::Windows::Foundation::Point pt) noexcept;
  // The offset from this elements parent to its children (accounts for things like scroll position)
  virtual facebook::react::Point getClientOffset() const noexcept;
  virtual void onLosingFocus(const winrt::Microsoft::ReactNative::LosingFocusEventArgs &args) noexcept;
  virtual void onGettingFocus(const winrt::Microsoft::ReactNative::GettingFocusEventArgs &args) noexcept;
  virtual void onLostFocus(const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept;
  virtual void onGotFocus(const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept;
  virtual void onMounted() noexcept;
  bool isMounted() noexcept;
  virtual void onUnmounted() noexcept;
  void onDestroying() noexcept;

  winrt::event_token Destroying(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView> const &handler) noexcept;
  void Destroying(winrt::event_token const &token) noexcept;
  winrt::event_token Mounted(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView> const &handler) noexcept;
  void Mounted(winrt::event_token const &token) noexcept;
  winrt::event_token Unmounted(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView> const &handler) noexcept;
  void Unmounted(winrt::event_token const &token) noexcept;
  winrt::event_token LosingFocus(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::LosingFocusEventArgs> const
          &handler) noexcept;
  void LosingFocus(winrt::event_token const &token) noexcept;
  winrt::event_token GettingFocus(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::GettingFocusEventArgs> const
          &handler) noexcept;
  void GettingFocus(winrt::event_token const &token) noexcept;
  winrt::event_token LostFocus(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> const
          &handler) noexcept;
  void LostFocus(winrt::event_token const &token) noexcept;
  winrt::event_token GotFocus(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> const
          &handler) noexcept;
  void GotFocus(winrt::event_token const &token) noexcept;
  winrt::event_token LayoutMetricsChanged(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::LayoutMetricsChangedArgs> const
          &handler) noexcept;
  void LayoutMetricsChanged(winrt::event_token const &token) noexcept;

  winrt::event_token KeyDown(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs> const &handler) noexcept;
  void KeyDown(winrt::event_token const &token) noexcept;
  winrt::event_token KeyUp(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs> const &handler) noexcept;
  void KeyUp(winrt::event_token const &token) noexcept;
  winrt::event_token CharacterReceived(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs> const &handler) noexcept;
  void CharacterReceived(winrt::event_token const &token) noexcept;
  winrt::event_token PointerPressed(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept;
  void PointerPressed(winrt::event_token const &token) noexcept;
  winrt::event_token PointerReleased(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept;
  void PointerReleased(winrt::event_token const &token) noexcept;
  winrt::event_token PointerMoved(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept;
  void PointerMoved(winrt::event_token const &token) noexcept;
  winrt::event_token PointerWheelChanged(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept;
  void PointerWheelChanged(winrt::event_token const &token) noexcept;
  winrt::event_token PointerEntered(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept;
  void PointerEntered(winrt::event_token const &token) noexcept;
  winrt::event_token PointerExited(
      winrt::Windows::Foundation::EventHandler<
          winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept;
  void PointerExited(winrt::event_token const &token) noexcept;
  winrt::event_token PointerCaptureLost(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView> const &handler) noexcept;
  void PointerCaptureLost(winrt::event_token const &token) noexcept;

  LayoutMetrics LayoutMetrics() const noexcept;

  bool TryFocus() noexcept;

  virtual bool focusable() const noexcept;
  virtual facebook::react::SharedViewEventEmitter eventEmitterAtPoint(facebook::react::Point pt) noexcept;
  virtual facebook::react::SharedViewEventEmitter eventEmitter() noexcept;
  virtual facebook::react::Tag Tag() const noexcept;
  // By default, hitTests according the pointerEvents prop on the Component.
  // If ignorePointerEvents = true, all Components are treated as valid targets
  virtual facebook::react::Tag
  hitTest(facebook::react::Point pt, facebook::react::Point &localPt, bool ignorePointerEvents = false) const noexcept;
  virtual winrt::Windows::Foundation::IInspectable EnsureUiaProvider() noexcept;
  virtual winrt::Windows::Foundation::IInspectable CreateAutomationProvider() noexcept;
  virtual std::optional<std::string> getAccessiblityValue() noexcept;
  virtual void setAcccessiblityValue(std::string &&value) noexcept;
  virtual bool getAcccessiblityIsReadOnly() noexcept;
  virtual ToggleState getToggleState() noexcept;
  virtual void Toggle() noexcept;
  winrt::Microsoft::ReactNative::IReactContext ReactContext() const noexcept;

  // Notify up the tree to bring the rect into view by scrolling as needed
  virtual void StartBringIntoView(BringIntoViewOptions &&args) noexcept;

  // Eventually PopupContentLink and similar APIs will remove the need for this.
  virtual HWND GetHwndForParenting() noexcept;

  virtual const winrt::Microsoft::ReactNative::IComponentProps userProps(
      facebook::react::Props::Shared const &props) noexcept;

  void UserData(const winrt::Windows::Foundation::IInspectable &userData) noexcept;
  winrt::Windows::Foundation::IInspectable UserData() const noexcept;

  virtual void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept;
  virtual void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept;
  virtual void HandleCommand(const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept;
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
  virtual void OnKeyDown(const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
  virtual void OnKeyUp(const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
  virtual void OnCharacterReceived(
      const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept;

 protected:
  winrt::com_ptr<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder> m_builder;
  bool m_mounted : 1 {false};
  const facebook::react::Tag m_tag;
  winrt::Windows::Foundation::IInspectable m_userData;
  mutable winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *m_rootView{nullptr};
  mutable winrt::Microsoft::ReactNative::Composition::implementation::Theme *m_theme{nullptr};
  const winrt::Microsoft::ReactNative::ReactContext m_reactContext;
  winrt::Microsoft::ReactNative::ComponentView m_parent{nullptr};
  facebook::react::LayoutMetrics m_layoutMetrics;
  winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::ComponentView> m_children{
      winrt::single_threaded_vector<winrt::Microsoft::ReactNative::ComponentView>()};
  winrt::Windows::Foundation::IInspectable m_uiaProvider{nullptr};

  winrt::event<
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs>>
      m_keyDownEvent;
  winrt::event<
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs>>
      m_keyUpEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs>>
      m_characterReceivedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs>>
      m_pointerPressedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs>>
      m_pointerReleasedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs>>
      m_pointerMovedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs>>
      m_pointerWheelChangedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs>>
      m_pointerEnteredEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<
      winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs>>
      m_pointerExitedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView>>
      m_pointerCaptureLostEvent;

  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::LayoutMetricsChangedArgs>>
      m_layoutMetricsChangedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView>>
      m_destroyingEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView>> m_mountedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView>> m_unmountedEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::LosingFocusEventArgs>>
      m_losingFocusEvent;
  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::GettingFocusEventArgs>>
      m_gettingFocusEvent;
  winrt::event<
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs>>
      m_lostFocusEvent;
  winrt::event<
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs>>
      m_gotFocusEvent;
};

// Run fn on all nodes of the component view tree starting from this one until fn returns true
// returns true if the fn ever returned true
bool walkTree(
    const winrt::Microsoft::ReactNative::ComponentView &view,
    bool forward,
    Mso::Functor<bool(const winrt::Microsoft::ReactNative::ComponentView &)> &fn) noexcept;

} // namespace winrt::Microsoft::ReactNative::implementation
