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
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"min", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"max", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"now", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"text", L"string");
  writer.WriteObjectEnd();
}

void FrameworkElementViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessible", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityRole", L"string");
  writer.WritePropertyName(L"accessibilityState");
  GetAccessibilityStateProps(writer);
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityHint", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"accessibilityLabel", L"string");
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
          assert(propertyValue.AsArray().size() == 16);
          winrt::Windows::Foundation::Numerics::float4x4 transformMatrix;
          transformMatrix.m11 = static_cast<float>(propertyValue[0].AsDouble());
          transformMatrix.m12 = static_cast<float>(propertyValue[1].AsDouble());
          transformMatrix.m13 = static_cast<float>(propertyValue[2].AsDouble());
          transformMatrix.m14 = static_cast<float>(propertyValue[3].AsDouble());
          transformMatrix.m21 = static_cast<float>(propertyValue[4].AsDouble());
          transformMatrix.m22 = static_cast<float>(propertyValue[5].AsDouble());
          transformMatrix.m23 = static_cast<float>(propertyValue[6].AsDouble());
          transformMatrix.m24 = static_cast<float>(propertyValue[7].AsDouble());
          transformMatrix.m31 = static_cast<float>(propertyValue[8].AsDouble());
          transformMatrix.m32 = static_cast<float>(propertyValue[9].AsDouble());
          transformMatrix.m33 = static_cast<float>(propertyValue[10].AsDouble());
          transformMatrix.m34 = static_cast<float>(propertyValue[11].AsDouble());
          transformMatrix.m41 = static_cast<float>(propertyValue[12].AsDouble());
          transformMatrix.m42 = static_cast<float>(propertyValue[13].AsDouble());
          transformMatrix.m43 = static_cast<float>(propertyValue[14].AsDouble());
          transformMatrix.m44 = static_cast<float>(propertyValue[15].AsDouble());

          if (!element.IsLoaded()) {
            element.Loaded([=](auto sender, auto &&) -> auto {
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
    } else if (propertyName == "width") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        double width = propertyValue.AsDouble();
        if (width >= 0)
          element.Width(width);
        // else
        // TODO report error
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::FrameworkElement::WidthProperty());
      }

    } else if (propertyName == "height") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        double height = propertyValue.AsDouble();
        if (height >= 0)
          element.Height(height);
        // else
        // TODO report error
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::FrameworkElement::HeightProperty());
      }
    } else if (propertyName == "minWidth") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        double minWidth = propertyValue.AsDouble();
        if (minWidth >= 0)
          element.MinWidth(minWidth);
        // else
        // TODO report error
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::FrameworkElement::MinWidthProperty());
      }
    } else if (propertyName == "maxWidth") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        double maxWidth = propertyValue.AsDouble();
        if (maxWidth >= 0)
          element.MaxWidth(maxWidth);
        // else
        // TODO report error
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::FrameworkElement::MaxWidthProperty());
      }

    } else if (propertyName == "minHeight") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        double minHeight = propertyValue.AsDouble();
        if (minHeight >= 0)
          element.MinHeight(minHeight);
        // else
        // TODO report error
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::FrameworkElement::MinHeightProperty());
      }
    } else if (propertyName == "maxHeight") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        double maxHeight = propertyValue.AsDouble();
        if (maxHeight >= 0)
          element.MaxHeight(maxHeight);
        // else
        // TODO report error
      } else if (propertyValue.IsNull()) {
        element.ClearValue(xaml::FrameworkElement::MaxHeightProperty());
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
        if (!propertyValue.AsBoolean())
          xaml::Automation::AutomationProperties::SetAccessibilityView(element, winrt::Peers::AccessibilityView::Raw);
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
      bool states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::CountStates)] = {};

      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Object) {
        for (const auto &pair : propertyValue.AsObject()) {
          const std::string &innerName = pair.first;
          const auto &innerValue = pair.second;

          if (innerName == "selected")
            states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Selected)] =
                innerValue.AsBoolean();
          else if (innerName == "disabled")
            states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Disabled)] =
                innerValue.AsBoolean();
          else if (innerName == "checked") {
            states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Checked)] =
                innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean && innerValue.AsBoolean();
            states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Unchecked)] =
                innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean && !innerValue.AsBoolean();
            // If the state is "mixed" we'll just set both Checked and Unchecked to false,
            // then later in the IToggleProvider implementation it will return the Intermediate state
            // due to both being set to false (see  DynamicAutomationPeer::ToggleState()).
          } else if (innerName == "busy")
            states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Busy)] =
                !innerValue.IsNull() && innerValue.AsBoolean();
          else if (innerName == "expanded") {
            states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Expanded)] =
                !innerValue.IsNull() && innerValue.AsBoolean();
            states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Collapsed)] =
                innerValue.IsNull() || !innerValue.AsBoolean();
          }
        }
      }

      DynamicAutomationProperties::SetAccessibilityStateSelected(
          element, states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Selected)]);
      DynamicAutomationProperties::SetAccessibilityStateDisabled(
          element, states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Disabled)]);
      DynamicAutomationProperties::SetAccessibilityStateChecked(
          element, states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Checked)]);
      DynamicAutomationProperties::SetAccessibilityStateUnchecked(
          element, states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Unchecked)]);
      DynamicAutomationProperties::SetAccessibilityStateBusy(
          element, states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Busy)]);
      DynamicAutomationProperties::SetAccessibilityStateExpanded(
          element, states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Expanded)]);
      DynamicAutomationProperties::SetAccessibilityStateCollapsed(
          element, states[static_cast<int32_t>(winrt::Microsoft::ReactNative::AccessibilityStates::Collapsed)]);
    } else if (propertyName == "accessibilityValue") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Object) {
        for (const auto &pair : propertyValue.AsObject()) {
          const std::string &innerName = pair.first;
          const auto &innerValue = pair.second;

          if (innerName == "min" &&
              (innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
               innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)) {
            DynamicAutomationProperties::SetAccessibilityValueMin(element, innerValue.AsDouble());
          } else if (
              innerName == "max" && innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
              innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
            DynamicAutomationProperties::SetAccessibilityValueMax(element, innerValue.AsDouble());
          } else if (
              innerName == "now" && innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
              innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
            DynamicAutomationProperties::SetAccessibilityValueNow(element, innerValue.AsDouble());
          } else if (innerName == "text" && innerValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
            auto value = asHstring(innerValue);
            DynamicAutomationProperties::SetAccessibilityValueText(element, value);
          }
        }
      }
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
