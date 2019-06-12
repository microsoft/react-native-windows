#include "pch.h"
#include "KeyboardEventHandler.h"
#include "Helpers.h"
#include "Utils/PropertyHandlerUtils.h"

using namespace std::placeholders;

static constexpr auto ALT_KEY = "altKey";
static constexpr auto CTRL_KEY = "ctrlKey";
static constexpr auto META_KEY = "metaKey";
static constexpr auto SHIFT_KEY = "shiftKey";
static constexpr auto EVENT_PHASE = "eventPhase";
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
        ev.altKey = as<bool>(propertyValue);
      else if (propertyName == SHIFT_KEY)
        ev.shiftKey = as<bool>(propertyValue);
      else if (propertyName == CTRL_KEY)
        ev.ctrlKey = as<bool>(propertyValue);
      else if (propertyName == META_KEY)
        ev.metaKey = as<bool>(propertyValue);
      else if (propertyName == KEY)
        ev.key = KeyboardHelper::ToVirtualKey(as<string>(propertyValue));
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
  //return (coreWindow.GetKeyState(vk) & winrt::CoreVirtualKeyStates::Down) == winrt::CoreVirtualKeyStates::Down;
  return false;
}

template<typename T> void UpdateModifiedKeyStatus(T& ev)
{
  auto const& coreWindow = winrt::CoreWindow::GetForCurrentThread();
  ev.altKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Menu);
  ev.shiftKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Shift);
  ev.metaKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::LeftWindows)
    || IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::RightWindows);
  ev.ctrlKey = IsModifiedKeyPressed(coreWindow, winrt::VirtualKey::Control);
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
        ev.key = KeyboardHelper::FromVirutalKey(args.Key());
        ev.target = reactId.tag;
        UpdateModifiedKeyStatus(ev);
        instance->DispatchEvent(ev.target, eventName, ToEventData(ev));
      }
    }
  }
}

KeyboardEvent KeyboardHelper::CreateKeyboardEvent(EventPhase phase, winrt::KeyRoutedEventArgs const& args)
{
  KeyboardEvent ev;
  ev.eventPhase = phase;
  ev.key = args.Key();
  UpdateModifiedKeyStatus(ev);
  return ev;
}

winrt::VirtualKey KeyboardHelper::ToVirtualKey(string key)
{
  return winrt::VirtualKey::A;
}

string KeyboardHelper::FromVirutalKey(winrt::VirtualKey key)
{
  return "A";
}

}}
