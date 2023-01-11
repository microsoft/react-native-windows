// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"
#include "TouchEventHandler.h"
#include "Utils/ShadowNodeTypeUtils.h"
#include "Utils/XamlIslandUtils.h"

#include <Views/ShadowNodeBase.h>
#include <Views/Text/TextVisitors.h>
#include <Views/VirtualTextViewManager.h>

#include <UI.Input.h>
#include <UI.Xaml.Automation.Peers.h>
#include <UI.Xaml.Automation.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
#include <UI.Xaml.Input.h>
#include <Utils/PropertyUtils.h>
#include <Utils/TransformableText.h>
#include <Utils/ValueUtils.h>

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Input.h>
#endif

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
  bool m_isTextOptimized{true};
  bool m_hasDescendantTextHighlighter{false};
  bool m_hasDescendantPressable{false};
  std::optional<winrt::Windows::UI::Color> m_backgroundColor{};
  std::optional<winrt::Windows::UI::Color> m_foregroundColor{};
  std::unique_ptr<TouchEventHandler> m_touchEventHandler = nullptr;
  winrt::event_revoker<xaml::Controls::ITextBlock> m_selectionChangedRevoker;

 public:
  bool ImplementsPadding() override {
    return true;
  }

  void AddView(ShadowNode &child, int64_t index) override {
    auto &childNode = static_cast<ShadowNodeBase &>(child);
    ApplyTextTransformToChild(&child);

    if (IsVirtualTextShadowNode(&childNode)) {
      auto &textChildNode = static_cast<VirtualTextShadowNode &>(childNode);
      m_hasDescendantTextHighlighter |= textChildNode.hasDescendantTextHighlighter;
      m_hasDescendantPressable |= textChildNode.hasDescendantPressable;
    }

    const auto wasOptimized = m_isTextOptimized;
    m_isTextOptimized = IsRawTextShadowNode(&childNode) && m_isTextOptimized;
    if (m_isTextOptimized) {
      // Re-build optimized text from children
      UpdateOptimizedText();
    } else if (wasOptimized) {
      // Remove optimized text and re-construct as Inline tree
      UpdateOptimizedText(true);
      if (const auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
        for (size_t i = 0; i < m_children.size(); ++i) {
          if (const auto childNode =
                  static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(m_children[i]))) {
            Super::AddView(*childNode, i);
          }
        }
      }
    } else {
      Super::AddView(child, index);
    }

    RecalculateTextHighlighters();
  }

  void removeAllChildren() override {
    if (m_isTextOptimized) {
      UpdateOptimizedText();
    } else {
      Super::removeAllChildren();
    }
    RecalculateTextHighlighters();
  }

  void RemoveChildAt(int64_t indexToRemove) override {
    if (m_isTextOptimized) {
      UpdateOptimizedText();
    } else {
      Super::RemoveChildAt(indexToRemove);
    }
    RecalculateTextHighlighters();
  }

  void UpdateOptimizedText(bool clearOptimizedText = false) {
    if (m_children.size() > 0 && m_isTextOptimized) {
      if (const auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
        winrt::hstring text = L"";
        for (const auto childTag : m_children) {
          if (const auto childNode =
                  static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(childTag))) {
            text = text + childNode->GetView().as<winrt::Run>().Text();
          }
        }
        auto textBlock = this->GetView().as<xaml::Controls::TextBlock>();
        textBlock.Text(text);
      }
    } else if (m_isTextOptimized || clearOptimizedText) {
      auto textBlock = this->GetView().as<xaml::Controls::TextBlock>();
      textBlock.ClearValue(xaml::Controls::TextBlock::TextProperty());
    }
  }

  void RecalculateTextHighlighters() {
    const auto textBlock = this->GetView().as<xaml::Controls::TextBlock>();
    textBlock.TextHighlighters().Clear();

    // Since TextShadowNode is not public, we lift some of the recursive
    // algorithm into the shadow node implementation to detect when no
    // descendants have background colors and we can skip recursion.
    if (m_hasDescendantTextHighlighter) {
      const auto highlighters = GetNestedTextHighlighters(this, m_foregroundColor, m_backgroundColor);
      if (highlighters.size() == 0) {
        m_hasDescendantTextHighlighter = false;
      } else {
        // We must add the highlighters in reverse order, as highlighters
        // "deeper" in the text tree should render at the top.
        auto iter = highlighters.rbegin();
        while (iter != highlighters.rend()) {
          textBlock.TextHighlighters().Append(*iter);
          ++iter;
        }
      }
    }

    if (m_backgroundColor) {
      winrt::TextHighlighter highlighter{};
      highlighter.Ranges().Append({0, static_cast<int32_t>(textBlock.Text().size())});
      highlighter.Background(SolidBrushFromColor(m_backgroundColor.value()));
      if (m_foregroundColor) {
        highlighter.Foreground(SolidBrushFromColor(m_foregroundColor.value()));
      }
      textBlock.TextHighlighters().InsertAt(0, highlighter);
    }
  }

  void ToggleTouchEvents(XamlView xamlView, bool selectable) {
    if (selectable) {
      if (m_touchEventHandler == nullptr) {
        m_touchEventHandler = std::make_unique<TouchEventHandler>(GetViewManager()->GetReactContext());
      }

      m_selectionChangedRevoker = xamlView.as<xaml::Controls::TextBlock>().SelectionChanged(
          winrt::auto_revoke, [selectionChanged = this->selectionChanged](const auto &sender, auto &&) {
            const auto textBlock = sender.as<xaml::Controls::TextBlock>();
            *selectionChanged =
                *selectionChanged || textBlock.SelectionStart().Offset() != textBlock.SelectionEnd().Offset();
          });

      m_touchEventHandler->AddTouchHandlers(xamlView, GetRootView(), true);
    } else {
      if (m_touchEventHandler != nullptr) {
        m_touchEventHandler->RemoveTouchHandlers();
        m_selectionChangedRevoker.revoke();
      }
    }
  }

  XamlView GetRootView() {
    if (auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
      auto shadowNode = uiManager->getHost()->FindShadowNodeForTag(m_rootTag);
      if (!shadowNode)
        return nullptr;

      return static_cast<::Microsoft::ReactNative::ShadowNodeBase *>(shadowNode)->GetView();
    }
    return nullptr;
  }

  TextTransform textTransform{TextTransform::Undefined};
  std::shared_ptr<bool> selectionChanged = std::make_shared<bool>(false);
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

void TextViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    winrt::Microsoft::ReactNative::JSValueObject &props) {
  // This could be optimized further, but rather than paying a penalty to mark
  // the node dirty for each relevant property in UpdateProperty (which should
  // be reasonably cheap given it just does an O(1) lookup of the Yoga node
  // for the tag, for now this just marks the node dirty for any prop update.
  MarkDirty(nodeToUpdate->m_tag);
  Super::UpdateProperties(nodeToUpdate, props);
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
    UpdateTextTransformForChildren(nodeToUpdate);
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
    const auto node = static_cast<TextShadowNode *>(nodeToUpdate);
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
      const auto selectable = propertyValue.AsBoolean();
      textBlock.IsTextSelectionEnabled(selectable);
      node->ToggleTouchEvents(textBlock, selectable);
      if (selectable) {
        EnsureUniqueTextFlyoutForXamlIsland(textBlock);
      }
    } else if (propertyValue.IsNull()) {
      textBlock.ClearValue(xaml::Controls::TextBlock::IsTextSelectionEnabledProperty());
      node->ToggleTouchEvents(textBlock, false);
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

void TextViewManager::OnPointerEvent(
    ShadowNodeBase *node,
    winrt::Microsoft::ReactNative::ReactPointerEventArgs const &args) {
  const auto textNode = static_cast<TextShadowNode *>(node);
  const auto textBlock = node->GetView().as<xaml::Controls::TextBlock>();
  if (textNode->m_hasDescendantPressable && args.Target() == node->GetView()) {
    // Set the target to null temporarily
    args.Target(nullptr);

    // Get the pointer point and hit test
    const auto point = args.Args().GetCurrentPoint(textBlock).Position();
    HitTest(node, args, point);

    // Set the target back to the current view if hit test failed
    if (!args.Target()) {
      args.Target(node->GetView());
    }
  }

  if (args.Kind() == winrt::Microsoft::ReactNative::PointerEventKind::CaptureLost) {
    if (!*textNode->selectionChanged) {
      args.Kind(winrt::Microsoft::ReactNative::PointerEventKind::End);
    }
    *textNode->selectionChanged = false;
  }

  Super::OnPointerEvent(node, args);
}

/*static*/ void TextViewManager::UpdateTextHighlighters(ShadowNodeBase *node, bool highlightAdded) {
  if (IsTextShadowNode(node)) {
    const auto textNode = static_cast<TextShadowNode *>(node);
    if (highlightAdded) {
      textNode->m_hasDescendantTextHighlighter = true;
    }
    textNode->RecalculateTextHighlighters();
  }
}

/*static*/ void TextViewManager::UpdateOptimizedText(ShadowNodeBase *node) {
  if (IsTextShadowNode(node)) {
    const auto textNode = static_cast<TextShadowNode *>(node);
    textNode->UpdateOptimizedText();
  }
}

/*static*/ void TextViewManager::SetDescendantPressable(ShadowNodeBase *node) {
  if (IsTextShadowNode(node)) {
    const auto textNode = static_cast<TextShadowNode *>(node);
    textNode->m_hasDescendantPressable = true;
  }
}

/*static*/ TextTransform TextViewManager::GetTextTransformValue(ShadowNodeBase *node) {
  if (IsTextShadowNode(node)) {
    return static_cast<TextShadowNode *>(node)->textTransform;
  }

  return TextTransform::Undefined;
}

} // namespace Microsoft::ReactNative
