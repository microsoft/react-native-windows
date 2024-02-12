
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ComponentView.h"

#include <Fabric/AbiState.h>
#include "DynamicReader.h"

#include "ComponentView.g.cpp"
#include "CreateComponentViewArgs.g.cpp"

namespace winrt::Microsoft::ReactNative::Composition::implementation {
struct RootComponentView;
}

namespace winrt::Microsoft::ReactNative::implementation {

CreateComponentViewArgs::CreateComponentViewArgs(
    const winrt::Microsoft::ReactNative::IReactContext &reactContext,
    facebook::react::Tag tag)
    : m_tag(tag), m_reactContext(reactContext){};

facebook::react::Tag CreateComponentViewArgs::Tag() const noexcept {
  return m_tag;
}

winrt::Microsoft::ReactNative::IReactContext CreateComponentViewArgs::ReactContext() const noexcept {
  return m_reactContext;
}

ComponentView::ComponentView(
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    bool customComponent)
    : m_tag(tag), m_reactContext(reactContext), m_customComponent(customComponent) {}

ComponentView::ComponentView(winrt::Microsoft::ReactNative::CreateComponentViewArgs const &args)
    : ComponentView(args.Tag(), args.ReactContext(), true) {}

std::vector<facebook::react::ComponentDescriptorProvider>
ComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ComponentView::mountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {}

void ComponentView::unmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {}

void ComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  if (m_customComponent) {
    // Review is it expected that I need this cast to call overridden methods?
    winrt::Microsoft::ReactNative::ComponentView outer(*this);
    outer.UpdateProps(userProps(props));
  }
}

void ComponentView::UpdateProps(const winrt::Microsoft::ReactNative::IComponentProps &props) noexcept {}

const winrt::Microsoft::ReactNative::IComponentProps ComponentView::userProps(
    facebook::react::Props::Shared const &props) noexcept {
  assert(false); // Custom components must provide an override to extract the IComponentProps from the specific Props
                 // object used by the component
  return nullptr;
}

void ComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}

void ComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  // Avoid new-ing up a new AbiComponentState on every state change if we are not a custom component
  if (m_customComponent) {
    // Review is it expected that I need this cast to call overridden methods?
    winrt::Microsoft::ReactNative::ComponentView outer(*this);
    outer.UpdateState(winrt::make<::Microsoft::ReactNative::AbiComponentState>(state));
  }
}

void ComponentView::UpdateState(const winrt::Microsoft::ReactNative::IComponentState &state) noexcept {}

void ComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Review is it expected that I need this cast to call overridden methods?
  winrt::Microsoft::ReactNative::ComponentView outer(*this);
  outer.UpdateLayoutMetrics(
      {{layoutMetrics.frame.origin.x,
        layoutMetrics.frame.origin.y,
        layoutMetrics.frame.size.width,
        layoutMetrics.frame.size.height},
       layoutMetrics.pointScaleFactor});
}

void ComponentView::UpdateLayoutMetrics(LayoutMetrics metrics) noexcept {}

void ComponentView::FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {}

void ComponentView::prepareForRecycle() noexcept {}

facebook::react::Props::Shared ComponentView::props() noexcept {
  assert(false);
  return {};
}

void ComponentView::HandleCommand(
    winrt::hstring commandName,
    const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept {}

winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *
ComponentView::rootComponentView() noexcept {
  assert(false);
  return nullptr;
}

void ComponentView::parent(const winrt::Microsoft::ReactNative::ComponentView &parent) noexcept {
  assert(false);
}

winrt::Microsoft::ReactNative::ComponentView ComponentView::Parent() const noexcept {
  return m_parent;
}

winrt::IVectorView<winrt::Microsoft::ReactNative::ComponentView> ComponentView::Children() const noexcept {
  return m_children.GetView();
}

void ComponentView::theme(winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme) noexcept {
  assert(false);
}

winrt::Microsoft::ReactNative::Composition::implementation::Theme *ComponentView::theme() const noexcept {
  assert(false);
  return nullptr;
}

void ComponentView::onThemeChanged() noexcept {}

// Run fn on all children of this node until fn returns true
// returns true if the fn ever returned true
bool ComponentView::runOnChildren(
    bool forward,
    Mso::Functor<bool(winrt::Microsoft::ReactNative::implementation::ComponentView &)> &fn) noexcept {
  if (forward) {
    for (auto it = m_children.begin(); it != m_children.end(); ++it) {
      if (fn(*winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(*it)))
        return true;
    }
  } else {
    // TODO is this conversion from rend correct?
    for (auto it = m_children.end(); it != m_children.begin(); --it) {
      if (fn(*winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(*it)))
        return true;
    }
  }
  return false;
}

RECT ComponentView::getClientRect() const noexcept {
  assert(false);
  return {};
}

// The offset from this elements parent to its children (accounts for things like scroll position)
facebook::react::Point ComponentView::getClientOffset() const noexcept {
  assert(false);
  return {};
}

void ComponentView::onFocusLost() noexcept {}

void ComponentView::onFocusGained() noexcept {}

void ComponentView::OnPointerEntered(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {}

void ComponentView::OnPointerExited(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {}

void ComponentView::OnPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnPointerPressed(args);
  }
}

void ComponentView::OnPointerReleased(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnPointerReleased(args);
  }
}

void ComponentView::OnPointerMoved(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnPointerMoved(args);
  }
}

void ComponentView::OnPointerWheelChanged(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnPointerWheelChanged(
        args);
  }
}

void ComponentView::OnPointerCaptureLost() noexcept {}

void ComponentView::OnKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnKeyDown(source, args);
  }
}

void ComponentView::OnKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnKeyUp(source, args);
  }
}

void ComponentView::OnCharacterReceived(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept {
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnCharacterReceived(
        source, args);
  }
}

bool ComponentView::focusable() const noexcept {
  return false;
}

facebook::react::SharedViewEventEmitter ComponentView::eventEmitterAtPoint(facebook::react::Point pt) noexcept {
  return nullptr;
}

facebook::react::SharedViewEventEmitter ComponentView::eventEmitter() noexcept {
  return nullptr;
}

facebook::react::Tag ComponentView::Tag() const noexcept {
  return m_tag;
}

// By default, hitTests according the pointerEvents prop on the Component.
// If ignorePointerEvents = true, all Components are treated as valid targets
facebook::react::Tag ComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  return -1;
}

winrt::IInspectable ComponentView::EnsureUiaProvider() noexcept {
  assert(false);
  return nullptr;
}

std::optional<std::string> ComponentView::getAcccessiblityValue() noexcept {
  return std::nullopt;
}

void ComponentView::setAcccessiblityValue(std::string &&value) noexcept {
  assert(false);
}

bool ComponentView::getAcccessiblityIsReadOnly() noexcept {
  return false;
}

// Notify up the tree to bring the rect into view by scrolling as needed
void ComponentView::StartBringIntoView(BringIntoViewOptions &&args) noexcept {
  assert(false);
}

winrt::Microsoft::ReactNative::IReactContext ComponentView::ReactContext() const noexcept {
  return m_reactContext.Handle();
}

} // namespace winrt::Microsoft::ReactNative::implementation
