// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
}

namespace react { namespace uwp {

TextViewManager::TextViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : FrameworkElementViewManager(reactInstance)
{
}

const char* TextViewManager::GetName() const
{
  return "RCTText";
}

XamlView TextViewManager::CreateViewCore(int64_t tag)
{
  auto textBlock = winrt::TextBlock();
  textBlock.TextWrapping(winrt::TextWrapping::Wrap); // Default behavior in React Native
  return textBlock;
}

void TextViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, XamlView viewToUpdate, folly::dynamic reactDiffMap)
{
  auto textBlock = viewToUpdate.as<winrt::TextBlock>();
  if (textBlock == nullptr)
    return;

  for (auto& pair : reactDiffMap.items())
  {
    if (TryUpdateForeground(textBlock, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateFontProperties(textBlock, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdatePadding(nodeToUpdate, textBlock, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateTextAlignment(textBlock, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateTextTrimming(textBlock, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateTextDecorationLine(textBlock, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateCharacterSpacing(textBlock, pair.first, pair.second))
    {
      continue;
    }
    else if (pair.first == "numberOfLines")
    {
      if (pair.second.isInt())
        textBlock.MaxLines(static_cast<int32_t>(pair.second.getInt()));
    }
    else if (pair.first == "lineHeight")
    {
      if (pair.second.isNumber())
        textBlock.LineHeight(static_cast<int32_t>(pair.second.asDouble()));
    }
  }

  Super::UpdateProperties(nodeToUpdate, viewToUpdate, reactDiffMap);
}

void TextViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  auto textBlock(parent.as<winrt::TextBlock>());
  auto childInline(child.as<winrt::Inline>());
  textBlock.Inlines().InsertAt(static_cast<uint32_t>(index), childInline);
}

XamlView TextViewManager::GetChildAt(XamlView parent, int64_t index)
{
  auto textBlock(parent.as<winrt::TextBlock>());
  return textBlock.Inlines().GetAt(static_cast<uint32_t>(index));
}

int64_t TextViewManager::GetChildCount(XamlView parent)
{
  auto textBlock(parent.as<winrt::TextBlock>());
  return textBlock.Inlines().Size();
}

void TextViewManager::RemoveAllChildren(XamlView parent)
{
  auto textBlock(parent.as<winrt::TextBlock>());
  textBlock.Inlines().Clear();
}

void TextViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  auto textBlock(parent.as<winrt::TextBlock>());
  return textBlock.Inlines().RemoveAt(static_cast<uint32_t>(index));
}

YGMeasureFunc TextViewManager::GetYogaCustomMeasureFunc() const
{
  return DefaultYogaSelfMeasureFunc;
}

} }
