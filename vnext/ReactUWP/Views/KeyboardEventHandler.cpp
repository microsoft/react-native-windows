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
struct json_type_traits<react::uwp::KeyboardEvent>
{
  static react::uwp::KeyboardEvent parseJson(const folly::dynamic& json)
  {
    react::uwp::KeyboardEvent ev;

    for (auto const& pair : json.items())
    {
      const std::string& propertyName = pair.first.getString();
      const folly::dynamic& propertyValue = pair.second;

      if (propertyName == ALT_KEY)
        ev.altKey = propertyValue.asBool();
      else if (propertyName == SHIFT_KEY)
        ev.shiftKey = propertyValue.asBool();
      else if (propertyName == CTRL_KEY)
        ev.ctrlKey = propertyValue.asBool();
      else if (propertyName == META_KEY)
        ev.metaKey = propertyValue.asBool();
      else if (propertyName == KEY)
        ev.key = propertyValue.asString();
      else if (propertyName == EVENT_PHASE)
        ev.handledEventPhase = asEnum<HandledEventPhase>(propertyValue);
    }
    return ev;
  }
};

namespace react {namespace uwp {

std::vector<KeyboardEvent> KeyboardHelper::FromJS(folly::dynamic const& obj)
{
  return json_type_traits<std::vector<KeyboardEvent>>::parseJson(obj);
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

KeyboardEventBaseHandler::KeyboardEventBaseHandler(KeyboardEventCallback keyDown, KeyboardEventCallback keyUp)
  :m_keyDownCallback(keyDown), m_keyUpCallback(keyUp)
{
}

PreviewKeyboardEventHandler::PreviewKeyboardEventHandler(KeyboardEventCallback keyDown, KeyboardEventCallback keyUp)
  : KeyboardEventBaseHandler(keyDown, keyUp)
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

KeyboardEventHandler::KeyboardEventHandler(KeyboardEventCallback keyDown, KeyboardEventCallback keyUp)
  :KeyboardEventBaseHandler(keyDown, keyUp)
{
}

void KeyboardEventHandler::hook(XamlView xamlView)
{
  auto uiElement = xamlView.as<winrt::UIElement>();
  if (m_keyDownCallback)
    m_KeyDownRevoker = uiElement.KeyDown(winrt::auto_revoke, m_keyDownCallback);

  if (m_keyUpCallback)
    m_KeyUpRevoker = uiElement.KeyUp(winrt::auto_revoke, m_keyUpCallback);
}

void KeyboardEventHandler::unhook()
{
  m_KeyDownRevoker.revoke();
  m_KeyUpRevoker.revoke();
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

inline bool IsModifiedKeyPressed(winrt::CoreWindow const& coreWindow, winrt::VirtualKey vk)
{
  return (coreWindow.GetKeyState(vk) & winrt::CoreVirtualKeyStates::Down) == winrt::CoreVirtualKeyStates::Down;
}

inline bool IsModifiedKeyLocked(winrt::CoreWindow const& coreWindow, winrt::VirtualKey vk)
{
  return (coreWindow.GetKeyState(vk) & winrt::CoreVirtualKeyStates::Locked) == winrt::CoreVirtualKeyStates::Locked;
}

template<typename T> void UpdateModifiedKeyStatusTo(T& ev)
{
  auto const& coreWindow = winrt::CoreWindow::GetForCurrentThread();
  ev.altKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Menu);
  ev.shiftKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Shift);
  ev.metaKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::LeftWindows)
    || IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::RightWindows);
  ev.ctrlKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Control);
  ev.capLocked = IsModifiedKeyLocked(coreWindow, winrt::VirtualKey::CapitalLock);
};

void PreviewKeyboardEventHandlerOnRoot::DispatchEventToJs(string eventName, winrt::KeyRoutedEventArgs args)
{
  if (auto instance = m_wkReactInstance.lock())
  {
    if (auto source = args.OriginalSource().try_as<winrt::FrameworkElement>())
    {
      auto reactId = getReactId(instance.operator->(), source);
      if (reactId.isValid)
      {
        ReactKeyboardEvent ev;
        ev.target = reactId.tag;
        UpdateModifiedKeyStatusTo(ev);
        ev.key = KeyboardHelper::FromVirutalKey(args.Key(), ev.shiftKey, ev.capLocked);
        instance->DispatchEvent(ev.target, eventName, ToEventData(ev));
      }
    }
  }
}

KeyboardEvent KeyboardHelper::CreateKeyboardEvent(HandledEventPhase phase, winrt::KeyRoutedEventArgs const& args)
{
  KeyboardEvent ev;
  ev.handledEventPhase = phase;
  UpdateModifiedKeyStatusTo(ev);
  ev.key = KeyboardHelper::FromVirutalKey(args.Key(), ev.shiftKey, ev.capLocked);

  return ev;
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

string KeyboardHelper::FromVirutalKey(winrt::VirtualKey virtualKey, bool shiftDown, bool capLocked)
{
  char key = (char)virtualKey;

  if (!isalnum(key))
  {
    auto it = g_virtualKeyToString.find(virtualKey);
    if (it == g_virtualKeyToString.end())
      return "Unidentified";
    else
      return it->second;
  }

  // https://docs.microsoft.com/en-us/uwp/api/windows.system.virtualkey
  // Virtual Keys for 0-9 and A-Z, they're just aligned to their ASCII representation (in uppercase, for the alphabet VKs)
  // Windows doesn't define key for a-z, convert it lower if shift is down.
  if (isalpha(key) && ((!shiftDown && !capLocked) || (shiftDown && capLocked)))
      key = (char)tolower(key);

  return string(1, key);
}

}}
