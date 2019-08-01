// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Windows.UI.Core.h>
#include "Utils/Helpers.h"
#include "Utils/PropertyHandlerUtils.h"
#include "Views/KeyboardEventHandler.h"

using namespace std::placeholders;

static constexpr auto ALT_KEY = "altKey";
static constexpr auto CTRL_KEY = "ctrlKey";
static constexpr auto META_KEY = "metaKey";
static constexpr auto SHIFT_KEY = "shiftKey";
static constexpr auto EVENT_PHASE = "handledEventPhase";
static constexpr auto KEY = "key";
static constexpr auto TARGET = "target";
static constexpr auto CODE = "code";

using namespace react::uwp;

template <>
struct json_type_traits<react::uwp::HandledKeyboardEvent> {
  static react::uwp::HandledKeyboardEvent parseJson(
      const folly::dynamic &json) {
    react::uwp::HandledKeyboardEvent event;

    for (auto const &pair : json.items()) {
      const std::string &propertyName = pair.first.getString();
      const folly::dynamic &propertyValue = pair.second;

      if (propertyName == ALT_KEY)
        event.altKey = propertyValue.asBool();
      else if (propertyName == SHIFT_KEY)
        event.shiftKey = propertyValue.asBool();
      else if (propertyName == CTRL_KEY)
        event.ctrlKey = propertyValue.asBool();
      else if (propertyName == META_KEY)
        event.metaKey = propertyValue.asBool();
      else if (propertyName == CODE)
        event.code = propertyValue.asString();
      else if (propertyName == EVENT_PHASE)
        event.handledEventPhase = asEnum<HandledEventPhase>(propertyValue);
    }

    return event;
  }
};

namespace react {
namespace uwp {

std::vector<HandledKeyboardEvent> KeyboardHelper::FromJS(
    folly::dynamic const &obj) {
  return json_type_traits<std::vector<HandledKeyboardEvent>>::parseJson(obj);
}

static folly::dynamic ToEventData(ReactKeyboardEvent event) {
  return folly::dynamic::object(TARGET, event.target)(ALT_KEY, event.altKey)(
      CTRL_KEY, event.ctrlKey)(KEY, event.key)(META_KEY, event.metaKey)(
      SHIFT_KEY, event.shiftKey)(CODE, event.code);
}

KeyboardEventBaseHandler::KeyboardEventBaseHandler(
    KeyboardEventCallback &&keyDown,
    KeyboardEventCallback &&keyUp)
    : m_keyDownCallback(std::move(keyDown)),
      m_keyUpCallback(std::move(keyUp)) {}

PreviewKeyboardEventHandler::PreviewKeyboardEventHandler(
    KeyboardEventCallback &&keyDown,
    KeyboardEventCallback &&keyUp)
    : KeyboardEventBaseHandler(std::move(keyDown), std::move(keyUp)) {}

void PreviewKeyboardEventHandler::hook(XamlView xamlView) {
  auto uiElement = xamlView.as<winrt::UIElement>();
  if (m_keyDownCallback)
    m_previewKeyDownRevoker =
        uiElement.PreviewKeyDown(winrt::auto_revoke, m_keyDownCallback);

  if (m_keyUpCallback)
    m_previewKeyUpRevoker =
        uiElement.PreviewKeyUp(winrt::auto_revoke, m_keyUpCallback);
}

void PreviewKeyboardEventHandler::unhook() {
  m_previewKeyDownRevoker.revoke();
  m_previewKeyUpRevoker.revoke();
}

KeyboardEventHandler::KeyboardEventHandler(
    KeyboardEventCallback &&keyDown,
    KeyboardEventCallback &&keyUp)
    : KeyboardEventBaseHandler(std::move(keyDown), std::move(keyUp)) {}

void KeyboardEventHandler::hook(XamlView xamlView) {
  auto uiElement = xamlView.as<winrt::UIElement>();
  if (m_keyDownCallback)
    m_keyDownRevoker = uiElement.KeyDown(winrt::auto_revoke, m_keyDownCallback);

  if (m_keyUpCallback)
    m_keyUpRevoker = uiElement.KeyUp(winrt::auto_revoke, m_keyUpCallback);
}

void KeyboardEventHandler::unhook() {
  m_keyDownRevoker.revoke();
  m_keyUpRevoker.revoke();
}

PreviewKeyboardEventHandlerOnRoot::PreviewKeyboardEventHandlerOnRoot(
    const std::weak_ptr<IReactInstance> &reactInstance)
    : PreviewKeyboardEventHandler(
          std::bind(
              &PreviewKeyboardEventHandlerOnRoot::OnPreKeyDown,
              this,
              _1,
              _2),
          std::bind(
              &PreviewKeyboardEventHandlerOnRoot::OnPreKeyUp,
              this,
              _1,
              _2)),
      m_wkReactInstance(reactInstance) {}

void PreviewKeyboardEventHandlerOnRoot::OnPreKeyDown(
    winrt::IInspectable const &sender,
    winrt::KeyRoutedEventArgs const &args) {
  DispatchEventToJs("topKeyDown", args);
}

void PreviewKeyboardEventHandlerOnRoot::OnPreKeyUp(
    winrt::IInspectable const &sender,
    winrt::KeyRoutedEventArgs const &args) {
  DispatchEventToJs("topKeyUp", args);
}

HandledKeyboardEventHandler::HandledKeyboardEventHandler() {}

void HandledKeyboardEventHandler::UpdateHandledKeyboardEvents(
    std::string const &propertyName,
    folly::dynamic const &value) {
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
            &HandledKeyboardEventHandler::KeyboardEventHandledHandler,
            this,
            KeyboardEventPhase::PreviewKeyUp,
            _1,
            _2));
  }

  if (!m_keyboardEventHandler) {
    m_keyboardEventHandler = make_unique<KeyboardEventHandler>(
        std::bind(
            &HandledKeyboardEventHandler::KeyboardEventHandledHandler,
            this,
            KeyboardEventPhase::KeyDown,
            _1,
            _2),
        std::bind(
            &HandledKeyboardEventHandler::KeyboardEventHandledHandler,
            this,
            KeyboardEventPhase::KeyUp,
            _1,
            _2));
  }
}

void HandledKeyboardEventHandler::KeyboardEventHandledHandler(
    KeyboardEventPhase phase,
    winrt::IInspectable const &sender,
    winrt::KeyRoutedEventArgs const &args) {
  HandledEventPhase currentEventPhase =
      (phase == KeyboardEventPhase::PreviewKeyUp ||
       phase == KeyboardEventPhase::PreviewKeyDown)
      ? HandledEventPhase::Capturing
      : HandledEventPhase::Bubbling;

  auto event = KeyboardHelper::CreateKeyboardEvent(currentEventPhase, args);

  bool shouldMarkHandled = false;
  if (phase == KeyboardEventPhase::PreviewKeyDown ||
      phase == KeyboardEventPhase::KeyDown)
    shouldMarkHandled =
        ShouldMarkKeyboardHandled(m_handledKeyDownKeyboardEvents, event);
  if (phase == KeyboardEventPhase::PreviewKeyDown ||
      phase == KeyboardEventPhase::KeyDown)
    shouldMarkHandled =
        ShouldMarkKeyboardHandled(m_handledKeyDownKeyboardEvents, event);
  else
    shouldMarkHandled =
        ShouldMarkKeyboardHandled(m_handledKeyUpKeyboardEvents, event);

  if (shouldMarkHandled)
    args.Handled(true);
}

bool HandledKeyboardEventHandler::ShouldMarkKeyboardHandled(
    std::vector<HandledKeyboardEvent> const &handledEvents,
    HandledKeyboardEvent currentEvent) {
  for (auto const &event : handledEvents) {
    if (event.code == currentEvent.code &&
        (event.altKey == currentEvent.altKey) &&
        (event.ctrlKey == currentEvent.ctrlKey) &&
        (event.shiftKey == currentEvent.shiftKey) &&
        (event.metaKey == currentEvent.metaKey) &&
        event.handledEventPhase == currentEvent.handledEventPhase)
      return true;
  }
  return false;
}

template <typename T>
void UpdateModifiedKeyStatusTo(T &event) {
  auto const &coreWindow = winrt::CoreWindow::GetForCurrentThread();
  event.altKey =
      KeyboardHelper::IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Menu);
  event.shiftKey = KeyboardHelper::IsModifiedKeyPressed(
      coreWindow, winrt::VirtualKey::Shift);
  event.metaKey = KeyboardHelper::IsModifiedKeyPressed(
                      coreWindow, winrt::VirtualKey::LeftWindows) ||
      KeyboardHelper::IsModifiedKeyPressed(
                      coreWindow, winrt::VirtualKey::RightWindows);
  event.ctrlKey = KeyboardHelper::IsModifiedKeyPressed(
      coreWindow, winrt::VirtualKey::Control);
  event.capLocked = KeyboardHelper::IsModifiedKeyLocked(
      coreWindow, winrt::VirtualKey::CapitalLock);
};

void PreviewKeyboardEventHandlerOnRoot::DispatchEventToJs(
    std::string const &eventName,
    winrt::KeyRoutedEventArgs const &args) {
  if (auto instance = m_wkReactInstance.lock()) {
    if (auto source = args.OriginalSource().try_as<winrt::FrameworkElement>()) {
      auto reactId = getViewId(instance.operator->(), source);
      if (reactId.isValid) {
        ReactKeyboardEvent event;
        event.target = reactId.tag;
        UpdateModifiedKeyStatusTo(event);
        event.key = KeyboardHelper::FromVirtualKey(
            args.Key(), event.shiftKey, event.capLocked);
        event.code = KeyboardHelper::CodeFromVirtualKey(args.OriginalKey());

        instance->DispatchEvent(event.target, eventName, ToEventData(event));
      }
    }
  }
}

HandledKeyboardEvent KeyboardHelper::CreateKeyboardEvent(
    HandledEventPhase phase,
    winrt::KeyRoutedEventArgs const &args) {
  HandledKeyboardEvent event;
  event.handledEventPhase = phase;
  UpdateModifiedKeyStatusTo(event);
  event.code = KeyboardHelper::CodeFromVirtualKey(args.OriginalKey());

  return event;
}

// Should align to
// https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values
static const std::vector<std::pair<winrt::VirtualKey, std::string>>
    g_virtualKeyToKey{
        // Modifier keys
        {winrt::VirtualKey::LeftMenu, "Alt"},
        {winrt::VirtualKey::RightMenu, "Alt"},
        {winrt::VirtualKey::Menu, "Alt"},
        {winrt::VirtualKey::CapitalLock, "CapsLock"},
        {winrt::VirtualKey::LeftControl, "Control"},
        {winrt::VirtualKey::RightControl, "Control"},
        {winrt::VirtualKey::Control, "Control"},
        {winrt::VirtualKey::LeftWindows, "Meta"},
        {winrt::VirtualKey::RightWindows, "Meta"},
        {winrt::VirtualKey::NumberKeyLock, "NumLock"},
        {winrt::VirtualKey::Scroll, "ScrollLock"},
        {winrt::VirtualKey::LeftShift, "Shift"},
        {winrt::VirtualKey::RightShift, "Shift"},
        {winrt::VirtualKey::Shift, "Shift"},

        // Whitespace keys
        {winrt::VirtualKey::Enter, "Enter"},
        {winrt::VirtualKey::Tab, "Tab"},
        {winrt::VirtualKey::Space, " "},

        // Navigation keys
        {winrt::VirtualKey::Down, "ArrowDown"},
        {winrt::VirtualKey::Left, "ArrowLeft"},
        {winrt::VirtualKey::Right, "ArrowRight"},
        {winrt::VirtualKey::Up, "ArrowUp"},
        {winrt::VirtualKey::End, "End"},
        {winrt::VirtualKey::Home, "Home"},
        {winrt::VirtualKey::PageDown, "PageDown"},
        {winrt::VirtualKey::PageUp, "PageUp"},

        // Editing keys
        {winrt::VirtualKey::Back, "Backspace"},
        {winrt::VirtualKey::Clear, "Clear"},
        {winrt::VirtualKey::Delete, "Delete"},
        {winrt::VirtualKey::Insert, "Insert"},

        // UI keys
        {winrt::VirtualKey::Accept, "Accept"},
        {winrt::VirtualKey::Application, "ContextMenu"},
        {winrt::VirtualKey::Escape, "Escape"},
        {winrt::VirtualKey::Execute, "Execute"},
        {winrt::VirtualKey::Help, "Help"},
        {winrt::VirtualKey::Pause, "Pause"},
        {winrt::VirtualKey::Select, "Select"},

        // Device keys
        {winrt::VirtualKey::Snapshot, "PrintScreen"},
        {winrt::VirtualKey::Sleep, "Standby"},

        // Common IME keys
        {winrt::VirtualKey::Convert, "Convert"},
        {winrt::VirtualKey::Final, "FinalMode"},
        {winrt::VirtualKey::ModeChange, "ModeChange"},
        {winrt::VirtualKey::NonConvert, "NonConvert"},

        // Korean keyboards only
        {winrt::VirtualKey::Hangul, "HangulMode"},
        {winrt::VirtualKey::Hanja, "HanjaMode"},
        {winrt::VirtualKey::Junja, "JunjaMode"},

        // Japanese keyboards only
        {winrt::VirtualKey::Kana, "KanaMode"},
        {winrt::VirtualKey::Kanji, "KanjiMode"},

        // Function keys
        {winrt::VirtualKey::F1, "F1"},
        {winrt::VirtualKey::F2, "F2"},
        {winrt::VirtualKey::F3, "F3"},
        {winrt::VirtualKey::F4, "F4"},
        {winrt::VirtualKey::F5, "F5"},
        {winrt::VirtualKey::F6, "F6"},
        {winrt::VirtualKey::F7, "F7"},
        {winrt::VirtualKey::F8, "F8"},
        {winrt::VirtualKey::F9, "F9"},
        {winrt::VirtualKey::F10, "F10"},
        {winrt::VirtualKey::F11, "F11"},
        {winrt::VirtualKey::F12, "F12"},
        {winrt::VirtualKey::F13, "F13"},
        {winrt::VirtualKey::F14, "F14"},
        {winrt::VirtualKey::F15, "F15"},
        {winrt::VirtualKey::F16, "F16"},
        {winrt::VirtualKey::F17, "F17"},
        {winrt::VirtualKey::F18, "F18"},
        {winrt::VirtualKey::F19, "F19"},
        {winrt::VirtualKey::F20, "F20"},

        // Numeric keypad keys
        {winrt::VirtualKey::Decimal, "Decimal"},
        {winrt::VirtualKey::Multiply, "Multiply"},
        {winrt::VirtualKey::Add, "Add"},
        {winrt::VirtualKey::Divide, "Divide"},
        {winrt::VirtualKey::Subtract, "Subtract"},
        {winrt::VirtualKey::Separator, "Separator"},

        {winrt::VirtualKey::NumberPad0, "0"},
        {winrt::VirtualKey::NumberPad1, "1"},
        {winrt::VirtualKey::NumberPad2, "2"},
        {winrt::VirtualKey::NumberPad3, "3"},
        {winrt::VirtualKey::NumberPad4, "4"},
        {winrt::VirtualKey::NumberPad5, "5"},
        {winrt::VirtualKey::NumberPad6, "6"},
        {winrt::VirtualKey::NumberPad7, "7"},
        {winrt::VirtualKey::NumberPad8, "8"},
        {winrt::VirtualKey::NumberPad9, "9"},
        //
    };

// Convert enum to its underlying type
template <typename E>
constexpr auto to_underlying(E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}

// Align to https://www.w3.org/TR/uievents-code/
// VirtualKey mapping is from
// https://docs.microsoft.com/en-us/uwp/api/Windows.System.VirtualKey Other
// codes are from
// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes All
// codes are listed except alpha and numbers. For the unsupported code, they are
// all commented and they key is 'to_underlying(winrt::VirtualKey::None)' For
// example, IntlBackslash is not sent to user:
// //{to_underlying(winrt::VirtualKey::None), "IntlBackslash"},
static const std::vector<std::pair<int, std::string>> g_virtualKeyToCode{
    // writing system keys in the Alphanumeric section
    {192, "Backquote"},
    {220, "Backslash"},
    {to_underlying(winrt::VirtualKey::Back), "Backspace"},
    {219, "BracketLeft"},
    {221, "BracketRight"},
    {192, "BracketLeft"},
    {188, "Comma"},
    {187, "Equal"},
    //{to_underlying(winrt::VirtualKey::None), "IntlBackslash"},
    //{to_underlying(winrt::VirtualKey::None), "IntlRo"},
    //{to_underlying(winrt::VirtualKey::None), "IntlYen"},
    {189, "Minus"},
    {190, "Period"},
    {222, "Quote"},
    {186, "Semicolon"},
    {191, "Slash"},

    // Functional Keys
    {to_underlying(winrt::VirtualKey::LeftMenu), "AltLeft"},
    {to_underlying(winrt::VirtualKey::RightMenu), "AltRight"},
    {to_underlying(winrt::VirtualKey::CapitalLock), "CapsLock"},
    {to_underlying(winrt::VirtualKey::Menu), "ContextMenu"},
    {to_underlying(winrt::VirtualKey::LeftControl), "ControlLeft"},
    {to_underlying(winrt::VirtualKey::RightControl), "ControlRight"},
    {to_underlying(winrt::VirtualKey::Enter), "Enter"},
    {to_underlying(winrt::VirtualKey::LeftWindows), "MetaLeft"},
    {to_underlying(winrt::VirtualKey::RightWindows), "MetaRight"},
    {to_underlying(winrt::VirtualKey::LeftShift), "ShiftLeft"},
    {to_underlying(winrt::VirtualKey::RightShift), "ShiftRight"},
    {to_underlying(winrt::VirtualKey::Space), "Space"},
    {to_underlying(winrt::VirtualKey::Tab), "Tab"},

    // List of code values for functional keys found on Japanese and Korean
    // keyboards.
    {to_underlying(winrt::VirtualKey::Convert), "Convert"},
    {to_underlying(winrt::VirtualKey::Kana), "KanaMode"},
    {to_underlying(winrt::VirtualKey::Hangul), "Lang1"},
    {to_underlying(winrt::VirtualKey::Hanja), "Lang2"},
    //{to_underlying(winrt::VirtualKey::None), "Lang3"},
    //{to_underlying(winrt::VirtualKey::None), "Lang4"},
    //{to_underlying(winrt::VirtualKey::None), "Lang5"},
    {to_underlying(winrt::VirtualKey::NonConvert), "NonConvert"},

    // Control Pad Section
    {to_underlying(winrt::VirtualKey::Delete), "Delete"},
    {to_underlying(winrt::VirtualKey::End), "End"},
    {to_underlying(winrt::VirtualKey::Help), "Help"},
    {to_underlying(winrt::VirtualKey::Home), "Home"},
    {to_underlying(winrt::VirtualKey::Insert), "Insert"},
    {to_underlying(winrt::VirtualKey::PageDown), "PageDown"},
    {to_underlying(winrt::VirtualKey::PageUp), "PageUp"},

    // Arrow Pad Section
    {to_underlying(winrt::VirtualKey::Down), "ArrowDown"},
    {to_underlying(winrt::VirtualKey::Left), "ArrowLeft"},
    {to_underlying(winrt::VirtualKey::Right), "ArrowRight"},
    {to_underlying(winrt::VirtualKey::Up), "ArrowUp"},

    // Numpad Section
    {to_underlying(winrt::VirtualKey::NumberKeyLock), "NumLock"},
    {to_underlying(winrt::VirtualKey::NumberPad0), "Numpad0"},
    {to_underlying(winrt::VirtualKey::NumberPad1), "Numpad1"},
    {to_underlying(winrt::VirtualKey::NumberPad2), "Numpad2"},
    {to_underlying(winrt::VirtualKey::NumberPad3), "Numpad3"},
    {to_underlying(winrt::VirtualKey::NumberPad4), "Numpad4"},
    {to_underlying(winrt::VirtualKey::NumberPad5), "Numpad5"},
    {to_underlying(winrt::VirtualKey::NumberPad6), "Numpad6"},
    {to_underlying(winrt::VirtualKey::NumberPad7), "Numpad7"},
    {to_underlying(winrt::VirtualKey::NumberPad8), "Numpad8"},
    {to_underlying(winrt::VirtualKey::NumberPad9), "Numpad9"},
    {to_underlying(winrt::VirtualKey::Add), "NumpadAdd"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadBackspace"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadClear"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadClearEntry"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadComma"},
    {to_underlying(winrt::VirtualKey::Decimal), "NumpadDecimal"},
    {to_underlying(winrt::VirtualKey::Divide), "NumpadDivide"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadEnter"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadEqual"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadHash"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadMemoryAdd"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadMemoryClear"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadMemoryRecall"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadMemoryStore"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadMemorySubtract"},
    {to_underlying(winrt::VirtualKey::Multiply), "NumpadMultiply"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadParenLeft"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadParenRight"},
    //{to_underlying(winrt::VirtualKey::None), "NumpadStar"},
    {to_underlying(winrt::VirtualKey::Subtract), "NumpadSubtract"},

    // Function Section
    {to_underlying(winrt::VirtualKey::Escape), "Escape"},
    {to_underlying(winrt::VirtualKey::F1), "F1"},
    {to_underlying(winrt::VirtualKey::F2), "F2"},
    {to_underlying(winrt::VirtualKey::F3), "F3"},
    {to_underlying(winrt::VirtualKey::F4), "F4"},
    {to_underlying(winrt::VirtualKey::F5), "F5"},
    {to_underlying(winrt::VirtualKey::F6), "F6"},
    {to_underlying(winrt::VirtualKey::F7), "F7"},
    {to_underlying(winrt::VirtualKey::F8), "F8"},
    {to_underlying(winrt::VirtualKey::F9), "F9"},
    {to_underlying(winrt::VirtualKey::F10), "F10"},
    {to_underlying(winrt::VirtualKey::F11), "F11"},
    {to_underlying(winrt::VirtualKey::F12), "F12"},
    {to_underlying(winrt::VirtualKey::F13), "F13"},
    {to_underlying(winrt::VirtualKey::F14), "F14"},
    {to_underlying(winrt::VirtualKey::F15), "F15"},
    {to_underlying(winrt::VirtualKey::F16), "F16"},
    {to_underlying(winrt::VirtualKey::F17), "F17"},
    {to_underlying(winrt::VirtualKey::F18), "F18"},
    {to_underlying(winrt::VirtualKey::F19), "F19"},
    {to_underlying(winrt::VirtualKey::F20), "F20"},
    {to_underlying(winrt::VirtualKey::F21), "F21"},
    {to_underlying(winrt::VirtualKey::F22), "F22"},
    {to_underlying(winrt::VirtualKey::F23), "F23"},
    {to_underlying(winrt::VirtualKey::F24), "F24"},
    //{to_underlying(winrt::VirtualKey::None), "Fn"},
    //{to_underlying(winrt::VirtualKey::None), "FnLock"},
    {to_underlying(winrt::VirtualKey::Print), "PrintScreen"},
    {to_underlying(winrt::VirtualKey::Scroll), "ScrollLock"},
    {to_underlying(winrt::VirtualKey::Pause), "Pause"},

    //// Media Keys
    {to_underlying(winrt::VirtualKey::GoBack), "BrowserBack"},
    {to_underlying(winrt::VirtualKey::Favorites), "BrowserFavorites"},
    {to_underlying(winrt::VirtualKey::GoForward), "BrowserForward"},
    {to_underlying(winrt::VirtualKey::GoHome), "BrowserHome"},
    {to_underlying(winrt::VirtualKey::Refresh), "BrowserRefresh"},
    {to_underlying(winrt::VirtualKey::Search), "BrowserSearch"},
    {to_underlying(winrt::VirtualKey::Stop), "BrowserStop"},
    //{to_underlying(winrt::VirtualKey::None), "Eject"},
    {182, "LaunchApp1"},
    {183, "LaunchApp2"},
    {180, "LaunchMail"},
    {179, "MediaPlayPause"},
    {181, "MediaSelect"},
    {178, "MediaStop"},
    {176, "MediaTrackNext"},
    {177, "MediaTrackPrevious"},
    //{to_underlying(winrt::VirtualKey::None), "Power"},
    {to_underlying(winrt::VirtualKey::Sleep), "Sleep"},
    {174, "AudioVolumeDown"},
    {173, "AudioVolumeMute"},
    {175, "AudioVolumeUp"},
    //{to_underlying(winrt::VirtualKey::None), "WakeUp"},

    ////List of code values for legacy modifier keys.
    //{to_underlying(winrt::VirtualKey::None), "Hyper"},
    //{to_underlying(winrt::VirtualKey::None), "Super"},
    //{to_underlying(winrt::VirtualKey::None), "Turbo"},

    ////List of code values for legacy process control keys.
    //{to_underlying(winrt::VirtualKey::None), "Abort"},
    //{to_underlying(winrt::VirtualKey::None), "Resume"},
    //{to_underlying(winrt::VirtualKey::None), "Suspend"},

    ////List of code values for legacy editing keys.
    //{to_underlying(winrt::VirtualKey::None), "Again"},
    //{to_underlying(winrt::VirtualKey::None), "Copy"},
    //{to_underlying(winrt::VirtualKey::None), "Cut"},
    //{to_underlying(winrt::VirtualKey::None), "Find"},
    //{to_underlying(winrt::VirtualKey::None), "Open"},
    //{to_underlying(winrt::VirtualKey::None), "Paste"},
    //{to_underlying(winrt::VirtualKey::None), "Props"},
    {to_underlying(winrt::VirtualKey::Select), "Select"},
    //{to_underlying(winrt::VirtualKey::None), "Undo"},

    // The following keys may be found on non-standard international keyboards.
    //{to_underlying(winrt::VirtualKey::None), "Hiragana"},
    //{to_underlying(winrt::VirtualKey::None), "Katakana"},
    //{to_underlying(winrt::VirtualKey::None), "Undo"},

    // Xbox Gamepad
    {to_underlying(winrt::VirtualKey::GamepadA), "GamepadA"},
    {to_underlying(winrt::VirtualKey::GamepadB), "GamepadB"},
    {to_underlying(winrt::VirtualKey::GamepadX), "GamepadX"},
    {to_underlying(winrt::VirtualKey::GamepadY), "GamepadY"},
    {to_underlying(winrt::VirtualKey::GamepadRightShoulder),
     "GamepadRightShoulder"},
    {to_underlying(winrt::VirtualKey::GamepadLeftShoulder),
     "GamepadLeftShoulder"},
    {to_underlying(winrt::VirtualKey::GamepadLeftTrigger),
     "GamepadLeftTrigger"},
    {to_underlying(winrt::VirtualKey::GamepadRightTrigger),
     "GamepadRightTrigger"},
    {to_underlying(winrt::VirtualKey::GamepadDPadUp), "GamepadDPadUp"},
    {to_underlying(winrt::VirtualKey::GamepadDPadDown), "GamepadDPadDown"},
    {to_underlying(winrt::VirtualKey::GamepadDPadLeft), "GamepadDPadLeft"},
    {to_underlying(winrt::VirtualKey::GamepadDPadRight), "GamepadDPadRight"},
    {to_underlying(winrt::VirtualKey::GamepadMenu), "GamepadMenu"},
    {to_underlying(winrt::VirtualKey::GamepadView), "GamepadView"},
    {to_underlying(winrt::VirtualKey::GamepadLeftThumbstickButton),
     "GamepadLeftThumbstickButton"},
    {to_underlying(winrt::VirtualKey::GamepadRightThumbstickButton),
     "GamepadRightThumbstickButton"},
    {to_underlying(winrt::VirtualKey::GamepadLeftThumbstickUp),
     "GamepadLeftThumbstickUp"},
    {to_underlying(winrt::VirtualKey::GamepadLeftThumbstickDown),
     "GamepadLeftThumbstickDown"},
    {to_underlying(winrt::VirtualKey::GamepadLeftThumbstickRight),
     "GamepadLeftThumbstickRight"},
    {to_underlying(winrt::VirtualKey::GamepadLeftThumbstickLeft),
     "GamepadLeftThumbstickLeft"},
    {to_underlying(winrt::VirtualKey::GamepadRightThumbstickUp),
     "GamepadRightThumbstickUp"},
    {to_underlying(winrt::VirtualKey::GamepadRightThumbstickDown),
     "GamepadRightThumbstickDown"},
    {to_underlying(winrt::VirtualKey::GamepadRightThumbstickRight),
     "GamepadRightThumbstickRight"},
    {to_underlying(winrt::VirtualKey::GamepadRightThumbstickLeft),
     "GamepadRightThumbstickLeft"}};

template <typename T>
static const std::string GetOrUnidentified(
    winrt::Windows::System::VirtualKey virtualKey,
    std::vector<std::pair<T, std::string>> const &vector) {
  for (auto const &pair : vector) {
    if (static_cast<int>(pair.first) == static_cast<int>(virtualKey))
      return pair.second;
  }
  return "Unidentified";
}

std::string KeyboardHelper::FromVirtualKey(
    winrt::VirtualKey virtualKey,
    bool shiftDown,
    bool capLocked) {
  int key = static_cast<int>(virtualKey);

  if (!isupper(key) && !isdigit(key)) {
    return GetOrUnidentified(virtualKey, g_virtualKeyToKey);
  }

  // Customer never receives a-z
  // https://docs.microsoft.com/en-us/uwp/api/windows.system.virtualkey
  // Virtual Keys for 0-9 and A-Z, they're just aligned to their ASCII
  // representation (in uppercase, for the alphabet VKs)
  return std::string(1, static_cast<char>(key));
}

inline winrt::VirtualKey GetLeftOrRightModifiedKey(
    winrt::CoreWindow const &coreWindow,
    winrt::VirtualKey leftKey,
    winrt::VirtualKey rightKey) {
  return KeyboardHelper::IsModifiedKeyPressed(coreWindow, leftKey) ? leftKey
                                                                   : rightKey;
}

std::string KeyboardHelper::CodeFromVirtualKey(winrt::VirtualKey virtualKey) {
  int key = static_cast<int>(virtualKey);

  if (isdigit(key)) {
    return "Digit" + std::string(1, static_cast<char>(key));
  } else if (isupper(key)) {
    return "Key" + std::string(1, static_cast<char>(key));
  } else {
    // Override the virtual key if it's modified key of Control, Shift or Menu
    auto const &coreWindow = winrt::CoreWindow::GetForCurrentThread();
    if (virtualKey == winrt::VirtualKey::Control) {
      virtualKey = GetLeftOrRightModifiedKey(
          coreWindow,
          winrt::VirtualKey::LeftControl,
          winrt::VirtualKey::RightControl);
    } else if (virtualKey == winrt::VirtualKey::Shift) {
      virtualKey = GetLeftOrRightModifiedKey(
          coreWindow,
          winrt::VirtualKey::LeftShift,
          winrt::VirtualKey::RightShift);
    } else if (virtualKey == winrt::VirtualKey::Menu) {
      virtualKey = GetLeftOrRightModifiedKey(
          coreWindow,
          winrt::VirtualKey::LeftMenu,
          winrt::VirtualKey::RightMenu);
    }
  }

  return GetOrUnidentified(virtualKey, g_virtualKeyToCode);
}

bool KeyboardHelper::IsModifiedKeyPressed(
    winrt::CoreWindow const &coreWindow,
    winrt::VirtualKey virtualKey) {
  return (coreWindow.GetKeyState(virtualKey) &
          winrt::CoreVirtualKeyStates::Down) ==
      winrt::CoreVirtualKeyStates::Down;
}

bool KeyboardHelper::IsModifiedKeyLocked(
    winrt::CoreWindow const &coreWindow,
    winrt::VirtualKey virtualKey) {
  return (coreWindow.GetKeyState(virtualKey) &
          winrt::CoreVirtualKeyStates::Locked) ==
      winrt::CoreVirtualKeyStates::Locked;
}

} // namespace uwp
} // namespace react
