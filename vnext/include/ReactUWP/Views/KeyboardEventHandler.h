// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <optional>
#include <set>
#include <folly/dynamic.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>

#include "XamlView.h"
#include <IReactInstance.h>

namespace winrt {
  using namespace Windows::UI;
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Controls;
  using namespace Windows::UI::Xaml::Input;
  using namespace Windows::Foundation;
  using namespace Windows::UI::Xaml::Media;
  using namespace Windows::System;
  using namespace Windows::UI::Core;
}

using namespace std;

namespace react { namespace uwp {
  enum class HandledEventPhase
  {
    CAPTURING = 1, // match the value with EventPhase in React. EventPhase includes None, Capturing, AtTarget, Bubbling
    BUBBLING = 3
  };

  struct ModifiedKeyStatue
  {
    bool altKey{ false };
    bool ctrlKey{ false };
    bool metaKey{ false };
    bool shiftKey{ false };
    bool capLocked{ false };
  };

  struct ReactKeyboardEvent: ModifiedKeyStatue
  {
    int64_t target = 0;
    string key{};
  };

  struct KeyboardEvent: ModifiedKeyStatue
  {
    HandledEventPhase handledEventPhase{ HandledEventPhase::BUBBLING };
    string key{};
  };

  typedef std::function<void(winrt::IInspectable const&, winrt::KeyRoutedEventArgs const&)> KeyboardEventCallback;

  class KeyboardEventBaseHandler {
  public:
    KeyboardEventBaseHandler(KeyboardEventCallback keyDown, KeyboardEventCallback keyUp);
    virtual ~KeyboardEventBaseHandler() {};

    virtual void hook(XamlView xamlView) = 0;
    virtual void unhook() = 0;

  protected:
    KeyboardEventCallback m_keyDownCallback;
    KeyboardEventCallback m_keyUpCallback;
  };

  class PreviewKeyboardEventHandler: public KeyboardEventBaseHandler
  {
  public:
    PreviewKeyboardEventHandler(KeyboardEventCallback keyDown, KeyboardEventCallback keyUp);

    void hook(XamlView xamlView);
    void unhook();

  private:
    winrt::UIElement::PreviewKeyDown_revoker m_previewKeyDownRevoker{};
    winrt::UIElement::PreviewKeyUp_revoker m_previewKeyUpRevoker{};
  };

  class KeyboardEventHandler : public KeyboardEventBaseHandler
  {
  public:
    KeyboardEventHandler(KeyboardEventCallback keyDown, KeyboardEventCallback keyUp);

    void hook(XamlView xamlView);
    void unhook();

  private:
    winrt::UIElement::KeyDown_revoker m_KeyDownRevoker{};
    winrt::UIElement::KeyUp_revoker m_KeyUpRevoker{};
  };

  class PreviewKeyboardEventHandlerOnRoot: public PreviewKeyboardEventHandler
  {
  public:
    PreviewKeyboardEventHandlerOnRoot(const std::weak_ptr<IReactInstance>& reactInstance);

  private:
    void OnPreKeyDown(winrt::IInspectable const& sender, winrt::KeyRoutedEventArgs const& args);
    void OnPreKeyUp(winrt::IInspectable const& sender, winrt::KeyRoutedEventArgs const& args);

    void DispatchEventToJs(string name, winrt::KeyRoutedEventArgs args);
    std::weak_ptr<IReactInstance> m_wkReactInstance;
  };

  struct KeyboardHelper
  {
    static std::vector<KeyboardEvent> FromJS(folly::dynamic const& obj);
    static KeyboardEvent CreateKeyboardEvent(HandledEventPhase phase, winrt::KeyRoutedEventArgs const& args);
    static string FromVirutalKey(winrt::VirtualKey key, bool shiftDown, bool capLocked);
  };
}}
