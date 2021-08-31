// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"
#include "Utils/ShadowNodeTypeUtils.h"
#include "Utils/XamlIslandUtils.h"

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <Views/RawTextViewManager.h>
#include <Views/ShadowNodeBase.h>
#include <Views/VirtualTextViewManager.h>

#include <UI.Xaml.Automation.Peers.h>
#include <UI.Xaml.Automation.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
#include <Utils/PropertyUtils.h>
#include <Utils/TransformableText.h>
#include <Utils/ValueUtils.h>

namespace winrt {
using namespace xaml::Documents;
using namespace xaml::Automation;
using namespace xaml::Automation::Peers;
} // namespace winrt

namespace Microsoft::ReactNative {

class TextShadowNode final : public ShadowNodeBase {
  using Super = ShadowNodeBase;
  friend TextViewManager;

 private:
  ShadowNode *m_firstChildNode;

  std::optional<winrt::Windows::UI::Color> m_backgroundColor{};
  std::optional<winrt::Windows::UI::Color> m_foregroundColor{};

  int32_t m_prevCursorEnd = 0;

 public:
  TextShadowNode() {
    m_firstChildNode = nullptr;
  };
  bool ImplementsPadding() override {
    return true;
  }

  void AddView(ShadowNode &child, int64_t index) override {
    auto &childNode = static_cast<ShadowNodeBase &>(child);
    VirtualTextShadowNode::ApplyTextTransform(
        childNode, textTransform, /* forceUpdate = */ false, /* isRoot = */ false);

    if (IsVirtualTextShadowNode(&childNode)) {
      auto &textChildNode = static_cast<VirtualTextShadowNode &>(childNode);
      m_hasDescendantBackgroundColor |= textChildNode.m_hasDescendantBackgroundColor;
    }

    auto addInline = true;
    if (index == 0) {
      auto run = childNode.GetView().try_as<winrt::Run>();
      if (run != nullptr) {
        m_firstChildNode = &child;
        auto textBlock = this->GetView().as<xaml::Controls::TextBlock>();
        textBlock.Text(run.Text());
        m_prevCursorEnd += textBlock.Text().size();
        addInline = false;
      }
    } else if (index == 1 && m_firstChildNode != nullptr) {
      auto textBlock = this->GetView().as<xaml::Controls::TextBlock>();
      textBlock.ClearValue(xaml::Controls::TextBlock::TextProperty());
      Super::AddView(*m_firstChildNode, 0);
      m_firstChildNode = nullptr;
    }

    if (addInline) {
      Super::AddView(child, index);
    }

    RecalculateTextHighlighters();
  }

  void removeAllChildren() override {
    m_firstChildNode = nullptr;
    Super::removeAllChildren();
    RecalculateTextHighlighters();
  }

  void RemoveChildAt(int64_t indexToRemove) override {
    if (indexToRemove == 0) {
      m_firstChildNode = nullptr;
    }
    Super::RemoveChildAt(indexToRemove);
    RecalculateTextHighlighters();
  }

  void RecalculateTextHighlighters() {
    const auto textBlock = this->GetView().as<xaml::Controls::TextBlock>();
    textBlock.TextHighlighters().Clear();

    auto nestedIndex = 0;
    if (m_hasDescendantBackgroundColor) {
      const auto highlighterCount = textBlock.TextHighlighters().Size();
      if (const auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
        for (auto childTag : m_children) {
          if (const auto childNode = uiManager->getHost()->FindShadowNodeForTag(childTag)) {
            nestedIndex = AddNestedTextHighlighter(
                m_backgroundColor, m_foregroundColor, static_cast<ShadowNodeBase *>(childNode), nestedIndex);
          }
        }
      }

      if (textBlock.TextHighlighters().Size() == 0) {
        m_hasDescendantBackgroundColor = false;
      }
    } else {
      nestedIndex = textBlock.Text().size();
    }

    if (m_backgroundColor) {
      winrt::TextHighlighter highlighter{};
      highlighter.Ranges().Append({0, nestedIndex});
      highlighter.Background(SolidBrushFromColor(m_backgroundColor.value()));
      if (m_foregroundColor) {
        highlighter.Foreground(SolidBrushFromColor(m_foregroundColor.value()));
      }
      GetView().as<xaml::Controls::TextBlock>().TextHighlighters().InsertAt(0, highlighter);
    }
  }

  int AddNestedTextHighlighter(
      const std::optional<winrt::Windows::UI::Color> &backgroundColor,
      const std::optional<winrt::Windows::UI::Color> &foregroundColor,
      ShadowNodeBase *node,
      int startIndex) {
    if (const auto run = node->GetView().try_as<winrt::Run>()) {
      return startIndex + run.Text().size();
    } else if (const auto span = node->GetView().try_as<winrt::Span>()) {
      const auto textBlock = GetView().as<xaml::Controls::TextBlock>();
      winrt::TextHighlighter highlighter{nullptr};
      auto parentBackgroundColor = backgroundColor;
      auto parentForegroundColor = foregroundColor;
      if (IsVirtualTextShadowNode(node)) {
        const auto virtualTextNode = static_cast<VirtualTextShadowNode *>(node);
        const auto requiresHighlighter =
            virtualTextNode->m_backgroundColor || (backgroundColor && virtualTextNode->m_foregroundColor);
        if (requiresHighlighter) {
          highlighter = {};
          parentBackgroundColor =
              virtualTextNode->m_backgroundColor ? virtualTextNode->m_backgroundColor : parentBackgroundColor;
          parentForegroundColor =
              virtualTextNode->m_foregroundColor ? virtualTextNode->m_foregroundColor : parentForegroundColor;
          highlighter.Background(SolidBrushFromColor(parentBackgroundColor.value()));
          if (parentForegroundColor) {
            highlighter.Foreground(SolidBrushFromColor(parentForegroundColor.value()));
          }
        }
      }

      const auto initialHighlighterCount = textBlock.TextHighlighters().Size();
      auto nestedIndex = startIndex;
      if (const auto uiManager = GetNativeUIManager(node->GetViewManager()->GetReactContext()).lock()) {
        for (auto childTag : node->m_children) {
          if (const auto childNode = uiManager->getHost()->FindShadowNodeForTag(childTag)) {
            nestedIndex = AddNestedTextHighlighter(
                parentBackgroundColor, parentForegroundColor, static_cast<ShadowNodeBase *>(childNode), nestedIndex);
          }
        }
      }

      if (highlighter) {
        highlighter.Ranges().Append({startIndex, nestedIndex - startIndex});
        textBlock.TextHighlighters().InsertAt(0, highlighter);
      } else if (IsVirtualTextShadowNode(node) && textBlock.TextHighlighters().Size() == initialHighlighterCount) {
        const auto virtualTextNode = static_cast<VirtualTextShadowNode *>(node);
        virtualTextNode->m_hasDescendantBackgroundColor = false;
      }

      return nestedIndex;
    }

    return 0;
  }

  TextTransform textTransform{TextTransform::Undefined};
  bool m_hasDescendantBackgroundColor{false};
};

TextViewManager::TextViewManager(const Mso::React::IReactContext &context) : Super(context) {}

ShadowNode *TextViewManager::createShadow() const {
  return new TextShadowNode();
}

const wchar_t *TextViewManager::GetName() const {
  return L"RCTText";
}

XamlView TextViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  auto textBlock = xaml::Controls::TextBlock();
  textBlock.TextWrapping(xaml::TextWrapping::Wrap); // Default behavior in React Native
  return textBlock;
}

bool TextViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto textBlock = nodeToUpdate->GetView().as<xaml::Controls::TextBlock>();
  if (textBlock == nullptr)
    return true;

  if (TryUpdateForeground(textBlock, propertyName, propertyValue)) {
    const auto node = static_cast<TextShadowNode *>(nodeToUpdate);
    if (IsValidOptionalColorValue(propertyValue)) {
      node->m_foregroundColor = OptionalColorFrom(propertyValue);
      node->RecalculateTextHighlighters();
    }
  } else if (TryUpdateFontProperties(textBlock, propertyName, propertyValue)) {
  } else if (propertyName == "textTransform") {
    auto textNode = static_cast<TextShadowNode *>(nodeToUpdate);
    textNode->textTransform = TransformableText::GetTextTransform(propertyValue);
    VirtualTextShadowNode::ApplyTextTransform(
        *textNode, textNode->textTransform, /* forceUpdate = */ true, /* isRoot = */ true);
  } else if (TryUpdatePadding(nodeToUpdate, textBlock, propertyName, propertyValue)) {
  } else if (TryUpdateTextAlignment(textBlock, propertyName, propertyValue)) {
  } else if (TryUpdateTextTrimming(textBlock, propertyName, propertyValue)) {
  } else if (TryUpdateTextDecorationLine(textBlock, propertyName, propertyValue)) {
    // Temporary workaround for bug in XAML which fails to flush old TextDecorationLine render
    // Link to Bug: https://github.com/microsoft/microsoft-ui-xaml/issues/1093#issuecomment-514282402
    winrt::hstring text(textBlock.Text().c_str());
    textBlock.Text(L"");
    textBlock.Text(text);
  } else if (TryUpdateCharacterSpacing(textBlock, propertyName, propertyValue)) {
  } else if (propertyName == "numberOfLines") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
      auto numberLines = propertyValue.AsInt32();
      if (numberLines == 1) {
        textBlock.TextWrapping(xaml::TextWrapping::NoWrap); // setting no wrap for single line
                                                            // text for better trimming
                                                            // experience
      } else {
        textBlock.TextWrapping(xaml::TextWrapping::Wrap);
      }
      textBlock.MaxLines(numberLines);
    } else if (propertyValue.IsNull()) {
      textBlock.TextWrapping(xaml::TextWrapping::Wrap); // set wrapping back to default
      textBlock.ClearValue(xaml::Controls::TextBlock::MaxLinesProperty());
    }
  } else if (propertyName == "lineHeight") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
      textBlock.LineHeight(propertyValue.AsInt32());
      textBlock.LineStackingStrategy(xaml::LineStackingStrategy::BlockLineHeight);
    } else if (propertyValue.IsNull()) {
      textBlock.ClearValue(xaml::Controls::TextBlock::LineHeightProperty());
      textBlock.ClearValue(xaml::Controls::TextBlock::LineStackingStrategyProperty());
    }
  } else if (propertyName == "selectable") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
      const auto selectable = propertyValue.AsBoolean();
      textBlock.IsTextSelectionEnabled(selectable);
      if (selectable) {
        EnsureUniqueTextFlyoutForXamlIsland(textBlock);
      }
    } else if (propertyValue.IsNull()) {
      textBlock.ClearValue(xaml::Controls::TextBlock::IsTextSelectionEnabledProperty());
      ClearUniqueTextFlyoutForXamlIsland(textBlock);
    }
  } else if (propertyName == "allowFontScaling") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
      textBlock.IsTextScaleFactorEnabled(propertyValue.AsBoolean());
    } else {
      textBlock.ClearValue(xaml::Controls::TextBlock::IsTextScaleFactorEnabledProperty());
    }
  } else if (propertyName == "selectionColor") {
    if (IsValidColorValue(propertyValue)) {
      textBlock.SelectionHighlightColor(SolidColorBrushFrom(propertyValue));
    } else
      textBlock.ClearValue(xaml::Controls::TextBlock::SelectionHighlightColorProperty());
  } else if (propertyName == "backgroundColor") {
    const auto node = static_cast<TextShadowNode *>(nodeToUpdate);
    if (IsValidOptionalColorValue(propertyValue)) {
      node->m_backgroundColor = OptionalColorFrom(propertyValue);
      node->RecalculateTextHighlighters();
    }
  } else if (propertyName == "accessibilityRole") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      const std::string &role = propertyValue.AsString();
      auto value = asHstring(propertyValue);
      auto boxedValue = winrt::Windows::Foundation::PropertyValue::CreateString(value);

      textBlock.SetValue(winrt::AutomationProperties::LocalizedControlTypeProperty(), boxedValue);
      if (role == "header") {
        xaml::Automation::AutomationProperties::SetHeadingLevel(
            textBlock, winrt::Peers::AutomationHeadingLevel::Level2);
      } else {
        textBlock.ClearValue(winrt::AutomationProperties::HeadingLevelProperty());
      }
    } else if (propertyValue.IsNull()) {
      textBlock.ClearValue(winrt::AutomationProperties::LocalizedControlTypeProperty());
      textBlock.ClearValue(winrt::AutomationProperties::HeadingLevelProperty());
    }
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }

  return true;
}

void TextViewManager::AddView(const XamlView &parent, const XamlView &child, int64_t index) {
  auto textBlock(parent.as<xaml::Controls::TextBlock>());

  if (auto childInline = child.try_as<winrt::Inline>()) {
    textBlock.Inlines().InsertAt(static_cast<uint32_t>(index), childInline);
  } else {
    // #6315 Text can embed non-text elements. Fail gracefully instead of crashing if that happens
    textBlock.Inlines().InsertAt(static_cast<uint32_t>(index), winrt::Run());
    GetReactContext().CallJSFunction(
        "RCTLog",
        "logToConsole",
        folly::dynamic::array(
            "warn", "React Native for Windows does not yet support nesting non-Text components under <Text>"));
  }
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

void TextViewManager::OnDescendantTextPropertyChanged(ShadowNodeBase *node, PropertyChangeType propertyChangeType) {
  if (IsTextShadowNode(node)) {
    const auto textNode = static_cast<TextShadowNode *>(node);

    if ((propertyChangeType & PropertyChangeType::Text) == PropertyChangeType::Text) {
      const auto element = node->GetView().as<xaml::Controls::TextBlock>();

      // If name is set, it's controlled by accessibilityLabel, and it's already
      // handled in FrameworkElementViewManager. Here it only handles when name is
      // not set.
      if (xaml::Automation::AutomationProperties::GetLiveSetting(element) != winrt::AutomationLiveSetting::Off &&
          xaml::Automation::AutomationProperties::GetName(element).empty() &&
          xaml::Automation::AutomationProperties::GetAccessibilityView(element) !=
              winrt::Peers::AccessibilityView::Raw) {
        if (auto peer = xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(element)) {
          peer.RaiseAutomationEvent(winrt::AutomationEvents::LiveRegionChanged);
        }
      }
    }

    // If a property change added a background color to the text tree, update
    // the flag to signal recursive highlighter updates are required.
    if ((propertyChangeType & PropertyChangeType::AddBackgroundColor) == PropertyChangeType::AddBackgroundColor) {
      textNode->m_hasDescendantBackgroundColor = true;
    }

    // Recalculate text highlighters
    textNode->RecalculateTextHighlighters();
  }
}

TextTransform TextViewManager::GetTextTransformValue(ShadowNodeBase *node) {
  if (IsTextShadowNode(node)) {
    return static_cast<TextShadowNode *>(node)->textTransform;
  }

  return TextTransform::Undefined;
}

} // namespace Microsoft::ReactNative
