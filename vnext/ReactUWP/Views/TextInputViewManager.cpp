// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextInputViewManager.h"

#include "unicode.h"

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
  void RegisterEvents();
  bool m_shouldClearTextOnFocus = false;
  bool m_shouldSelectTextOnFocus = false;
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

  winrt::PasswordBox::PasswordChanging_revoker
      m_passwordBoxPasswordChangingRevoker{};
  winrt::PasswordBox::PasswordChanged_revoker
      m_passwordBoxPasswordChangedRevoker{};

  winrt::TextBox::GotFocus_revoker m_controlGotFocusRevoker{};
  winrt::TextBox::LostFocus_revoker m_controlLostFocusRevoker{};
  winrt::Control::SizeChanged_revoker m_controlSizeChangedRevoker{};
  winrt::Control::CharacterReceived_revoker m_controlCharacterReceivedRevoker{};
  winrt::ScrollViewer::ViewChanging_revoker m_scrollViewerViewChangingRevoker{};
  winrt::Control::Loaded_revoker m_controlLoadedRevoker{};
};

void TextInputShadowNode::createView() {
  Super::createView();
  RegisterEvents();
}

void TextInputShadowNode::RegisterEvents() {
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
    m_textBoxTextChangingRevoker =
        control.as<winrt::TextBox>().TextChanging(
            winrt::auto_revoke,
            [=](auto &&, auto &&) { m_nativeEventCount++; });
  } else {
    if (control.try_as<winrt::IPasswordBox4>()) {
      m_passwordBoxPasswordChangingRevoker =
          control.as<winrt::IPasswordBox4>().PasswordChanging(
              winrt::auto_revoke,
              [=](auto &&, auto &&) { m_nativeEventCount++; });
    }
  }

  if (m_isTextBox) {
    auto textBox = control.as<winrt::TextBox>();
    m_textBoxTextChangedRevoker =
        textBox.TextChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
          if (auto instance = wkinstance.lock()) {
            m_nativeEventCount++;
            folly::dynamic eventData = folly::dynamic::object("target", tag)(
                "text", HstringToDynamic(textBox.Text()))(
                "eventCount", m_nativeEventCount);
            instance->DispatchEvent(
                tag, "topTextInputChange", std::move(eventData));
          }
        });
  } else {
    auto passwordBox = control.as<winrt::PasswordBox>();
    m_passwordBoxPasswordChangedRevoker =
        passwordBox.PasswordChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
          if (auto instance = wkinstance.lock()) {
            m_nativeEventCount++;
            folly::dynamic eventData = folly::dynamic::object("target", tag)(
                "text", HstringToDynamic(passwordBox.Password()))(
                "eventCount", m_nativeEventCount);
            instance->DispatchEvent(
                tag, "topTextInputChange", std::move(eventData));
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
              "text",
              HstringToDynamic(control.as<winrt::TextBox>().Text()));
        } else {
          eventDataEndEditing = folly::dynamic::object("target", tag)(
              "text",
              HstringToDynamic(
                  control.as<winrt::PasswordBox>().Password()));
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
        auto textBoxView =
            control.GetTemplateChild(asHstring("ContentElement"))
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

  for (auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    // Applicable properties for both PasswordBoxTextBox and PasswordBox
    if (TryUpdateFontProperties(control, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateCharacterSpacing(
                   control, propertyName, propertyValue)) {
      continue;
    } else if (propertyName == "allowFontScaling") {
      if (propertyValue.isBool())
        control.IsTextScaleFactorEnabled(propertyValue.asBool());
      else if (propertyValue.isNull())
        control.ClearValue(
            winrt::Control::IsTextScaleFactorEnabledProperty());
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
    } else if (propertyName == "secureTextEntry") {
      if (propertyValue.isBool()) {
        if (propertyValue.asBool()) {
          if (m_isTextBox) {
            winrt::PasswordBox pwBox;
            ReparentView(pwBox);
            m_isTextBox = false;
            RegisterEvents();
            control = pwBox.as<winrt::Control>();
          }
        } else {
          if (!m_isTextBox) {
            winrt::TextBox textBox;
            ReparentView(textBox);
            m_isTextBox = true;
            RegisterEvents();
            control = textBox.as<winrt::Control>();
          }
        }
      }
    } else {
      if (m_isTextBox) { // Applicable properties for TextBox
        auto textBox = control.as<winrt::TextBox>();
        if (TryUpdateTextAlignment(textBox, propertyName, propertyValue)) {
          continue;
        } else if (propertyName == "multiline") {
          if (propertyValue.isBool())
            textBox.TextWrapping(
                propertyValue.asBool() ? winrt::TextWrapping::Wrap
                                       : winrt::TextWrapping::NoWrap);
          else if (propertyValue.isNull())
            textBox.ClearValue(winrt::TextBox::TextWrappingProperty());
        } else if (propertyName == "editable") {
          if (propertyValue.isBool())
            textBox.IsReadOnly(!propertyValue.asBool());
          else if (propertyValue.isNull())
            textBox.ClearValue(winrt::TextBox::IsReadOnlyProperty());
        } else if (propertyName == "maxLength") {
          if (propertyValue.isNumber())
            textBox.MaxLength(static_cast<int32_t>(propertyValue.asDouble()));
          else if (propertyValue.isNull())
            textBox.ClearValue(winrt::TextBox::MaxLengthProperty());
        } else if (propertyName == "placeholder") {
          if (propertyValue.isString())
            textBox.PlaceholderText(asHstring(propertyValue));
          else if (propertyValue.isNull())
            textBox.ClearValue(winrt::TextBox::PlaceholderTextProperty());
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
        } else if (propertyName == "selectionColor") {
          if (IsValidColorValue(propertyValue))
            textBox.SelectionHighlightColor(SolidColorBrushFrom(propertyValue));
          else if (propertyValue.isNull())
            textBox.ClearValue(
                winrt::TextBox::SelectionHighlightColorProperty());
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
        auto pwBox = control.as<winrt::PasswordBox>();
        if (propertyName == "maxLength") {
          if (propertyValue.isNumber())
            pwBox.MaxLength(static_cast<int32_t>(propertyValue.asDouble()));
          else if (propertyValue.isNull())
            pwBox.ClearValue(winrt::PasswordBox::MaxLengthProperty());
        } else if (propertyName == "placeholder") {
          if (propertyValue.isString())
            pwBox.PlaceholderText(asHstring(propertyValue));
          else if (propertyValue.isNull())
            pwBox.ClearValue(winrt::PasswordBox::PlaceholderTextProperty());
        } else if (propertyName == "selectionColor") {
          if (IsValidColorValue(propertyValue))
            pwBox.SelectionHighlightColor(SolidColorBrushFrom(propertyValue));
          else if (propertyValue.isNull())
            pwBox.ClearValue(
                winrt::PasswordBox::SelectionHighlightColorProperty());
        } else if (propertyName == "text") {
          if (m_mostRecentEventCount == m_nativeEventCount) {
            if (propertyValue.isString()) {
              auto oldValue = pwBox.Password();
              auto newValue = asHstring(propertyValue);
              if (oldValue != newValue) {
                pwBox.Password(newValue);
              }
            } else if (propertyValue.isNull())
              pwBox.ClearValue(winrt::PasswordBox::PasswordProperty());
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
      "secureTextEntry", "boolean"));

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

void TextInputViewManager::TransferProperties(
    XamlView oldView,
    XamlView newView) {
  __super::TransferProperties(oldView, newView);

  bool copyToPasswordBox = oldView.try_as<winrt::TextBox>() != nullptr;

  // sync common control properties
  TransferProperty(oldView, newView, winrt::Control::FontSizeProperty());
  TransferProperty(oldView, newView, winrt::Control::FontFamilyProperty());
  TransferProperty(oldView, newView, winrt::Control::FontWeightProperty());
  TransferProperty(oldView, newView, winrt::Control::FontStyleProperty());
  TransferProperty(
      oldView, newView, winrt::Control::CharacterSpacingProperty());
  TransferProperty(
      oldView, newView, winrt::Control::IsTextScaleFactorEnabledProperty());
  TransferProperty(oldView, newView, winrt::Control::BackgroundProperty());
  TransferProperty(oldView, newView, winrt::Control::BorderBrushProperty());
  TransferProperty(oldView, newView, winrt::Control::BorderThicknessProperty());
  TransferProperty(oldView, newView, winrt::Control::PaddingProperty());
  TransferProperty(oldView, newView, winrt::Control::ForegroundProperty());
  TransferProperty(oldView, newView, winrt::Control::TabIndexProperty());

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

  // Set focus if current control has focus
  auto focusState = oldView.as<winrt::Control>().FocusState();
  if (focusState != winrt::FocusState::Unfocused) {
    newView.as<winrt::Control>().Focus(focusState);
  }
}
} // namespace uwp
} // namespace react
