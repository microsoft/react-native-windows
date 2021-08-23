// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Xaml.Documents.h>
#include <Utils/TextTransform.h>
#include <Views/ShadowNodeBase.h>
#include <stack>

namespace Microsoft::ReactNative {

class TextVisitor {
 public:
  void Visit(ShadowNode *node);

 protected:
  virtual void VisitExtensionText(ShadowNodeBase *node);

  virtual void VisitRawText(ShadowNodeBase *node) {}

  virtual void VisitText(ShadowNodeBase *node);

  virtual void VisitVirtualText(ShadowNodeBase *node);

  virtual void VisitChildren(ShadowNodeBase *node);

 private:
  std::shared_ptr<NativeUIManager> m_uiManager;
  std::shared_ptr<NativeUIManager> EnsureNativeUIManager(ShadowNode *node);
};

class TextTransformVisitor : public TextVisitor {
  using Super = TextVisitor;

 public:
  TextTransformVisitor() : Super(), m_forceUpdate{true} {}

  TextTransformVisitor(TextTransform parentTransform) : Super(), m_forceUpdate{false} {
    m_textTransforms.push(parentTransform);
  }

 protected:
  void VisitRawText(ShadowNodeBase *node) override;

  void VisitText(ShadowNodeBase *node) override;

  void VisitVirtualText(ShadowNodeBase *node) override;

 private:
  std::stack<TextTransform> m_textTransforms;
  bool m_forceUpdate;

  bool ShouldApplyTransform(TextTransform transform);
  bool Push(TextTransform transform);
  void Pop();
  TextTransform Top();
};

class TextHighlighterVisitor : public TextVisitor {
  using Super = TextVisitor;
  using Color = std::optional<winrt::Windows::UI::Color>;

 public:
  TextHighlighterVisitor(Color foregroundColor, Color backgroundColor) : Super() {
    m_foregroundColors.push(foregroundColor);
    m_backgroundColors.push(backgroundColor);
  }

  std::vector<xaml::Documents::TextHighlighter> highlighters{};

 protected:
  void VisitRawText(ShadowNodeBase *node) override;

  void VisitVirtualText(ShadowNodeBase *node) override;

 private:
  int m_startIndex{0};
  std::stack<Color> m_foregroundColors;
  std::stack<Color> m_backgroundColors;

  bool RequiresTextHighlighter(Color foregroundColor, Color backgroundColor);
  static bool Push(std::stack<Color> &stack, Color color);
};

}; // namespace Microsoft::ReactNative
