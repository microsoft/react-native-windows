// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/FrameworkElementViewManager.h>

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
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
    newView.SetValue(dp, oldValue);
}

void FrameworkElementViewManager::TransferProperties(XamlView oldView, XamlView newView)
{
  // Render Properties
  TransferProperty(oldView, newView, winrt::UIElement::OpacityProperty());

  if (oldView.try_as<winrt::IUIElement9>())
  {
    auto oldElement = oldView.as<winrt::UIElement>();
    auto newElement = newView.as<winrt::UIElement>();
    newElement.TransformMatrix(oldElement.TransformMatrix());
  }

  // Layout Properties
  TransferProperty(oldView, newView, winrt::FrameworkElement::WidthProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::HeightProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::MinWidthProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::MinHeightProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::MaxWidthProperty());
  TransferProperty(oldView, newView, winrt::FrameworkElement::MaxHeightProperty());

  // Accessibility Properties
  TransferProperty(oldView, newView, winrt::AutomationProperties::NameProperty());
  TransferProperty(oldView, newView, winrt::AutomationProperties::LiveSettingProperty());
  auto accessibilityView = winrt::AutomationProperties::GetAccessibilityView(oldView);
  winrt::AutomationProperties::SetAccessibilityView(newView, accessibilityView);

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
        if (element.try_as<winrt::IUIElement9>()) // Works on RS5+
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
            element.TransformMatrix(transformMatrix);
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
        AnnounceIfNeeded(element);
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
          auto value = propertyValue.asString();

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
        AnnounceIfNeeded(element);
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

void FrameworkElementViewManager::AnnounceIfNeeded(winrt::FrameworkElement element)
{
  if (winrt::AutomationProperties::GetLiveSetting(element) != winrt::AutomationLiveSetting::Off
    && !winrt::AutomationProperties::GetName(element).empty())
  {
    auto peer = winrt::FrameworkElementAutomationPeer::FromElement(element);
    if (nullptr != peer)
    {
      peer.RaiseAutomationEvent(winrt::AutomationEvents::LiveRegionChanged);
    }
  }
}

} }
