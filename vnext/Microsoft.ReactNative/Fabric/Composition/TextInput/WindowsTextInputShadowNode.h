// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/textlayoutmanager/WindowsTextLayoutManager.h>
#include "WindowsTextInputEventEmitter.h"
#include "WindowsTextInputProps.h"
#include "WindowsTextInputState.h"

#include <react/components/rnwcore/EventEmitters.h>
#include <unordered_map>

#include <react/renderer/attributedstring/AttributedString.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/utils/ContextContainer.h>

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
  using ConcreteViewShadowNode::ConcreteViewShadowNode;

  static ShadowNodeTraits BaseTraits() {
    auto traits = ConcreteViewShadowNode::BaseTraits();
    traits.set(ShadowNodeTraits::Trait::LeafYogaNode);
    traits.set(ShadowNodeTraits::Trait::MeasurableYogaNode);
    traits.set(ShadowNodeTraits::Trait::BaselineYogaNode);
    return traits;
  }

  /*
   * Associates a shared TextLayoutManager with the node.
   * `TextInputShadowNode` uses the manager to measure text content
   * and construct `TextInputState` objects.
   */
  void setTextLayoutManager(std::shared_ptr<const TextLayoutManager> textLayoutManager);

#pragma mark - LayoutableShadowNode
 protected:
  Size measureContent(const LayoutContext &layoutContext, const LayoutConstraints &layoutConstraints) const override;

  void layout(LayoutContext layoutContext) override;

  Float baseline(const LayoutContext &layoutContext, Size size) const override {
    // Calculate baseline as 80% of the text height
    return size.height * 0.8f;
  }

  std::shared_ptr<const TextLayoutManager> textLayoutManager_;

  /*
   * Determines the constraints to use while measure the underlying text
   */
  LayoutConstraints getTextConstraints(const LayoutConstraints &layoutConstraints) const;

 private:
  /*
   * Creates a `State` object (with `AttributedText` and
   * `TextLayoutManager`) if needed.
   */
  void updateStateIfNeeded(const LayoutContext &layoutContext);

  /*
   * Returns a `AttributedString` which represents text content of the node.
   */
  AttributedString getAttributedString(const LayoutContext &layoutContext) const;

  /**
   * Get the most up-to-date attributed string for measurement and State.
   */
  AttributedString getMostRecentAttributedString(const LayoutContext &layoutContext) const;

  AttributedString getPlaceholderAttributedString(const LayoutContext &layoutContext) const;
};

} // namespace facebook::react
