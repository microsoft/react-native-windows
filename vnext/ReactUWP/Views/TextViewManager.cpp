// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.UI.Xaml.Documents.h>

namespace winrt {
using namespace Windows::UI::Xaml::Documents;
} // namespace winrt

namespace react {
namespace uwp {

class TextShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 private:
  ShadowNode *m_firstChildNode;

 public:
  TextShadowNode() {
    m_firstChildNode = nullptr;
  };
  bool ImplementsPadding() override {
    return true;
  }

  void AddView(ShadowNode &child, int64_t index) override {
    if (index == 0) {
      auto run =
          static_cast<ShadowNodeBase &>(child).GetView().try_as<winrt::Run>();
      if (run != nullptr) {
        m_firstChildNode = &child;
        auto textBlock = this->GetView().as<winrt::TextBlock>();
        textBlock.Text(run.Text());
        return;
      }
    } else if (index == 1 && m_firstChildNode != nullptr) {
      auto textBlock = this->GetView().as<winrt::TextBlock>();
      textBlock.ClearValue(winrt::TextBlock::TextProperty());
      Super::AddView(*m_firstChildNode, 0);
      m_firstChildNode = nullptr;
    }
    Super::AddView(child, index);
  }

  void removeAllChildren() {
    m_firstChildNode = nullptr;
    Super::removeAllChildren();
  }

  void RemoveChildAt(int64_t indexToRemove) {
    if (indexToRemove == 0) {
      m_firstChildNode = nullptr;
    }
    Super::RemoveChildAt(indexToRemove);
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
  if (textBlock == nullptr)
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
      if (propertyValue.isNumber()) {
        auto numberLines = static_cast<int32_t>(propertyValue.asDouble());
        if (numberLines == 1) {
          textBlock.TextWrapping(
              winrt::TextWrapping::NoWrap); // setting no wrap for single line
                                            // text for better trimming
                                            // experience
        } else {
          textBlock.TextWrapping(winrt::TextWrapping::Wrap);
        }
        textBlock.MaxLines(numberLines);
      } else if (propertyValue.isNull())
        textBlock.ClearValue(winrt::TextBlock::MaxLinesProperty());
    } else if (propertyName == "lineHeight") {
      if (propertyValue.isNumber())
        textBlock.LineHeight(static_cast<int32_t>(propertyValue.asDouble()));
      else if (propertyValue.isNull())
        textBlock.ClearValue(winrt::TextBlock::LineHeightProperty());
    } else if (propertyName == "selectable") {
      if (propertyValue.isBool())
        textBlock.IsTextSelectionEnabled(propertyValue.asBool());
      else if (propertyValue.isNull())
        textBlock.ClearValue(
            winrt::TextBlock::IsTextSelectionEnabledProperty());
    } else if (propertyName == "allowFontScaling") {
      if (propertyValue.isBool())
        textBlock.IsTextScaleFactorEnabled(propertyValue.asBool());
      else
        textBlock.ClearValue(
            winrt::TextBlock::IsTextScaleFactorEnabledProperty());
    } else if (propertyName == "selectionColor") {
      if (IsValidColorValue(propertyValue)) {
        textBlock.SelectionHighlightColor(SolidColorBrushFrom(propertyValue));
      } else
        textBlock.ClearValue(
            winrt::TextBlock::SelectionHighlightColorProperty());
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void TextViewManager::AddView(XamlView parent, XamlView child, int64_t index) {
  auto textBlock(parent.as<winrt::TextBlock>());
  auto childInline(child.as<winrt::Inline>());
  textBlock.Inlines().InsertAt(static_cast<uint32_t>(index), childInline);
}

void TextViewManager::RemoveAllChildren(XamlView parent) {
  auto textBlock(parent.as<winrt::TextBlock>());
  textBlock.Inlines().Clear();
}

void TextViewManager::RemoveChildAt(XamlView parent, int64_t index) {
  auto textBlock(parent.as<winrt::TextBlock>());
  return textBlock.Inlines().RemoveAt(static_cast<uint32_t>(index));
}

YGMeasureFunc TextViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

} // namespace uwp
} // namespace react
