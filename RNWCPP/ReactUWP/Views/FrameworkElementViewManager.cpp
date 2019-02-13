// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "FrameworkElementViewManager.h"

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Automation.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Automation;
}

namespace react { namespace uwp {

FrameworkElementViewManager::FrameworkElementViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : ViewManagerBase(reactInstance)
{
}

folly::dynamic FrameworkElementViewManager::GetNativeProps() const
{
  folly::dynamic props = Super::GetNativeProps();
  props.update(folly::dynamic::object
    ("accessibilityHint", "string")
    ("accessibilityLabel", "string")
    ("testID", "string")
  );
  return props;
}


void FrameworkElementViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, XamlView viewToUpdate, folly::dynamic reactDiffMap)
{
  auto element(viewToUpdate.as<winrt::FrameworkElement>());

  if (element != nullptr)
  {
    for (auto& pair : reactDiffMap.items())
    {
      const folly::dynamic& propertyName = pair.first;
      const folly::dynamic& propertyValue = pair.second;

      if (propertyName == "opacity")
      {
        if (!propertyValue.isNumber())
          continue;

        double opacity = propertyValue.asDouble();
        if (opacity >= 0 && opacity <= 1)
          element.Opacity(opacity);
        // else
        // TODO report error
      }
      else if (propertyName == "width")
      {
        if (!propertyValue.isNumber())
          continue;

        double width = propertyValue.asDouble();
        if (width >= 0)
          element.Width(width);
        // else
        // TODO report error
      }
      else if (propertyName == "height")
      {
        if (!propertyValue.isNumber())
          continue;

        double height = propertyValue.asDouble();
        if (height >= 0)
          element.Height(height);
        // else
        // TODO report error
      }
      else if (propertyName == "minWidth")
      {
        if (!propertyValue.isNumber())
          continue;

        double minWidth = propertyValue.asDouble();
        if (minWidth >= 0)
          element.MinWidth(minWidth);
        // else
        // TODO report error
      }
      else if (propertyName == "maxWidth")
      {
        if (!propertyValue.isNumber())
          continue;

        double maxWidth = propertyValue.asDouble();
        if (maxWidth >= 0)
          element.MaxWidth(maxWidth);
        // else
        // TODO report error
      }
      else if (propertyName == "minHeight")
      {
        if (!propertyValue.isNumber())
          continue;

        double minHeight = propertyValue.asDouble();
        if (minHeight >= 0)
          element.MinHeight(minHeight);
        // else
        // TODO report error
      }
      else if (propertyName == "maxHeight")
      {
        if (!propertyValue.isNumber())
          continue;

        double maxHeight = propertyValue.asDouble();
        if (maxHeight >= 0)
          element.MaxHeight(maxHeight);
        // else
        // TODO report error
      }
      else if (propertyName == "accessibilityHint")
      {
         auto value = react::uwp::asHstring(propertyValue);
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

        element.SetValue(winrt::AutomationProperties::HelpTextProperty(), boxedValue);
      }
      else if (propertyName == "accessibilityLabel")
      {
        auto value = react::uwp::asHstring(propertyValue);
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

        element.SetValue(winrt::AutomationProperties::NameProperty(), boxedValue);
      }
      else if (propertyName == "accessible")
      {
        if (propertyValue.isBool())
        {
          if (!propertyValue.asBool())
            winrt::AutomationProperties::SetAccessibilityView(element, winrt::Peers::AccessibilityView::Raw);
        }
      }
      else if (propertyName == "testID")
      {
        auto value = react::uwp::asHstring(propertyValue);
        auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

        element.SetValue(winrt::AutomationProperties::AutomationIdProperty(), boxedValue);
      }
      else if (TryUpdateFlowDirection(element, propertyName, propertyValue))
      {
        continue;
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, viewToUpdate, reactDiffMap);
}

} }
