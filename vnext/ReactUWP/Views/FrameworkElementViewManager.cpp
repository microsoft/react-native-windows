// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>

#include <Views/ExpressionAnimationStore.h>
#include <Views/FrameworkElementViewManager.h>

#include <Utils/AccessibilityUtils.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <WindowsNumerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.h>
#include "Utils/PropertyHandlerUtils.h"

#include "AccessibilityAction.h"
#include "DynamicAutomationProperties.h"

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::Foundation::Collections;
} // namespace winrt

template <>
struct json_type_traits<winrt::react::uwp::AccessibilityAction> {
  static winrt::react::uwp::AccessibilityAction parseJson(
      const folly::dynamic &json) {
    auto action =
        winrt::make<winrt::react::uwp::implementation::AccessibilityAction>();

    for (auto &item : json.items()) {
      if (item.first == "name") {
        action.Name(react::uwp::asHstring(item.second));
      } else if (item.first == "label") {
        action.Label(react::uwp::asHstring(item.second));
      }
    }
    return action;
  }
};

template <>
struct json_type_traits<winrt::IVector<winrt::react::uwp::AccessibilityAction>> {
  static winrt::IVector<winrt::react::uwp::AccessibilityAction>
  parseJson(
      const folly::dynamic &json) {
    auto vector =
        winrt::single_threaded_vector<winrt::react::uwp::AccessibilityAction>();

    if (json.isArray()) {
      for (const auto &action : json) {
        if (!action.isObject())
          continue;

        vector.Append(json_type_traits<winrt::react::uwp::AccessibilityAction>::parseJson(action));
      }
    }
    return vector;
  }
};

namespace react {
namespace uwp {

FrameworkElementViewManager::FrameworkElementViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

void FrameworkElementViewManager::TransferProperty(
    XamlView oldView,
    XamlView newView,
    winrt::DependencyProperty dp) {
  TransferProperty(oldView, newView, dp, dp);
}

void FrameworkElementViewManager::TransferProperty(
    XamlView oldView,
    XamlView newView,
    winrt::DependencyProperty oldViewDP,
    winrt::DependencyProperty newViewDP) {
  auto oldValue = oldView.ReadLocalValue(oldViewDP);
  if (oldValue != nullptr) {
    oldView.ClearValue(oldViewDP);
    newView.SetValue(newViewDP, oldValue);
  }
}

void FrameworkElementViewManager::TransferProperties(
    XamlView oldView,
    XamlView newView) {
  // Render Properties
  TransferProperty(oldView, newView, winrt::UIElement::OpacityProperty());

  // Layout Properties
  TransferProperty(oldView, newView, winrt::FrameworkElement::WidthProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::HeightProperty());
  TransferProperty(
      oldView, newView, winrt::FrameworkElement::MinWidthProperty());
  TransferProperty(
      oldView, newView, winrt::FrameworkElement::MinHeightProperty());
  TransferProperty(
      oldView, newView, winrt::FrameworkElement::MaxWidthProperty());
  TransferProperty(
      oldView, newView, winrt::FrameworkElement::MaxHeightProperty());
  TransferProperty(
      oldView, newView, winrt::FrameworkElement::FlowDirectionProperty());
  TransferProperty(oldView, newView, winrt::Canvas::ZIndexProperty());
  TransferProperty(oldView, newView, ViewPanel::LeftProperty());
  TransferProperty(oldView, newView, ViewPanel::TopProperty());

  // Accessibility Properties
  TransferProperty(
      oldView, newView, winrt::AutomationProperties::AutomationIdProperty());
  TransferProperty(
      oldView, newView, winrt::AutomationProperties::NameProperty());
  TransferProperty(
      oldView, newView, winrt::AutomationProperties::HelpTextProperty());
  TransferProperty(
      oldView, newView, winrt::AutomationProperties::LiveSettingProperty());
  TransferProperty(
      oldView, newView, winrt::AutomationProperties::PositionInSetProperty());
  TransferProperty(
      oldView, newView, winrt::AutomationProperties::SizeOfSetProperty());
  auto accessibilityView =
      winrt::AutomationProperties::GetAccessibilityView(oldView);
  winrt::AutomationProperties::SetAccessibilityView(newView, accessibilityView);
  winrt::AutomationProperties::SetAccessibilityView(
      oldView, winrt::Peers::AccessibilityView::Raw);
  TransferProperty(
      oldView,
      newView,
      DynamicAutomationProperties::AccessibilityRoleProperty());
  TransferProperty(
      oldView,
      newView,
      DynamicAutomationProperties::AccessibilityStateSelectedProperty());
  TransferProperty(
      oldView,
      newView,
      DynamicAutomationProperties::AccessibilityStateDisabledProperty());
  TransferProperty(
      oldView,
      newView,
      DynamicAutomationProperties::AccessibilityStateCheckedProperty());
  TransferProperty(
      oldView,
      newView,
      DynamicAutomationProperties::AccessibilityStateUncheckedProperty());
  TransferProperty(
      oldView,
      newView,
      DynamicAutomationProperties::AccessibilityStateBusyProperty());
  TransferProperty(
      oldView,
      newView,
      DynamicAutomationProperties::AccessibilityStateExpandedProperty());
  TransferProperty(
      oldView,
      newView,
      DynamicAutomationProperties::AccessibilityStateCollapsedProperty());
  TransferProperty(
      oldView,
      newView,
      DynamicAutomationProperties::AccessibilityInvokeEventHandlerProperty());

  auto tooltip = winrt::ToolTipService::GetToolTip(oldView);
  oldView.ClearValue(winrt::ToolTipService::ToolTipProperty());
  winrt::ToolTipService::SetToolTip(newView, tooltip);

  // Clear the TransformMatrix from the old View.  The TransformMatrix will be
  // set on the new View a bit later in RefreshProperties() (as we need data
  // from the ShadowNode not available here).
  auto oldElement = oldView.try_as<winrt::UIElement>();
  if (oldElement && oldElement.try_as<winrt::IUIElement10>()) {
    oldElement.TransformMatrix(
        winrt::Windows::Foundation::Numerics::float4x4::identity());
  }
}

folly::dynamic FrameworkElementViewManager::GetNativeProps() const {
  folly::dynamic props = Super::GetNativeProps();
  props.update(folly::dynamic::object("accessible", "boolean")(
      "accessibilityRole", "string")("accessibilityStates", "array")(
      "accessibilityHint", "string")("accessibilityLabel", "string")(
      "accessibilityPosInSet", "number")("accessibilitySetSize", "number")(
      "testID", "string")("tooltip", "string")(
      "accessibilityActions", "array"));
  return props;
}

void FrameworkElementViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    const folly::dynamic &reactDiffMap) {
  auto element(nodeToUpdate->GetView().as<winrt::FrameworkElement>());
  if (element != nullptr) {
    for (const auto &pair : reactDiffMap.items()) {
      const std::string &propertyName = pair.first.getString();
      const folly::dynamic &propertyValue = pair.second;

      if (propertyName == "opacity") {
        if (propertyValue.isNumber()) {
          double opacity = propertyValue.asDouble();
          if (opacity >= 0 && opacity <= 1)
            element.Opacity(opacity);
          // else
          // TODO report error
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::UIElement::OpacityProperty());
          continue;
        }
      } else if (propertyName == "transform") {
        if (element.try_as<winrt::IUIElement10>()) // Works on 19H1+
        {
          if (propertyValue.isArray()) {
            assert(propertyValue.size() == 16);
            winrt::Windows::Foundation::Numerics::float4x4 transformMatrix;
            transformMatrix.m11 =
                static_cast<float>(propertyValue[0].asDouble());
            transformMatrix.m12 =
                static_cast<float>(propertyValue[1].asDouble());
            transformMatrix.m13 =
                static_cast<float>(propertyValue[2].asDouble());
            transformMatrix.m14 =
                static_cast<float>(propertyValue[3].asDouble());
            transformMatrix.m21 =
                static_cast<float>(propertyValue[4].asDouble());
            transformMatrix.m22 =
                static_cast<float>(propertyValue[5].asDouble());
            transformMatrix.m23 =
                static_cast<float>(propertyValue[6].asDouble());
            transformMatrix.m24 =
                static_cast<float>(propertyValue[7].asDouble());
            transformMatrix.m31 =
                static_cast<float>(propertyValue[8].asDouble());
            transformMatrix.m32 =
                static_cast<float>(propertyValue[9].asDouble());
            transformMatrix.m33 =
                static_cast<float>(propertyValue[10].asDouble());
            transformMatrix.m34 =
                static_cast<float>(propertyValue[11].asDouble());
            transformMatrix.m41 =
                static_cast<float>(propertyValue[12].asDouble());
            transformMatrix.m42 =
                static_cast<float>(propertyValue[13].asDouble());
            transformMatrix.m43 =
                static_cast<float>(propertyValue[14].asDouble());
            transformMatrix.m44 =
                static_cast<float>(propertyValue[15].asDouble());

            ApplyTransformMatrix(element, nodeToUpdate, transformMatrix);
          } else if (propertyValue.isNull()) {
            element.TransformMatrix(
                winrt::Windows::Foundation::Numerics::float4x4::identity());
          }
        }
      } else if (propertyName == "width") {
        if (propertyValue.isNumber()) {
          double width = propertyValue.asDouble();
          if (width >= 0)
            element.Width(width);
          // else
          // TODO report error
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::FrameworkElement::WidthProperty());
          continue;
        }

      } else if (propertyName == "height") {
        if (propertyValue.isNumber()) {
          double height = propertyValue.asDouble();
          if (height >= 0)
            element.Height(height);
          // else
          // TODO report error
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::FrameworkElement::HeightProperty());
          continue;
        }
      } else if (propertyName == "minWidth") {
        if (propertyValue.isNumber()) {
          double minWidth = propertyValue.asDouble();
          if (minWidth >= 0)
            element.MinWidth(minWidth);
          // else
          // TODO report error
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::FrameworkElement::MinWidthProperty());
          continue;
        }
      } else if (propertyName == "maxWidth") {
        if (propertyValue.isNumber()) {
          double maxWidth = propertyValue.asDouble();
          if (maxWidth >= 0)
            element.MaxWidth(maxWidth);
          // else
          // TODO report error
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::FrameworkElement::MaxWidthProperty());
          continue;
        }

      } else if (propertyName == "minHeight") {
        if (propertyValue.isNumber()) {
          double minHeight = propertyValue.asDouble();
          if (minHeight >= 0)
            element.MinHeight(minHeight);
          // else
          // TODO report error
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::FrameworkElement::MinHeightProperty());
          continue;
        }
      } else if (propertyName == "maxHeight") {
        if (propertyValue.isNumber()) {
          double maxHeight = propertyValue.asDouble();
          if (maxHeight >= 0)
            element.MaxHeight(maxHeight);
          // else
          // TODO report error
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::FrameworkElement::MaxHeightProperty());
          continue;
        }

      } else if (propertyName == "accessibilityHint") {
        if (propertyValue.isString()) {
          auto value = react::uwp::asHstring(propertyValue);
          auto boxedValue =
              winrt::Windows::Foundation::PropertyValue::CreateString(value);

          element.SetValue(
              winrt::AutomationProperties::HelpTextProperty(), boxedValue);
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::AutomationProperties::HelpTextProperty());
        }
      } else if (propertyName == "accessibilityLabel") {
        if (propertyValue.isString()) {
          auto value = react::uwp::asHstring(propertyValue);
          auto boxedValue =
              winrt::Windows::Foundation::PropertyValue::CreateString(value);

          element.SetValue(
              winrt::AutomationProperties::NameProperty(), boxedValue);
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::AutomationProperties::NameProperty());
        }
        AnnounceLiveRegionChangedIfNeeded(element);
      } else if (propertyName == "accessible") {
        if (propertyValue.isBool()) {
          if (!propertyValue.asBool())
            winrt::AutomationProperties::SetAccessibilityView(
                element, winrt::Peers::AccessibilityView::Raw);
        }
      } else if (propertyName == "accessibilityLiveRegion") {
        if (propertyValue.isString()) {
          auto value = propertyValue.getString();

          auto liveSetting = winrt::AutomationLiveSetting::Off;

          if (value == "polite") {
            liveSetting = winrt::AutomationLiveSetting::Polite;
          } else if (value == "assertive") {
            liveSetting = winrt::AutomationLiveSetting::Assertive;
          }

          element.SetValue(
              winrt::AutomationProperties::LiveSettingProperty(),
              winrt::box_value(liveSetting));
        } else if (propertyValue.isNull()) {
          element.ClearValue(
              winrt::AutomationProperties::LiveSettingProperty());
        }
        AnnounceLiveRegionChangedIfNeeded(element);
      } else if (propertyName == "accessibilityPosInSet") {
        if (propertyValue.isNumber()) {
          auto value = static_cast<int>(propertyValue.asDouble());
          auto boxedValue =
              winrt::Windows::Foundation::PropertyValue::CreateInt32(value);

          element.SetValue(
              winrt::AutomationProperties::PositionInSetProperty(), boxedValue);
        } else if (propertyValue.isNull()) {
          element.ClearValue(
              winrt::AutomationProperties::PositionInSetProperty());
        }
      } else if (propertyName == "accessibilitySetSize") {
        if (propertyValue.isNumber()) {
          auto value = static_cast<int>(propertyValue.asDouble());
          auto boxedValue =
              winrt::Windows::Foundation::PropertyValue::CreateInt32(value);

          element.SetValue(
              winrt::AutomationProperties::SizeOfSetProperty(), boxedValue);
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::AutomationProperties::SizeOfSetProperty());
        }
      } else if (propertyName == "accessibilityRole") {
        if (propertyValue.isString()) {
          const std::string &role = propertyValue.getString();
          if (role == "none")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::None);
          else if (role == "button")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Button);
          else if (role == "link")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Link);
          else if (role == "search")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Search);
          else if (role == "image")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Image);
          else if (role == "keyboardkey")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::KeyboardKey);
          else if (role == "text")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Text);
          else if (role == "adjustable")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Adjustable);
          else if (role == "imagebutton")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::ImageButton);
          else if (role == "header")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Header);
          else if (role == "summary")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Summary);
          else if (role == "alert")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Alert);
          else if (role == "checkbox")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::CheckBox);
          else if (role == "combobox")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::ComboBox);
          else if (role == "menu")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Menu);
          else if (role == "menubar")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::MenuBar);
          else if (role == "menuitem")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::MenuItem);
          else if (role == "progressbar")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::ProgressBar);
          else if (role == "radio")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Radio);
          else if (role == "radiogroup")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::RadioGroup);
          else if (role == "scrollbar")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::ScrollBar);
          else if (role == "spinbutton")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::SpinButton);
          else if (role == "switch")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Switch);
          else if (role == "tab")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Tab);
          else if (role == "tablist")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::TabList);
          else if (role == "timer")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Timer);
          else if (role == "toolbar")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::ToolBar);
          else if (role == "list")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::List);
          else if (role == "listitem")
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::ListItem);
          else
            DynamicAutomationProperties::SetAccessibilityRole(
                element, winrt::react::uwp::AccessibilityRoles::Unknown);
        } else if (propertyValue.isNull()) {
          element.ClearValue(
              DynamicAutomationProperties::AccessibilityRoleProperty());
        }
      } else if (propertyName == "accessibilityStates") {
        bool states[static_cast<int32_t>(
            winrt::react::uwp::AccessibilityStates::CountStates)] = {};

        if (propertyValue.isArray()) {
          for (const auto &state : propertyValue) {
            if (!state.isString())
              continue;

            if (state.getString() == "selected")
              states[static_cast<int32_t>(
                  winrt::react::uwp::AccessibilityStates::Selected)] = true;
            else if (state.getString() == "disabled")
              states[static_cast<int32_t>(
                  winrt::react::uwp::AccessibilityStates::Disabled)] = true;
            else if (state.getString() == "checked")
              states[static_cast<int32_t>(
                  winrt::react::uwp::AccessibilityStates::Checked)] = true;
            else if (state.getString() == "unchecked")
              states[static_cast<int32_t>(
                  winrt::react::uwp::AccessibilityStates::Unchecked)] = true;
            else if (state.getString() == "busy")
              states[static_cast<int32_t>(
                  winrt::react::uwp::AccessibilityStates::Busy)] = true;
            else if (state.getString() == "expanded")
              states[static_cast<int32_t>(
                  winrt::react::uwp::AccessibilityStates::Expanded)] = true;
            else if (state.getString() == "collapsed")
              states[static_cast<int32_t>(
                  winrt::react::uwp::AccessibilityStates::Collapsed)] = true;
          }
        }

        DynamicAutomationProperties::SetAccessibilityStateSelected(
            element,
            states[static_cast<int32_t>(
                winrt::react::uwp::AccessibilityStates::Selected)]);
        DynamicAutomationProperties::SetAccessibilityStateDisabled(
            element,
            states[static_cast<int32_t>(
                winrt::react::uwp::AccessibilityStates::Disabled)]);
        DynamicAutomationProperties::SetAccessibilityStateChecked(
            element,
            states[static_cast<int32_t>(
                winrt::react::uwp::AccessibilityStates::Checked)]);
        DynamicAutomationProperties::SetAccessibilityStateUnchecked(
            element,
            states[static_cast<int32_t>(
                winrt::react::uwp::AccessibilityStates::Unchecked)]);
        DynamicAutomationProperties::SetAccessibilityStateBusy(
            element,
            states[static_cast<int32_t>(
                winrt::react::uwp::AccessibilityStates::Busy)]);
        DynamicAutomationProperties::SetAccessibilityStateExpanded(
            element,
            states[static_cast<int32_t>(
                winrt::react::uwp::AccessibilityStates::Expanded)]);
        DynamicAutomationProperties::SetAccessibilityStateCollapsed(
            element,
            states[static_cast<int32_t>(
                winrt::react::uwp::AccessibilityStates::Collapsed)]);
      } else if (propertyName == "testID") {
        if (propertyValue.isString()) {
          auto value = react::uwp::asHstring(propertyValue);
          auto boxedValue =
              winrt::Windows::Foundation::PropertyValue::CreateString(value);

          element.SetValue(
              winrt::AutomationProperties::AutomationIdProperty(), boxedValue);
        } else if (propertyValue.isNull()) {
          element.ClearValue(
              winrt::AutomationProperties::AutomationIdProperty());
        }
      } else if (propertyName == "tooltip") {
        if (propertyValue.isString()) {
          winrt::TextBlock tooltip = winrt::TextBlock();
          tooltip.Text(asHstring(propertyValue));
          winrt::ToolTipService::SetToolTip(element, tooltip);
        }
      } else if (propertyName == "zIndex") {
        if (propertyValue.isNumber()) {
          auto value = static_cast<int>(propertyValue.asDouble());
          auto boxedValue =
              winrt::Windows::Foundation::PropertyValue::CreateInt32(value);

          element.SetValue(winrt::Canvas::ZIndexProperty(), boxedValue);
        } else if (propertyValue.isNull()) {
          element.ClearValue(winrt::Canvas::ZIndexProperty());
        }
      } else if (TryUpdateFlowDirection(element, propertyName, propertyValue)) {
        continue;
      } else if (propertyName == "accessibilityActions") {
        auto value = json_type_traits<winrt::IVector<
            winrt::react::uwp::AccessibilityAction>>::parseJson(propertyValue);
        DynamicAutomationProperties::SetAccessibilityActions(element, value);
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
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
    winrt::UIElement uielement,
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
    winrt::UIElement uielement,
    winrt::Windows::UI::Composition::CompositionPropertySet transformPS) {
  auto instance = GetReactInstance().lock();
  assert(instance != nullptr);
  auto expression =
      instance->GetExpressionAnimationStore().GetTransformCenteringExpression();
  expression.SetReferenceParameter(L"PS", transformPS);
  expression.Target(L"TransformMatrix");
  uielement.StartAnimation(expression);
}

// Used in scenario where View changes its backing Xaml element.
void FrameworkElementViewManager::RefreshTransformMatrix(
    ShadowNodeBase *shadowNode) {
  if (shadowNode->HasTransformPS()) {
    // First we need to update the reference parameter on the centering
    // expression to point to the new backing UIElement.
    shadowNode->UpdateTransformPS();
    auto uielement = shadowNode->GetView().try_as<winrt::UIElement>();
    assert(uielement != nullptr);

    // Start a new ExpressionAnimation targeting the new
    // UIElement.TransformMatrix.
    StartTransformAnimation(uielement, shadowNode->EnsureTransformPS());
  }
}

} // namespace uwp
} // namespace react
