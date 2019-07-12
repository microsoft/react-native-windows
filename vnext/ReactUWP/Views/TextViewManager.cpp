// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>
#include <Utils/XamlDirect.h>

#include <winrt/Windows.UI.Xaml.Documents.h>

namespace winrt {
using namespace Windows::UI::Xaml::Documents;
} // namespace winrt

namespace react {
namespace uwp {

class TextShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  TextShadowNode() = default;
  bool ImplementsPadding() override {
    return true;
  }
};

TextViewManager::TextViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

facebook::react::ShadowNode *TextViewManager::createShadow() const {
  return new TextShadowNode();
}

const char *TextViewManager::GetName() const {
  return "RCTText";
}

XamlView TextViewManager::CreateViewCore(int64_t tag) {
  auto textBlock = winrt::TextBlock();
  textBlock.TextWrapping(
      winrt::TextWrapping::Wrap); // Default behavior in React Native
  return textBlock;
}

void TextViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    const folly::dynamic &reactDiffMap) {
  auto textBlock = nodeToUpdate->GetView().as<winrt::TextBlock>();

  const auto textBlockXD =
    GetXamlDirect().GetXamlDirectObject(textBlock);

  if (textBlockXD == nullptr)
    return;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (TryUpdateForeground(textBlock, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateFontProperties(
                   textBlock, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdatePadding(
                   nodeToUpdate, textBlock, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateTextAlignment(textBlock, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateTextTrimming(textBlock, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateTextDecorationLine(
                   textBlock, propertyName, propertyValue)) {
      continue;
    } else if (TryUpdateCharacterSpacing(
                   textBlock, propertyName, propertyValue)) {
      continue;
    } else if (propertyName == "numberOfLines") {
      if (propertyValue.isNumber())
        GetXamlDirect().SetInt32Property(
            textBlockXD,
            XDPropertyIndex::TextBlock_MaxLines,
            static_cast<int32_t>(propertyValue.asDouble()));
      else if (propertyValue.isNull())
        GetXamlDirect().ClearProperty(
            textBlockXD, XDPropertyIndex::TextBlock_MaxLines);
    } else if (propertyName == "lineHeight") {
      if (propertyValue.isNumber())
        GetXamlDirect().SetDoubleProperty(
            textBlockXD,
            XDPropertyIndex::TextBlock_LineHeight,
            propertyValue.asDouble());
      else if (propertyValue.isNull())
        GetXamlDirect().ClearProperty(
            textBlockXD, XDPropertyIndex::TextBlock_LineHeight);
    } else if (propertyName == "selectable") {
      if (propertyValue.isBool())
        GetXamlDirect().SetBooleanProperty(
            textBlockXD,
            XDPropertyIndex::TextBlock_IsTextSelectionEnabled,
            propertyValue.asBool());
      else if (propertyValue.isNull())
        GetXamlDirect().ClearProperty(
            textBlockXD,
            XDPropertyIndex::TextBlock_IsTextSelectionEnabled);
    } else if (propertyName == "allowFontScaling") {
      if (propertyValue.isBool())
        GetXamlDirect().SetBooleanProperty(
            textBlockXD,
            XDPropertyIndex::TextBlock_IsTextScaleFactorEnabled,
            propertyValue.asBool());
      else
        GetXamlDirect().ClearProperty(
            textBlockXD,
            XDPropertyIndex::TextBlock_IsTextScaleFactorEnabled);
    } else if (propertyName == "selectionColor") {
      if (IsValidColorValue(propertyValue)) {
        textBlock.SelectionHighlightColor(SolidColorBrushFrom(propertyValue));
      } else
        GetXamlDirect().ClearProperty(
            textBlockXD,
            XDPropertyIndex::TextBlock_SelectionHighlightColor);
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void TextViewManager::AddView(XamlView parent, XamlView child, int64_t index) {
  const auto textBlockXD =
      GetXamlDirect().GetXamlDirectObject(
      parent.as<winrt::TextBlock>());
  const auto childInlineXD =
      GetXamlDirect().GetXamlDirectObject(
      child.as<winrt::Inline>());
  const auto textBlockInlinesXD =
      GetXamlDirect().GetXamlDirectObjectProperty(
          textBlockXD, XDPropertyIndex::TextBlock_Inlines);
  GetXamlDirect().InsertIntoCollectionAt(
      textBlockInlinesXD, static_cast<uint32_t>(index), childInlineXD);
}

void TextViewManager::RemoveAllChildren(XamlView parent) {
  const auto textBlockXD =
      GetXamlDirect().GetXamlDirectObject(
      parent.as<winrt::TextBlock>());
  const auto textBlockInlinesXD =
      GetXamlDirect().GetXamlDirectObjectProperty(
          textBlockXD, XDPropertyIndex::TextBlock_Inlines);
  GetXamlDirect().ClearCollection(textBlockInlinesXD);
}

void TextViewManager::RemoveChildAt(XamlView parent, int64_t index) {
  const auto textBlockXD =
      GetXamlDirect().GetXamlDirectObject(
      parent.as<winrt::TextBlock>());
  const auto textBlockInlinesXD =
      GetXamlDirect().GetXamlDirectObjectProperty(
          textBlockXD, XDPropertyIndex::TextBlock_Inlines);
  GetXamlDirect().RemoveFromCollectionAt(
      textBlockInlinesXD, static_cast<uint32_t>(index));
}

YGMeasureFunc TextViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

} // namespace uwp
} // namespace react
