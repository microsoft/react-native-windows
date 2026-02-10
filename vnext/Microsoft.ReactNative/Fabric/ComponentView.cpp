
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ComponentView.h"

#include <Fabric/AbiState.h>
#include "DynamicReader.h"

#include "ComponentView.g.cpp"
#include "CreateAutomationPeerArgs.g.h"
#include "LayoutMetricsChangedArgs.g.cpp"
#include "MountChildComponentViewArgs.g.cpp"
#include "UnmountChildComponentViewArgs.g.cpp"
#include <Fabric/Composition/RootComponentView.h>
#include "AbiEventEmitter.h"
#include "AbiShadowNode.h"
#include "ReactCoreInjection.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {
struct RootComponentView;
}

namespace winrt::Microsoft::ReactNative::implementation {

ComponentView::ComponentView(
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder *builder)
    : m_tag(tag), m_reactContext(reactContext) {
  m_builder.copy_from(builder);
}

std::vector<facebook::react::ComponentDescriptorProvider>
ComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

winrt::event_token ComponentView::Destroying(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView> const &handler) noexcept {
  return m_destroyingEvent.add(handler);
}

void ComponentView::Destroying(winrt::event_token const &token) noexcept {
  m_destroyingEvent.remove(token);
}

void ComponentView::onDestroying() noexcept {
  m_destroyingEvent(*this, *this);
}

void ComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  m_children.InsertAt(index, childComponentView);
  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(childComponentView)->parent(*this);
  if (m_builder && m_builder->MountChildComponentViewHandler()) {
    m_builder->MountChildComponentViewHandler()(
        *this, winrt::make<MountChildComponentViewArgs>(childComponentView, index));
  }
  if (m_mounted) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(childComponentView)->onMounted();
  }
}

void ComponentView::onMounted() noexcept {
  assert(!m_mounted);
  m_mounted = true;
  for (auto it = m_children.begin(); it != m_children.end(); ++it) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(*it)->onMounted();
  }
  m_mountedEvent(*this, *this);
}

bool ComponentView::isMounted() noexcept {
  return m_mounted;
}

winrt::event_token ComponentView::Mounted(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView> const &handler) noexcept {
  return m_mountedEvent.add(handler);
}

void ComponentView::Mounted(winrt::event_token const &token) noexcept {
  m_mountedEvent.remove(token);
}

void ComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  if (m_builder && m_builder->UnmountChildComponentViewHandler()) {
    m_builder->UnmountChildComponentViewHandler()(
        *this, winrt::make<UnmountChildComponentViewArgs>(childComponentView, index));
  }
  m_children.RemoveAt(index);
  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(childComponentView)->parent(nullptr);
  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(childComponentView)->onUnmounted();
}

void ComponentView::onUnmounted() noexcept {
  if (!m_mounted)
    return;
  m_mounted = false;
  for (auto it = m_children.begin(); it != m_children.end(); ++it) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(*it)->onUnmounted();
  }
  m_unmountedEvent(*this, *this);
}

winrt::event_token ComponentView::Unmounted(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView> const &handler) noexcept {
  return m_unmountedEvent.add(handler);
}

void ComponentView::Unmounted(winrt::event_token const &token) noexcept {
  m_unmountedEvent.remove(token);
}

MountChildComponentViewArgs::MountChildComponentViewArgs(
    const winrt::Microsoft::ReactNative::ComponentView &child,
    uint32_t index)
    : m_child(child), m_index(index) {}

winrt::Microsoft::ReactNative::ComponentView MountChildComponentViewArgs::Child() const noexcept {
  return m_child;
}

uint32_t MountChildComponentViewArgs::Index() const noexcept {
  return m_index;
}

UnmountChildComponentViewArgs::UnmountChildComponentViewArgs(
    const winrt::Microsoft::ReactNative::ComponentView &child,
    uint32_t index)
    : m_child(child), m_index(index) {}

winrt::Microsoft::ReactNative::ComponentView UnmountChildComponentViewArgs::Child() const noexcept {
  return m_child;
}

uint32_t UnmountChildComponentViewArgs::Index() const noexcept {
  return m_index;
}

void ComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  if (m_builder && m_builder->UpdatePropsHandler()) {
    m_builder->UpdatePropsHandler()(*this, userProps(props), oldProps ? userProps(oldProps) : nullptr);
  }
}

const winrt::Microsoft::ReactNative::IComponentProps ComponentView::userProps(
    facebook::react::Props::Shared const &props) noexcept {
  const auto &abiProps =
      *std::static_pointer_cast<const winrt::Microsoft::ReactNative::implementation::AbiProps>(props);
  return abiProps.UserProps();
}

void ComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {
  if (m_builder && m_builder->UpdateEventEmitterHandler()) {
    m_builder->UpdateEventEmitterHandler()(*this, winrt::make<EventEmitter>(eventEmitter));
  }
}

void ComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  // Avoid new-ing up a new AbiComponentState on every state change if we are not a custom component
  if (m_builder && m_builder->UpdateStateHandler()) {
    m_builder->UpdateStateHandler()(*this, winrt::make<::Microsoft::ReactNative::AbiComponentState>(state));
  }
}

LayoutMetricsChangedArgs::LayoutMetricsChangedArgs(
    const winrt::Microsoft::ReactNative::LayoutMetrics &newLayoutMetrics,
    const winrt::Microsoft::ReactNative::LayoutMetrics &oldLayoutMetrics)
    : m_old(oldLayoutMetrics), m_new(newLayoutMetrics) {}

LayoutMetrics LayoutMetricsChangedArgs::OldLayoutMetrics() const noexcept {
  return m_old;
}

LayoutMetrics LayoutMetricsChangedArgs::NewLayoutMetrics() const noexcept {
  return m_new;
}

void ComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  winrt::Microsoft::ReactNative::LayoutMetrics oldMetrics{
      {oldLayoutMetrics.frame.origin.x,
       oldLayoutMetrics.frame.origin.y,
       oldLayoutMetrics.frame.size.width,
       oldLayoutMetrics.frame.size.height},
      oldLayoutMetrics.pointScaleFactor};
  winrt::Microsoft::ReactNative::LayoutMetrics newMetrics{
      {layoutMetrics.frame.origin.x,
       layoutMetrics.frame.origin.y,
       layoutMetrics.frame.size.width,
       layoutMetrics.frame.size.height},
      layoutMetrics.pointScaleFactor};

  if (m_builder && m_builder->UpdateLayoutMetricsHandler()) {
    m_builder->UpdateLayoutMetricsHandler()(*this, newMetrics, oldMetrics);
  }

  m_layoutMetrics = layoutMetrics;

  m_layoutMetricsChangedEvent(*this, winrt::make<LayoutMetricsChangedArgs>(newMetrics, oldMetrics));
}

LayoutMetrics ComponentView::LayoutMetrics() const noexcept {
  return {
      {m_layoutMetrics.frame.origin.x,
       m_layoutMetrics.frame.origin.y,
       m_layoutMetrics.frame.size.width,
       m_layoutMetrics.frame.size.height},
      m_layoutMetrics.pointScaleFactor};
}

winrt::event_token ComponentView::LayoutMetricsChanged(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::LayoutMetricsChangedArgs> const
        &handler) noexcept {
  return m_layoutMetricsChangedEvent.add(handler);
}

void ComponentView::LayoutMetricsChanged(winrt::event_token const &token) noexcept {
  m_layoutMetricsChangedEvent.remove(token);
}

void ComponentView::FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  if (m_builder && m_builder->FinalizeUpdateHandler()) {
    m_builder->FinalizeUpdateHandler()(*this, updateMask);
  }
}

void ComponentView::prepareForRecycle() noexcept {}

facebook::react::Props::Shared ComponentView::props() noexcept {
  assert(false);
  return {};
}

void ComponentView::HandleCommand(const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept {
  if (m_builder && m_builder->CustomCommandHandler()) {
    m_builder->CustomCommandHandler()(*this, args);
  }
}

HWND ComponentView::GetHwndForParenting() noexcept {
  if (m_parent) {
    return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)
        ->GetHwndForParenting();
  }

  // Fallback if we do not know any more specific HWND
  return reinterpret_cast<HWND>(winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::GetTopLevelWindowId(
      m_reactContext.Properties().Handle()));
}

winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *ComponentView::rootComponentView()
    const noexcept {
  if (m_rootView)
    return m_rootView;

  if (m_parent) {
    m_rootView =
        winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->rootComponentView();
  }

  return m_rootView;
}

void ComponentView::parent(const winrt::Microsoft::ReactNative::ComponentView &parent) noexcept {
  if (m_parent != parent) {
    auto oldRootView = rootComponentView();
    m_rootView = nullptr;
    auto oldParent = m_parent;
    m_parent = parent;
    if (!parent) {
      if (oldRootView && oldRootView->GetFocusedComponent() == *this) {
        oldRootView->TrySetFocusedComponent(
            oldParent,
            winrt::Microsoft::ReactNative::FocusNavigationDirection::None,
            true /*forceNoSelectionIfCannotMove*/);
      }
    }
    if (parent) {
      theme(winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(parent)->theme());
    }
  }
}

winrt::Microsoft::ReactNative::ComponentView ComponentView::Parent() const noexcept {
  return m_parent;
}

winrt::IVectorView<winrt::Microsoft::ReactNative::ComponentView> ComponentView::Children() const noexcept {
  return m_children.GetView();
}

void ComponentView::theme(winrt::Microsoft::ReactNative::Composition::implementation::Theme *value) noexcept {
  if (m_theme != value) {
    for (auto it = m_children.begin(); it != m_children.end(); ++it) {
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(*it)->theme(value);
    }

    m_theme = value;
    onThemeChanged();
  }
}

winrt::Microsoft::ReactNative::Composition::implementation::Theme *ComponentView::theme() const noexcept {
  return m_theme ? m_theme
                 : winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(
                       winrt::Microsoft::ReactNative::Composition::implementation::Theme::EmptyTheme());
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
    if (m_children.Size()) {
      auto it = m_children.end();
      do {
        it--;
        if (fn(*winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(*it)))
          return true;
      } while (it != m_children.begin());
    }
  }
  return false;
}

RECT ComponentView::getClientRect() const noexcept {
  assert(false);
  return {};
}

winrt::Windows::Foundation::Point ComponentView::ScreenToLocal(winrt::Windows::Foundation::Point pt) noexcept {
  return rootComponentView()->ConvertScreenToLocal(pt);
}

winrt::Windows::Foundation::Point ComponentView::LocalToScreen(winrt::Windows::Foundation::Point pt) noexcept {
  return rootComponentView()->ConvertLocalToScreen(pt);
}

// The offset from this elements parent to its children (accounts for things like scroll position)
facebook::react::Point ComponentView::getClientOffset() const noexcept {
  assert(false);
  return {};
}

void ComponentView::onLostFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  m_lostFocusEvent(*this, args);
  if (m_parent) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->onLostFocus(args);
  }
}

void ComponentView::onGotFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  m_gotFocusEvent(*this, args);
  if (m_parent) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->onGotFocus(args);
  }
}

void ComponentView::onLosingFocus(const winrt::Microsoft::ReactNative::LosingFocusEventArgs &args) noexcept {
  m_losingFocusEvent(*this, args);
  if (m_parent) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->onLosingFocus(args);
  }
}

void ComponentView::onGettingFocus(const winrt::Microsoft::ReactNative::GettingFocusEventArgs &args) noexcept {
  m_gettingFocusEvent(*this, args);
  if (m_parent) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->onGettingFocus(args);
  }
}

winrt::event_token ComponentView::LosingFocus(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::LosingFocusEventArgs> const
        &handler) noexcept {
  return m_losingFocusEvent.add(handler);
}

void ComponentView::LosingFocus(winrt::event_token const &token) noexcept {
  m_losingFocusEvent.remove(token);
}

winrt::event_token ComponentView::GettingFocus(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::GettingFocusEventArgs> const
        &handler) noexcept {
  return m_gettingFocusEvent.add(handler);
}

void ComponentView::GettingFocus(winrt::event_token const &token) noexcept {
  m_gettingFocusEvent.remove(token);
}

winrt::event_token ComponentView::LostFocus(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> const
        &handler) noexcept {
  return m_lostFocusEvent.add(handler);
}

void ComponentView::LostFocus(winrt::event_token const &token) noexcept {
  m_lostFocusEvent.remove(token);
}

winrt::event_token ComponentView::GotFocus(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs> const
        &handler) noexcept {
  return m_gotFocusEvent.add(handler);
}

void ComponentView::GotFocus(winrt::event_token const &token) noexcept {
  m_gotFocusEvent.remove(token);
}

bool ComponentView::TryFocus() noexcept {
  if (auto root = rootComponentView()) {
    return root->TrySetFocusedComponent(*get_strong(), winrt::Microsoft::ReactNative::FocusNavigationDirection::None);
  }

  return false;
}

winrt::event_token ComponentView::KeyDown(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs> const &handler) noexcept {
  return m_keyDownEvent.add(handler);
}

void ComponentView::KeyDown(winrt::event_token const &token) noexcept {
  m_keyDownEvent.remove(token);
}

winrt::event_token ComponentView::KeyUp(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs> const &handler) noexcept {
  return m_keyUpEvent.add(handler);
}

void ComponentView::KeyUp(winrt::event_token const &token) noexcept {
  m_keyUpEvent.remove(token);
}

winrt::event_token ComponentView::CharacterReceived(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs> const &handler) noexcept {
  return m_characterReceivedEvent.add(handler);
}

void ComponentView::CharacterReceived(winrt::event_token const &token) noexcept {
  m_characterReceivedEvent.remove(token);
}

winrt::event_token ComponentView::ContextMenuKey(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::ContextMenuKeyEventArgs> const &handler) noexcept {
  return m_contextMenuKeyEvent.add(handler);
}

void ComponentView::ContextMenuKey(winrt::event_token const &token) noexcept {
  m_contextMenuKeyEvent.remove(token);
}

winrt::event_token ComponentView::PointerPressed(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept {
  return m_pointerPressedEvent.add(handler);
}

void ComponentView::PointerPressed(winrt::event_token const &token) noexcept {
  m_pointerPressedEvent.remove(token);
}

winrt::event_token ComponentView::PointerReleased(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept {
  return m_pointerReleasedEvent.add(handler);
}

void ComponentView::PointerReleased(winrt::event_token const &token) noexcept {
  m_pointerReleasedEvent.remove(token);
}

winrt::event_token ComponentView::PointerMoved(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept {
  return m_pointerMovedEvent.add(handler);
}

void ComponentView::PointerMoved(winrt::event_token const &token) noexcept {
  m_pointerMovedEvent.remove(token);
}

winrt::event_token ComponentView::PointerWheelChanged(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept {
  return m_pointerWheelChangedEvent.add(handler);
}

void ComponentView::PointerWheelChanged(winrt::event_token const &token) noexcept {
  m_pointerWheelChangedEvent.remove(token);
}

winrt::event_token ComponentView::PointerEntered(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept {
  return m_pointerEnteredEvent.add(handler);
}

void ComponentView::PointerEntered(winrt::event_token const &token) noexcept {
  m_pointerEnteredEvent.remove(token);
}

winrt::event_token ComponentView::PointerExited(
    winrt::Windows::Foundation::EventHandler<
        winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs> const &handler) noexcept {
  return m_pointerExitedEvent.add(handler);
}

void ComponentView::PointerExited(winrt::event_token const &token) noexcept {
  m_pointerExitedEvent.remove(token);
}

winrt::event_token ComponentView::PointerCaptureLost(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::ComponentView> const &handler) noexcept {
  return m_pointerCaptureLostEvent.add(handler);
}

void ComponentView::PointerCaptureLost(winrt::event_token const &token) noexcept {
  m_pointerCaptureLostEvent.remove(token);
}

void ComponentView::OnPointerEntered(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_pointerEnteredEvent(*this, args);
}

void ComponentView::OnPointerExited(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_pointerExitedEvent(*this, args);
}

void ComponentView::OnPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_pointerPressedEvent(*this, args);
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnPointerPressed(args);
  }
}

void ComponentView::OnPointerReleased(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_pointerReleasedEvent(*this, args);
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnPointerReleased(args);
  }
}

void ComponentView::OnPointerMoved(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_pointerMovedEvent(*this, args);
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnPointerMoved(args);
  }
}

void ComponentView::OnPointerWheelChanged(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  m_pointerWheelChangedEvent(*this, args);
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnPointerWheelChanged(
        args);
  }
}

void ComponentView::OnPointerCaptureLost() noexcept {
  m_pointerCaptureLostEvent(*this, *this);
}

void ComponentView::OnKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  m_keyDownEvent(*this, args);
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnKeyDown(args);
  }
}

void ComponentView::OnKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  m_keyUpEvent(*this, args);
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnKeyUp(args);
  }
}

void ComponentView::OnCharacterReceived(
    const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept {
  m_characterReceivedEvent(*this, args);
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnCharacterReceived(args);
  }
}

void ComponentView::OnContextMenuKey(
    const winrt::Microsoft::ReactNative::Composition::Input::ContextMenuKeyEventArgs &args) noexcept {
  m_contextMenuKeyEvent(*this, args);
  if (m_parent && !args.Handled()) {
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->OnContextMenuKey(args);
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

void ComponentView::UserData(const winrt::IInspectable &userData) noexcept {
  m_userData = userData;
}

winrt::IInspectable ComponentView::UserData() const noexcept {
  return m_userData;
}

// By default, hitTests according the pointerEvents prop on the Component.
// If ignorePointerEvents = true, all Components are treated as valid targets
facebook::react::Tag ComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  return -1;
}

struct CreateAutomationPeerArgs
    : public winrt::Microsoft::ReactNative::implementation::CreateAutomationPeerArgsT<CreateAutomationPeerArgs> {
  CreateAutomationPeerArgs(winrt::Windows::Foundation::IInspectable defaultAutomationPeer)
      : m_defaultAutomationPeer(defaultAutomationPeer) {}

  winrt::Windows::Foundation::IInspectable DefaultAutomationPeer() const noexcept {
    return m_defaultAutomationPeer;
  }

 private:
  winrt::Windows::Foundation::IInspectable m_defaultAutomationPeer;
};

winrt::IInspectable ComponentView::EnsureUiaProvider() noexcept {
  if (m_uiaProvider == nullptr) {
    if (m_builder && m_builder->CreateAutomationPeerHandler()) {
      m_uiaProvider = m_builder->CreateAutomationPeerHandler()(
          *this, winrt::make<CreateAutomationPeerArgs>(CreateAutomationProvider()));
    } else {
      m_uiaProvider = CreateAutomationProvider();
    }
  }
  return m_uiaProvider;
}

winrt::Windows::Foundation::IInspectable ComponentView::CreateAutomationProvider() noexcept {
  return nullptr;
}

std::optional<std::string> ComponentView::getAccessiblityValue() noexcept {
  return std::nullopt;
}

void ComponentView::setAcccessiblityValue(std::string &&value) noexcept {
  assert(false);
}

bool ComponentView::getAcccessiblityIsReadOnly() noexcept {
  return false;
}

ToggleState ComponentView::getToggleState() noexcept {
  return ToggleState::ToggleState_Off;
}

void ComponentView::Toggle() noexcept {
  assert(false);
}

// Notify up the tree to bring the rect into view by scrolling as needed
void ComponentView::StartBringIntoView(BringIntoViewOptions &&args) noexcept {
  assert(false);
}

winrt::Microsoft::ReactNative::IReactContext ComponentView::ReactContext() const noexcept {
  return m_reactContext.Handle();
}

} // namespace winrt::Microsoft::ReactNative::implementation
