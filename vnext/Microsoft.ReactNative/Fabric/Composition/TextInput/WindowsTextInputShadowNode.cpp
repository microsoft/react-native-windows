// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WindowsTextInputShadowNode.h"

#include <react/featureflags/ReactNativeFeatureFlags.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/attributedstring/TextAttributes.h>
#include <react/renderer/components/text/BaseTextShadowNode.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/core/LayoutContext.h>
#include <react/renderer/core/conversions.h>
#include <react/renderer/textlayoutmanager/TextLayoutContext.h>

namespace facebook::react {

extern const char WindowsTextInputComponentName[] = "WindowsTextInput";

void WindowsTextInputShadowNode::setTextLayoutManager(std::shared_ptr<const TextLayoutManager> textLayoutManager) {
  ensureUnsealed();
  textLayoutManager_ = std::move(textLayoutManager);
}

Size WindowsTextInputShadowNode::measureContent(
    const LayoutContext &layoutContext,
    const LayoutConstraints &layoutConstraints) const {
  // Layout is called right after measure.
  // Measure is marked as `const`, and `layout` is not; so State can be
  // updated during layout, but not during `measure`. If State is out-of-date
  // in layout, it's too late: measure will have already operated on old
  // State. Thus, we use the same value here that we *will* use in layout to
  // update the state.
  AttributedString attributedString = getMostRecentAttributedString(layoutContext);

  if (attributedString.isEmpty()) {
    attributedString = getPlaceholderAttributedString(layoutContext);
  }

  // BaseTextShadowNode only gets children. We must detect and prepend text
  // value attributes manually.
  if (!getConcreteProps().text.empty()) {
    auto textAttributes = TextAttributes::defaultTextAttributes();
    textAttributes.apply(getConcreteProps().textAttributes);
    auto fragment = AttributedString::Fragment{};
    fragment.string = getConcreteProps().text;
    fragment.textAttributes = textAttributes;
    // If the TextInput opacity is 0 < n < 1, the opacity of the TextInput and
    // text value's background will stack. This is a hack/workaround to prevent
    // that effect.
    fragment.textAttributes.backgroundColor = clearColor();
    fragment.parentShadowView = ShadowView(*this);
    attributedString.prependFragment(std::move(fragment));
  }

  TextLayoutContext textLayoutContext;
  textLayoutContext.pointScaleFactor = layoutContext.pointScaleFactor;

  facebook::react::ParagraphAttributes paragraphAttributes{};
  paragraphAttributes.maximumNumberOfLines = getConcreteProps().multiline ? 0 : 1;
  return textLayoutManager_
      ->measure(AttributedStringBox{attributedString}, paragraphAttributes, textLayoutContext, layoutConstraints)
      .size;
}

void WindowsTextInputShadowNode::layout(LayoutContext layoutContext) {
  updateStateIfNeeded(layoutContext);
  ConcreteViewShadowNode::layout(layoutContext);
}

LayoutConstraints WindowsTextInputShadowNode::getTextConstraints(const LayoutConstraints &layoutConstraints) const {
  if (getConcreteProps().multiline) {
    return layoutConstraints;
  } else {
    // A single line TextInput acts as a horizontal scroller of infinitely
    // expandable text, so we want to measure the text as if it is allowed to
    // infinitely expand horizontally, and later clamp to the constraints of the
    // input.
    return LayoutConstraints{
        .minimumSize = layoutConstraints.minimumSize,
        .maximumSize =
            Size{
                .width = std::numeric_limits<Float>::infinity(),
                .height = layoutConstraints.maximumSize.height,
            },
        .layoutDirection = layoutConstraints.layoutDirection,
    };
  }
}

void WindowsTextInputShadowNode::updateStateIfNeeded(const LayoutContext &layoutContext) {
  ensureUnsealed();
  const auto &stateData = getStateData();
  auto reactTreeAttributedString = getAttributedString(layoutContext);

  // Tree is often out of sync with the value of the TextInput.
  // This is by design - don't change the value of the TextInput in the State,
  // and therefore in Java, unless the tree itself changes.
  if (stateData.reactTreeAttributedString == reactTreeAttributedString) {
    return;
  }

  // If props event counter is less than what we already have in state, skip it
  const auto &props = BaseShadowNode::getConcreteProps();
  if (props.mostRecentEventCount < stateData.mostRecentEventCount) {
    return;
  }

  // Store default TextAttributes in state.
  // In the case where the TextInput is completely empty (no value, no
  // defaultValue, no placeholder, no children) there are therefore no fragments
  // in the AttributedString, and when State is updated, it needs some way to
  // reconstruct a Fragment with default TextAttributes.
  auto defaultTextAttributes = TextAttributes::defaultTextAttributes();
  defaultTextAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
  defaultTextAttributes.apply(getConcreteProps().textAttributes);

  auto newEventCount = stateData.reactTreeAttributedString.isContentEqual(reactTreeAttributedString)
      ? 0
      : getConcreteProps().mostRecentEventCount;
  auto newAttributedString = getMostRecentAttributedString(layoutContext);

  // Even if we're here and updating state, it may be only to update the layout
  // manager If that is the case, make sure we don't update text: pass in the
  // current attributedString unchanged, and pass in zero for the "event count"
  // so no changes are applied There's no way to prevent a state update from
  // flowing to Java, so we just ensure it's a noop in those cases.

  setStateData(
      facebook::react::TextInputState{
          AttributedStringBox(newAttributedString), reactTreeAttributedString, {}, newEventCount});
}

AttributedString WindowsTextInputShadowNode::getAttributedString(const LayoutContext &layoutContext) const {
  // Use BaseTextShadowNode to get attributed string from children

  auto childTextAttributes = TextAttributes::defaultTextAttributes();
  childTextAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
  childTextAttributes.apply(getConcreteProps().textAttributes);
  // Don't propagate the background color of the TextInput onto the attributed
  // string. Android tries to render shadow of the background alongside the
  // shadow of the text which results in weird artifacts.
  childTextAttributes.backgroundColor = HostPlatformColor::UndefinedColor;

  auto attributedString = AttributedString{};
  auto attachments = BaseTextShadowNode::Attachments{};
  BaseTextShadowNode::buildAttributedString(childTextAttributes, *this, attributedString, attachments);
  attributedString.setBaseTextAttributes(childTextAttributes);

  // BaseTextShadowNode only gets children. We must detect and prepend text
  // value attributes manually.
  if (!getConcreteProps().text.empty()) {
    auto textAttributes = TextAttributes::defaultTextAttributes();
    textAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
    textAttributes.apply(getConcreteProps().textAttributes);
    textAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
    auto fragment = AttributedString::Fragment{};
    fragment.string = getConcreteProps().text;
    fragment.textAttributes = textAttributes;
    // If the TextInput opacity is 0 < n < 1, the opacity of the TextInput and
    // text value's background will stack. This is a hack/workaround to prevent
    // that effect.
    fragment.textAttributes.backgroundColor = clearColor();
    fragment.parentShadowView = ShadowView(*this);
    attributedString.prependFragment(std::move(fragment));
  }

  return attributedString;
}

AttributedString WindowsTextInputShadowNode::getMostRecentAttributedString(const LayoutContext &layoutContext) const {
  const auto &state = getStateData();

  auto reactTreeAttributedString = getAttributedString(layoutContext);

  // Sometimes the treeAttributedString will only differ from the state
  // not by inherent properties (string or prop attributes), but by the frame of
  // the parent which has changed Thus, we can't directly compare the entire
  // AttributedString
  bool treeAttributedStringChanged =
      !state.reactTreeAttributedString.compareTextAttributesWithoutFrame(reactTreeAttributedString);

  return (!treeAttributedStringChanged ? state.attributedStringBox.getValue() : reactTreeAttributedString);
}

// For measurement purposes, we want to make sure that there's at least a
// single character in the string so that the measured height is greater
// than zero. Otherwise, empty TextInputs with no placeholder don't
// display at all.
// TODO T67606511: We will redefine the measurement of empty strings as part
// of T67606511
AttributedString WindowsTextInputShadowNode::getPlaceholderAttributedString(const LayoutContext &layoutContext) const {
  const auto &props = BaseShadowNode::getConcreteProps();

  AttributedString attributedString;
  auto placeholderString = !props.placeholder.empty() ? props.placeholder : BaseTextShadowNode::getEmptyPlaceholder();
  auto textAttributes = TextAttributes::defaultTextAttributes();
  textAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
  textAttributes.apply(props.textAttributes);
  attributedString.appendFragment(
      {.string = std::move(placeholderString),
       .textAttributes = textAttributes,
       .parentShadowView = ShadowView(*this)});
  return attributedString;
}

#pragma mark - LayoutableShadowNode

} // namespace facebook::react
