// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ButtonContentViewManager.h"

#include <Utils/ValueUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

using namespace winrt;
using namespace ::Windows::Foundation;
using namespace ::Windows::UI;
using namespace ::Windows::UI::Xaml;
using namespace ::Windows::UI::Xaml::Controls;
using namespace ::Windows::UI::Xaml::Media;

namespace react { namespace uwp { namespace polyester {

ButtonContentViewManager::ButtonContentViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

void ButtonContentViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  auto stackPanel(parent.as<StackPanel>());
  if (stackPanel != nullptr)
  {
      stackPanel.Children().InsertAt(static_cast<uint32_t>(index), child.as<UIElement>());
  }
}

void ButtonContentViewManager::RemoveAllChildren(XamlView parent)
{
  auto stackPanel(parent.as<StackPanel>());
  if (stackPanel != nullptr)
  {
    stackPanel.Children().Clear();
    stackPanel.InvalidateMeasure();
    stackPanel.InvalidateArrange();
  }
}

void ButtonContentViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  auto stackPanel(parent.as<StackPanel>());
  if (stackPanel != nullptr)
  {
    stackPanel.Children().RemoveAt(static_cast<uint32_t>(index));
    stackPanel.InvalidateMeasure();
    stackPanel.InvalidateArrange();
  }
}

const char* ButtonContentViewManager::GetName() const
{
  return "PLYButtonContent";
}

XamlView ButtonContentViewManager::CreateViewCore(int64_t tag)
{
  auto stackPanel = StackPanel();
  stackPanel.Orientation(Orientation::Horizontal);
  return stackPanel;
}

}}}
