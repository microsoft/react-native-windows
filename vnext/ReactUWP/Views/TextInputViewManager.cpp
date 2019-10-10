// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextInputViewManager.h"

#include "Unicode.h"

#include <Utils/PropertyHandlerUtils.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

namespace react {
namespace uwp {
struct Selection {
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
} // namespace uwp
} // namespace react

// Such code is better to move to a seperate parser layer
template <>
struct json_type_traits<react::uwp::Selection> {
  static react::uwp::Selection parseJson(const folly::dynamic &json) {
    react::uwp::Selection selection;
    for (auto &item : json.items()) {
      if (item.first == "start") {
        auto start = item.second.asDouble();
        if (start == static_cast<int>(start))
          selection.start = static_cast<int>(start);
      } else if (item.first == "end") {
        auto end = item.second.asDouble();
        if (end == static_cast<int>(end))
          selection.end = static_cast<int>(end);
      }
    }
    return selection;
  }
};

static const std::unordered_map<std::string, winrt::InputScopeNameValue>
    textBoxKeyboardTypeMap = {
        {"default", winrt::InputScopeNameValue::Default},
        {"number-pad", winrt::InputScopeNameValue::TelephoneNumber},
        {"decimal-pad", winrt::InputScopeNameValue::Digits},
        {"email-address", winrt::InputScopeNameValue::EmailNameOrAddress},
        {"phone-pad", winrt::InputScopeNameValue::TelephoneNumber},
        {"numeric", winrt::InputScopeNameValue::Number},
        {"url", winrt::InputScopeNameValue::Url},
        {"web-search", winrt::InputScopeNameValue::Search}};

static const std::unordered_map<std::string, winrt::InputScopeNameValue>
    passwordBoxKeyboardTypeMap = {
        {"default", winrt::InputScopeNameValue::Password},
        {"numeric", winrt::InputScopeNameValue::NumericPin}};

static winrt::InputScopeNameValue parseKeyboardType(
    const folly::dynamic &val,
    const bool isTextBox) {
  auto keyboardTypeMap =
      isTextBox ? textBoxKeyboardTypeMap : passwordBoxKeyboardTypeMap;

  auto iter = keyboardTypeMap.find(val.asString());

  if (iter != keyboardTypeMap.end()) {
    return iter->second;
  }

  return isTextBox ? winrt::InputScopeNameValue::Default
                   : winrt::InputScopeNameValue::Password;
}

namespace react {
namespace uwp {

class TextInputShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  TextInputShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

  bool ImplementsPadding() override {
    return true;
  }

 private:
  void dispatchTextInputChangeEvent(winrt::hstring newText);
  void registerEvents();
  bool m_shouldClearTextOnFocus = false;
  bool m_shouldSelectTextOnFocus = false;
  bool m_contextMenuHidden = false;
  bool m_isTextBox = true;

  // Javascripts is running in a different thread. If the typing is very fast,
  // It's possible that two TextChanged are raised but TextInput just got the
  // first response from Javascript. So the first response should be dropped.
  // EventCount is introduced to resolve this problem
  uint32_t m_nativeEventCount{0}; // EventCount to javascript
  uint32_t m_mostRecentEventCount{0}; // EventCount from javascript

 private:
  winrt::TextBox::TextChanging_revoker m_textBoxTextChangingRevoker{};
  winrt::TextBox::TextChanged_revoker m_textBoxTextChangedRevoker{};
  winrt::TextBox::SelectionChanged_revoker m_textBoxSelectionChangedRevoker{};
  winrt::TextBox::ContextMenuOpening_revoker
      m_textBoxContextMenuOpeningRevoker{};

  winrt::PasswordBox::PasswordChanging_revoker
      m_passwordBoxPasswordChangingRevoker{};
  winrt::PasswordBox::PasswordChanged_revoker
      m_passwordBoxPasswordChangedRevoker{};
  winrt::PasswordBox::ContextMenuOpening_revoker
      m_passwordBoxContextMenuOpeningRevoker{};

  winrt::Control::GotFocus_revoker m_controlGotFocusRevoker{};
  winrt::Control::LostFocus_revoker m_controlLostFocusRevoker{};
  winrt::Control::SizeChanged_revoker m_controlSizeChangedRevoker{};
  winrt::Control::CharacterReceived_revoker m_controlCharacterReceivedRevoker{};
  winrt::ScrollViewer::ViewChanging_revoker m_scrollViewerViewChangingRevoker{};
  winrt::Control::Loaded_revoker m_controlLoadedRevoker{};
};

void TextInputShadowNode::createView() {
  Super::createView();
  registerEvents();
}

void TextInputShadowNode::dispatchTextInputChangeEvent(winrt::hstring newText) {
  auto wkinstance = GetViewManager()->GetReactInstance();

  if (auto instance = wkinstance.lock()) {
    m_nativeEventCount++;
    folly::dynamic eventData = folly::dynamic::object("target", m_tag)(
        "text", HstringToDynamic(newText))("eventCount", m_nativeEventCount);
    instance->DispatchEvent(m_tag, "topTextInputChange", std::move(eventData));
  }
}

void TextInputShadowNode::registerEvents() {
  auto control = GetView().as<winrt::Control>();
  auto wkinstance = GetViewManager()->GetReactInstance();
  auto tag = m_tag;

  // TextChanged is implemented as async event in Xaml. If Javascript is like
  // this:
  //    onChangeText={text => this.setState({text})}
  // And user type 'AB' very fast, then 'B' is possible to be lost in below
  // situation.
  //    Input 'A' -> TextChanged for 'A' -> Javascript processing 'A' -> Input
  //    becomes 'AB' -> Processing javascript response and set text to 'A'
  //    TextChanged for 'AB' but textbox.Text is 'A' -> Javascript processing
  //    'A'
  //
  // TextChanging is used to drop the Javascript response of 'A' and expect
  // another TextChanged event with correct event count.
  if (m_isTextBox) {
    m_passwordBoxPasswordChangingRevoker = {};
    m_textBoxTextChangingRevoker = control.as<winrt::TextBox>().TextChanging(
        winrt::auto_revoke, [=](auto &&, auto &&) { m_nativeEventCount++; });
  } else {
    m_textBoxTextChangingRevoker = {};
    if (control.try_as<winrt::IPasswordBox4>()) {
      m_passwordBoxPasswordChangingRevoker =
          control.as<winrt::IPasswordBox4>().PasswordChanging(
              winrt::auto_revoke,
              [=](auto &&, auto &&) { m_nativeEventCount++; });
    }
  }

  if (m_isTextBox) {
    m_passwordBoxPasswordChangedRevoker = {};
    auto textBox = control.as<winrt::TextBox>();
    m_textBoxTextChangedRevoker =
        textBox.TextChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
          dispatchTextInputChangeEvent(textBox.Text());
        });
  } else {
    m_textBoxTextChangedRevoker = {};
    auto passwordBox = control.as<winrt::PasswordBox>();
    m_passwordBoxPasswordChangedRevoker =
        passwordBox.PasswordChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
          dispatchTextInputChangeEvent(passwordBox.Password());
        });
  }

  if (m_isTextBox) {
    m_passwordBoxContextMenuOpeningRevoker = {};
    auto textBox = control.as<winrt::TextBox>();
    m_textBoxContextMenuOpeningRevoker = textBox.ContextMenuOpening(
        winrt::auto_revoke, [=](auto &&, winrt::ContextMenuEventArgs const &e) {
          if (m_contextMenuHidden) {
            e.Handled(true);
          }
        });
  } else {
    m_textBoxContextMenuOpeningRevoker = {};
    auto passwordBox = control.as<winrt::PasswordBox>();
    m_passwordBoxContextMenuOpeningRevoker = passwordBox.ContextMenuOpening(
        winrt::auto_revoke, [=](auto &&, winrt::ContextMenuEventArgs const &e) {
          if (m_contextMenuHidden) {
            e.Handled(true);
          }
        });
  }

  m_controlGotFocusRevoker =
      control.GotFocus(winrt::auto_revoke, [=](auto &&, auto &&) {
        if (m_shouldClearTextOnFocus) {
          if (m_isTextBox) {
            control.as<winrt::TextBox>().ClearValue(
                winrt::TextBox::TextProperty());
          } else {
            control.as<winrt::PasswordBox>().ClearValue(
                winrt::PasswordBox::PasswordProperty());
          }
        }

        if (m_shouldSelectTextOnFocus) {
          if (m_isTextBox) {
            control.as<winrt::TextBox>().SelectAll();
          } else {
            control.as<winrt::PasswordBox>().SelectAll();
          }
        }

        auto instance = wkinstance.lock();
        folly::dynamic eventData = folly::dynamic::object("target", tag);
        if (!m_updating && instance != nullptr)
          instance->DispatchEvent(
              tag, "topTextInputFocus", std::move(eventData));
      });

  m_controlLostFocusRevoker =
      control.LostFocus(winrt::auto_revoke, [=](auto &&, auto &&) {
        auto instance = wkinstance.lock();
        folly::dynamic eventDataBlur = folly::dynamic::object("target", tag);
        folly::dynamic eventDataEndEditing = {};
        if (m_isTextBox) {
          eventDataEndEditing = folly::dynamic::object("target", tag)(
              "text", HstringToDynamic(control.as<winrt::TextBox>().Text()));
        } else {
          eventDataEndEditing = folly::dynamic::object("target", tag)(
              "text",
              HstringToDynamic(control.as<winrt::PasswordBox>().Password()));
        }
        if (!m_updating && instance != nullptr) {
          instance->DispatchEvent(
              tag, "topTextInputBlur", std::move(eventDataBlur));
          instance->DispatchEvent(
              tag, "topTextInputEndEditing", std::move(eventDataEndEditing));
        }
      });

  if (m_isTextBox) {
    auto textBox = control.as<winrt::TextBox>();
    m_textBoxSelectionChangedRevoker =
        textBox.SelectionChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
          auto instance = wkinstance.lock();
          folly::dynamic selectionData =
              folly::dynamic::object("start", textBox.SelectionStart())(
                  "end", textBox.SelectionStart() + textBox.SelectionLength());
          folly::dynamic eventData = folly::dynamic::object("target", tag)(
              "selection", std::move(selectionData));
          if (!m_updating && instance != nullptr)
            instance->DispatchEvent(
                tag, "topTextInputSelectionChange", std::move(eventData));
        });
  }

  m_controlSizeChangedRevoker = control.SizeChanged(
      winrt::auto_revoke,
      [=](auto &&, winrt::SizeChangedEventArgs const &args) {
        if (m_isTextBox) {
          if (control.as<winrt::TextBox>().TextWrapping() ==
              winrt::TextWrapping::Wrap) {
            auto instance = wkinstance.lock();
            folly::dynamic contentSizeData = folly::dynamic::object(
                "width", args.NewSize().Width)("height", args.NewSize().Height);
            folly::dynamic eventData = folly::dynamic::object("target", tag)(
                "contentSize", std::move(contentSizeData));
            if (!m_updating && instance != nullptr)
              instance->DispatchEvent(
                  tag, "topTextInputContentSizeChange", std::move(eventData));
          }
        }
      });

  m_controlLoadedRevoker =
      control.Loaded(winrt::auto_revoke, [=](auto &&, auto &&) {
        auto textBoxView = control.GetTemplateChild(asHstring("ContentElement"))
                               .as<winrt::ScrollViewer>();
        if (textBoxView) {
          m_scrollViewerViewChangingRevoker = textBoxView.ViewChanging(
              winrt::auto_revoke,
              [=](auto &&,
                  winrt::ScrollViewerViewChangingEventArgs const &args) {
                auto instance = wkinstance.lock();
                if (!m_updating && instance != nullptr) {
                  folly::dynamic offsetData = folly::dynamic::object(
                      "x", args.FinalView().HorizontalOffset())(
                      "y", args.FinalView().VerticalOffset());
                  folly::dynamic eventData = folly::dynamic::object(
                      "target", tag)("contentOffset", std::move(offsetData));
                  instance->DispatchEvent(
                      tag, "topTextInputOnScroll", std::move(eventData));
                }
              });
        }
      });

  if (control.try_as<winrt::IUIElement7>()) {
    m_controlCharacterReceivedRevoker = control.CharacterReceived(
        winrt::auto_revoke,
        [=](auto &&, winrt::CharacterReceivedRoutedEventArgs const &args) {
          auto instance = wkinstance.lock();
          std::string key;
          wchar_t s[2] = L" ";
          s[0] = args.Character();
          key = facebook::react::unicode::utf16ToUtf8(s, 1);

          if (key.compare("\r") == 0) {
            key = "Enter";
          } else if (key.compare("\b") == 0) {
            key = "Backspace";
          }

          if (!m_updating && instance != nullptr) {
            folly::dynamic eventData = folly::dynamic::object("target", tag)(
                "key", folly::dynamic(key));
            instance->DispatchEvent(
                tag, "topTextInputKeyPress", std::move(eventData));
          }
        });
  }
}

void TextInputShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  auto control = GetView().as<winrt::Control>();
  auto textBox = control.try_as<winrt::TextBox>();
  auto passwordBox = control.try_as<winrt::PasswordBox>();

  for (auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    // Applicable properties for both TextBox and PasswordBox
    if (TryUpdateFontProperties(control, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateCharacterSpacing(
                   control, propertyName, propertyValue)) {
      continue;
    } else if (propertyName == "allowFontScaling") {
      if (propertyValue.isBool())
        control.IsTextScaleFactorEnabled(propertyValue.asBool());
      else if (propertyValue.isNull())
        control.ClearValue(winrt::Control::IsTextScaleFactorEnabledProperty());
    } else if (propertyName == "clearTextOnFocus") {
      if (propertyValue.isBool())
        m_shouldClearTextOnFocus = propertyValue.asBool();
    } else if (propertyName == "selectTextOnFocus") {
      if (propertyValue.isBool())
        m_shouldSelectTextOnFocus = propertyValue.asBool();
    } else if (propertyName == "mostRecentEventCount") {
      if (propertyValue.isNumber()) {
        m_mostRecentEventCount = static_cast<uint32_t>(propertyValue.asInt());
      }
    } else if (propertyName == "contextMenuHidden") {
      if (propertyValue.isBool())
        m_contextMenuHidden = propertyValue.asBool();
    } else if (propertyName == "secureTextEntry") {
      if (propertyValue.isBool()) {
        if (propertyValue.asBool()) {
          if (m_isTextBox) {
            winrt::PasswordBox newPasswordBox;
            ReparentView(newPasswordBox);
            m_isTextBox = false;
            registerEvents();
            control = newPasswordBox.as<winrt::Control>();
            passwordBox = newPasswordBox;
          }
        } else {
          if (!m_isTextBox) {
            winrt::TextBox newTextBox;
            ReparentView(newTextBox);
            m_isTextBox = true;
            registerEvents();
            control = newTextBox.as<winrt::Control>();
            textBox = newTextBox;
          }
        }
      }
    } else if (propertyName == "maxLength") {
      if (propertyValue.isNumber()) {
        control.SetValue(
            m_isTextBox ? winrt::TextBox::MaxLengthProperty()
                        : winrt::PasswordBox::MaxLengthProperty(),
            winrt::PropertyValue::CreateInt32(
                static_cast<int32_t>(propertyValue.asDouble())));
      } else if (propertyValue.isNull()) {
        control.ClearValue(
            m_isTextBox ? winrt::TextBox::MaxLengthProperty()
                        : winrt::PasswordBox::MaxLengthProperty());
      }
    } else if (propertyName == "placeholder") {
      if (propertyValue.isString()) {
        control.SetValue(
            m_isTextBox ? winrt::TextBox::PlaceholderTextProperty()
                        : winrt::PasswordBox::PlaceholderTextProperty(),
            winrt::PropertyValue::CreateString(asHstring(propertyValue)));
      } else if (propertyValue.isNull()) {
        control.ClearValue(
            m_isTextBox ? winrt::TextBox::PlaceholderTextProperty()
                        : winrt::PasswordBox::PlaceholderTextProperty());
      }
    } else if (propertyName == "selectionColor") {
      if (IsValidColorValue(propertyValue)) {
        control.SetValue(
            m_isTextBox ? winrt::TextBox::SelectionHighlightColorProperty()
                        : winrt::PasswordBox::SelectionHighlightColorProperty(),
            SolidColorBrushFrom(propertyValue));
      } else if (propertyValue.isNull())
        control.ClearValue(
            m_isTextBox
                ? winrt::TextBox::SelectionHighlightColorProperty()
                : winrt::PasswordBox::SelectionHighlightColorProperty());
    } else if (propertyName == "keyboardType") {
      if (propertyValue.isString()) {
        auto inputScopeNameVaue = parseKeyboardType(propertyValue, m_isTextBox);
        auto scope = winrt::InputScope();
        auto scopeName = winrt::InputScopeName(inputScopeNameVaue);
        auto names = scope.Names();
        names.Append(scopeName);
        control.SetValue(
            m_isTextBox ? winrt::TextBox::InputScopeProperty()
                        : winrt::PasswordBox::InputScopeProperty(),
            scope);
      } else if (propertyValue.isNull())
        control.ClearValue(
            m_isTextBox ? winrt::TextBox::InputScopeProperty()
                        : winrt::PasswordBox::InputScopeProperty());
    } else {
      if (m_isTextBox) { // Applicable properties for TextBox
        if (TryUpdateTextAlignment(textBox, propertyName, propertyValue)) {
          continue;
        } else if (propertyName == "multiline") {
          if (propertyValue.isBool()) {
            const bool isMultiline = propertyValue.asBool();
            textBox.TextWrapping(
                isMultiline ? winrt::TextWrapping::Wrap
                            : winrt::TextWrapping::NoWrap);
            textBox.AcceptsReturn(isMultiline);
          } else if (propertyValue.isNull())
            textBox.ClearValue(winrt::TextBox::TextWrappingProperty());
        } else if (propertyName == "editable") {
          if (propertyValue.isBool())
            textBox.IsReadOnly(!propertyValue.asBool());
          else if (propertyValue.isNull())
            textBox.ClearValue(winrt::TextBox::IsReadOnlyProperty());
        } else if (propertyName == "placeholderTextColor") {
          if (textBox.try_as<winrt::ITextBox6>()) {
            if (IsValidColorValue(propertyValue))
              textBox.PlaceholderForeground(SolidColorBrushFrom(propertyValue));
            else if (propertyValue.isNull())
              textBox.ClearValue(
                  winrt::TextBox::PlaceholderForegroundProperty());
          }
        } else if (propertyName == "scrollEnabled") {
          if (propertyValue.isBool() &&
              textBox.TextWrapping() == winrt::TextWrapping::Wrap) {
            auto scrollMode = propertyValue.asBool()
                ? winrt::ScrollMode::Auto
                : winrt::ScrollMode::Disabled;
            winrt::ScrollViewer::SetVerticalScrollMode(textBox, scrollMode);
            winrt::ScrollViewer::SetHorizontalScrollMode(textBox, scrollMode);
          }
        } else if (propertyName == "selection") {
          if (propertyValue.isObject()) {
            auto selection =
                json_type_traits<Selection>::parseJson(propertyValue);

            if (selection.isValid())
              textBox.Select(selection.start, selection.end - selection.start);
          }
        } else if (propertyName == "spellCheck") {
          if (propertyValue.isBool())
            textBox.IsSpellCheckEnabled(propertyValue.asBool());
          else if (propertyValue.isNull())
            textBox.ClearValue(winrt::TextBox::IsSpellCheckEnabledProperty());
        } else if (propertyName == "text") {
          if (m_mostRecentEventCount == m_nativeEventCount) {
            if (propertyValue.isString()) {
              auto oldValue = textBox.Text();
              auto newValue = asHstring(propertyValue);
              if (oldValue != newValue) {
                textBox.Text(newValue);
              }
            } else if (propertyValue.isNull())
              textBox.ClearValue(winrt::TextBox::TextProperty());
          }
        }
      } else { // Applicable properties for PasswordBox
        if (propertyName == "text") {
          if (m_mostRecentEventCount == m_nativeEventCount) {
            if (propertyValue.isString()) {
              auto oldValue = passwordBox.Password();
              auto newValue = asHstring(propertyValue);
              if (oldValue != newValue) {
                passwordBox.Password(newValue);
              }
            } else if (propertyValue.isNull())
              passwordBox.ClearValue(winrt::PasswordBox::PasswordProperty());
          }
        }
      }
    }
  }

  Super::updateProperties(std::move(props));
  m_updating = false;
}

TextInputViewManager::TextInputViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *TextInputViewManager::GetName() const {
  return "RCTTextInput";
}

folly::dynamic TextInputViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("allowFontScaling", "boolean")(
      "clearTextOnFocus", "boolean")("editable", "boolean")("maxLength", "int")(
      "multiline", "boolean")("placeholder", "string")(
      "placeholderTextColor", "Color")("scrollEnabled", "boolean")(
      "selection", "Map")("selectionColor", "Color")(
      "selectTextOnFocus", "boolean")("spellCheck", "boolean")(
      "text", "string")("mostRecentEventCount", "int")(
      "secureTextEntry", "boolean")("keyboardType", "string")(
      "contextMenuHidden", "boolean"));

  return props;
}

folly::dynamic TextInputViewManager::GetExportedCustomDirectEventTypeConstants()
    const {
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topTextInputChange"] =
      folly::dynamic::object("registrationName", "onChange");
  directEvents["topTextInputFocus"] =
      folly::dynamic::object("registrationName", "onFocus");
  directEvents["topTextInputBlur"] =
      folly::dynamic::object("registrationName", "onBlur");
  directEvents["topTextInputEndEditing"] =
      folly::dynamic::object("registrationName", "onEndEditing");
  directEvents["topTextInputSelectionChange"] =
      folly::dynamic::object("registrationName", "onSelectionChange");
  directEvents["topTextInputContentSizeChange"] =
      folly::dynamic::object("registrationName", "onContentSizeChange");
  directEvents["topTextInputKeyPress"] =
      folly::dynamic::object("registrationName", "onKeyPress");
  directEvents["topTextInputOnScroll"] =
      folly::dynamic::object("registrationName", "onScroll");

  return directEvents;
}

facebook::react::ShadowNode *TextInputViewManager::createShadow() const {
  return new TextInputShadowNode();
}

XamlView TextInputViewManager::CreateViewCore(int64_t tag) {
  winrt::TextBox textBox;
  return textBox;
}

YGMeasureFunc TextInputViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

void TextInputViewManager::TransferInputScope(
    XamlView oldView,
    XamlView newView,
    const bool copyToPasswordBox) {
  // transfer input scope, only common keyboardType between secureTextEntry
  // on/off is numeric, so only need to transfer input scope "Number" <=>
  // "NumericPin", everything else leave it as default.
  winrt::InputScope inputScope;
  if (copyToPasswordBox) {
    inputScope = oldView.try_as<winrt::TextBox>().InputScope();
  } else {
    inputScope = oldView.try_as<winrt::PasswordBox>().InputScope();
  }

  if (inputScope != nullptr) {
    auto nameValue = inputScope.Names().GetAt(0).NameValue();

    if ((nameValue == winrt::InputScopeNameValue::Number &&
         copyToPasswordBox) ||
        (nameValue == winrt::InputScopeNameValue::NumericPin &&
         !copyToPasswordBox)) {
      auto newScope = winrt::InputScope();
      auto scopeName = winrt::InputScopeName(
          copyToPasswordBox ? winrt::InputScopeNameValue::NumericPin
                            : winrt::InputScopeNameValue::Number);
      auto names = newScope.Names();
      names.Append(scopeName);
      newView.SetValue(
          copyToPasswordBox ? winrt::PasswordBox::InputScopeProperty()
                            : winrt::TextBox::InputScopeProperty(),
          newScope);
    }
  }
}

void TextInputViewManager::TransferProperties(
    XamlView oldView,
    XamlView newView) {
  if ((oldView.try_as<winrt::TextBox>() != nullptr &&
       newView.try_as<winrt::PasswordBox>() != nullptr) ||
      (oldView.try_as<winrt::PasswordBox>() != nullptr &&
       newView.try_as<winrt::TextBox>() != nullptr)) {
    bool copyToPasswordBox = oldView.try_as<winrt::TextBox>() != nullptr;

    // sync common properties between TextBox and PasswordBox
    if (copyToPasswordBox) {
      TransferProperty(
          oldView,
          newView,
          winrt::TextBox::MaxLengthProperty(),
          winrt::PasswordBox::MaxLengthProperty());
      TransferProperty(
          oldView,
          newView,
          winrt::TextBox::PlaceholderTextProperty(),
          winrt::PasswordBox::PlaceholderTextProperty());
      TransferProperty(
          oldView,
          newView,
          winrt::TextBox::SelectionHighlightColorProperty(),
          winrt::PasswordBox::SelectionHighlightColorProperty());
      newView.as<winrt::PasswordBox>().Password(
          oldView.as<winrt::TextBox>().Text());
    } else {
      TransferProperty(
          oldView,
          newView,
          winrt::PasswordBox::MaxLengthProperty(),
          winrt::TextBox::MaxLengthProperty());
      TransferProperty(
          oldView,
          newView,
          winrt::PasswordBox::PlaceholderTextProperty(),
          winrt::TextBox::PlaceholderTextProperty());
      TransferProperty(
          oldView,
          newView,
          winrt::PasswordBox::SelectionHighlightColorProperty(),
          winrt::TextBox::SelectionHighlightColorProperty());
      newView.as<winrt::TextBox>().Text(
          oldView.as<winrt::PasswordBox>().Password());
    }

    TransferInputScope(oldView, newView, copyToPasswordBox);
    // Set focus if current control has focus
    auto focusState = oldView.as<winrt::Control>().FocusState();
    if (focusState != winrt::FocusState::Unfocused) {
      newView.as<winrt::Control>().Focus(focusState);
    }
  }
  Super::TransferProperties(oldView, newView);
}
} // namespace uwp
} // namespace react
