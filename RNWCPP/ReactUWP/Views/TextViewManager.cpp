// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"

#include <Views/ShadowNodeBase.h>

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

class TextShadowNode : public ShadowNodeBase
{
  using Super = ShadowNodeBase;
public:
  TextShadowNode() = default;
  bool ImplementsPadding() override { return true; }
};

TextViewManager::TextViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

facebook::react::ShadowNode* TextViewManager::createShadow() const
{
  return new TextShadowNode();
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

void TextViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap)
{
  auto textBlock = nodeToUpdate->GetView().as<winrt::TextBlock>();
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
      else if (pair.second.isNull())
        textBlock.ClearValue(winrt::TextBlock::MaxLinesProperty());
    }
    else if (pair.first == "lineHeight")
    {
      if (pair.second.isNumber())
        textBlock.LineHeight(static_cast<int32_t>(pair.second.asDouble()));
      else if (pair.second.isNull())
        textBlock.ClearValue(winrt::TextBlock::LineHeightProperty());
    }
    else if (pair.first == "selectable")
    {
      if (pair.second.isBool())
        textBlock.IsTextSelectionEnabled(pair.second.asBool());
      else if (pair.second.isNull())
        textBlock.ClearValue(winrt::TextBlock::IsTextSelectionEnabledProperty());
    }
    else if (pair.first == "allowFontScaling")
    {
      if (pair.second.isBool())
        textBlock.IsTextScaleFactorEnabled(pair.second.asBool());
      else
        textBlock.ClearValue(winrt::TextBlock::IsTextScaleFactorEnabledProperty());
    }
    else if (pair.first == "selectionColor")
    {
      if (pair.second.isInt())
      {
        textBlock.SelectionHighlightColor(SolidColorBrushFrom(pair.second));
      }
      else
        textBlock.ClearValue(winrt::TextBlock::SelectionHighlightColorProperty());
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void TextViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  auto textBlock(parent.as<winrt::TextBlock>());
  auto childInline(child.as<winrt::Inline>());
  textBlock.Inlines().InsertAt(static_cast<uint32_t>(index), childInline);
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
