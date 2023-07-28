// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>

#include <Views/ExpressionAnimationStore.h>
#include <Views/FrameworkElementTransferProperties.h>
#include <Views/FrameworkElementViewManager.h>

#include <JSValueWriter.h>
#include <Utils/AccessibilityUtils.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <WindowsNumerics.h>
#include <winrt/Windows.Foundation.h>

#include <UI.Xaml.Automation.Peers.h>
#include <UI.Xaml.Automation.h>
#include <UI.Xaml.Controls.h>
#include "Utils/PropertyHandlerUtils.h"

#include "DynamicAutomationProperties.h"

#include <Views/ViewPanel.h>
#include "Unicode.h"
#include "cdebug.h"

namespace winrt {
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Automation;
using namespace xaml::Automation::Peers;
using namespace Windows::Foundation::Collections;
} // namespace winrt

template <>
struct json_type_traits<winrt::Microsoft::ReactNative::AccessibilityAction> {
  static winrt::Microsoft::ReactNative::AccessibilityAction parseJson(
      const winrt::Microsoft::ReactNative::JSValue &json) {
    auto action = winrt::Microsoft::ReactNative::AccessibilityAction();

    for (auto &item : json.AsObject()) {
      if (item.first == "name") {
        action.Name = Microsoft::ReactNative::asHstring(item.second);
      } else if (item.first == "label") {
        action.Label = Microsoft::ReactNative::asHstring(item.second);
      }
    }
    return action;
  }
};

template <>
struct json_type_traits<winrt::IVector<winrt::Microsoft::ReactNative::AccessibilityAction>> {
  static winrt::IVector<winrt::Microsoft::ReactNative::AccessibilityAction> parseJson(
      const winrt::Microsoft::ReactNative::JSValue &json) {
    auto vector = winrt::single_threaded_vector<winrt::Microsoft::ReactNative::AccessibilityAction>();

    if (json.Type() == winrt::Microsoft::ReactNative::JSValueType::Array) {
      for (const auto &action : json.AsArray()) {
        if (action.Type() != winrt::Microsoft::ReactNative::JSValueType::Object)
          continue;

        vector.Append(json_type_traits<winrt::Microsoft::ReactNative::AccessibilityAction>::parseJson(action));
      }
    }
    return vector;
  }
};

namespace Microsoft::ReactNative {

using DynamicAutomationProperties = Microsoft::ReactNative::DynamicAutomationProperties;

FrameworkElementViewManager::FrameworkElementViewManager(const Mso::React::IReactContext &context) : Super(context) {}

void FrameworkElementViewManager::TransferProperties(const XamlView &oldView, const XamlView &newView) {
  TransferFrameworkElementProperties(oldView, newView);
}

static void GetAccessibilityStateProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) {
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"selected", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"disabled", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"checked", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"busy", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"expanded", L"boolean");
  writer.WriteObjectEnd();
}

static void GetAccessibilityValueProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) {
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"min", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"max", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"now", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"text", L"string");
  writer.WriteObjectEnd();
}

inline float ToRadians(const winrt::Microsoft::ReactNative::JSValue &value) {
  if ((value.Type() == winrt::Microsoft::ReactNative::JSValueType::Double)) {
    return value.AsSingle();
  }
  assert(value.Type() == winrt::Microsoft::ReactNative::JSValueType::String);

  auto stringValue = value.AsString();
  char *suffixStart;
  double num = strtod(stringValue.c_str(), &suffixStart);
  if (0 == strncmp(suffixStart, "deg", 3)) {
    return static_cast<float>(num * M_PI / 180.0f);
  }
  return static_cast<float>(num); // assume suffix is "rad"
}

static void MultiplyInto(
    winrt::Windows::Foundation::Numerics::float4x4 &m,
    winrt::Windows::Foundation::Numerics::float4x4 o) {
  m = o * m;
}

void FrameworkElementViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessible", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityRole", L"string");
  writer.WritePropertyName(L"accessibilityState");
  GetAccessibilityStateProps(writer);
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityHint", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityLabel", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityLevel", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityPosInSet", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilitySetSize", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"testID", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"tooltip", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityActions", L"array");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityLiveRegion", L"string");
  writer.WritePropertyName(L"accessibilityValue");
  GetAccessibilityValueProps(writer);
}

bool FrameworkElementViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto element(nodeToUpdate->GetView().as<xaml::FrameworkElement>());
  if (element != nullptr) {
    if (propertyName == "opacity") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        double opacity = propertyValue.AsDouble();
        if (opacity >= 0 && opacity <= 1)
          element.Opacity(opacity);
        // else
        // TODO report error
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::UIElement::OpacityProperty());
      }
    } else if (propertyName == "transform") {
      if (element.try_as<xaml::IUIElement10>()) // Works on 19H1+
      {
        if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Array) {
          winrt::Windows::Foundation::Numerics::float4x4 transformMatrix{
              winrt::Windows::Foundation::Numerics::float4x4::identity()};
          for (const auto &transform : propertyValue.AsArray()) {
            for (const auto &operation : transform.AsObject()) {
              const std::string &transformType = operation.first;
              const auto &innerValue = operation.second;

              if (transformType == "matrix") {
                assert(innerValue.AsArray().size() == 16);
                winrt::Windows::Foundation::Numerics::float4x4 innerMatrix;
                innerMatrix.m11 = static_cast<float>(innerValue[0].AsDouble());
                innerMatrix.m12 = static_cast<float>(innerValue[1].AsDouble());
                innerMatrix.m13 = static_cast<float>(innerValue[2].AsDouble());
                innerMatrix.m14 = static_cast<float>(innerValue[3].AsDouble());
                innerMatrix.m21 = static_cast<float>(innerValue[4].AsDouble());
                innerMatrix.m22 = static_cast<float>(innerValue[5].AsDouble());
                innerMatrix.m23 = static_cast<float>(innerValue[6].AsDouble());
                innerMatrix.m24 = static_cast<float>(innerValue[7].AsDouble());
                innerMatrix.m31 = static_cast<float>(innerValue[8].AsDouble());
                innerMatrix.m32 = static_cast<float>(innerValue[9].AsDouble());
                innerMatrix.m33 = static_cast<float>(innerValue[10].AsDouble());
                innerMatrix.m34 = static_cast<float>(innerValue[11].AsDouble());
                innerMatrix.m41 = static_cast<float>(innerValue[12].AsDouble());
                innerMatrix.m42 = static_cast<float>(innerValue[13].AsDouble());
                innerMatrix.m43 = static_cast<float>(innerValue[14].AsDouble());
                innerMatrix.m44 = static_cast<float>(innerValue[15].AsDouble());
                MultiplyInto(transformMatrix, innerMatrix);
              } else if (transformType == "perspective") {
                auto innerMatrix = winrt::Windows::Foundation::Numerics::float4x4::identity();
                innerMatrix.m34 = -1 / innerValue.AsSingle();
                MultiplyInto(transformMatrix, innerMatrix);
              } else if (transformType == "rotateX") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::make_float4x4_rotation_x(ToRadians(innerValue)));
              } else if (transformType == "rotateY") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::make_float4x4_rotation_y(ToRadians(innerValue)));
              } else if (transformType == "rotate" || transformType == "rotateZ") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::make_float4x4_rotation_z(ToRadians(innerValue)));
              } else if (transformType == "scale") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::make_float4x4_scale(
                        innerValue.AsSingle(), innerValue.AsSingle(), 1));
              } else if (transformType == "scaleX") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::make_float4x4_scale(innerValue.AsSingle(), 1, 1));
              } else if (transformType == "scaleY") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::make_float4x4_scale(1, innerValue.AsSingle(), 1));
              } else if (transformType == "translate") {
                auto &params = innerValue.AsArray();
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::make_float4x4_translation(
                        params[0].AsSingle(), params[1].AsSingle(), params.size() > 2 ? params[2].AsSingle() : 0.f));
              } else if (transformType == "translateX") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::make_float4x4_translation(innerValue.AsSingle(), 0.f, 0.f));
              } else if (transformType == "translateY") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::make_float4x4_translation(0.f, innerValue.AsSingle(), 0.f));
              } else if (transformType == "skewX") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::float4x4(
                        winrt::Windows::Foundation::Numerics::make_float3x2_skew(ToRadians(innerValue), 0.f)));
              } else if (transformType == "skewY") {
                MultiplyInto(
                    transformMatrix,
                    winrt::Windows::Foundation::Numerics::float4x4(
                        winrt::Windows::Foundation::Numerics::make_float3x2_skew(0.f, ToRadians(innerValue))));
              }
            }
          }

          if (!element.IsLoaded()) {
            element.Loaded([=](auto sender, auto &&) -> auto{
              ApplyTransformMatrix(sender.as<xaml::UIElement>(), nodeToUpdate, transformMatrix);
            });
          } else {
            ApplyTransformMatrix(element, nodeToUpdate, transformMatrix);
          }
        } else if (propertyValue.IsNull()) {
          element.TransformMatrix(winrt::Windows::Foundation::Numerics::float4x4::identity());
        }
      } else {
        cdebug << "[Dim down] " << propertyName << "\n";
      }
    } else if (propertyName == "accessibilityHint") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        auto value = asHstring(propertyValue);
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

        element.SetValue(xaml::Automation::AutomationProperties::HelpTextProperty(), boxedValue);
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::Automation::AutomationProperties::HelpTextProperty());
      }
    } else if (propertyName == "accessibilityLabel") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        auto value = asHstring(propertyValue);
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

        element.SetValue(xaml::Automation::AutomationProperties::NameProperty(), boxedValue);
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::Automation::AutomationProperties::NameProperty());
      }
      AnnounceLiveRegionChangedIfNeeded(element);
    } else if (propertyName == "accessible") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
        if (propertyValue.AsBoolean()) {
          xaml::Automation::AutomationProperties::SetAccessibilityView(element, winrt::AccessibilityView::Content);
        } else {
          xaml::Automation::AutomationProperties::SetAccessibilityView(element, winrt::AccessibilityView::Raw);
        }
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::Automation::AutomationProperties::AccessibilityViewProperty());
      }
    } else if (propertyName == "accessibilityLevel") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        auto value = static_cast<int>(propertyValue.AsDouble());
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateInt32(value);
        element.SetValue(xaml::Automation::AutomationProperties::LevelProperty(), boxedValue);
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::Automation::AutomationProperties::LevelProperty());
      }
    } else if (propertyName == "accessibilityLiveRegion") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        auto value = propertyValue.AsString();

        auto liveSetting = winrt::AutomationLiveSetting::Off;

        if (value == "polite") {
          liveSetting = winrt::AutomationLiveSetting::Polite;
        } else if (value == "assertive") {
          liveSetting = winrt::AutomationLiveSetting::Assertive;
        }

        element.SetValue(xaml::Automation::AutomationProperties::LiveSettingProperty(), winrt::box_value(liveSetting));
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::Automation::AutomationProperties::LiveSettingProperty());
      }
      AnnounceLiveRegionChangedIfNeeded(element);
    } else if (propertyName == "accessibilityPosInSet") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        auto value = static_cast<int>(propertyValue.AsDouble());
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateInt32(value);

        element.SetValue(xaml::Automation::AutomationProperties::PositionInSetProperty(), boxedValue);
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::Automation::AutomationProperties::PositionInSetProperty());
      }
    } else if (propertyName == "accessibilitySetSize") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        auto value = static_cast<int>(propertyValue.AsDouble());
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateInt32(value);

        element.SetValue(xaml::Automation::AutomationProperties::SizeOfSetProperty(), boxedValue);
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::Automation::AutomationProperties::SizeOfSetProperty());
      }
    } else if (propertyName == "accessibilityRole") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        const std::string &role = propertyValue.AsString();
        if (role == "none")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::None);
        else if (role == "button")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Button);
        else if (role == "link")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Link);
        else if (role == "search")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Search);
        else if (role == "image")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Image);
        else if (role == "keyboardkey")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::KeyboardKey);
        else if (role == "text")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Text);
        else if (role == "adjustable")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Adjustable);
        else if (role == "imagebutton")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::ImageButton);
        else if (role == "header")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Header);
        else if (role == "summary")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Summary);
        else if (role == "alert")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Alert);
        else if (role == "checkbox")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::CheckBox);
        else if (role == "combobox")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::ComboBox);
        else if (role == "menu")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Menu);
        else if (role == "menubar")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::MenuBar);
        else if (role == "menuitem")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::MenuItem);
        else if (role == "progressbar")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::ProgressBar);
        else if (role == "radio")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Radio);
        else if (role == "radiogroup")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::RadioGroup);
        else if (role == "scrollbar")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::ScrollBar);
        else if (role == "spinbutton")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::SpinButton);
        else if (role == "switch")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Switch);
        else if (role == "tab")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Tab);
        else if (role == "tablist")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::TabList);
        else if (role == "timer")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Timer);
        else if (role == "togglebutton")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::ToggleButton);
        else if (role == "toolbar")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::ToolBar);
        else if (role == "list")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::List);
        else if (role == "listitem")
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::ListItem);
        else
          DynamicAutomationProperties::SetAccessibilityRole(
              element, winrt::Microsoft::ReactNative::AccessibilityRoles::Unknown);
      } else if (propertyValue.IsNull()) {
        element.ClearValue(DynamicAutomationProperties::AccessibilityRoleProperty());
      }
    } else if (propertyName == "accessibilityState") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Object) {
        for (const auto &pair : propertyValue.AsObject()) {
          // States are provided with the name and either a valid value if the state is supported or null if the state
          // isn't
          const std::string &innerName = pair.first;
          const auto &innerValue = pair.second;

          xaml::DependencyProperty targetDP = nullptr;
          xaml::Automation::AutomationProperty targetAP = nullptr;

          bool valueChanged = false;

          winrt::IInspectable oldDPValue = xaml::DependencyProperty::UnsetValue();
          winrt::IInspectable newDPValue = xaml::DependencyProperty::UnsetValue();

          winrt::IInspectable oldAPValue = nullptr;
          winrt::IInspectable newAPValue = nullptr;

          // Determine which attached dependency property to update and which automation property to raise a changed
          // event
          if (innerName == "selected") {
            targetDP = DynamicAutomationProperties::AccessibilityStateSelectedProperty();
            targetAP = winrt::SelectionItemPatternIdentifiers::IsSelectedProperty();
          } else if (innerName == "disabled") {
            targetDP = DynamicAutomationProperties::AccessibilityStateDisabledProperty();
            targetAP = winrt::AutomationElementIdentifiers::IsEnabledProperty();
          } else if (innerName == "checked") {
            targetDP = DynamicAutomationProperties::AccessibilityStateCheckedProperty();
            targetAP = winrt::TogglePatternIdentifiers::ToggleStateProperty();
          } else if (innerName == "busy") {
            targetDP = DynamicAutomationProperties::AccessibilityStateBusyProperty();
            targetAP = winrt::AutomationElementIdentifiers::ItemStatusProperty();
          } else if (innerName == "expanded") {
            targetDP = DynamicAutomationProperties::AccessibilityStateExpandedProperty();
            targetAP = winrt::ExpandCollapsePatternIdentifiers::ExpandCollapseStateProperty();
          }

          if (targetDP != nullptr) {
            // Determine values for DP and AP

            oldDPValue = element.ReadLocalValue(targetDP);

            const bool hasOldValue = oldDPValue != xaml::DependencyProperty::UnsetValue();

            if (innerName == "checked") {
              // Checked is special and can support null, boolean true, boolean false, or the string "mixed"

              auto oldValue = hasOldValue
                  ? winrt::unbox_value<winrt::Microsoft::ReactNative::AccessibilityStateCheckedValue>(oldDPValue)
                  : winrt::Microsoft::ReactNative::AccessibilityStateCheckedValue::Unchecked;

              winrt::Microsoft::ReactNative::AccessibilityStateCheckedValue newValue =
                  winrt::Microsoft::ReactNative::AccessibilityStateCheckedValue::Unchecked;

              const auto pRawBooleanNewValue = innerValue.TryGetBoolean();
              const auto pRawStringNewValue = innerValue.TryGetString();

              if (pRawBooleanNewValue != nullptr) {
                // State value is a valid boolean
                newValue = *pRawBooleanNewValue
                    ? winrt::Microsoft::ReactNative::AccessibilityStateCheckedValue::Checked
                    : winrt::Microsoft::ReactNative::AccessibilityStateCheckedValue::Unchecked;
                newDPValue = winrt::box_value<winrt::Microsoft::ReactNative::AccessibilityStateCheckedValue>(newValue);
                valueChanged = !hasOldValue || (oldValue != newValue); // Value was unset before or set but different
              } else if (pRawStringNewValue != nullptr && *pRawStringNewValue == "mixed") {
                // State value is the valid string "mixed"
                newValue = winrt::Microsoft::ReactNative::AccessibilityStateCheckedValue::Mixed;
                newDPValue = winrt::box_value<winrt::Microsoft::ReactNative::AccessibilityStateCheckedValue>(newValue);
                valueChanged = !hasOldValue || (oldValue != newValue); // Value was unset before or set but different
              } else {
                // State value is null or not valid
                valueChanged = hasOldValue; // Value was set before
              }

              oldAPValue =
                  winrt::box_value(hasOldValue ? static_cast<winrt::ToggleState>(oldValue) : winrt::ToggleState::Off);
              newAPValue = winrt::box_value(static_cast<winrt::ToggleState>(newValue));
            } else {
              // All other accessibility state values support null, boolean true or boolean false

              auto oldValue = hasOldValue ? winrt::unbox_value<bool>(oldDPValue) : false;

              const bool *pNewValue = innerValue.TryGetBoolean();

              if (pNewValue != nullptr) {
                // State value is a valid boolean
                newDPValue = winrt::box_value<bool>(*pNewValue);
                valueChanged = !hasOldValue || (oldValue != *pNewValue); // Value was unset before or set but different
              } else {
                // State value is null or not valid
                valueChanged = hasOldValue; // Value was set before
              }

              if (innerName == "disabled") {
                // Disabled maps to enabled, flip boolean
                oldAPValue = winrt::box_value(!(hasOldValue ? oldValue : false));
                newAPValue = winrt::box_value(!(pNewValue != nullptr ? *pNewValue : false));
              } else if (innerName == "busy") {
                oldAPValue = winrt::box_value(hasOldValue && oldValue ? L"Busy" : L"");
                newAPValue = winrt::box_value(pNewValue != nullptr && *pNewValue ? L"Busy" : L"");
              } else if (innerName == "expanded") {
                oldAPValue = winrt::box_value(
                    hasOldValue && oldValue ? winrt::ExpandCollapseState::Expanded
                                            : winrt::ExpandCollapseState::Collapsed);
                newAPValue = winrt::box_value(
                    pNewValue != nullptr && *pNewValue ? winrt::ExpandCollapseState::Expanded
                                                       : winrt::ExpandCollapseState::Collapsed);
              } else {
                // Direct mapping of boolean
                oldAPValue = winrt::box_value(hasOldValue ? oldValue : false);
                newAPValue = winrt::box_value(pNewValue != nullptr ? *pNewValue : false);
              }
            }

            // Update attached dependency property for element
            if (newDPValue != xaml::DependencyProperty::UnsetValue()) {
              // State value was valid, so set
              element.SetValue(targetDP, newDPValue);
            } else {
              // State value is null or not valid, so clear
              element.ClearValue(targetDP);
            }

            // Raise automation property changed event if the value changed
            if (valueChanged && targetAP != nullptr && oldAPValue != nullptr && newAPValue != nullptr) {
              if (auto peer = xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(element)) {
                peer.RaisePropertyChangedEvent(targetAP, oldAPValue, newAPValue);
              }
            }
          }
        }
      }
    } else if (propertyName == "accessibilityValue") {
      winrt::hstring textValue;
      const int numericValuesCount = 3;
      double numericValues[numericValuesCount] = {};

      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Object) {
        for (const auto &pair : propertyValue.AsObject()) {
          const std::string &innerName = pair.first;
          const auto &innerValue = pair.second;

          auto peer = xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(element);

          if (innerName == "min" &&
              (innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
               innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)) {
            numericValues[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityValue::Min)] =
                innerValue.AsDouble();

            const auto prevMinValue = DynamicAutomationProperties::GetAccessibilityValueMin(element);
            if (peer != nullptr && prevMinValue != innerValue.AsDouble()) {
              peer.RaisePropertyChangedEvent(
                  winrt::RangeValuePatternIdentifiers::MinimumProperty(),
                  winrt::box_value(prevMinValue),
                  winrt::box_value(innerValue.AsDouble()));
            }
          } else if (
              innerName == "max" &&
              (innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
               innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)) {
            numericValues[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityValue::Max)] =
                innerValue.AsDouble();

            const auto prevMaxValue = DynamicAutomationProperties::GetAccessibilityValueMax(element);
            if (peer != nullptr && prevMaxValue != innerValue.AsDouble()) {
              peer.RaisePropertyChangedEvent(
                  winrt::RangeValuePatternIdentifiers::MaximumProperty(),
                  winrt::box_value(prevMaxValue),
                  winrt::box_value(innerValue.AsDouble()));
            }
          } else if (
              innerName == "now" &&
              (innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
               innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)) {
            numericValues[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityValue::Now)] =
                innerValue.AsDouble();

            const auto prevNowValue = DynamicAutomationProperties::GetAccessibilityValueNow(element);
            if (peer != nullptr && prevNowValue != innerValue.AsDouble()) {
              peer.RaisePropertyChangedEvent(
                  winrt::RangeValuePatternIdentifiers::ValueProperty(),
                  winrt::box_value(prevNowValue),
                  winrt::box_value(innerValue.AsDouble()));
            }
          } else if (innerName == "text" && innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
            textValue = asHstring(innerValue);

            const auto prevTextValue = DynamicAutomationProperties::GetAccessibilityValueText(element);
            if (peer != nullptr && prevTextValue != textValue) {
              peer.RaisePropertyChangedEvent(
                  winrt::ValuePatternIdentifiers::ValueProperty(),
                  winrt::box_value(prevTextValue),
                  winrt::box_value(textValue));
            }
          }
        }
      }

      DynamicAutomationProperties::SetAccessibilityValueMin(
          element, numericValues[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityValue::Min)]);
      DynamicAutomationProperties::SetAccessibilityValueMax(
          element, numericValues[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityValue::Max)]);
      DynamicAutomationProperties::SetAccessibilityValueNow(
          element, numericValues[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityValue::Now)]);
      DynamicAutomationProperties::SetAccessibilityValueText(element, textValue);
    } else if (propertyName == "testID") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        auto value = asHstring(propertyValue);
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

        element.SetValue(xaml::Automation::AutomationProperties::AutomationIdProperty(), boxedValue);
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::Automation::AutomationProperties::AutomationIdProperty());
      }
    } else if (propertyName == "tooltip") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        winrt::ToolTipService::SetToolTip(element, winrt::box_value(asHstring(propertyValue)));
      }
    } else if (propertyName == "zIndex") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        auto value = static_cast<int>(propertyValue.AsDouble());
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateInt32(value);

        element.SetValue(winrt::Canvas::ZIndexProperty(), boxedValue);
      } else if (propertyValue.IsNull()) {
        element.ClearValue(winrt::Canvas::ZIndexProperty());
      }
    } else if (TryUpdateFlowDirection(element, propertyName, propertyValue)) {
    } else if (propertyName == "accessibilityActions") {
      auto value = json_type_traits<winrt::IVector<winrt::Microsoft::ReactNative::AccessibilityAction>>::parseJson(
          propertyValue);
      DynamicAutomationProperties::SetAccessibilityActions(element, value);
    } else if (propertyName == "display") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        auto value = propertyValue.AsString();
        if (value == "none") {
          element.Visibility(xaml::Visibility::Collapsed);
        } else {
          element.Visibility(xaml::Visibility::Visible);
        }
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::UIElement::VisibilityProperty());
      }
    } else {
      return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
    }
  }
  return true;
}

// Applies a TransformMatrix to the backing UIElement.
// In react-native, rotates and scales are applied about the center of the
// component, unlike XAML. Since the javascript layer sends a non-centered
// TransformMatrix, we need to compute the UIElement center and apply centering
// manually via this formula: tx = -TranslateCenter * TransformMatrix *
// TranslateCenter We accomplish this in several steps: 1) Create a PropertySet
// to hold both the TranslateCenter matrix and TransformMatrix.  Stored on the
// shadow node.
//    This allows us to generalize to the animated scenario as well.
// 2) To compute the CenterTransform we create an ExpressionAnimation that
// references the UIElement.ActualSize facade.
//    This way whenever the ActualSize changes, the animation will automatically
//    pick up the new value.
// 3) Create an ExpressionAnimation to multiply everything together.
void FrameworkElementViewManager::ApplyTransformMatrix(
    xaml::UIElement uielement,
    ShadowNodeBase *shadowNode,
    winrt::Windows::Foundation::Numerics::float4x4 transformMatrix) {
  // Get our PropertySet from the ShadowNode and insert the TransformMatrix as
  // the "transform" property
  auto transformPS = shadowNode->EnsureTransformPS();
  transformPS.InsertMatrix4x4(L"transform", transformMatrix);

  // Start the overall animation to multiply everything together
  StartTransformAnimation(uielement, transformPS);
}

// Starts ExpressionAnimation targeting UIElement.TransformMatrix with centered
// transform
void FrameworkElementViewManager::StartTransformAnimation(
    xaml::UIElement uielement,
    comp::CompositionPropertySet transformPS) {
  auto expression =
      GetExpressionAnimationStore()->GetTransformCenteringExpression(Microsoft::ReactNative::GetCompositor(uielement));
  expression.SetReferenceParameter(L"PS", transformPS);
  expression.Target(L"TransformMatrix");
  uielement.StartAnimation(expression);
}

// Used in scenario where View changes its backing Xaml element.
void FrameworkElementViewManager::RefreshTransformMatrix(ShadowNodeBase *shadowNode) {
  if (shadowNode->HasTransformPS()) {
    // First we need to update the reference parameter on the centering
    // expression to point to the new backing UIElement.
    shadowNode->UpdateTransformPS();
    auto uielement = shadowNode->GetView().try_as<xaml::UIElement>();
    assert(uielement != nullptr);

    // Start a new ExpressionAnimation targeting the new
    // UIElement.TransformMatrix.
    StartTransformAnimation(uielement, shadowNode->EnsureTransformPS());
  }
}

} // namespace Microsoft::ReactNative
