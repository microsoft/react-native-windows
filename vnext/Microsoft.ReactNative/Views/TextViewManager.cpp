// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <UI.Xaml.Automation.Peers.h>
#include <UI.Xaml.Automation.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

namespace winrt {
using namespace xaml::Documents;
using namespace xaml::Automation;
using namespace xaml::Automation::Peers;
} // namespace winrt

namespace react::uwp {

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
      auto run = static_cast<ShadowNodeBase &>(child).GetView().try_as<winrt::Run>();
      if (run != nullptr) {
        m_firstChildNode = &child;
        auto textBlock = this->GetView().as<xaml::Controls::TextBlock>();
        textBlock.Text(run.Text());
        return;
      }
    } else if (index == 1 && m_firstChildNode != nullptr) {
      auto textBlock = this->GetView().as<xaml::Controls::TextBlock>();
      textBlock.ClearValue(xaml::Controls::TextBlock::TextProperty());
      Super::AddView(*m_firstChildNode, 0);
      m_firstChildNode = nullptr;
    }
    Super::AddView(child, index);
  }

  void removeAllChildren() override {
    m_firstChildNode = nullptr;
    Super::removeAllChildren();
  }

  void RemoveChildAt(int64_t indexToRemove) override {
    if (indexToRemove == 0) {
      m_firstChildNode = nullptr;
    }
    Super::RemoveChildAt(indexToRemove);
  }
};

TextViewManager::TextViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {}

facebook::react::ShadowNode *TextViewManager::createShadow() const {
  return new TextShadowNode();
}

const char *TextViewManager::GetName() const {
  return "RCTText";
}

XamlView TextViewManager::CreateViewCore(int64_t /*tag*/) {
  auto textBlock = xaml::Controls::TextBlock();
  textBlock.TextWrapping(xaml::TextWrapping::Wrap); // Default behavior in React Native
  return textBlock;
}

bool TextViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  auto textBlock = nodeToUpdate->GetView().as<xaml::Controls::TextBlock>();
  if (textBlock == nullptr)
    return true;

  if (TryUpdateForeground(textBlock, propertyName, propertyValue)) {
  } else if (TryUpdateFontProperties(textBlock, propertyName, propertyValue)) {
  } else if (TryUpdatePadding(nodeToUpdate, textBlock, propertyName, propertyValue)) {
  } else if (TryUpdateTextAlignment(textBlock, propertyName, propertyValue)) {
  } else if (TryUpdateTextTrimming(textBlock, propertyName, propertyValue)) {
  } else if (TryUpdateTextDecorationLine(textBlock, propertyName, propertyValue)) {
  } else if (TryUpdateCharacterSpacing(textBlock, propertyName, propertyValue)) {
  } else if (propertyName == "numberOfLines") {
    if (propertyValue.isNumber()) {
      auto numberLines = static_cast<int32_t>(propertyValue.asDouble());
      if (numberLines == 1) {
        textBlock.TextWrapping(xaml::TextWrapping::NoWrap); // setting no wrap for single line
                                                            // text for better trimming
                                                            // experience
      } else {
        textBlock.TextWrapping(xaml::TextWrapping::Wrap);
      }
      textBlock.MaxLines(numberLines);
    } else if (propertyValue.isNull()) {
      textBlock.TextWrapping(xaml::TextWrapping::Wrap); // set wrapping back to default
      textBlock.ClearValue(xaml::Controls::TextBlock::MaxLinesProperty());
    }
  } else if (propertyName == "lineHeight") {
    if (propertyValue.isNumber())
      textBlock.LineHeight(static_cast<int32_t>(propertyValue.asDouble()));
    else if (propertyValue.isNull())
      textBlock.ClearValue(xaml::Controls::TextBlock::LineHeightProperty());
  } else if (propertyName == "selectable") {
    if (propertyValue.isBool())
      textBlock.IsTextSelectionEnabled(propertyValue.asBool());
    else if (propertyValue.isNull())
      textBlock.ClearValue(xaml::Controls::TextBlock::IsTextSelectionEnabledProperty());
  } else if (propertyName == "allowFontScaling") {
    if (propertyValue.isBool()) {
      textBlock.IsTextScaleFactorEnabled(propertyValue.asBool());
    } else {
      textBlock.ClearValue(xaml::Controls::TextBlock::IsTextScaleFactorEnabledProperty());
    }
  } else if (propertyName == "selectionColor") {
    if (IsValidColorValue(propertyValue)) {
      textBlock.SelectionHighlightColor(SolidColorBrushFrom(propertyValue));
    } else
      textBlock.ClearValue(xaml::Controls::TextBlock::SelectionHighlightColorProperty());
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

void TextViewManager::AddView(const XamlView &parent, const XamlView &child, int64_t index) {
  auto textBlock(parent.as<xaml::Controls::TextBlock>());
  auto childInline(child.as<winrt::Inline>());
  textBlock.Inlines().InsertAt(static_cast<uint32_t>(index), childInline);
}

void TextViewManager::RemoveAllChildren(const XamlView &parent) {
  auto textBlock(parent.as<xaml::Controls::TextBlock>());
  textBlock.Inlines().Clear();
}

void TextViewManager::RemoveChildAt(const XamlView &parent, int64_t index) {
  auto textBlock(parent.as<xaml::Controls::TextBlock>());
  return textBlock.Inlines().RemoveAt(static_cast<uint32_t>(index));
}

YGMeasureFunc TextViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

void TextViewManager::OnDescendantTextPropertyChanged(ShadowNodeBase *node) {
  if (auto element = node->GetView().try_as<xaml::Controls::TextBlock>()) {
    // If name is set, it's controlled by accessibilityLabel, and it's already
    // handled in FrameworkElementViewManager. Here it only handles when name is
    // not set.
    if (xaml::Automation::AutomationProperties::GetLiveSetting(element) != winrt::AutomationLiveSetting::Off &&
        xaml::Automation::AutomationProperties::GetName(element).empty() &&
        xaml::Automation::AutomationProperties::GetAccessibilityView(element) != winrt::Peers::AccessibilityView::Raw) {
      if (auto peer = xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(element)) {
        peer.RaiseAutomationEvent(winrt::AutomationEvents::LiveRegionChanged);
      }
    }
  }
}

} // namespace react::uwp
