// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "TextViewManager.h"

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

  std::optional<winrt::Windows::UI::Color> m_ColorValue = std::nullopt;
  int32_t m_prevCursorEnd = 0;

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
        std::wstring text(run.Text().c_str());
        transformableText.originalText = text;
        text = transformableText.TransformText();
        textBlock.Text(winrt::hstring(text));

        if (m_ColorValue) {
          AddHighlighter(m_ColorValue.value(), text.size());
        }

        m_prevCursorEnd += textBlock.Text().size();

        return;
      }
    } else if (index == 1 && m_firstChildNode != nullptr) {
      auto textBlock = this->GetView().as<xaml::Controls::TextBlock>();
      textBlock.ClearValue(xaml::Controls::TextBlock::TextProperty());
      Super::AddView(*m_firstChildNode, 0);
      m_firstChildNode = nullptr;
    }

    Super::AddView(child, index);

    if (auto run = static_cast<ShadowNodeBase &>(child).GetView().try_as<winrt::Run>()) {
      if (m_ColorValue) {
        AddHighlighter(m_ColorValue.value(), run.Text().size());
      }

      m_prevCursorEnd += run.Text().size();
    } else if (auto span = static_cast<ShadowNodeBase &>(child).GetView().try_as<winrt::Span>()) {
      AddNestedTextHighlighter(m_ColorValue, span, static_cast<VirtualTextShadowNode &>(child).m_highlightData);
    }
  }

  void AddNestedTextHighlighter(
      const std::optional<winrt::Windows::UI::Color> &parentColor,
      winrt::Span &span,
      VirtualTextShadowNode::HighlightData highData) {
    if (!highData.color && parentColor) {
      highData.color = parentColor;
    }

    for (const auto &el : span.Inlines()) {
      if (auto run = el.try_as<winrt::Run>()) {
        if (highData.color) {
          AddHighlighter(highData.color.value(), run.Text().size());
        }

        m_prevCursorEnd += run.Text().size();
      } else if (auto spanChild = el.try_as<winrt::Span>()) {
        AddNestedTextHighlighter(highData.color, spanChild, highData.data[highData.spanIdx++]);
      }
    }
  }

  void AddHighlighter(const winrt::Windows::UI::Color &color, size_t runSize) {
    auto newHigh = winrt::TextHighlighter{};
    newHigh.Background(react::uwp::SolidBrushFromColor(color));

    winrt::TextRange newRange{m_prevCursorEnd, static_cast<int32_t>(runSize)};
    newHigh.Ranges().Append(newRange);

    this->GetView().as<xaml::Controls::TextBlock>().TextHighlighters().Append(newHigh);
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

  TransformableText transformableText{};
};

TextViewManager::TextViewManager(const Mso::React::IReactContext &context) : Super(context) {}

ShadowNode *TextViewManager::createShadow() const {
  return new TextShadowNode();
}

const wchar_t *TextViewManager::GetName() const {
  return L"RCTText";
}

XamlView TextViewManager::CreateViewCore(int64_t /*tag*/) {
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
  } else if (TryUpdateFontProperties(textBlock, propertyName, propertyValue)) {
  } else if (propertyName == "textTransform") {
    auto textNode = static_cast<TextShadowNode *>(nodeToUpdate);
    auto textTransform = TransformableText::GetTextTransform(propertyValue);
    textNode->transformableText.textTransform = textTransform;
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
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      textBlock.LineHeight(propertyValue.AsInt32());
    else if (propertyValue.IsNull())
      textBlock.ClearValue(xaml::Controls::TextBlock::LineHeightProperty());
  } else if (propertyName == "selectable") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
      textBlock.IsTextSelectionEnabled(propertyValue.AsBoolean());
    else if (propertyValue.IsNull())
      textBlock.ClearValue(xaml::Controls::TextBlock::IsTextSelectionEnabledProperty());
  } else if (propertyName == "allowFontScaling") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
      textBlock.IsTextScaleFactorEnabled(propertyValue.AsBoolean());
    } else {
      textBlock.ClearValue(xaml::Controls::TextBlock::IsTextScaleFactorEnabledProperty());
    }
  } else if (propertyName == "selectionColor") {
    if (react::uwp::IsValidColorValue(propertyValue)) {
      textBlock.SelectionHighlightColor(react::uwp::SolidColorBrushFrom(propertyValue));
    } else
      textBlock.ClearValue(xaml::Controls::TextBlock::SelectionHighlightColorProperty());
  } else if (propertyName == "backgroundColor") {
    if (react::uwp::IsValidColorValue(propertyValue)) {
      static_cast<TextShadowNode *>(nodeToUpdate)->m_ColorValue = react::uwp::ColorFrom(propertyValue);
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

} // namespace Microsoft::ReactNative
