// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <IReactInstance.h>
#include <JSValue.h>
#include <React.h>
#include <folly/dynamic.h>
#include <optional>
#include <set>
#include "CppWinRTIncludes.h"
#include "XamlView.h"

namespace winrt {
using namespace Windows::UI::Core;
} // namespace winrt

namespace Microsoft::ReactNative {
enum class HandledEventPhase {
  Capturing = 1, // match the value with EventPhase in React. EventPhase
                 // includes None, Capturing, AtTarget, Bubbling
  Bubbling = 3
};

struct ModifiedKeyState {
  bool altKey{false};
  bool ctrlKey{false};
  bool metaKey{false};
  bool shiftKey{false};
  bool capLocked{false};
};

struct ReactKeyboardEvent : ModifiedKeyState {
  int64_t target{0};
  std::string key{};
  std::string code{};
};

struct HandledKeyboardEvent : ModifiedKeyState {
  HandledEventPhase handledEventPhase{HandledEventPhase::Bubbling};
  std::string code{};
};

typedef std::function<void(winrt::IInspectable const &, xaml::Input::KeyRoutedEventArgs const &)> KeyboardEventCallback;

class KeyboardEventBaseHandler {
 public:
  KeyboardEventBaseHandler(KeyboardEventCallback &&keyDown, KeyboardEventCallback &&keyUp);
  virtual ~KeyboardEventBaseHandler(){};

  virtual void hook(XamlView xamlView) = 0;
  virtual void unhook() = 0;

 protected:
  KeyboardEventCallback m_keyDownCallback;
  KeyboardEventCallback m_keyUpCallback;
};

class PreviewKeyboardEventHandler : public KeyboardEventBaseHandler {
 public:
  PreviewKeyboardEventHandler(KeyboardEventCallback &&keyDown, KeyboardEventCallback &&keyUp);

  void hook(XamlView xamlView);
  void unhook();

 private:
  xaml::UIElement::PreviewKeyDown_revoker m_previewKeyDownRevoker{};
  xaml::UIElement::PreviewKeyUp_revoker m_previewKeyUpRevoker{};
};

class KeyboardEventHandler : public KeyboardEventBaseHandler {
 public:
  KeyboardEventHandler(KeyboardEventCallback &&keyDown, KeyboardEventCallback &&keyUp);

  void hook(XamlView xamlView);
  void unhook();

 private:
  xaml::UIElement::KeyDown_revoker m_keyDownRevoker{};
  xaml::UIElement::KeyUp_revoker m_keyUpRevoker{};
};

class PreviewKeyboardEventHandlerOnRoot : public PreviewKeyboardEventHandler {
 public:
  PreviewKeyboardEventHandlerOnRoot(const Mso::React::IReactContext &context);

 private:
  void OnPreKeyDown(winrt::IInspectable const &sender, xaml::Input::KeyRoutedEventArgs const &args);
  void OnPreKeyUp(winrt::IInspectable const &sender, xaml::Input::KeyRoutedEventArgs const &args);

  void DispatchEventToJs(std::string &&name, xaml::Input::KeyRoutedEventArgs const &args);
  Mso::CntPtr<const Mso::React::IReactContext> m_context;
};

class HandledKeyboardEventHandler {
 public:
  enum class KeyboardEventPhase { PreviewKeyUp, PreviewKeyDown, KeyUp, KeyDown };

  HandledKeyboardEventHandler();

  void hook(XamlView xamlView);
  void unhook();

 public:
  void UpdateHandledKeyboardEvents(
      std::string const &propertyName,
      winrt::Microsoft::ReactNative::JSValue const &value);

 private:
  void EnsureKeyboardEventHandler();

  void KeyboardEventHandledHandler(
      KeyboardEventPhase phase,
      winrt::IInspectable const &sender,
      xaml::Input::KeyRoutedEventArgs const &args);

  std::vector<HandledKeyboardEvent> m_handledKeyUpKeyboardEvents;
  std::vector<HandledKeyboardEvent> m_handledKeyDownKeyboardEvents;

  std::unique_ptr<PreviewKeyboardEventHandler> m_previewKeyboardEventHandler;
  std::unique_ptr<KeyboardEventHandler> m_keyboardEventHandler;
};

struct KeyboardHelper {
  static std::vector<HandledKeyboardEvent> FromJS(winrt::Microsoft::ReactNative::JSValue const &obj);
  static HandledKeyboardEvent CreateKeyboardEvent(HandledEventPhase phase, xaml::Input::KeyRoutedEventArgs const &args);
  static std::string FromVirtualKey(winrt::Windows::System::VirtualKey key, bool shiftDown, bool capLocked);
  static std::string CodeFromVirtualKey(winrt::Windows::System::VirtualKey key);
  static bool IsModifiedKeyPressed(winrt::CoreWindow const &coreWindow, winrt::Windows::System::VirtualKey virtualKey);
  static bool IsModifiedKeyLocked(winrt::CoreWindow const &coreWindow, winrt::Windows::System::VirtualKey virtualKey);
  static bool ShouldMarkKeyboardHandled(
      std::vector<HandledKeyboardEvent> const &handledEvents,
      HandledKeyboardEvent currentEvent);
};
} // namespace Microsoft::ReactNative
