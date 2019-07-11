// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextInputViewManager.h"

#include "unicode.h"

#include <Utils/PropertyHandlerUtils.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>
#include <Utils/XamlDirectInstance.h>

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
  bool m_shouldClearTextOnFocus = false;
  bool m_shouldSelectTextOnFocus = false;

  // Javascripts is running in a different thread. If the typing is very fast,
  // It's possible that two TextChanged are raised but TextInput just got the
  // first response from Javascript. So the first response should be dropped.
  // EventCount is introduced to resolve this problem
  uint32_t m_nativeEventCount{0}; // EventCount to javascript
  uint32_t m_mostRecentEventCount{0}; // EventCount from javascript

 private:
  winrt::TextBox::TextChanging_revoker m_textBoxTextChangingRevoker{};
  winrt::TextBox::TextChanged_revoker m_textBoxTextChangedRevoker{};
  winrt::TextBox::GotFocus_revoker m_textBoxGotFocusRevoker{};
  winrt::TextBox::LostFocus_revoker m_textBoxLostFocusRevoker{};
  winrt::TextBox::SelectionChanged_revoker m_textBoxSelectionChangedRevoker{};
  winrt::TextBox::SizeChanged_revoker m_textBoxSizeChangedRevoker{};
  winrt::ScrollViewer::ViewChanging_revoker m_scrollViewerViewChangingRevoker{};
  winrt::TextBox::CharacterReceived_revoker m_textBoxCharacterReceivedRevoker{};
};

void TextInputShadowNode::createView() {
  Super::createView();

  auto textBox = GetView().as<winrt::TextBox>();
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
  m_textBoxTextChangingRevoker = textBox.TextChanging(
      winrt::auto_revoke, [=](auto &&, auto &&) { m_nativeEventCount++; });

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

  m_textBoxGotFocusRevoker =
      textBox.GotFocus(winrt::auto_revoke, [=](auto &&, auto &&) {
        if (m_shouldClearTextOnFocus)
          textBox.ClearValue(winrt::TextBox::TextProperty());

        if (m_shouldSelectTextOnFocus)
          textBox.SelectAll();

        auto instance = wkinstance.lock();
        folly::dynamic eventData = folly::dynamic::object("target", tag);
        if (!m_updating && instance != nullptr)
          instance->DispatchEvent(
              tag, "topTextInputFocus", std::move(eventData));
      });

  m_textBoxLostFocusRevoker =
      textBox.LostFocus(winrt::auto_revoke, [=](auto &&, auto &&) {
        auto instance = wkinstance.lock();
        folly::dynamic eventDataBlur = folly::dynamic::object("target", tag);
        folly::dynamic eventDataEndEditing = folly::dynamic::object(
            "target", tag)("text", HstringToDynamic(textBox.Text()));
        if (!m_updating && instance != nullptr) {
          instance->DispatchEvent(
              tag, "topTextInputBlur", std::move(eventDataBlur));
          instance->DispatchEvent(
              tag, "topTextInputEndEditing", std::move(eventDataEndEditing));
        }
      });

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

  m_textBoxSizeChangedRevoker = textBox.SizeChanged(
      winrt::auto_revoke,
      [=](auto &&, winrt::SizeChangedEventArgs const &args) {
        if (textBox.TextWrapping() == winrt::TextWrapping::Wrap) {
          auto instance = wkinstance.lock();
          folly::dynamic contentSizeData = folly::dynamic::object(
              "width", args.NewSize().Width)("height", args.NewSize().Height);
          folly::dynamic eventData = folly::dynamic::object("target", tag)(
              "contentSize", std::move(contentSizeData));
          if (!m_updating && instance != nullptr)
            instance->DispatchEvent(
                tag, "topTextInputContentSizeChange", std::move(eventData));
        }
      });

  if (textBox.ApplyTemplate()) {
    m_scrollViewerViewChangingRevoker =
        textBox.GetTemplateChild(asHstring("ContentElement"))
            .as<winrt::ScrollViewer>()
            .ViewChanging(
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

  if (textBox.try_as<winrt::IUIElement7>()) {
    m_textBoxCharacterReceivedRevoker = textBox.CharacterReceived(
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
  auto textBox = GetView().as<winrt::TextBox>();
  if (textBox == nullptr)
    return;

  auto control = textBox.as<winrt::Control>();

  const auto textBoxXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(textBox);
  const auto controlXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(control);

  for (auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (TryUpdateFontProperties(control, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateTextAlignment(textBox, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateCharacterSpacing(
                   control, propertyName, propertyValue)) {
      continue;
    } else if (propertyName == "multiline") {
      if (propertyValue.isBool())
      XamlDirectInstance::GetXamlDirect().SetEnumProperty(
          textBoxXD,
          XD::XamlPropertyIndex::TextBox_TextWrapping,
          static_cast<uint32_t>(
              propertyValue.asBool() ? winrt::TextWrapping::Wrap
                                     : winrt::TextWrapping::NoWrap));
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBoxXD, XD::XamlPropertyIndex::TextBox_TextWrapping);
    } else if (propertyName == "allowFontScaling") {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
            textBoxXD,
            XD::XamlPropertyIndex::Control_IsTextScaleFactorEnabled,
            propertyValue.asBool());
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBoxXD, XD::XamlPropertyIndex::Control_IsTextScaleFactorEnabled);
    } else if (propertyName == "clearTextOnFocus") {
      if (propertyValue.isBool())
        m_shouldClearTextOnFocus = propertyValue.asBool();
    } else if (propertyName == "editable") {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
            textBoxXD,
            XD::XamlPropertyIndex::TextBox_IsReadOnly,
            !propertyValue.asBool());
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBoxXD, XD::XamlPropertyIndex::TextBox_IsReadOnly);
    } else if (propertyName == "maxLength") {
      if (propertyValue.isNumber())
        XamlDirectInstance::GetXamlDirect().SetInt32Property(
            textBoxXD,
            XD::XamlPropertyIndex::TextBox_MaxLength,
            static_cast<int32_t>(propertyValue.asDouble()));
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBoxXD, XD::XamlPropertyIndex::TextBox_MaxLength);
    } else if (propertyName == "placeholder") {
      if (propertyValue.isString())
        XamlDirectInstance::GetXamlDirect().SetStringProperty(
            textBoxXD,
            XD::XamlPropertyIndex::TextBox_PlaceholderText,
            asHstring(propertyValue));
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBoxXD, XD::XamlPropertyIndex::TextBox_PlaceholderText);
    } else if (propertyName == "placeholderTextColor") {
      if (textBox.try_as<winrt::ITextBlock6>()) {
        if (IsValidColorValue(propertyValue))
          XamlDirectInstance::GetXamlDirect().SetColorProperty(
              textBoxXD,
              XD::XamlPropertyIndex::TextBox_PlaceholderForeground,
              SolidColorBrushFrom(propertyValue).Color());
        else if (propertyValue.isNull())
          XamlDirectInstance::GetXamlDirect().ClearProperty(
              textBoxXD, XD::XamlPropertyIndex::TextBox_PlaceholderForeground);
      }
    } else if (propertyName == "scrollEnabled") {
      if (propertyValue.isBool() &&
          textBox.TextWrapping() == winrt::TextWrapping::Wrap) {
        auto scrollMode = propertyValue.asBool() ? winrt::ScrollMode::Auto
                                                 : winrt::ScrollMode::Disabled;
        XamlDirectInstance::GetXamlDirect().SetEnumProperty(
            textBoxXD,
            XD::XamlPropertyIndex::ScrollViewer_VerticalScrollMode,
            static_cast<uint32_t>(scrollMode));
        XamlDirectInstance::GetXamlDirect().SetEnumProperty(
            textBoxXD,
            XD::XamlPropertyIndex::ScrollViewer_HorizontalScrollMode,
            static_cast<uint32_t>(scrollMode));
      }
    } else if (propertyName == "selection") {
      if (propertyValue.isObject()) {
        auto selection = json_type_traits<Selection>::parseJson(propertyValue);

        if (selection.isValid())
          textBox.Select(selection.start, selection.end - selection.start);
      }
    } else if (propertyName == "selectionColor") {
      if (IsValidColorValue(propertyValue))
        XamlDirectInstance::GetXamlDirect().SetColorProperty(
            textBoxXD,
            XD::XamlPropertyIndex::TextBox_SelectionHighlightColor,
            SolidColorBrushFrom(propertyValue).Color());
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBoxXD, XD::XamlPropertyIndex::TextBox_SelectionHighlightColor);
    } else if (propertyName == "selectTextOnFocus") {
      if (propertyValue.isBool())
        m_shouldSelectTextOnFocus = propertyValue.asBool();
    } else if (propertyName == "spellCheck") {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
            textBoxXD,
            XD::XamlPropertyIndex::TextBox_IsSpellCheckEnabled,
            propertyValue.asBool());
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBoxXD, XD::XamlPropertyIndex::TextBox_IsSpellCheckEnabled);
    } else if (propertyName == "text") {
      if (m_mostRecentEventCount == m_nativeEventCount) {
        if (propertyValue.isString()) {
          auto oldValue = XamlDirectInstance::GetXamlDirect().GetStringProperty(
              textBoxXD, XD::XamlPropertyIndex::TextBox_Text);
          auto newValue = asHstring(propertyValue);
          if (oldValue != newValue) {
            XamlDirectInstance::GetXamlDirect().SetStringProperty(
                textBoxXD, XD::XamlPropertyIndex::TextBox_Text, newValue);
          }
        } else if (propertyValue.isNull())
          XamlDirectInstance::GetXamlDirect().ClearProperty(
              textBoxXD, XD::XamlPropertyIndex::TextBox_Text);
      }
    } else if (propertyName == "mostRecentEventCount") {
      if (propertyValue.isNumber()) {
        m_mostRecentEventCount = static_cast<uint32_t>(propertyValue.asInt());
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
      "text", "string")("mostRecentEventCount", "int"));

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

} // namespace uwp
} // namespace react
