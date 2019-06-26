// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>

#include <Views/FrameworkElementViewManager.h>
#include <Views/ExpressionAnimationStore.h>

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>
#include <Utils/AccessibilityUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <WindowsNumerics.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
}

namespace react { namespace uwp {

FrameworkElementViewManager::FrameworkElementViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

void FrameworkElementViewManager::TransferProperty(XamlView oldView, XamlView newView, winrt::DependencyProperty dp)
{
  auto oldValue = oldView.ReadLocalValue(dp);
  if (oldValue != nullptr)
  {
    newView.SetValue(dp, oldValue);
    oldView.ClearValue(dp);
  }
}

void FrameworkElementViewManager::TransferProperties(XamlView oldView, XamlView newView)
{
  // Render Properties
  TransferProperty(oldView, newView, winrt::UIElement::OpacityProperty());

  // Layout Properties
  TransferProperty(oldView, newView, winrt::FrameworkElement::WidthProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::HeightProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::MinWidthProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::MinHeightProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::MaxWidthProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::MaxHeightProperty());

  // Accessibility Properties
  TransferProperty(oldView, newView, winrt::AutomationProperties::NameProperty());
  TransferProperty(oldView, newView, winrt::AutomationProperties::HelpTextProperty());
  TransferProperty(oldView, newView, winrt::AutomationProperties::LiveSettingProperty());
  auto accessibilityView = winrt::AutomationProperties::GetAccessibilityView(oldView);
  winrt::AutomationProperties::SetAccessibilityView(newView, accessibilityView);
  winrt::AutomationProperties::SetAccessibilityView(oldView, winrt::Peers::AccessibilityView::Raw);

  auto tooltip = winrt::ToolTipService::GetToolTip(oldView);
  oldView.ClearValue(winrt::ToolTipService::ToolTipProperty());
  winrt::ToolTipService::SetToolTip(newView, tooltip);
}

folly::dynamic FrameworkElementViewManager::GetNativeProps() const
{
  folly::dynamic props = Super::GetNativeProps();
  props.update(folly::dynamic::object
    ("accessibilityHint", "string")
    ("accessibilityLabel", "string")
    ("testID", "string")
    ("tooltip", "string")
  );
  return props;
}


void FrameworkElementViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap)
{
  auto element(nodeToUpdate->GetView().as<winrt::FrameworkElement>());
  if (element != nullptr)
  {
    for (const auto& pair : reactDiffMap.items())
    {
      const std::string& propertyName = pair.first.getString();
      const folly::dynamic& propertyValue = pair.second;

      if (propertyName == "opacity")
      {
        if (propertyValue.isNumber())
        {
          double opacity = propertyValue.asDouble();
          if (opacity >= 0 && opacity <= 1)
            element.Opacity(opacity);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::UIElement::OpacityProperty());
          continue;
        }
      }
      else if (propertyName == "transform")
      {
        if (element.try_as<winrt::IUIElement10>()) // Works on 19H1+
        {
          if (propertyValue.isArray())
          {
            assert(propertyValue.size() == 16);
            winrt::Windows::Foundation::Numerics::float4x4 transformMatrix;
            transformMatrix.m11 = static_cast<float>(propertyValue[0].asDouble());
            transformMatrix.m12 = static_cast<float>(propertyValue[1].asDouble());
            transformMatrix.m13 = static_cast<float>(propertyValue[2].asDouble());
            transformMatrix.m14 = static_cast<float>(propertyValue[3].asDouble());
            transformMatrix.m21 = static_cast<float>(propertyValue[4].asDouble());
            transformMatrix.m22 = static_cast<float>(propertyValue[5].asDouble());
            transformMatrix.m23 = static_cast<float>(propertyValue[6].asDouble());
            transformMatrix.m24 = static_cast<float>(propertyValue[7].asDouble());
            transformMatrix.m31 = static_cast<float>(propertyValue[8].asDouble());
            transformMatrix.m32 = static_cast<float>(propertyValue[9].asDouble());
            transformMatrix.m33 = static_cast<float>(propertyValue[10].asDouble());
            transformMatrix.m34 = static_cast<float>(propertyValue[11].asDouble());
            transformMatrix.m41 = static_cast<float>(propertyValue[12].asDouble());
            transformMatrix.m42 = static_cast<float>(propertyValue[13].asDouble());
            transformMatrix.m43 = static_cast<float>(propertyValue[14].asDouble());
            transformMatrix.m44 = static_cast<float>(propertyValue[15].asDouble());

            ApplyTransformMatrix(element, nodeToUpdate, transformMatrix);
          }
          else if (propertyValue.isNull())
          {
            element.TransformMatrix(winrt::Windows::Foundation::Numerics::float4x4::identity());
          }
        }
      }
      else if (propertyName == "width")
      {
        if (propertyValue.isNumber())
        {
          double width = propertyValue.asDouble();
          if (width >= 0)
            element.Width(width);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::FrameworkElement::WidthProperty());
          continue;
        }

      }
      else if (propertyName == "height")
      {
        if (propertyValue.isNumber())
        {
          double height = propertyValue.asDouble();
          if (height >= 0)
            element.Height(height);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::FrameworkElement::HeightProperty());
          continue;
        }
      }
      else if (propertyName == "minWidth")
      {
        if (propertyValue.isNumber())
        {
          double minWidth = propertyValue.asDouble();
          if (minWidth >= 0)
            element.MinWidth(minWidth);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::FrameworkElement::MinWidthProperty());
          continue;
        }
      }
      else if (propertyName == "maxWidth")
      {
        if (propertyValue.isNumber())
        {
          double maxWidth = propertyValue.asDouble();
          if (maxWidth >= 0)
            element.MaxWidth(maxWidth);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::FrameworkElement::MaxWidthProperty());
          continue;
        }

      }
      else if (propertyName == "minHeight")
      {
        if (propertyValue.isNumber())
        {
          double minHeight = propertyValue.asDouble();
          if (minHeight >= 0)
            element.MinHeight(minHeight);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::FrameworkElement::MinHeightProperty());
          continue;
        }
      }
      else if (propertyName == "maxHeight")
      {
        if (propertyValue.isNumber())
        {
          double maxHeight = propertyValue.asDouble();
          if (maxHeight >= 0)
            element.MaxHeight(maxHeight);
          // else
          // TODO report error
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::FrameworkElement::MaxHeightProperty());
          continue;
        }

      }
      else if (propertyName == "accessibilityHint")
      {
        if (propertyValue.isString())
        {
          auto value = react::uwp::asHstring(propertyValue);
          auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

          element.SetValue(winrt::AutomationProperties::HelpTextProperty(), boxedValue);
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::AutomationProperties::HelpTextProperty());
        }
      }
      else if (propertyName == "accessibilityLabel")
      {
        if (propertyValue.isString())
        {
          auto value = react::uwp::asHstring(propertyValue);
          auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

          element.SetValue(winrt::AutomationProperties::NameProperty(), boxedValue);
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::AutomationProperties::NameProperty());
        }
        AnnounceLiveRegionChangedIfNeeded(element);
      }
      else if (propertyName == "accessible")
      {
        if (propertyValue.isBool())
        {
          if (!propertyValue.asBool())
            winrt::AutomationProperties::SetAccessibilityView(element, winrt::Peers::AccessibilityView::Raw);
        }
      }
      else if (propertyName == "accessibilityLiveRegion")
      {
        if (propertyValue.isString())
        {
          auto value = propertyValue.getString();

          auto liveSetting = winrt::AutomationLiveSetting::Off;

          if (value == "polite")
          {
            liveSetting = winrt::AutomationLiveSetting::Polite;
          }
          else if (value == "assertive")
          {
            liveSetting = winrt::AutomationLiveSetting::Assertive;
          }

          element.SetValue(winrt::AutomationProperties::LiveSettingProperty(), winrt::box_value(liveSetting));
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::AutomationProperties::LiveSettingProperty());
        }
        AnnounceLiveRegionChangedIfNeeded(element);
      }
      else if (propertyName == "testID")
      {
        if (propertyValue.isString())
        {
          auto value = react::uwp::asHstring(propertyValue);
          auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

          element.SetValue(winrt::AutomationProperties::AutomationIdProperty(), boxedValue);
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::AutomationProperties::AutomationIdProperty());
        }
      }
      else if (propertyName == "tooltip")
      {
        if (propertyValue.isString())
        {
          winrt::TextBlock tooltip = winrt::TextBlock();
          tooltip.Text(asHstring(propertyValue));
          winrt::ToolTipService::SetToolTip(element, tooltip);
        }
      }
      else if (propertyName == "zIndex")
      {
        if (propertyValue.isNumber())
        {
          auto value = static_cast<int>(propertyValue.asDouble());
          auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateInt32(value);

          element.SetValue(winrt::Canvas::ZIndexProperty(), boxedValue);
        }
        else if (propertyValue.isNull())
        {
          element.ClearValue(winrt::Canvas::ZIndexProperty());
        }
      }
      else if (TryUpdateFlowDirection(element, propertyName, propertyValue))
      {
        continue;
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

// Applies a TransformMatrix to the backing UIElement.
// In react-native, rotates and scales are applied about the center of the component, unlike XAML.
// Since the javascript layer sends a non-centered TransformMatrix, we need to compute the UIElement center and apply
// centering manually via this formula:
// tx = -TranslateCenter * TransformMatrix * TranslateCenter
// We accomplish this in several steps:
// 1) Create a PropertySet to hold both the TranslateCenter matrix and TransformMatrix.  Stored on the shadow node.
//    This allows us to generalize to the animated scenario as well.
// 2) To compute the CenterTransform we create an ExpressionAnimation that references the UIElement.ActualSize facade.
//    This way whenever the ActualSize changes, the animation will automatically pick up the new value.
// 3) Create an ExpressionAnimation to multiply everything together.
void FrameworkElementViewManager::ApplyTransformMatrix(winrt::UIElement uielement, ShadowNodeBase* shadowNode, winrt::Windows::Foundation::Numerics::float4x4 transformMatrix)
{
  // Get our PropertySet from the ShadowNode and insert the TransformMatrix as the "transform" property
  auto transformPS = shadowNode->EnsureTransformPS();
  transformPS.InsertMatrix4x4(L"transform", transformMatrix);

  // Start the overall animation to multiply everything together
  StartTransformAnimation(uielement, transformPS);
}

// Starts ExpressionAnimation targeting UIElement.TransformMatrix with centered transform
void FrameworkElementViewManager::StartTransformAnimation(
  winrt::UIElement uielement,
  winrt::Windows::UI::Composition::CompositionPropertySet transformPS)
{
  auto instance = GetReactInstance().lock();
  assert(instance != nullptr);
  auto expression = instance->GetExpressionAnimationStore().GetTransformCenteringExpression();
  expression.SetReferenceParameter(L"PS", transformPS);
  expression.Target(L"TransformMatrix");
  uielement.StartAnimation(expression);
}

folly::dynamic FrameworkElementViewManager::GetCommands() const
{
  // Don't update SetFocus/Blur commands in subclass, otherwise commands may not match with what defined in js side.
  auto commands = Super::GetCommands();
  commands.update(folly::dynamic::object
  ("SetFocus", static_cast<std::underlying_type_t<FocusCommand>>(FocusCommand::SetFocus))
  );
  commands.update(folly::dynamic::object
  ("Blur", static_cast<std::underlying_type_t<FocusCommand>>(FocusCommand::Blur))
  );
  return commands;
}

void FrameworkElementViewManager::DispatchCommand(XamlView viewToUpdate, int64_t commandId, const folly::dynamic& commandArgs)
{
  // UWP doesn't have the blur concept. Here we use FocusState::Pointer to move away to keyboard focused visual.
  if (viewToUpdate)
  {
    auto focusCommand = (static_cast<FocusCommand>(commandId));
    if (focusCommand == FocusCommand::SetFocus)
      winrt::FocusManager::TryFocusAsync(viewToUpdate, winrt::FocusState::Programmatic);
    else if (focusCommand == FocusCommand::Blur)
      winrt::FocusManager::TryFocusAsync(viewToUpdate, winrt::FocusState::Pointer);    
  }
}

// Used in scenario where View changes its backing Xaml element.
void FrameworkElementViewManager::RefreshTransformMatrix(ShadowNodeBase* shadowNode)
{
  if (shadowNode->HasTransformPS())
  {
    // First we need to update the reference parameter on the centering expression to point to the new backing UIElement.
    shadowNode->UpdateTransformPS();
    auto uielement = shadowNode->GetView().try_as<winrt::UIElement>();
    assert(uielement != nullptr);

    // Start a new ExpressionAnimation targeting the new UIElement.TransformMatrix.
    StartTransformAnimation(uielement, shadowNode->EnsureTransformPS());
  }
}

} }
