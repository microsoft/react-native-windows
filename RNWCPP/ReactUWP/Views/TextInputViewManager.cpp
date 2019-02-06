// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextInputViewManager.h"

#include <Utils/PropertyHandlerUtils.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
}

namespace react {
  namespace uwp {
    struct Selection
    {
      int start = -1;
      int end = -1;
      bool isValid();
    };

    bool Selection::isValid() {
      if (start < 0)
        return false;
      if (end < 0)
        return false;
      if (start > end)
        return false;
      return true;
    }
} }

// Such code is better to move to a seperate parser layer
template<>
struct json_type_traits<react::uwp::Selection>
{
  static react::uwp::Selection parseJson(folly::dynamic& json)
  {
    react::uwp::Selection selection;
    for (auto& item : json.items())
    {
      if (item.first == "start")
        selection.start = item.second.asInt();
      else if (item.first == "end")
        selection.end = item.second.asInt();
    }
    return selection;
  }
};

namespace react { namespace uwp {

enum class TextInputCommands
{
  SetFocus = 1,
  Blur = 2,
};

TextInputViewManager::TextInputViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : FrameworkElementViewManager(reactInstance)
{
}

const char* TextInputViewManager::GetName() const
{
  return "RCTTextInput";
}

folly::dynamic TextInputViewManager::GetCommands() const
{
  auto commands = Super::GetCommands();
  commands.update(folly::dynamic::object
    ("SetFocus", static_cast<std::underlying_type_t<TextInputCommands>>(TextInputCommands::SetFocus))
  );
  commands.update(folly::dynamic::object
    ("Blur", static_cast<std::underlying_type_t<TextInputCommands>>(TextInputCommands::Blur))
  );
  return commands;
}

folly::dynamic TextInputViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("allowFontScaling", "boolean")
    ("clearTextOnFocus", "boolean")
    ("editable", "boolean")
    ("maxLength", "int")
    ("multiline", "boolean")
    ("placeholder", "string")
    ("placeholderTextColor", "Color")
    ("scrollEnabled", "boolean")
    ("selection", "Map")
    ("selectionColor", "Color")
    ("selectTextOnFocus", "boolean")
    ("spellCheck", "boolean")
    ("text", "string")
  );

  return props;
}

folly::dynamic TextInputViewManager::GetExportedCustomDirectEventTypeConstants() const
{
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topTextInputChange"] = folly::dynamic::object("registrationName", "onChange");
  directEvents["topTextInputFocus"] = folly::dynamic::object("registrationName", "onFocus");
  directEvents["topTextInputBlur"] = folly::dynamic::object("registrationName", "onBlur");
  directEvents["topTextInputEndEditing"] = folly::dynamic::object("registrationName", "onEndEditing");
  directEvents["topTextInputSelectionChange"] = folly::dynamic::object("registrationName", "onSelectionChange");
  directEvents["topTextInputContentSizeChange"] = folly::dynamic::object("registrationName", "onContentSizeChange");

  return directEvents;
}

XamlView TextInputViewManager::CreateViewCore(int64_t tag)
{
  winrt::TextBox textBox;

  textBox.TextChanged([=](auto &&, auto &&)
  {
    auto instance = m_wkReactInstance.lock();
    folly::dynamic eventData = folly::dynamic::object("target", tag)("text", HstringToDynamic(textBox.Text()));
    if (instance != nullptr)
      instance->DispatchEvent(tag, "topTextInputChange", std::move(eventData));
  });

  textBox.GotFocus([=](auto &&, auto &&)
  {
    if (m_shouldClearTextOnFocus)
      textBox.ClearValue(winrt::TextBox::TextProperty());

    if (m_shouldSelectTextOnFocus)
      textBox.SelectAll();

    auto instance = m_wkReactInstance.lock();
    folly::dynamic eventData = folly::dynamic::object("target", tag);
    if (instance != nullptr)
      instance->DispatchEvent(tag, "topTextInputFocus", std::move(eventData));
  });

  textBox.LostFocus([=](auto &&, auto &&)
  {
    auto instance = m_wkReactInstance.lock();
    folly::dynamic eventDataBlur = folly::dynamic::object("target", tag);
    folly::dynamic eventDataEndEditing = folly::dynamic::object("target", tag)("text", HstringToDynamic(textBox.Text()));
    if (instance != nullptr)
    {
      instance->DispatchEvent(tag, "topTextInputBlur", std::move(eventDataBlur));
      instance->DispatchEvent(tag, "topTextInputEndEditing", std::move(eventDataEndEditing));
    }
  });

  textBox.SelectionChanged([=](auto &&, auto &&)
  {
    auto instance = m_wkReactInstance.lock();
    folly::dynamic selectionData = folly::dynamic::object("start", textBox.SelectionStart())("end", textBox.SelectionStart() + textBox.SelectionLength());
    folly::dynamic eventData = folly::dynamic::object("target", tag)("selection", std::move(selectionData));
    if (instance != nullptr)
      instance->DispatchEvent(tag, "topTextInputSelectionChange", std::move(eventData));
  });

  textBox.SizeChanged([=](auto &&, winrt::SizeChangedEventArgs const& args)
  {
    if (textBox.TextWrapping() == winrt::TextWrapping::Wrap)
    {
      auto instance = m_wkReactInstance.lock();
      folly::dynamic contentSizeData = folly::dynamic::object("width", args.NewSize().Width)("height", args.NewSize().Height);
      folly::dynamic eventData = folly::dynamic::object("target", tag)("contentSize", std::move(contentSizeData));
      if (instance != nullptr)
        instance->DispatchEvent(tag, "topTextInputContentSizeChange", std::move(eventData));
    }
  });

  return textBox;
}

void TextInputViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, XamlView viewToUpdate, folly::dynamic reactDiffMap)
{
  auto textBox = viewToUpdate.as<winrt::TextBox>();
  if (textBox == nullptr)
    return;

  auto control = textBox.as<winrt::Control>();
  for (auto& pair : reactDiffMap.items())
  {
    // FUTURE: In the future cppwinrt will generate code where static methods on base types can
    // be called.  For now we specify the base type explicitly
    if (TryUpdateForeground<winrt::Control>(textBox, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateFontProperties(textBox, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdatePadding(nodeToUpdate, textBox, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateTextAlignment(textBox, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateCharacterSpacing(textBox, pair.first, pair.second))
    {
      continue;
    }
    // FUTURE: In the future cppwinrt will generate code where static methods on base types can
    // be called.  For now we specify the base type explicitly
    else if (TryUpdateBackgroundBrush(control, pair.first, pair.second))
    {
      continue;
    }
    // FUTURE: In the future cppwinrt will generate code where static methods on base types can
    // be called.  For now we specify the base type explicitly
    else if (TryUpdateBorderProperties(nodeToUpdate, control, pair.first, pair.second))
    {
      continue;
    }
    else if (pair.first == "multiline")
    {
      if (pair.second.isBool())
        textBox.TextWrapping(pair.second.asBool() ? winrt::TextWrapping::Wrap : winrt::TextWrapping::NoWrap);
    }
    else if (pair.first == "allowFontScaling")
    {
      if (pair.second.isBool())
        textBox.IsTextScaleFactorEnabled(pair.second.asBool());
    }
    else if (pair.first == "clearTextOnFocus")
    {
      if (pair.second.isBool())
        m_shouldClearTextOnFocus = pair.second.asBool();
    }
    else if (pair.first == "editable")
    {
      if (pair.second.isBool())
        textBox.IsReadOnly(!pair.second.asBool());
    }
    else if (pair.first == "maxLength")
    {
      if (pair.second.isInt())
        textBox.MaxLength(static_cast<int32_t>(pair.second.asInt()));
    }
    else if (pair.first == "placeholder")
    {
      if (pair.second.isString())
        textBox.PlaceholderText(asHstring(pair.second));
    }
    else if (pair.first == "placeholderTextColor")
    {
      if (pair.second.isInt())
        textBox.PlaceholderForeground(SolidColorBrushFrom(pair.second));
      else if (pair.second.isNull())
        textBox.ClearValue(winrt::TextBox::PlaceholderForegroundProperty());
    }
    else if (pair.first == "scrollEnabled")
    {
      if (pair.second.isBool() && textBox.TextWrapping() == winrt::TextWrapping::Wrap)
      {
        auto scrollMode = pair.second.asBool() ? winrt::ScrollMode::Auto : winrt::ScrollMode::Disabled;
        winrt::ScrollViewer::SetVerticalScrollMode(textBox, scrollMode);
        winrt::ScrollViewer::SetHorizontalScrollMode(textBox, scrollMode);
      }
    }
    else if (pair.first == "selection")
    {
      if (pair.second.isObject())
      {
        auto selection = json_type_traits<Selection>::parseJson(pair.second);

        if (selection.isValid())
          textBox.Select(selection.start, selection.end - selection.start);
      }
    }
    else if (pair.first == "selectionColor")
    {
      if (pair.second.isInt())
        textBox.SelectionHighlightColor(SolidColorBrushFrom(pair.second));
      else if (pair.second.isNull())
        textBox.ClearValue(winrt::TextBox::SelectionHighlightColorProperty());
    }
    else if (pair.first == "selectTextOnFocus")
    {
      if (pair.second.isBool())
        m_shouldSelectTextOnFocus = pair.second.asBool();
    }
    else if (pair.first == "spellCheck")
    {
      if (pair.second.isBool())
        textBox.IsSpellCheckEnabled(pair.second.asBool());
    }
    else if (pair.first == "text")
    {
      if (pair.second.isString())
        textBox.Text(asHstring(pair.second));
    }
  }

  Super::UpdateProperties(nodeToUpdate, viewToUpdate, reactDiffMap);
}

void TextInputViewManager::DispatchCommand(XamlView viewToUpdate, int64_t commandId, const folly::dynamic& commandArgs)
{
  auto textBox = viewToUpdate.as<winrt::TextBox>();
  if (textBox == nullptr)
    return;

  switch (static_cast<TextInputCommands>(commandId))
  {
    case TextInputCommands::SetFocus:
    {
      textBox.Focus(winrt::FocusState::Programmatic);
      break;
    }

    case TextInputCommands::Blur:
    {
      auto content = winrt::Windows::UI::Xaml::Window::Current().Content();
      if (content == nullptr)
        break;

      auto frame = content.try_as<winrt::Windows::UI::Xaml::Controls::Frame>();
      if (frame != nullptr)
        frame.Focus(winrt::FocusState::Programmatic);
      break;
    }
  }
}

YGMeasureFunc TextInputViewManager::GetYogaCustomMeasureFunc() const
{
  return DefaultYogaSelfMeasureFunc;
}

} }
