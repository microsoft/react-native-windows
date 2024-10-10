// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/components/rnwcore/EventEmitters.h>
#include <unordered_map>
#include "WindowsTextInputEventEmitter.h"
#include "WindowsTextInputProps.h"
#include "WindowsTextInputState.h"

#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/utils/ContextContainer.h>

#include <react/renderer/attributedstring/AttributedString.h>
#include <react/renderer/textlayoutmanager/TextLayoutManager.h>

namespace facebook::react {

extern const char WindowsTextInputComponentName[];

/*
 * `ShadowNode` for <WindowsTextInput> component.
 */
class WindowsTextInputShadowNode final : public ConcreteViewShadowNode<
                                             WindowsTextInputComponentName,
                                             WindowsTextInputProps,
                                             WindowsTextInputEventEmitter,
                                             WindowsTextInputState> {
 public:
  static ShadowNodeTraits BaseTraits() {
    auto traits = ConcreteViewShadowNode::BaseTraits();
    traits.set(ShadowNodeTraits::Trait::LeafYogaNode);
    return traits;
  }

  using ConcreteViewShadowNode::ConcreteViewShadowNode;

  void setContextContainer(ContextContainer *contextContainer);

  /*
   * Returns a `AttributedString` which represents text content of the node.
   */
  AttributedString getAttributedString(const LayoutContext &layoutContext) const;
  AttributedString getPlaceholderAttributedString(const LayoutContext &layoutContext) const;

  /*
   * Associates a shared TextLayoutManager with the node.
   * `ParagraphShadowNode` uses the manager to measure text content
   * and construct `ParagraphState` objects.
   */
  void setTextLayoutManager(SharedTextLayoutManager textLayoutManager);

#pragma mark - LayoutableShadowNode

  Size measureContent(const LayoutContext &layoutContext, const LayoutConstraints &layoutConstraints) const override;
  void layout(LayoutContext layoutContext) override;

 private:
  ContextContainer *m_contextContainer{};

  /**
   * Get the most up-to-date attributed string for measurement and State.
   */
  AttributedString getMostRecentAttributedString(const LayoutContext &layoutContext) const;

  /*
   * Creates a `State` object (with `AttributedText` and
   * `TextLayoutManager`) if needed.
   */
  void updateStateIfNeeded(const LayoutContext &layoutContext);

  SharedTextLayoutManager m_textLayoutManager;

  /*
   * Cached attributed string that represents the content of the subtree started
   * from the node.
   */
  mutable std::optional<AttributedString> m_cachedAttributedString{};
};

} // namespace facebook::react
