// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "TextInputViewManager.h"

#include "Unicode.h"

#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Shapes.h>
#include <Utils/PropertyHandlerUtils.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

#ifdef USE_WINUI3
namespace winrt::Microsoft::UI::Xaml::Controls {
using IPasswordBox4 = ::xaml::Controls::IPasswordBox;
using ITextBox6 = ::xaml::Controls::ITextBox;
}; // namespace winrt::Microsoft::UI::Xaml::Controls
#endif

namespace winrt {
using namespace xaml;
using namespace xaml::Media;
using namespace xaml::Shapes;
} // namespace winrt

namespace react::uwp {
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
} // namespace react::uwp

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

static const std::unordered_map<std::string, xaml::Input::InputScopeNameValue> textBoxKeyboardTypeMap = {
    {"default", xaml::Input::InputScopeNameValue::Default},
    {"number-pad", xaml::Input::InputScopeNameValue::TelephoneNumber},
    {"decimal-pad", xaml::Input::InputScopeNameValue::Digits},
    {"email-address", xaml::Input::InputScopeNameValue::EmailNameOrAddress},
    {"phone-pad", xaml::Input::InputScopeNameValue::TelephoneNumber},
    {"numeric", xaml::Input::InputScopeNameValue::Number},
    {"url", xaml::Input::InputScopeNameValue::Url},
    {"web-search", xaml::Input::InputScopeNameValue::Search}};

static const std::unordered_map<std::string, xaml::Input::InputScopeNameValue> passwordBoxKeyboardTypeMap = {
    {"default", xaml::Input::InputScopeNameValue::Password},
    {"numeric", xaml::Input::InputScopeNameValue::NumericPin}};

static xaml::Input::InputScopeNameValue parseKeyboardType(const folly::dynamic &val, const bool isTextBox) {
  auto keyboardTypeMap = isTextBox ? textBoxKeyboardTypeMap : passwordBoxKeyboardTypeMap;

  auto iter = keyboardTypeMap.find(val.asString());

  if (iter != keyboardTypeMap.end()) {
    return iter->second;
  }

  return isTextBox ? xaml::Input::InputScopeNameValue::Default : xaml::Input::InputScopeNameValue::Password;
}

namespace react::uwp {

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
  void HideCaretIfNeeded();
  void setPasswordBoxPlaceholderForeground(xaml::Controls::PasswordBox passwordBox, folly::dynamic color);
  winrt::Shape FindCaret(xaml::DependencyObject element);

  bool m_shouldClearTextOnFocus = false;
  bool m_shouldSelectTextOnFocus = false;
  bool m_contextMenuHidden = false;
  bool m_hideCaret = false;
  bool m_isTextBox = true;
  folly::dynamic m_placeholderTextColor;

  // Javascripts is running in a different thread. If the typing is very fast,
  // It's possible that two TextChanged are raised but TextInput just got the
  // first response from Javascript. So the first response should be dropped.
  // EventCount is introduced to resolve this problem
  uint32_t m_nativeEventCount{0}; // EventCount to javascript
  uint32_t m_mostRecentEventCount{0}; // EventCount from javascript

 private:
  xaml::Controls::TextBox::TextChanging_revoker m_textBoxTextChangingRevoker{};
  xaml::Controls::TextBox::TextChanged_revoker m_textBoxTextChangedRevoker{};
  xaml::Controls::TextBox::SelectionChanged_revoker m_textBoxSelectionChangedRevoker{};
  xaml::Controls::TextBox::ContextMenuOpening_revoker m_textBoxContextMenuOpeningRevoker{};

  xaml::Controls::PasswordBox::PasswordChanging_revoker m_passwordBoxPasswordChangingRevoker{};
  xaml::Controls::PasswordBox::PasswordChanged_revoker m_passwordBoxPasswordChangedRevoker{};
  xaml::Controls::PasswordBox::ContextMenuOpening_revoker m_passwordBoxContextMenuOpeningRevoker{};

  xaml::Controls::Control::GotFocus_revoker m_controlGotFocusRevoker{};
  xaml::Controls::Control::LostFocus_revoker m_controlLostFocusRevoker{};
  xaml::Controls::Control::KeyDown_revoker m_controlKeyDownRevoker{};
  xaml::Controls::Control::SizeChanged_revoker m_controlSizeChangedRevoker{};
  xaml::Controls::Control::CharacterReceived_revoker m_controlCharacterReceivedRevoker{};
  xaml::Controls::ScrollViewer::ViewChanging_revoker m_scrollViewerViewChangingRevoker{};
  xaml::Controls::Control::Loaded_revoker m_controlLoadedRevoker{};
};

void TextInputShadowNode::createView() {
  Super::createView();
  registerEvents();
}

void TextInputShadowNode::dispatchTextInputChangeEvent(winrt::hstring newText) {
  auto wkinstance = GetViewManager()->GetReactInstance();

  if (auto instance = wkinstance.lock()) {
    m_nativeEventCount++;
    folly::dynamic eventData =
        folly::dynamic::object("target", m_tag)("text", HstringToDynamic(newText))("eventCount", m_nativeEventCount);
    instance->DispatchEvent(m_tag, "topTextInputChange", std::move(eventData));
  }
}

void TextInputShadowNode::registerEvents() {
  auto control = GetView().as<xaml::Controls::Control>();
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
    m_textBoxTextChangingRevoker = control.as<xaml::Controls::TextBox>().TextChanging(
        winrt::auto_revoke, [=](auto &&, auto &&) { m_nativeEventCount++; });
  } else {
    m_textBoxTextChangingRevoker = {};

    if (control.try_as<xaml::Controls::IPasswordBox4>()) {
      m_passwordBoxPasswordChangingRevoker = control.as<xaml::Controls::IPasswordBox4>().PasswordChanging(
          winrt::auto_revoke, [=](auto &&, auto &&) { m_nativeEventCount++; });
    }
  }

  if (m_isTextBox) {
    m_passwordBoxPasswordChangedRevoker = {};
    auto textBox = control.as<xaml::Controls::TextBox>();
    m_textBoxTextChangedRevoker = textBox.TextChanged(
        winrt::auto_revoke, [=](auto &&, auto &&) { dispatchTextInputChangeEvent(textBox.Text()); });
  } else {
    m_textBoxTextChangedRevoker = {};
    auto passwordBox = control.as<xaml::Controls::PasswordBox>();
    m_passwordBoxPasswordChangedRevoker = passwordBox.PasswordChanged(
        winrt::auto_revoke, [=](auto &&, auto &&) { dispatchTextInputChangeEvent(passwordBox.Password()); });
  }

  if (m_isTextBox) {
    m_passwordBoxContextMenuOpeningRevoker = {};
    auto textBox = control.as<xaml::Controls::TextBox>();
    m_textBoxContextMenuOpeningRevoker =
        textBox.ContextMenuOpening(winrt::auto_revoke, [=](auto &&, xaml::Controls::ContextMenuEventArgs const &e) {
          if (m_contextMenuHidden) {
            e.Handled(true);
          }
        });
  } else {
    m_textBoxContextMenuOpeningRevoker = {};
    auto passwordBox = control.as<xaml::Controls::PasswordBox>();
    m_passwordBoxContextMenuOpeningRevoker =
        passwordBox.ContextMenuOpening(winrt::auto_revoke, [=](auto &&, xaml::Controls::ContextMenuEventArgs const &e) {
          if (m_contextMenuHidden) {
            e.Handled(true);
          }
        });
  }

  m_controlGotFocusRevoker = control.GotFocus(winrt::auto_revoke, [=](auto &&, auto &&) {
    if (m_shouldClearTextOnFocus) {
      if (m_isTextBox) {
        control.as<xaml::Controls::TextBox>().ClearValue(xaml::Controls::TextBox::TextProperty());
      } else {
        control.as<xaml::Controls::PasswordBox>().ClearValue(xaml::Controls::PasswordBox::PasswordProperty());
      }
    }

    if (m_shouldSelectTextOnFocus) {
      if (m_isTextBox) {
        control.as<xaml::Controls::TextBox>().SelectAll();
      } else {
        control.as<xaml::Controls::PasswordBox>().SelectAll();
      }
    }
    HideCaretIfNeeded();

    auto instance = wkinstance.lock();
    folly::dynamic eventData = folly::dynamic::object("target", tag);
    if (!m_updating && instance != nullptr)
      instance->DispatchEvent(tag, "topTextInputFocus", std::move(eventData));
  });

  m_controlLostFocusRevoker = control.LostFocus(winrt::auto_revoke, [=](auto &&, auto &&) {
    auto instance = wkinstance.lock();
    folly::dynamic eventDataBlur = folly::dynamic::object("target", tag);
    folly::dynamic eventDataEndEditing = {};
    if (m_isTextBox) {
      eventDataEndEditing =
          folly::dynamic::object("target", tag)("text", HstringToDynamic(control.as<xaml::Controls::TextBox>().Text()));
    } else {
      eventDataEndEditing = folly::dynamic::object("target", tag)(
          "text", HstringToDynamic(control.as<xaml::Controls::PasswordBox>().Password()));
    }
    if (!m_updating && instance != nullptr) {
      instance->DispatchEvent(tag, "topTextInputBlur", std::move(eventDataBlur));
      instance->DispatchEvent(tag, "topTextInputEndEditing", std::move(eventDataEndEditing));
    }
  });

  m_controlKeyDownRevoker =
      control.KeyDown(winrt::auto_revoke, [=](auto &&, xaml::Input::KeyRoutedEventArgs const &args) {
        if (args.Key() == winrt::Windows::System::VirtualKey::Enter && !args.Handled()) {
          if (auto instance = wkinstance.lock()) {
            folly::dynamic eventDataSubmitEditing = {};
            if (m_isTextBox) {
              eventDataSubmitEditing = folly::dynamic::object("target", tag)(
                  "text", HstringToDynamic(control.as<xaml::Controls::TextBox>().Text()));
            } else {
              eventDataSubmitEditing = folly::dynamic::object("target", tag)(
                  "text", HstringToDynamic(control.as<xaml::Controls::PasswordBox>().Password()));
            }
            instance->DispatchEvent(tag, "topTextInputSubmitEditing", std::move(eventDataSubmitEditing));
          }
        }
      });

  if (m_isTextBox) {
    auto textBox = control.as<xaml::Controls::TextBox>();
    m_textBoxSelectionChangedRevoker = textBox.SelectionChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
      auto instance = wkinstance.lock();
      folly::dynamic selectionData = folly::dynamic::object("start", textBox.SelectionStart())(
          "end", textBox.SelectionStart() + textBox.SelectionLength());
      folly::dynamic eventData = folly::dynamic::object("target", tag)("selection", std::move(selectionData));
      if (!m_updating && instance != nullptr)
        instance->DispatchEvent(tag, "topTextInputSelectionChange", std::move(eventData));
    });
  }

  m_controlSizeChangedRevoker =
      control.SizeChanged(winrt::auto_revoke, [=](auto &&, winrt::SizeChangedEventArgs const &args) {
        if (m_isTextBox) {
          if (control.as<xaml::Controls::TextBox>().TextWrapping() == xaml::TextWrapping::Wrap) {
            auto instance = wkinstance.lock();
            folly::dynamic contentSizeData =
                folly::dynamic::object("width", args.NewSize().Width)("height", args.NewSize().Height);
            folly::dynamic eventData = folly::dynamic::object("target", tag)("contentSize", std::move(contentSizeData));
            if (!m_updating && instance != nullptr)
              instance->DispatchEvent(tag, "topTextInputContentSizeChange", std::move(eventData));
          }
        }
      });

  m_controlLoadedRevoker = control.Loaded(winrt::auto_revoke, [=](auto &&, auto &&) {
    auto textBoxView = control.GetTemplateChild(asHstring("ContentElement")).as<xaml::Controls::ScrollViewer>();
    if (textBoxView) {
      m_scrollViewerViewChangingRevoker = textBoxView.ViewChanging(
          winrt::auto_revoke, [=](auto &&, xaml::Controls::ScrollViewerViewChangingEventArgs const &args) {
            auto instance = wkinstance.lock();
            if (!m_updating && instance != nullptr) {
              folly::dynamic offsetData = folly::dynamic::object("x", args.FinalView().HorizontalOffset())(
                  "y", args.FinalView().VerticalOffset());
              folly::dynamic eventData = folly::dynamic::object("target", tag)("contentOffset", std::move(offsetData));
              instance->DispatchEvent(tag, "topTextInputOnScroll", std::move(eventData));
            }
          });
    }
    HideCaretIfNeeded();
  });

  if (control.try_as<xaml::IUIElement7>()) {
    m_controlCharacterReceivedRevoker = control.CharacterReceived(
        winrt::auto_revoke, [=](auto &&, xaml::Input::CharacterReceivedRoutedEventArgs const &args) {
          auto instance = wkinstance.lock();
          std::string key;
          wchar_t s[2] = L" ";
          s[0] = args.Character();
          key = Microsoft::Common::Unicode::Utf16ToUtf8(s, 1);

          if (key.compare("\r") == 0) {
            key = "Enter";
          } else if (key.compare("\b") == 0) {
            key = "Backspace";
          }

          if (!m_updating && instance != nullptr) {
            folly::dynamic eventData = folly::dynamic::object("target", tag)("key", folly::dynamic(key));
            instance->DispatchEvent(tag, "topTextInputKeyPress", std::move(eventData));
          }
        });
  }
}

xaml::Shapes::Shape TextInputShadowNode::FindCaret(xaml::DependencyObject element) {
  if (element == nullptr)
    return nullptr;

  if (auto shape = element.try_as<winrt::Shape>())
    return shape;

  int childrenCount = winrt::VisualTreeHelper::GetChildrenCount(element);
  for (int i = 0; i < childrenCount; i++) {
    auto result = FindCaret(winrt::VisualTreeHelper::GetChild(element, i));
    if (result != nullptr)
      return result;
  }

  return nullptr;
}

// hacking solution to hide the caret
void TextInputShadowNode::HideCaretIfNeeded() {
  if (m_hideCaret) {
    auto control = GetView().as<xaml::Controls::Control>();
    if (auto caret = FindCaret(control)) {
      caret.CompositeMode(xaml::Media::ElementCompositeMode::Inherit);
      xaml::Media::SolidColorBrush transparentColor(winrt::Colors::Transparent());
      caret.Fill(transparentColor);
    }
  }
}

void TextInputShadowNode::setPasswordBoxPlaceholderForeground(
    xaml::Controls::PasswordBox passwordBox,
    folly::dynamic color) {
  m_placeholderTextColor = color;
  auto defaultRD = xaml::ResourceDictionary();
  auto solidColorBrush = ColorFrom(color);
  defaultRD.Insert(winrt::box_value(L"TextControlPlaceholderForeground"), winrt::box_value(solidColorBrush));
  defaultRD.Insert(winrt::box_value(L"TextControlPlaceholderForegroundFocused"), winrt::box_value(solidColorBrush));
  defaultRD.Insert(winrt::box_value(L"TextControlPlaceholderForegroundPointerOver"), winrt::box_value(solidColorBrush));
  auto passwordBoxResource = xaml::ResourceDictionary();
  auto themeDictionaries = passwordBoxResource.ThemeDictionaries();
  themeDictionaries.Insert(winrt::box_value(L"Default"), defaultRD);
  passwordBoxResource.Insert(winrt::box_value(L"ThemeDictionaries"), themeDictionaries);
  passwordBox.Resources(passwordBoxResource);
}

void TextInputShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  auto control = GetView().as<xaml::Controls::Control>();
  auto textBox = control.try_as<xaml::Controls::TextBox>();
  auto passwordBox = control.try_as<xaml::Controls::PasswordBox>();

  for (auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    // Applicable properties for both TextBox and PasswordBox
    if (TryUpdateFontProperties(control, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateCharacterSpacing(control, propertyName, propertyValue)) {
      continue;
    } else if (propertyName == "allowFontScaling") {
      if (propertyValue.isBool())
        control.IsTextScaleFactorEnabled(propertyValue.asBool());
      else if (propertyValue.isNull())
        control.ClearValue(xaml::Controls::Control::IsTextScaleFactorEnabledProperty());
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
    } else if (propertyName == "caretHidden") {
      if (propertyValue.isBool()) {
        m_hideCaret = propertyValue.asBool();
        HideCaretIfNeeded();
      }
    } else if (propertyName == "secureTextEntry") {
      if (propertyValue.isBool()) {
        if (propertyValue.asBool()) {
          if (m_isTextBox) {
            xaml::Controls::PasswordBox newPasswordBox;
            ReparentView(newPasswordBox);
            m_isTextBox = false;
            registerEvents();
            control = newPasswordBox.as<xaml::Controls::Control>();
            passwordBox = newPasswordBox;
            if (!m_placeholderTextColor.isNull()) {
              setPasswordBoxPlaceholderForeground(newPasswordBox, m_placeholderTextColor);
            }
          }
        } else {
          if (!m_isTextBox) {
            xaml::Controls::TextBox newTextBox;
            ReparentView(newTextBox);
            m_isTextBox = true;
            registerEvents();
            control = newTextBox.as<xaml::Controls::Control>();
            textBox = newTextBox;
            if (!m_placeholderTextColor.isNull()) {
              textBox.PlaceholderForeground(SolidColorBrushFrom(m_placeholderTextColor));
            }
          }
        }
      }
    } else if (propertyName == "maxLength") {
      if (propertyValue.isNumber()) {
        control.SetValue(
            m_isTextBox ? xaml::Controls::TextBox::MaxLengthProperty()
                        : xaml::Controls::PasswordBox::MaxLengthProperty(),
            winrt::PropertyValue::CreateInt32(static_cast<int32_t>(propertyValue.asDouble())));
      } else if (propertyValue.isNull()) {
        control.ClearValue(
            m_isTextBox ? xaml::Controls::TextBox::MaxLengthProperty()
                        : xaml::Controls::PasswordBox::MaxLengthProperty());
      }
    } else if (propertyName == "placeholder") {
      if (propertyValue.isString()) {
        control.SetValue(
            m_isTextBox ? xaml::Controls::TextBox::PlaceholderTextProperty()
                        : xaml::Controls::PasswordBox::PlaceholderTextProperty(),
            winrt::PropertyValue::CreateString(asHstring(propertyValue)));
      } else if (propertyValue.isNull()) {
        control.ClearValue(
            m_isTextBox ? xaml::Controls::TextBox::PlaceholderTextProperty()
                        : xaml::Controls::PasswordBox::PlaceholderTextProperty());
      }
    } else if (propertyName == "selectionColor") {
      if (IsValidColorValue(propertyValue)) {
        control.SetValue(
            m_isTextBox ? xaml::Controls::TextBox::SelectionHighlightColorProperty()
                        : xaml::Controls::PasswordBox::SelectionHighlightColorProperty(),
            SolidColorBrushFrom(propertyValue));
      } else if (propertyValue.isNull())
        control.ClearValue(
            m_isTextBox ? xaml::Controls::TextBox::SelectionHighlightColorProperty()
                        : xaml::Controls::PasswordBox::SelectionHighlightColorProperty());
    } else if (propertyName == "keyboardType") {
      if (propertyValue.isString()) {
        auto inputScopeNameVaue = parseKeyboardType(propertyValue, m_isTextBox);
        auto scope = xaml::Input::InputScope();
        auto scopeName = xaml::Input::InputScopeName(inputScopeNameVaue);
        auto names = scope.Names();
        names.Append(scopeName);
        control.SetValue(
            m_isTextBox ? xaml::Controls::TextBox::InputScopeProperty()
                        : xaml::Controls::PasswordBox::InputScopeProperty(),
            scope);
      } else if (propertyValue.isNull())
        control.ClearValue(
            m_isTextBox ? xaml::Controls::TextBox::InputScopeProperty()
                        : xaml::Controls::PasswordBox::InputScopeProperty());
    } else if (propertyName == "placeholderTextColor") {
      m_placeholderTextColor = nullptr;
      if (textBox.try_as<xaml::Controls::ITextBox6>() && m_isTextBox) {
        if (IsValidColorValue(propertyValue)) {
          m_placeholderTextColor = propertyValue;
          textBox.PlaceholderForeground(SolidColorBrushFrom(propertyValue));
        } else if (propertyValue.isNull())
          textBox.ClearValue(xaml::Controls::TextBox::PlaceholderForegroundProperty());
      } else if (m_isTextBox != true && IsValidColorValue(propertyValue)) {
        setPasswordBoxPlaceholderForeground(passwordBox, propertyValue);
      }
    } else {
      if (m_isTextBox) { // Applicable properties for TextBox
        if (TryUpdateTextAlignment(textBox, propertyName, propertyValue)) {
          continue;
        } else if (propertyName == "multiline") {
          if (propertyValue.isBool()) {
            const bool isMultiline = propertyValue.asBool();
            textBox.TextWrapping(isMultiline ? xaml::TextWrapping::Wrap : xaml::TextWrapping::NoWrap);
            textBox.AcceptsReturn(isMultiline);
          } else if (propertyValue.isNull())
            textBox.ClearValue(xaml::Controls::TextBox::TextWrappingProperty());
        } else if (propertyName == "editable") {
          if (propertyValue.isBool())
            textBox.IsReadOnly(!propertyValue.asBool());
          else if (propertyValue.isNull())
            textBox.ClearValue(xaml::Controls::TextBox::IsReadOnlyProperty());
        } else if (propertyName == "scrollEnabled") {
          if (propertyValue.isBool() && textBox.TextWrapping() == xaml::TextWrapping::Wrap) {
            auto scrollMode =
                propertyValue.asBool() ? xaml::Controls::ScrollMode::Auto : xaml::Controls::ScrollMode::Disabled;
            xaml::Controls::ScrollViewer::SetVerticalScrollMode(textBox, scrollMode);
            xaml::Controls::ScrollViewer::SetHorizontalScrollMode(textBox, scrollMode);
          }
        } else if (propertyName == "selection") {
          if (propertyValue.isObject()) {
            auto selection = json_type_traits<Selection>::parseJson(propertyValue);

            if (selection.isValid())
              textBox.Select(selection.start, selection.end - selection.start);
          }
        } else if (propertyName == "spellCheck") {
          if (propertyValue.isBool())
            textBox.IsSpellCheckEnabled(propertyValue.asBool());
          else if (propertyValue.isNull())
            textBox.ClearValue(xaml::Controls::TextBox::IsSpellCheckEnabledProperty());
        } else if (propertyName == "text") {
          if (m_mostRecentEventCount == m_nativeEventCount) {
            if (propertyValue.isString()) {
              auto oldValue = textBox.Text();
              auto newValue = asHstring(propertyValue);
              if (oldValue != newValue) {
                textBox.Text(newValue);
              }
            } else if (propertyValue.isNull())
              textBox.ClearValue(xaml::Controls::TextBox::TextProperty());
          }
        } else if (propertyName == "autoCapitalize") {
          if (textBox.try_as<xaml::Controls::ITextBox6>()) {
            if (propertyValue.isString()) {
              if (propertyValue.asString() == "characters") {
                textBox.CharacterCasing(xaml::Controls::CharacterCasing::Upper);
              } else { // anything else turns off autoCap (should be "None" but
                       // we don't support "words"/"senetences" yet)
                textBox.CharacterCasing(xaml::Controls::CharacterCasing::Normal);
              }
            } else if (propertyValue.isNull())
              textBox.ClearValue(xaml::Controls::TextBox::CharacterCasingProperty());
          }
        }
      } else { // Applicable properties for PasswordBox
        if (propertyName == "text" && !m_isTextBox) {
          if (m_mostRecentEventCount == m_nativeEventCount) {
            if (propertyValue.isString()) {
              auto oldValue = passwordBox.Password();
              auto newValue = asHstring(propertyValue);
              if (oldValue != newValue) {
                passwordBox.Password(newValue);
              }
            } else if (propertyValue.isNull())
              passwordBox.ClearValue(xaml::Controls::PasswordBox::PasswordProperty());
          }
        }
      }
    }
  }

  Super::updateProperties(std::move(props));
  m_updating = false;
}

TextInputViewManager::TextInputViewManager(const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *TextInputViewManager::GetName() const {
  return "RCTTextInput";
}

folly::dynamic TextInputViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("allowFontScaling", "boolean")("clearTextOnFocus", "boolean")(
      "editable", "boolean")("maxLength", "int")("multiline", "boolean")("placeholder", "string")(
      "placeholderTextColor", "Color")("scrollEnabled", "boolean")("selection", "Map")("selectionColor", "Color")(
      "selectTextOnFocus", "boolean")("spellCheck", "boolean")("text", "string")("mostRecentEventCount", "int")(
      "secureTextEntry", "boolean")("keyboardType", "string")("contextMenuHidden", "boolean")("caretHidden", "boolean")(
      "autoCapitalize", "string"));

  return props;
}

folly::dynamic TextInputViewManager::GetExportedCustomDirectEventTypeConstants() const {
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topTextInputChange"] = folly::dynamic::object("registrationName", "onChange");
  directEvents["topTextInputFocus"] = folly::dynamic::object("registrationName", "onFocus");
  directEvents["topTextInputBlur"] = folly::dynamic::object("registrationName", "onBlur");
  directEvents["topTextInputEndEditing"] = folly::dynamic::object("registrationName", "onEndEditing");
  directEvents["topTextInputSelectionChange"] = folly::dynamic::object("registrationName", "onSelectionChange");
  directEvents["topTextInputContentSizeChange"] = folly::dynamic::object("registrationName", "onContentSizeChange");
  directEvents["topTextInputKeyPress"] = folly::dynamic::object("registrationName", "onKeyPress");
  directEvents["topTextInputOnScroll"] = folly::dynamic::object("registrationName", "onScroll");
  directEvents["topTextInputSubmitEditing"] = folly::dynamic::object("registrationName", "onSubmitEditing");

  return directEvents;
}

facebook::react::ShadowNode *TextInputViewManager::createShadow() const {
  return new TextInputShadowNode();
}

XamlView TextInputViewManager::CreateViewCore(int64_t /*tag*/) {
  xaml::Controls::TextBox textBox;
  textBox.TabIndex(0);
  return textBox;
}

YGMeasureFunc TextInputViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

void TextInputViewManager::TransferInputScope(
    const XamlView &oldView,
    const XamlView &newView,
    const bool copyToPasswordBox) {
  // transfer input scope, only common keyboardType between secureTextEntry
  // on/off is numeric, so only need to transfer input scope "Number" <=>
  // "NumericPin", everything else leave it as default.
  xaml::Input::InputScope inputScope;
  if (copyToPasswordBox) {
    inputScope = oldView.try_as<xaml::Controls::TextBox>().InputScope();
  } else {
    inputScope = oldView.try_as<xaml::Controls::PasswordBox>().InputScope();
  }

  if (inputScope != nullptr) {
    auto nameValue = inputScope.Names().GetAt(0).NameValue();

    if ((nameValue == xaml::Input::InputScopeNameValue::Number && copyToPasswordBox) ||
        (nameValue == xaml::Input::InputScopeNameValue::NumericPin && !copyToPasswordBox)) {
      auto newScope = xaml::Input::InputScope();
      auto scopeName = xaml::Input::InputScopeName(
          copyToPasswordBox ? xaml::Input::InputScopeNameValue::NumericPin : xaml::Input::InputScopeNameValue::Number);
      auto names = newScope.Names();
      names.Append(scopeName);
      newView.SetValue(
          copyToPasswordBox ? xaml::Controls::PasswordBox::InputScopeProperty()
                            : xaml::Controls::TextBox::InputScopeProperty(),
          newScope);
    }
  }
}

void TextInputViewManager::TransferProperties(const XamlView &oldView, const XamlView &newView) {
  if ((oldView.try_as<xaml::Controls::TextBox>() != nullptr &&
       newView.try_as<xaml::Controls::PasswordBox>() != nullptr) ||
      (oldView.try_as<xaml::Controls::PasswordBox>() != nullptr &&
       newView.try_as<xaml::Controls::TextBox>() != nullptr)) {
    bool copyToPasswordBox = oldView.try_as<xaml::Controls::TextBox>() != nullptr;

    // sync common properties between TextBox and PasswordBox
    if (copyToPasswordBox) {
      TransferProperty(
          oldView,
          newView,
          xaml::Controls::TextBox::MaxLengthProperty(),
          xaml::Controls::PasswordBox::MaxLengthProperty());
      TransferProperty(
          oldView,
          newView,
          xaml::Controls::TextBox::PlaceholderTextProperty(),
          xaml::Controls::PasswordBox::PlaceholderTextProperty());
      TransferProperty(
          oldView,
          newView,
          xaml::Controls::TextBox::SelectionHighlightColorProperty(),
          xaml::Controls::PasswordBox::SelectionHighlightColorProperty());
      newView.as<xaml::Controls::PasswordBox>().Password(oldView.as<xaml::Controls::TextBox>().Text());
    } else {
      TransferProperty(
          oldView,
          newView,
          xaml::Controls::PasswordBox::MaxLengthProperty(),
          xaml::Controls::TextBox::MaxLengthProperty());
      TransferProperty(
          oldView,
          newView,
          xaml::Controls::PasswordBox::PlaceholderTextProperty(),
          xaml::Controls::TextBox::PlaceholderTextProperty());
      TransferProperty(
          oldView,
          newView,
          xaml::Controls::PasswordBox::SelectionHighlightColorProperty(),
          xaml::Controls::TextBox::SelectionHighlightColorProperty());
      newView.as<xaml::Controls::TextBox>().Text(oldView.as<xaml::Controls::PasswordBox>().Password());
    }

    TransferInputScope(oldView, newView, copyToPasswordBox);
    // Set focus if current control has focus
    auto focusState = oldView.as<xaml::Controls::Control>().FocusState();
    if (focusState != winrt::FocusState::Unfocused) {
      newView.as<xaml::Controls::Control>().Focus(focusState);
    }
  }
  Super::TransferProperties(oldView, newView);
}
} // namespace react::uwp
