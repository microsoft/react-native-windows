// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <optional>
#include <set>

#include <IReactInstance.h>
#include "XamlView.h"

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::System;
using namespace Windows::UI::Core;
} // namespace winrt

namespace react {
namespace uwp {
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

typedef std::function<
    void(winrt::IInspectable const &, winrt::KeyRoutedEventArgs const &)>
    KeyboardEventCallback;

class KeyboardEventBaseHandler {
 public:
  KeyboardEventBaseHandler(
      KeyboardEventCallback &&keyDown,
      KeyboardEventCallback &&keyUp);
  virtual ~KeyboardEventBaseHandler(){};

  virtual void hook(XamlView xamlView) = 0;
  virtual void unhook() = 0;

 protected:
  KeyboardEventCallback m_keyDownCallback;
  KeyboardEventCallback m_keyUpCallback;
};

class PreviewKeyboardEventHandler : public KeyboardEventBaseHandler {
 public:
  PreviewKeyboardEventHandler(
      KeyboardEventCallback &&keyDown,
      KeyboardEventCallback &&keyUp);

  void hook(XamlView xamlView);
  void unhook();

 private:
  winrt::UIElement::PreviewKeyDown_revoker m_previewKeyDownRevoker{};
  winrt::UIElement::PreviewKeyUp_revoker m_previewKeyUpRevoker{};
};

class KeyboardEventHandler : public KeyboardEventBaseHandler {
 public:
  KeyboardEventHandler(
      KeyboardEventCallback &&keyDown,
      KeyboardEventCallback &&keyUp);

  void hook(XamlView xamlView);
  void unhook();

 private:
  winrt::UIElement::KeyDown_revoker m_keyDownRevoker{};
  winrt::UIElement::KeyUp_revoker m_keyUpRevoker{};
};

class PreviewKeyboardEventHandlerOnRoot : public PreviewKeyboardEventHandler {
 public:
  PreviewKeyboardEventHandlerOnRoot(
      const std::weak_ptr<IReactInstance> &reactInstance);

 private:
  void OnPreKeyDown(
      winrt::IInspectable const &sender,
      winrt::KeyRoutedEventArgs const &args);
  void OnPreKeyUp(
      winrt::IInspectable const &sender,
      winrt::KeyRoutedEventArgs const &args);

  void DispatchEventToJs(
      std::string const &name,
      winrt::KeyRoutedEventArgs const &args);
  std::weak_ptr<IReactInstance> m_wkReactInstance;
};

class HandledKeyboardEventHandler {
 public:
  enum class KeyboardEventPhase {
    PreviewKeyUp,
    PreviewKeyDown,
    KeyUp,
    KeyDown
  };

  HandledKeyboardEventHandler();

  void hook(XamlView xamlView);
  void unhook();

 public:
  void UpdateHandledKeyboardEvents(
      std::string const &propertyName,
      folly::dynamic const &value);

 private:
  void EnsureKeyboardEventHandler();

  void KeyboardEventHandledHandler(
      KeyboardEventPhase phase,
      winrt::IInspectable const &sender,
      winrt::KeyRoutedEventArgs const &args);
  bool ShouldMarkKeyboardHandled(
      std::vector<HandledKeyboardEvent> const &handledEvents,
      HandledKeyboardEvent currentEvent);

  std::vector<HandledKeyboardEvent> m_handledKeyUpKeyboardEvents;
  std::vector<HandledKeyboardEvent> m_handledKeyDownKeyboardEvents;

  std::unique_ptr<PreviewKeyboardEventHandler> m_previewKeyboardEventHandler;
  std::unique_ptr<KeyboardEventHandler> m_keyboardEventHandler;
};

struct KeyboardHelper {
  static std::vector<HandledKeyboardEvent> FromJS(folly::dynamic const &obj);
  static HandledKeyboardEvent CreateKeyboardEvent(
      HandledEventPhase phase,
      winrt::KeyRoutedEventArgs const &args);
  static std::string
  FromVirtualKey(winrt::VirtualKey key, bool shiftDown, bool capLocked);
  static std::string CodeFromVirtualKey(winrt::VirtualKey key);
};
} // namespace uwp
} // namespace react
