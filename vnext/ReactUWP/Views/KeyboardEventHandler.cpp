// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "Views/KeyboardEventHandler.h"
#include "Utils/Helpers.h"
#include "Utils/PropertyHandlerUtils.h"
#include <winrt/Windows.UI.Core.h>

using namespace std::placeholders;

static constexpr auto ALT_KEY = "altKey";
static constexpr auto CTRL_KEY = "ctrlKey";
static constexpr auto META_KEY = "metaKey";
static constexpr auto SHIFT_KEY = "shiftKey";
static constexpr auto EVENT_PHASE = "handledEventPhase";
static constexpr auto KEY = "key";
static constexpr auto TARGET = "target";

using namespace react::uwp;

template<>
struct json_type_traits<react::uwp::HandledKeyboardEvent>
{
  static react::uwp::HandledKeyboardEvent parseJson(const folly::dynamic& json)
  {
    react::uwp::HandledKeyboardEvent event;

    for (auto const& pair : json.items())
    {
      const std::string& propertyName = pair.first.getString();
      const folly::dynamic& propertyValue = pair.second;

      if (propertyName == ALT_KEY)
        event.altKey = propertyValue.asBool();
      else if (propertyName == SHIFT_KEY)
        event.shiftKey = propertyValue.asBool();
      else if (propertyName == CTRL_KEY)
        event.ctrlKey = propertyValue.asBool();
      else if (propertyName == META_KEY)
        event.metaKey = propertyValue.asBool();
      else if (propertyName == KEY)
        event.key = propertyValue.asString();
      else if (propertyName == EVENT_PHASE)
        event.handledEventPhase = asEnum<HandledEventPhase>(propertyValue);
    }

    //To simplify the implementaion, key from JS is converted to upper case.
    toUpperInplace(event.key);
    return event;
  }
};

namespace react {namespace uwp {

std::vector<HandledKeyboardEvent> KeyboardHelper::FromJS(folly::dynamic const& obj)
{
  return json_type_traits<std::vector<HandledKeyboardEvent>>::parseJson(obj);
}

static folly::dynamic ToEventData(ReactKeyboardEvent event)
{
  return folly::dynamic::object
    (TARGET, event.target)
    (ALT_KEY, event.altKey)
    (CTRL_KEY, event.ctrlKey)
    (KEY, event.key)
    (META_KEY, event.metaKey)
    (SHIFT_KEY, event.shiftKey);
}

KeyboardEventBaseHandler::KeyboardEventBaseHandler(KeyboardEventCallback&& keyDown, KeyboardEventCallback&& keyUp)
  :m_keyDownCallback(std::move(keyDown)), m_keyUpCallback(std::move(keyUp))
{
}

PreviewKeyboardEventHandler::PreviewKeyboardEventHandler(KeyboardEventCallback&& keyDown, KeyboardEventCallback&& keyUp)
  : KeyboardEventBaseHandler(std::move(keyDown), std::move(keyUp))
{
}

void PreviewKeyboardEventHandler::hook(XamlView xamlView)
{
  auto uiElement = xamlView.as<winrt::UIElement>();
  if (m_keyDownCallback)
    m_previewKeyDownRevoker = uiElement.PreviewKeyDown(winrt::auto_revoke, m_keyDownCallback);

  if (m_keyUpCallback)
    m_previewKeyUpRevoker = uiElement.PreviewKeyUp(winrt::auto_revoke, m_keyUpCallback);
}

void PreviewKeyboardEventHandler::unhook()
{
  m_previewKeyDownRevoker.revoke();
  m_previewKeyUpRevoker.revoke();
}

KeyboardEventHandler::KeyboardEventHandler(KeyboardEventCallback&& keyDown, KeyboardEventCallback&& keyUp)
  :KeyboardEventBaseHandler(std::move(keyDown), std::move(keyUp))
{
}

void KeyboardEventHandler::hook(XamlView xamlView)
{
  auto uiElement = xamlView.as<winrt::UIElement>();
  if (m_keyDownCallback)
    m_keyDownRevoker = uiElement.KeyDown(winrt::auto_revoke, m_keyDownCallback);

  if (m_keyUpCallback)
    m_keyUpRevoker = uiElement.KeyUp(winrt::auto_revoke, m_keyUpCallback);
}

void KeyboardEventHandler::unhook()
{
  m_keyDownRevoker.revoke();
  m_keyUpRevoker.revoke();
}

PreviewKeyboardEventHandlerOnRoot::PreviewKeyboardEventHandlerOnRoot(const std::weak_ptr<IReactInstance>& reactInstance)
  :PreviewKeyboardEventHandler(
    std::bind(&PreviewKeyboardEventHandlerOnRoot::OnPreKeyDown, this, _1, _2),
    std::bind(&PreviewKeyboardEventHandlerOnRoot::OnPreKeyUp, this, _1, _2)),
  m_wkReactInstance(reactInstance)
{
}

void PreviewKeyboardEventHandlerOnRoot::OnPreKeyDown(winrt::IInspectable const& sender, winrt::KeyRoutedEventArgs const&args)
{
  DispatchEventToJs("topKeyDown", args);
}

void PreviewKeyboardEventHandlerOnRoot::OnPreKeyUp(winrt::IInspectable const& sender, winrt::KeyRoutedEventArgs const&args)
{
  DispatchEventToJs("topKeyUp", args);
}

HandledKeyboardEventHandler::HandledKeyboardEventHandler()
{
}

void HandledKeyboardEventHandler::UpdateHandledKeyboardEvents(string const& propertyName, folly::dynamic const& value)
{
  if (propertyName == "keyDownEvents") {
    m_handledKeyDownKeyboardEvents = KeyboardHelper::FromJS(value);
  }
  else if (propertyName == "keyUpEvents")
    m_handledKeyUpKeyboardEvents = KeyboardHelper::FromJS(value);
}

void HandledKeyboardEventHandler::hook(XamlView xamlView)
{
  unhook();

  EnsureKeyboardEventHandler();
  m_previewKeyboardEventHandler->hook(xamlView);
  m_keyboardEventHandler->hook(xamlView);
}

void HandledKeyboardEventHandler::unhook()
{
  if (m_previewKeyboardEventHandler)
    m_previewKeyboardEventHandler->unhook();
  if (m_keyboardEventHandler)
    m_keyboardEventHandler->unhook();
}

void HandledKeyboardEventHandler::EnsureKeyboardEventHandler()
{
  if (!m_previewKeyboardEventHandler)
  {
    m_previewKeyboardEventHandler = make_unique<PreviewKeyboardEventHandler>(
      std::bind(&HandledKeyboardEventHandler::KeyboardEventHandledHandler, this, KeyboardEventPhase::PreviewKeyDown, _1, _2),
      std::bind(&HandledKeyboardEventHandler::KeyboardEventHandledHandler, this, KeyboardEventPhase::PreviewKeyUp, _1, _2)
      );
  }

  if (!m_keyboardEventHandler)
  {
    m_keyboardEventHandler = make_unique<KeyboardEventHandler>(
      std::bind(&HandledKeyboardEventHandler::KeyboardEventHandledHandler, this, KeyboardEventPhase::KeyDown, _1, _2),
      std::bind(&HandledKeyboardEventHandler::KeyboardEventHandledHandler, this, KeyboardEventPhase::KeyUp, _1, _2)
      );
  }
}

void HandledKeyboardEventHandler::KeyboardEventHandledHandler(KeyboardEventPhase phase, winrt::IInspectable const& sender, winrt::KeyRoutedEventArgs const& args)
{
  HandledEventPhase currentEventPhase = (phase == KeyboardEventPhase::PreviewKeyUp || phase == KeyboardEventPhase::PreviewKeyDown)
    ? HandledEventPhase::Capturing : HandledEventPhase::Bubbling;

  auto event = KeyboardHelper::CreateKeyboardEvent(currentEventPhase, args);

  bool shouldMarkHandled = false;
  if (phase == KeyboardEventPhase::PreviewKeyDown || phase == KeyboardEventPhase::KeyDown)
    shouldMarkHandled = ShouldMarkKeyboardHandled(m_handledKeyDownKeyboardEvents, event);
  else
    shouldMarkHandled = ShouldMarkKeyboardHandled(m_handledKeyUpKeyboardEvents, event);

  if (shouldMarkHandled)
    args.Handled(true);
}

bool HandledKeyboardEventHandler::ShouldMarkKeyboardHandled(std::vector<HandledKeyboardEvent> const& handledEvents, HandledKeyboardEvent currentEvent)
{
  for (auto const& event : handledEvents)
  {
    if (event.key == currentEvent.key &&
      (event.altKey == currentEvent.altKey) &&
      (event.ctrlKey == currentEvent.ctrlKey) &&
      (event.shiftKey == currentEvent.shiftKey) &&
      (event.metaKey == currentEvent.metaKey) &&
      event.handledEventPhase == currentEvent.handledEventPhase)
      return true;
  }
  return false;
}

inline bool IsModifiedKeyPressed(winrt::CoreWindow const& coreWindow, winrt::VirtualKey virtualKey)
{
  return (coreWindow.GetKeyState(virtualKey) & winrt::CoreVirtualKeyStates::Down) == winrt::CoreVirtualKeyStates::Down;
}

inline bool IsModifiedKeyLocked(winrt::CoreWindow const& coreWindow, winrt::VirtualKey virtualKey)
{
  return (coreWindow.GetKeyState(virtualKey) & winrt::CoreVirtualKeyStates::Locked) == winrt::CoreVirtualKeyStates::Locked;
}

template<typename T> void UpdateModifiedKeyStatusTo(T& event)
{
  auto const& coreWindow = winrt::CoreWindow::GetForCurrentThread();
  event.altKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Menu);
  event.shiftKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Shift);
  event.metaKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::LeftWindows)
    || IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::RightWindows);
  event.ctrlKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Control);
  event.capLocked = IsModifiedKeyLocked(coreWindow, winrt::VirtualKey::CapitalLock);
};

void PreviewKeyboardEventHandlerOnRoot::DispatchEventToJs(string const& eventName, winrt::KeyRoutedEventArgs const& args)
{
  if (auto instance = m_wkReactInstance.lock())
  {
    if (auto source = args.OriginalSource().try_as<winrt::FrameworkElement>())
    {
      auto reactId = getViewId(instance.operator->(), source);
      if (reactId.isValid)
      {
        ReactKeyboardEvent event;
        event.target = reactId.tag;
        UpdateModifiedKeyStatusTo(event);
        event.key = KeyboardHelper::FromVirtualKey(args.Key(), event.shiftKey, event.capLocked);
        instance->DispatchEvent(event.target, eventName, ToEventData(event));
      }
    }
  }
}

HandledKeyboardEvent KeyboardHelper::CreateKeyboardEvent(HandledEventPhase phase, winrt::KeyRoutedEventArgs const& args)
{
  HandledKeyboardEvent event;
  event.handledEventPhase = phase;
  UpdateModifiedKeyStatusTo(event);
  event.key = KeyboardHelper::FromVirtualKey(args.Key(), event.shiftKey, event.capLocked);

  return event;
}

// Should align to https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values
static const std::map<winrt::VirtualKey, string> g_virtualKeyToString
{
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

  //Whitespace keys
  {winrt::VirtualKey::Enter, "Enter"},
  {winrt::VirtualKey::Tab, "Tab"},
  {winrt::VirtualKey::Space, " "},

  //Navigation keys
  {winrt::VirtualKey::Down, "ArrowDown"},
  {winrt::VirtualKey::Left, "ArrowLeft"},
  {winrt::VirtualKey::Right, "ArrowRight"},
  {winrt::VirtualKey::Up, "ArrowUp"},
  {winrt::VirtualKey::End, "End"},
  {winrt::VirtualKey::Home, "Home"},
  {winrt::VirtualKey::PageDown, "PageDown"},
  {winrt::VirtualKey::PageUp, "PageUp"},

  //Editing keys
  {winrt::VirtualKey::Back, "Backspace"},
  {winrt::VirtualKey::Clear, "Clear"},
  {winrt::VirtualKey::Delete, "Delete"},
  {winrt::VirtualKey::Insert, "Insert"},

  //UI keys
  {winrt::VirtualKey::Accept, "Accept"},
  {winrt::VirtualKey::Application, "ContextMenu"},
  {winrt::VirtualKey::Escape, "Escape"},
  {winrt::VirtualKey::Execute, "Execute"},
  {winrt::VirtualKey::Help, "Help"},
  {winrt::VirtualKey::Pause, "Pause"},
  {winrt::VirtualKey::Select, "Select"},

  //Device keys
  {winrt::VirtualKey::Snapshot, "PrintScreen"},
  {winrt::VirtualKey::Sleep, "Standby"},

  //Common IME keys
  {winrt::VirtualKey::Convert, "Convert"},
  {winrt::VirtualKey::Final, "FinalMode"},
  {winrt::VirtualKey::ModeChange, "ModeChange"},
  {winrt::VirtualKey::NonConvert, "NonConvert"},

  //Korean keyboards only
  {winrt::VirtualKey::Hangul, "HangulMode"},
  {winrt::VirtualKey::Hanja, "HanjaMode"},
  {winrt::VirtualKey::Junja, "JunjaMode"},

  //Japanese keyboards only
  {winrt::VirtualKey::Kana, "KanaMode"},
  {winrt::VirtualKey::Kanji, "KanjiMode"},

  //Function keys
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

  //Numeric keypad keys
  {winrt::VirtualKey::Decimal, "Decimal"},
  {winrt::VirtualKey::Multiply, "Multiply"},
  {winrt::VirtualKey::Add, "Add"},
  {winrt::VirtualKey::Divide, "Divide"},
  {winrt::VirtualKey::Subtract, "Subtract"},
  {winrt::VirtualKey::Separator, "Separator"},

  {winrt::VirtualKey::Number0, "0"},
  {winrt::VirtualKey::Number1, "1"},
  {winrt::VirtualKey::Number2, "2"},
  {winrt::VirtualKey::Number3, "3"},
  {winrt::VirtualKey::Number4, "4"},
  {winrt::VirtualKey::Number5, "5"},
  {winrt::VirtualKey::Number6, "6"},
  {winrt::VirtualKey::Number7, "7"},
  {winrt::VirtualKey::Number8, "8"},
  {winrt::VirtualKey::Number9, "9"},
  //
};

string KeyboardHelper::FromVirtualKey(winrt::VirtualKey virtualKey, bool shiftDown, bool capLocked)
{
  char key = static_cast<char>(virtualKey);

  if (!isalnum(key))
  {
    auto it = g_virtualKeyToString.find(virtualKey);
    if (it == g_virtualKeyToString.end())
      return "Unidentified";
    else
      return it->second;
  }

  // Customer never receives a-z
  // https://docs.microsoft.com/en-us/uwp/api/windows.system.virtualkey
  // Virtual Keys for 0-9 and A-Z, they're just aligned to their ASCII representation (in uppercase, for the alphabet VKs)
  return string(1, key);
}

}}
