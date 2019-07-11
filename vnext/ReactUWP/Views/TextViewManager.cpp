// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>
#include <Utils/XamlDirectInstance.h>

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
    XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(textBlock);

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
                   nodeToUpdate,
                   textBlock,
                   textBlockXD,
                   propertyName,
                   propertyValue,
                   XD::XamlPropertyIndex::TextBlock_Padding)) {
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
        XamlDirectInstance::GetXamlDirect().SetInt32Property(
            textBlockXD,
            XD::XamlPropertyIndex::TextBlock_MaxLines,
            static_cast<int32_t>(propertyValue.asDouble()));
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBlockXD, XD::XamlPropertyIndex::TextBlock_MaxLines);
    } else if (propertyName == "lineHeight") {
      if (propertyValue.isNumber())
        XamlDirectInstance::GetXamlDirect().SetDoubleProperty(
            textBlockXD,
            XD::XamlPropertyIndex::TextBlock_LineHeight,
            propertyValue.asDouble());
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBlockXD, XD::XamlPropertyIndex::TextBlock_LineHeight);
    } else if (propertyName == "selectable") {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
            textBlockXD,
            XD::XamlPropertyIndex::TextBlock_IsTextSelectionEnabled,
            propertyValue.asBool());
      else if (propertyValue.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBlockXD,
            XD::XamlPropertyIndex::TextBlock_IsTextSelectionEnabled);
    } else if (propertyName == "allowFontScaling") {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
            textBlockXD,
            XD::XamlPropertyIndex::TextBlock_IsTextScaleFactorEnabled,
            propertyValue.asBool());
      else
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBlockXD,
            XD::XamlPropertyIndex::TextBlock_IsTextScaleFactorEnabled);
    } else if (propertyName == "selectionColor") {
      if (IsValidColorValue(propertyValue)) {
        XamlDirectInstance::GetXamlDirect().SetColorProperty(
            textBlockXD,
            XD::XamlPropertyIndex::TextBlock_SelectionHighlightColor,
            SolidColorBrushFrom(propertyValue).Color());
      } else
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            textBlockXD,
            XD::XamlPropertyIndex::TextBlock_SelectionHighlightColor);
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void TextViewManager::AddView(XamlView parent, XamlView child, int64_t index) {
  const auto textBlockXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(
      parent.as<winrt::TextBlock>());
  const auto childInlineXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(
      child.as<winrt::Inline>());
  const auto textBlockInlinesXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(
          textBlockXD, XD::XamlPropertyIndex::TextBlock_Inlines);
  XamlDirectInstance::GetXamlDirect().InsertIntoCollectionAt(
      textBlockInlinesXD, static_cast<uint32_t>(index), childInlineXD);
}

void TextViewManager::RemoveAllChildren(XamlView parent) {
  const auto textBlockXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(
      parent.as<winrt::TextBlock>());
  const auto textBlockInlinesXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(
          textBlockXD, XD::XamlPropertyIndex::TextBlock_Inlines);
  XamlDirectInstance::GetXamlDirect().ClearCollection(textBlockInlinesXD);
}

void TextViewManager::RemoveChildAt(XamlView parent, int64_t index) {
  const auto textBlockXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(
      parent.as<winrt::TextBlock>());
  const auto textBlockInlinesXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObjectProperty(
          textBlockXD, XD::XamlPropertyIndex::TextBlock_Inlines);
  XamlDirectInstance::GetXamlDirect().RemoveFromCollectionAt(
      textBlockInlinesXD, static_cast<uint32_t>(index));
}

YGMeasureFunc TextViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

} // namespace uwp
} // namespace react
