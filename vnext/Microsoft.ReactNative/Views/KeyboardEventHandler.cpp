// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Xaml.Input.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include "Utils/Helpers.h"
#include "Utils/KeyboardUtils.h"
#include "Utils/PropertyHandlerUtils.h"
#include "Views/KeyboardEventHandler.h"
#include "XamlHelper.h"

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Input.h>
#endif

using namespace std::placeholders;

static constexpr auto ALT_KEY = "altKey";
static constexpr auto CTRL_KEY = "ctrlKey";
static constexpr auto META_KEY = "metaKey";
static constexpr auto SHIFT_KEY = "shiftKey";
static constexpr auto EVENT_PHASE = "handledEventPhase";
static constexpr auto KEY = "key";
static constexpr auto TARGET = "target";
static constexpr auto CODE = "code";
static constexpr auto TIMESTAMP = "timestamp";

template <>
struct json_type_traits<Microsoft::ReactNative::HandledKeyboardEvent> {
  static Microsoft::ReactNative::HandledKeyboardEvent parseJson(const winrt::Microsoft::ReactNative::JSValue &json) {
    Microsoft::ReactNative::HandledKeyboardEvent event;

    for (auto const &pair : json.AsObject()) {
      const std::string &propertyName = pair.first;
      const auto &propertyValue = pair.second;

      if (propertyName == ALT_KEY)
        event.altKey = propertyValue.AsBoolean();
      else if (propertyName == SHIFT_KEY)
        event.shiftKey = propertyValue.AsBoolean();
      else if (propertyName == CTRL_KEY)
        event.ctrlKey = propertyValue.AsBoolean();
      else if (propertyName == META_KEY)
        event.metaKey = propertyValue.AsBoolean();
      else if (propertyName == CODE)
        event.code = propertyValue.AsString();
      else if (propertyName == EVENT_PHASE)
        event.handledEventPhase =
            Microsoft::ReactNative::asEnum<Microsoft::ReactNative::HandledEventPhase>(propertyValue);
    }

    return event;
  }
};

namespace Microsoft::ReactNative {

std::vector<HandledKeyboardEvent> KeyboardHelper::FromJS(winrt::Microsoft::ReactNative::JSValue const &obj) {
  if (obj.Type() == winrt::Microsoft::ReactNative::JSValueType::Array) {
    return json_type_traits<std::vector<HandledKeyboardEvent>>::parseJson(obj);
  }
  return std::vector<HandledKeyboardEvent>{};
}

static folly::dynamic ToEventData(ReactKeyboardEvent event, double timestamp) {
  return folly::dynamic::object(TARGET, event.target)(ALT_KEY, event.altKey)(CTRL_KEY, event.ctrlKey)(KEY, event.key)(
      META_KEY, event.metaKey)(SHIFT_KEY, event.shiftKey)(CODE, event.code)(TIMESTAMP, timestamp);
}

KeyboardEventBaseHandler::KeyboardEventBaseHandler(KeyboardEventCallback &&keyDown, KeyboardEventCallback &&keyUp)
    : m_keyDownCallback(std::move(keyDown)), m_keyUpCallback(std::move(keyUp)) {}

PreviewKeyboardEventHandler::PreviewKeyboardEventHandler(KeyboardEventCallback &&keyDown, KeyboardEventCallback &&keyUp)
    : KeyboardEventBaseHandler(std::move(keyDown), std::move(keyUp)) {}

void PreviewKeyboardEventHandler::hook(XamlView xamlView) {
  auto uiElement = xamlView.as<xaml::UIElement>();
  if (uiElement.try_as<xaml::IUIElement7>()) {
    if (m_keyDownCallback)
      m_previewKeyDownRevoker = uiElement.PreviewKeyDown(winrt::auto_revoke, m_keyDownCallback);

    if (m_keyUpCallback)
      m_previewKeyUpRevoker = uiElement.PreviewKeyUp(winrt::auto_revoke, m_keyUpCallback);
  }
}

void PreviewKeyboardEventHandler::unhook() {
  m_previewKeyDownRevoker.revoke();
  m_previewKeyUpRevoker.revoke();
}

KeyboardEventHandler::KeyboardEventHandler(KeyboardEventCallback &&keyDown, KeyboardEventCallback &&keyUp)
    : KeyboardEventBaseHandler(std::move(keyDown), std::move(keyUp)) {}

void KeyboardEventHandler::hook(XamlView xamlView) {
  auto uiElement = xamlView.as<xaml::UIElement>();
  if (m_keyDownCallback)
    m_keyDownRevoker = uiElement.KeyDown(winrt::auto_revoke, m_keyDownCallback);

  if (m_keyUpCallback)
    m_keyUpRevoker = uiElement.KeyUp(winrt::auto_revoke, m_keyUpCallback);
}

void KeyboardEventHandler::unhook() {
  m_keyDownRevoker.revoke();
  m_keyUpRevoker.revoke();
}

PreviewKeyboardEventHandlerOnRoot::PreviewKeyboardEventHandlerOnRoot(const Mso::React::IReactContext &context)
    : PreviewKeyboardEventHandler(
          std::bind(&PreviewKeyboardEventHandlerOnRoot::OnPreKeyDown, this, _1, _2),
          std::bind(&PreviewKeyboardEventHandlerOnRoot::OnPreKeyUp, this, _1, _2)),
      m_context(&context) {}

void PreviewKeyboardEventHandlerOnRoot::OnPreKeyDown(
    winrt::IInspectable const & /*sender*/,
    xaml::Input::KeyRoutedEventArgs const &args) {
  DispatchEventToJs("topKeyDown", args);
}

void PreviewKeyboardEventHandlerOnRoot::OnPreKeyUp(
    winrt::IInspectable const & /*sender*/,
    xaml::Input::KeyRoutedEventArgs const &args) {
  DispatchEventToJs("topKeyUp", args);
}

HandledKeyboardEventHandler::HandledKeyboardEventHandler() {}

void HandledKeyboardEventHandler::UpdateHandledKeyboardEvents(
    std::string const &propertyName,
    winrt::Microsoft::ReactNative::JSValue const &value) {
  if (propertyName == "keyDownEvents") {
    m_handledKeyDownKeyboardEvents = KeyboardHelper::FromJS(value);
  } else if (propertyName == "keyUpEvents")
    m_handledKeyUpKeyboardEvents = KeyboardHelper::FromJS(value);
}

void HandledKeyboardEventHandler::hook(XamlView xamlView) {
  unhook();

  EnsureKeyboardEventHandler();
  m_previewKeyboardEventHandler->hook(xamlView);
  m_keyboardEventHandler->hook(xamlView);
}

void HandledKeyboardEventHandler::unhook() {
  if (m_previewKeyboardEventHandler)
    m_previewKeyboardEventHandler->unhook();
  if (m_keyboardEventHandler)
    m_keyboardEventHandler->unhook();
}

void HandledKeyboardEventHandler::EnsureKeyboardEventHandler() {
  if (!m_previewKeyboardEventHandler) {
    m_previewKeyboardEventHandler = make_unique<PreviewKeyboardEventHandler>(
        std::bind(
            &HandledKeyboardEventHandler::KeyboardEventHandledHandler,
            this,
            KeyboardEventPhase::PreviewKeyDown,
            _1,
            _2),
        std::bind(
            &HandledKeyboardEventHandler::KeyboardEventHandledHandler, this, KeyboardEventPhase::PreviewKeyUp, _1, _2));
  }

  if (!m_keyboardEventHandler) {
    m_keyboardEventHandler = make_unique<KeyboardEventHandler>(
        std::bind(&HandledKeyboardEventHandler::KeyboardEventHandledHandler, this, KeyboardEventPhase::KeyDown, _1, _2),
        std::bind(&HandledKeyboardEventHandler::KeyboardEventHandledHandler, this, KeyboardEventPhase::KeyUp, _1, _2));
  }
}

void HandledKeyboardEventHandler::KeyboardEventHandledHandler(
    KeyboardEventPhase phase,
    winrt::IInspectable const & /*sender*/,
    xaml::Input::KeyRoutedEventArgs const &args) {
  HandledEventPhase currentEventPhase =
      (phase == KeyboardEventPhase::PreviewKeyUp || phase == KeyboardEventPhase::PreviewKeyDown)
      ? HandledEventPhase::Capturing
      : HandledEventPhase::Bubbling;

  auto event = KeyboardHelper::CreateKeyboardEvent(currentEventPhase, args);

  bool shouldMarkHandled = false;
  if (phase == KeyboardEventPhase::PreviewKeyDown || phase == KeyboardEventPhase::KeyDown)
    shouldMarkHandled = KeyboardHelper::ShouldMarkKeyboardHandled(m_handledKeyDownKeyboardEvents, event);
  else
    shouldMarkHandled = KeyboardHelper::ShouldMarkKeyboardHandled(m_handledKeyUpKeyboardEvents, event);

  if (shouldMarkHandled)
    args.Handled(true);
}

/* static */ bool KeyboardHelper::ShouldMarkKeyboardHandled(
    std::vector<HandledKeyboardEvent> const &handledEvents,
    HandledKeyboardEvent currentEvent) {
  for (auto const &event : handledEvents) {
    if (event.code == currentEvent.code && (event.altKey == currentEvent.altKey) &&
        (event.ctrlKey == currentEvent.ctrlKey) && (event.shiftKey == currentEvent.shiftKey) &&
        (event.metaKey == currentEvent.metaKey) && event.handledEventPhase == currentEvent.handledEventPhase)
      return true;
  }
  return false;
}

template <typename T>
void UpdateModifiedKeyStatusTo(T &event) {
  auto const &coreWindow = winrt::CoreWindow::GetForCurrentThread();
  event.altKey = IsModifiedKeyPressed(coreWindow, winrt::Windows::System::VirtualKey::Menu);
  event.shiftKey = IsModifiedKeyPressed(coreWindow, winrt::Windows::System::VirtualKey::Shift);
  event.metaKey = IsModifiedKeyPressed(coreWindow, winrt::Windows::System::VirtualKey::LeftWindows) ||
      IsModifiedKeyPressed(coreWindow, winrt::Windows::System::VirtualKey::RightWindows);
  event.ctrlKey = IsModifiedKeyPressed(coreWindow, winrt::Windows::System::VirtualKey::Control);
  event.capLocked = KeyboardHelper::IsModifiedKeyLocked(coreWindow, winrt::Windows::System::VirtualKey::CapitalLock);
};

void PreviewKeyboardEventHandlerOnRoot::DispatchEventToJs(
    std::string &&eventName,
    xaml::Input::KeyRoutedEventArgs const &args) {
  const auto timestamp =
      std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
  if (auto source = args.OriginalSource().try_as<xaml::FrameworkElement>()) {
    auto reactId = getViewId(*m_context, source);
    if (reactId.isValid) {
      ReactKeyboardEvent event;
      event.target = reactId.tag;
      UpdateModifiedKeyStatusTo(event);
      event.key = FromVirtualKey(args.Key(), event.shiftKey, event.capLocked);
      event.code = CodeFromVirtualKey(args.OriginalKey());

      m_context->DispatchEvent(event.target, std::move(eventName), ToEventData(event, timestamp));
    }
  }
}

HandledKeyboardEvent KeyboardHelper::CreateKeyboardEvent(
    HandledEventPhase phase,
    xaml::Input::KeyRoutedEventArgs const &args) {
  HandledKeyboardEvent event;
  event.handledEventPhase = phase;
  UpdateModifiedKeyStatusTo(event);
  event.code = CodeFromVirtualKey(args.OriginalKey());

  return event;
}

bool KeyboardHelper::IsModifiedKeyLocked(
    winrt::CoreWindow const &coreWindow,
    winrt::Windows::System::VirtualKey virtualKey) {
#ifdef USE_WINUI3
  auto const &keyState = winrt::Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(virtualKey);
#else
  auto const &keyState = coreWindow.GetKeyState(virtualKey);
#endif // USE_WINUI3
  return (keyState & winrt::CoreVirtualKeyStates::Locked) == winrt::CoreVirtualKeyStates::Locked;
}

} // namespace Microsoft::ReactNative
