/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "YogaLayoutableShadowNode.h"
#include <logger/react_native_log.h>
#include <react/debug/flags.h>
#include <react/debug/react_native_assert.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/components/view/ViewShadowNode.h>
#include <react/renderer/components/view/conversions.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/core/LayoutContext.h>
#include <react/renderer/core/TraitCast.h>
#include <react/renderer/debug/DebugStringConvertibleItem.h>
#include <react/renderer/debug/SystraceSection.h>
#include <react/utils/CoreFeatures.h>
#include <yoga/Yoga.h>
#include <algorithm>
#include <limits>
#include <memory>

namespace facebook::react {

static int FabricDefaultYogaLog(
    const YGConfigRef /*unused*/,
    const YGNodeRef /*unused*/,
    YGLogLevel level,
    const char *format,
    va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);

  // Adding 1 to add space for terminating null character.
  int size_s = vsnprintf(nullptr, 0, format, args);
  auto size = static_cast<size_t>(size_s);
  std::vector<char> buffer(size);

  vsnprintf(buffer.data(), size, format, args_copy);
  switch (level) {
    case YGLogLevelError:
      react_native_log_error(buffer.data());
      break;
    case YGLogLevelFatal:
      react_native_log_fatal(buffer.data());
      break;
    case YGLogLevelWarn:
      react_native_log_warn(buffer.data());
      break;
    case YGLogLevelInfo:
    case YGLogLevelDebug:
    case YGLogLevelVerbose:
    default:
      react_native_log_info(buffer.data());
  }

  return size_s;
}

thread_local LayoutContext threadLocalLayoutContext;

ShadowNodeTraits YogaLayoutableShadowNode::BaseTraits() {
  auto traits = LayoutableShadowNode::BaseTraits();
  traits.set(IdentifierTrait());
  return traits;
}

ShadowNodeTraits::Trait YogaLayoutableShadowNode::IdentifierTrait() {
  return ShadowNodeTraits::Trait::YogaLayoutableKind;
}

YogaLayoutableShadowNode::YogaLayoutableShadowNode(
    ShadowNodeFragment const &fragment,
    ShadowNodeFamily::Shared const &family,
    ShadowNodeTraits traits)
    : LayoutableShadowNode(fragment, family, traits),
      yogaConfig_(FabricDefaultYogaLog),
      yogaNode_(&initializeYogaConfig(yogaConfig_)) {
  yogaNode_.setContext(this);

  // Newly created node must be `dirty` just because it is new.
  // This is not a default for `YGNode`.
  yogaNode_.setDirty(true);

  if (getTraits().check(ShadowNodeTraits::Trait::MeasurableYogaNode)) {
    react_native_assert(
        getTraits().check(ShadowNodeTraits::Trait::LeafYogaNode));

    yogaNode_.setMeasureFunc(
        YogaLayoutableShadowNode::yogaNodeMeasureCallbackConnector);
  }

  updateYogaProps();
  updateYogaChildren();

  ensureConsistency();
}

YogaLayoutableShadowNode::YogaLayoutableShadowNode(
    ShadowNode const &sourceShadowNode,
    ShadowNodeFragment const &fragment)
    : LayoutableShadowNode(sourceShadowNode, fragment),
      yogaConfig_(FabricDefaultYogaLog),
      yogaNode_(static_cast<YogaLayoutableShadowNode const &>(sourceShadowNode)
                    .yogaNode_) {
  // Note, cloned `YGNode` instance (copied using copy-constructor) inherits
  // dirty flag, measure function, and other properties being set originally in
  // the `YogaLayoutableShadowNode` constructor above.

  react_native_assert(
      static_cast<YogaLayoutableShadowNode const &>(sourceShadowNode)
              .yogaNode_.isDirty() == yogaNode_.isDirty() &&
      "Yoga node must inherit dirty flag.");

  if (!getTraits().check(ShadowNodeTraits::Trait::LeafYogaNode)) {
    for (auto &child : getChildren()) {
      if (auto layoutableChild = traitCast<YogaLayoutableShadowNode>(child)) {
        yogaLayoutableChildren_.push_back(layoutableChild);
      }
    }
  }

  YGConfigRef previousConfig = YGNodeGetConfig(
      &static_cast<YogaLayoutableShadowNode const &>(sourceShadowNode)
           .yogaNode_);

  yogaNode_.setContext(this);
  yogaNode_.setOwner(nullptr);
  yogaNode_.setConfig(&initializeYogaConfig(yogaConfig_, previousConfig));
  updateYogaChildrenOwnersIfNeeded();

  // This is the only legit place where we can dirty cloned Yoga node.
  // If we do it later, ancestor nodes will not be able to observe this and
  // dirty (and clone) themselves as a result.
  if (getTraits().check(ShadowNodeTraits::Trait::DirtyYogaNode) ||
      getTraits().check(ShadowNodeTraits::Trait::MeasurableYogaNode)) {
    yogaNode_.setDirty(true);
  }

  // We do not need to reconfigure this subtree before the next layout pass if
  // the previous node with the same props and children has already been
  // configured.
  if (!fragment.props && !fragment.children) {
    yogaTreeHasBeenConfigured_ =
        static_cast<YogaLayoutableShadowNode const &>(sourceShadowNode)
            .yogaTreeHasBeenConfigured_;
  }

  if (fragment.props) {
    updateYogaProps();
  }

  if (fragment.children) {
    updateYogaChildren();
  }

  ensureConsistency();
}

void YogaLayoutableShadowNode::cleanLayout() {
  yogaNode_.setDirty(false);
}

void YogaLayoutableShadowNode::dirtyLayout() {
  yogaNode_.setDirty(true);
}

bool YogaLayoutableShadowNode::getIsLayoutClean() const {
  return !yogaNode_.isDirty();
}

#pragma mark - Mutating Methods

void YogaLayoutableShadowNode::enableMeasurement() {
  ensureUnsealed();

  yogaNode_.setMeasureFunc(
      YogaLayoutableShadowNode::yogaNodeMeasureCallbackConnector);
}

void YogaLayoutableShadowNode::appendYogaChild(
    YogaLayoutableShadowNode::Shared const &childNode) {
  // The caller must check this before calling this method.
  react_native_assert(
      !getTraits().check(ShadowNodeTraits::Trait::LeafYogaNode));

  ensureYogaChildrenLookFine();

  yogaLayoutableChildren_.push_back(childNode);
  yogaNode_.insertChild(
      &childNode->yogaNode_,
      static_cast<uint32_t>(yogaNode_.getChildren().size()));

  ensureYogaChildrenLookFine();
}

void YogaLayoutableShadowNode::adoptYogaChild(size_t index) {
  ensureUnsealed();
  ensureYogaChildrenLookFine();

  // The caller must check this before calling this method.
  react_native_assert(
      !getTraits().check(ShadowNodeTraits::Trait::LeafYogaNode));

  auto &childNode =
      traitCast<YogaLayoutableShadowNode const &>(*getChildren().at(index));

  if (childNode.yogaNode_.getOwner() == nullptr) {
    // The child node is not owned.
    childNode.yogaNode_.setOwner(&yogaNode_);
    // At this point the child yoga node must be already inserted by the caller.
    // react_native_assert(layoutableChildNode.yogaNode_.isDirty());
  } else {
    // The child is owned by some other node, we need to clone that.
    // TODO: At this point, React has wrong reference to the node. (T138668036)
    auto clonedChildNode = childNode.clone({});

    // Replace the child node with a newly cloned one in the children list.
    replaceChild(childNode, clonedChildNode, static_cast<int>(index));
  }

  ensureYogaChildrenLookFine();
}

void YogaLayoutableShadowNode::appendChild(
    ShadowNode::Shared const &childNode) {
  ensureUnsealed();
  ensureConsistency();

  // Calling the base class (`ShadowNode`) method.
  LayoutableShadowNode::appendChild(childNode);

  if (getTraits().check(ShadowNodeTraits::Trait::LeafYogaNode)) {
    // This node is a declared leaf.
    return;
  }

  if (auto yogaLayoutableChild =
          traitCast<YogaLayoutableShadowNode>(childNode)) {
    // Here we don't have information about the previous structure of the node
    // (if it that existed before), so we don't have anything to compare the
    // Yoga node with (like a previous version of this node). Therefore we must
    // dirty the node.
    yogaNode_.setDirty(true);

    // Appending the Yoga node.
    appendYogaChild(yogaLayoutableChild);

    ensureYogaChildrenLookFine();
    ensureYogaChildrenAlignment();

    // Adopting the Yoga node.
    adoptYogaChild(getChildren().size() - 1);

    ensureConsistency();
  }
}

void YogaLayoutableShadowNode::replaceChild(
    ShadowNode const &oldChild,
    ShadowNode::Shared const &newChild,
    int32_t suggestedIndex) {
  LayoutableShadowNode::replaceChild(oldChild, newChild, suggestedIndex);

  ensureUnsealed();
  ensureYogaChildrenLookFine();

  auto layoutableOldChild =
      traitCast<YogaLayoutableShadowNode const *>(&oldChild);
  auto layoutableNewChild = traitCast<YogaLayoutableShadowNode>(newChild);

  if (layoutableOldChild == nullptr && layoutableNewChild == nullptr) {
    // No need to mutate yogaLayoutableChildren_
    return;
  }

  // [Windows]
  bool suggestedIndexAccurate = suggestedIndex >= 0 &&
      suggestedIndex < static_cast<int32_t>(yogaLayoutableChildren_.size()) &&
      yogaLayoutableChildren_[suggestedIndex].get() == layoutableOldChild;

  auto oldChildIter = suggestedIndexAccurate
      ? yogaLayoutableChildren_.begin() + suggestedIndex
      : std::find_if(
            yogaLayoutableChildren_.begin(),
            yogaLayoutableChildren_.end(),
            [&](YogaLayoutableShadowNode::Shared const &layoutableChild) {
              return layoutableChild.get() == layoutableOldChild;
            });
  auto oldChildIndex =
      static_cast<int32_t>(oldChildIter - yogaLayoutableChildren_.begin());

  if (oldChildIter == yogaLayoutableChildren_.end()) {
    // oldChild does not exist as part of our node
    return;
  }

  if (layoutableNewChild) {
    // Both children are layoutable, replace the old one with the new one
    react_native_assert(layoutableNewChild->yogaNode_.getOwner() == nullptr);
    layoutableNewChild->yogaNode_.setOwner(&yogaNode_);
    *oldChildIter = layoutableNewChild;
    yogaNode_.replaceChild(&layoutableNewChild->yogaNode_, oldChildIndex);
  } else {
    // Layoutable child replaced with non layoutable child. Remove the previous
    // child from the layoutable children list.
    yogaLayoutableChildren_.erase(oldChildIter);
    yogaNode_.removeChild(oldChildIndex);
  }

  ensureYogaChildrenLookFine();
}

bool YogaLayoutableShadowNode::doesOwn(
    YogaLayoutableShadowNode const &child) const {
  return child.yogaNode_.getOwner() == &yogaNode_;
}

void YogaLayoutableShadowNode::updateYogaChildrenOwnersIfNeeded() {
  for (auto &childYogaNode : yogaNode_.getChildren()) {
    if (childYogaNode->getOwner() == &yogaNode_) {
      childYogaNode->setOwner(reinterpret_cast<YGNodeRef>(0xBADC0FFEE0DDF00D));
    }
  }
}

void YogaLayoutableShadowNode::updateYogaChildren() {
  if (getTraits().check(ShadowNodeTraits::Trait::LeafYogaNode)) {
    return;
  }

  ensureUnsealed();

  bool isClean = !yogaNode_.isDirty() &&
      getChildren().size() == yogaNode_.getChildren().size();

  auto oldYogaChildren = isClean ? yogaNode_.getChildren() : YGVector{};
  yogaNode_.setChildren({});
  yogaLayoutableChildren_.clear();

  for (size_t i = 0; i < getChildren().size(); i++) {
    if (auto yogaLayoutableChild =
            traitCast<YogaLayoutableShadowNode>(getChildren()[i])) {
      appendYogaChild(yogaLayoutableChild);
      adoptYogaChild(i);

      if (isClean) {
        auto yogaChildIndex = yogaLayoutableChildren_.size() - 1;
        auto &oldYogaChildNode = *oldYogaChildren.at(yogaChildIndex);
        auto &newYogaChildNode =
            yogaLayoutableChildren_.at(yogaChildIndex)->yogaNode_;

        isClean = isClean && !newYogaChildNode.isDirty() &&
            (newYogaChildNode.getStyle() == oldYogaChildNode.getStyle());
      }
    }
  }

  react_native_assert(
      yogaLayoutableChildren_.size() == yogaNode_.getChildren().size());

  yogaNode_.setDirty(!isClean);
}

void YogaLayoutableShadowNode::updateYogaProps() {
  ensureUnsealed();

  auto props = static_cast<YogaStylableProps const &>(*props_);
  auto styleResult = applyAliasedProps(props.yogaStyle, props);

  // Resetting `dirty` flag only if `yogaStyle` portion of `Props` was changed.
  if (!yogaNode_.isDirty() && (styleResult != yogaNode_.getStyle())) {
    yogaNode_.setDirty(true);
  }

  yogaNode_.setStyle(styleResult);
}

/*static*/ YGStyle YogaLayoutableShadowNode::applyAliasedProps(
    const YGStyle &baseStyle,
    const YogaStylableProps &props) {
  YGStyle result{baseStyle};

  // Aliases with precedence
  if (!props.inset.isUndefined()) {
    result.position()[YGEdgeAll] = props.inset;
  }
  if (!props.insetBlock.isUndefined()) {
    result.position()[YGEdgeVertical] = props.insetBlock;
  }
  if (!props.insetInline.isUndefined()) {
    result.position()[YGEdgeHorizontal] = props.insetInline;
  }
  if (!props.insetInlineEnd.isUndefined()) {
    result.position()[YGEdgeEnd] = props.insetInlineEnd;
  }
  if (!props.insetInlineStart.isUndefined()) {
    result.position()[YGEdgeStart] = props.insetInlineStart;
  }
  if (!props.marginInline.isUndefined()) {
    result.margin()[YGEdgeHorizontal] = props.marginInline;
  }
  if (!props.marginInlineStart.isUndefined()) {
    result.margin()[YGEdgeStart] = props.marginInlineStart;
  }
  if (!props.marginInlineEnd.isUndefined()) {
    result.margin()[YGEdgeEnd] = props.marginInlineEnd;
  }
  if (!props.marginBlock.isUndefined()) {
    result.margin()[YGEdgeVertical] = props.marginBlock;
  }
  if (!props.paddingInline.isUndefined()) {
    result.padding()[YGEdgeHorizontal] = props.paddingInline;
  }
  if (!props.paddingInlineStart.isUndefined()) {
    result.padding()[YGEdgeStart] = props.paddingInlineStart;
  }
  if (!props.paddingInlineEnd.isUndefined()) {
    result.padding()[YGEdgeEnd] = props.paddingInlineEnd;
  }
  if (!props.paddingBlock.isUndefined()) {
    result.padding()[YGEdgeVertical] = props.paddingBlock;
  }

  // Aliases without precedence
  if (CompactValue(result.position()[YGEdgeBottom]).isUndefined()) {
    result.position()[YGEdgeBottom] = props.insetBlockEnd;
  }
  if (CompactValue(result.position()[YGEdgeTop]).isUndefined()) {
    result.position()[YGEdgeTop] = props.insetBlockStart;
  }
  if (CompactValue(result.margin()[YGEdgeTop]).isUndefined()) {
    result.margin()[YGEdgeTop] = props.marginBlockStart;
  }
  if (CompactValue(result.margin()[YGEdgeBottom]).isUndefined()) {
    result.margin()[YGEdgeBottom] = props.marginBlockEnd;
  }
  if (CompactValue(result.padding()[YGEdgeTop]).isUndefined()) {
    result.padding()[YGEdgeTop] = props.paddingBlockStart;
  }
  if (CompactValue(result.padding()[YGEdgeBottom]).isUndefined()) {
    result.padding()[YGEdgeBottom] = props.paddingBlockEnd;
  }

  return result;
}

void YogaLayoutableShadowNode::configureYogaTree(
    float pointScaleFactor,
    YGErrata defaultErrata,
    bool swapLeftAndRight) {
  ensureUnsealed();

  // Set state on our own Yoga node
  YGErrata errata = resolveErrata(defaultErrata);
  YGConfigSetErrata(&yogaConfig_, errata);
  YGConfigSetPointScaleFactor(&yogaConfig_, pointScaleFactor);

  // TODO: `swapLeftAndRight` modified backing props and cannot be undone
  if (swapLeftAndRight) {
    swapStyleLeftAndRight();
  }

  yogaTreeHasBeenConfigured_ = true;

  // Recursively propagate the configuration to child nodes. If a child was
  // already configured as part of a previous ShadowTree generation, we only
  // need to reconfigure it if the context values passed to the Node have
  // changed.
  // [Windows]
  for (size_t i = 0; i < yogaLayoutableChildren_.size(); i++) {
    const auto &child = *yogaLayoutableChildren_[i];
    auto childLayoutMetrics = child.getLayoutMetrics();
    auto childErrata =
        YGConfigGetErrata(const_cast<YGConfigRef>(&child.yogaConfig_));

    if (child.yogaTreeHasBeenConfigured_ &&
        childLayoutMetrics.pointScaleFactor == pointScaleFactor &&
        childLayoutMetrics.wasLeftAndRightSwapped == swapLeftAndRight &&
        childErrata == child.resolveErrata(errata)) {
      continue;
    }

    if (doesOwn(child)) {
      auto &mutableChild = const_cast<YogaLayoutableShadowNode &>(child);
      mutableChild.configureYogaTree(
          pointScaleFactor, child.resolveErrata(errata), swapLeftAndRight);
    } else {
      cloneChildInPlace(static_cast<int32_t>(i)).configureYogaTree(
          pointScaleFactor, errata, swapLeftAndRight);
    }
  }
}

YGErrata YogaLayoutableShadowNode::resolveErrata(YGErrata defaultErrata) const {
  if (auto viewShadowNode = traitCast<ViewShadowNode const *>(this)) {
    const auto &props = viewShadowNode->getConcreteProps();
    switch (props.experimental_layoutConformance) {
      case LayoutConformance::Classic:
        return YGErrataAll;
      case LayoutConformance::Strict:
        return YGErrataNone;
      case LayoutConformance::Undefined:
        return defaultErrata;
    }
  }

  return defaultErrata;
}

YogaLayoutableShadowNode &YogaLayoutableShadowNode::cloneChildInPlace(
    int32_t layoutableChildIndex) {
  ensureUnsealed();

  const auto &childNode = *yogaLayoutableChildren_[layoutableChildIndex];

  // TODO: Why does this not use `ShadowNodeFragment::statePlaceholder()` like
  // `adoptYogaChild()`?
  auto clonedChildNode = childNode.clone(
      {ShadowNodeFragment::propsPlaceholder(),
       ShadowNodeFragment::childrenPlaceholder(),
       childNode.getState()});

  replaceChild(childNode, clonedChildNode, layoutableChildIndex);
  return static_cast<YogaLayoutableShadowNode &>(*clonedChildNode);
}

void YogaLayoutableShadowNode::setSize(Size size) const {
  ensureUnsealed();

  auto style = yogaNode_.getStyle();
  style.dimensions()[YGDimensionWidth] = yogaStyleValueFromFloat(size.width);
  style.dimensions()[YGDimensionHeight] = yogaStyleValueFromFloat(size.height);
  yogaNode_.setStyle(style);
  yogaNode_.setDirty(true);
}

void YogaLayoutableShadowNode::setPadding(RectangleEdges<Float> padding) const {
  ensureUnsealed();

  auto style = yogaNode_.getStyle();

  auto leftPadding = yogaStyleValueFromFloat(padding.left);
  auto topPadding = yogaStyleValueFromFloat(padding.top);
  auto rightPadding = yogaStyleValueFromFloat(padding.right);
  auto bottomPadding = yogaStyleValueFromFloat(padding.bottom);

  if (leftPadding != style.padding()[YGEdgeLeft] ||
      topPadding != style.padding()[YGEdgeTop] ||
      rightPadding != style.padding()[YGEdgeRight] ||
      bottomPadding != style.padding()[YGEdgeBottom]) {
    style.padding()[YGEdgeTop] = yogaStyleValueFromFloat(padding.top);
    style.padding()[YGEdgeLeft] = yogaStyleValueFromFloat(padding.left);
    style.padding()[YGEdgeRight] = yogaStyleValueFromFloat(padding.right);
    style.padding()[YGEdgeBottom] = yogaStyleValueFromFloat(padding.bottom);
    yogaNode_.setStyle(style);
    yogaNode_.setDirty(true);
  }
}

void YogaLayoutableShadowNode::setPositionType(
    YGPositionType positionType) const {
  ensureUnsealed();

  auto style = yogaNode_.getStyle();
  style.positionType() = positionType;
  yogaNode_.setStyle(style);
  yogaNode_.setDirty(true);
}

void YogaLayoutableShadowNode::layoutTree(
    LayoutContext layoutContext,
    LayoutConstraints layoutConstraints) {
  ensureUnsealed();

  SystraceSection s1("YogaLayoutableShadowNode::layoutTree");

  bool swapLeftAndRight = layoutContext.swapLeftAndRightInRTL &&
      (layoutConstraints.layoutDirection == LayoutDirection::RightToLeft ||
       !CoreFeatures::doNotSwapLeftAndRightOnAndroidInLTR);

  {
    SystraceSection s2("YogaLayoutableShadowNode::configureYogaTree");
    configureYogaTree(
        layoutContext.pointScaleFactor,
        YGErrataAll /*defaultErrata*/,
        swapLeftAndRight);
  }

  auto minimumSize = layoutConstraints.minimumSize;
  auto maximumSize = layoutConstraints.maximumSize;

  // The caller must ensure that layout constraints make sense.
  // Values cannot be NaN.
  react_native_assert(!std::isnan(minimumSize.width));
  react_native_assert(!std::isnan(minimumSize.height));
  react_native_assert(!std::isnan(maximumSize.width));
  react_native_assert(!std::isnan(maximumSize.height));
  // Values cannot be negative.
  react_native_assert(minimumSize.width >= 0);
  react_native_assert(minimumSize.height >= 0);
  react_native_assert(maximumSize.width >= 0);
  react_native_assert(maximumSize.height >= 0);
  // Minimum size cannot be infinity.
  react_native_assert(!std::isinf(minimumSize.width));
  react_native_assert(!std::isinf(minimumSize.height));

  // Internally Yoga uses three different measurement modes controlling layout
  // constraints: `Undefined`, `Exactly`, and `AtMost`. These modes are an
  // implementation detail and are not defined in `CSS Flexible Box Layout
  // Module`. Yoga C++ API (and `YGNodeCalculateLayout` function particularly)
  // does not allow to specify the measure modes explicitly. Instead, it infers
  // these from styles associated with the root node.
  // To pass the actual layout constraints to Yoga we represent them as
  // `(min/max)(Height/Width)` style properties. Also, we pass `ownerWidth` &
  // `ownerHeight` to allow proper calculation of relative (e.g. specified in
  // percents) style values.

  auto &yogaStyle = yogaNode_.getStyle();

  auto ownerWidth = yogaFloatFromFloat(maximumSize.width);
  auto ownerHeight = yogaFloatFromFloat(maximumSize.height);

  yogaStyle.maxDimensions()[YGDimensionWidth] = std::isfinite(maximumSize.width)
      ? yogaStyleValueFromFloat(maximumSize.width)
      : YGValueUndefined;

  yogaStyle.maxDimensions()[YGDimensionHeight] =
      std::isfinite(maximumSize.height)
      ? yogaStyleValueFromFloat(maximumSize.height)
      : YGValueUndefined;

  yogaStyle.minDimensions()[YGDimensionWidth] = minimumSize.width > 0
      ? yogaStyleValueFromFloat(minimumSize.width)
      : YGValueUndefined;

  yogaStyle.minDimensions()[YGDimensionHeight] = minimumSize.height > 0
      ? yogaStyleValueFromFloat(minimumSize.height)
      : YGValueUndefined;

  auto direction =
      yogaDirectionFromLayoutDirection(layoutConstraints.layoutDirection);

  threadLocalLayoutContext = layoutContext;

  {
    SystraceSection s3("YogaLayoutableShadowNode::YGNodeCalculateLayout");
    YGNodeCalculateLayout(&yogaNode_, ownerWidth, ownerHeight, direction);
  }

  // Update layout metrics for root node. Updated for children in
  // YogaLayoutableShadowNode::layout
  if (yogaNode_.getHasNewLayout()) {
    auto layoutMetrics = layoutMetricsFromYogaNode(yogaNode_);
    layoutMetrics.pointScaleFactor = layoutContext.pointScaleFactor;
    layoutMetrics.wasLeftAndRightSwapped = swapLeftAndRight;
    setLayoutMetrics(layoutMetrics);
    yogaNode_.setHasNewLayout(false);
  }

  layout(layoutContext);
}

static EdgeInsets calculateOverflowInset(
    Rect containerFrame,
    Rect contentFrame) {
  auto size = containerFrame.size;
  auto overflowInset = EdgeInsets{};
  overflowInset.left = std::min(contentFrame.getMinX(), Float{0.0});
  overflowInset.top = std::min(contentFrame.getMinY(), Float{0.0});
  overflowInset.right =
      -std::max(contentFrame.getMaxX() - size.width, Float{0.0});
  overflowInset.bottom =
      -std::max(contentFrame.getMaxY() - size.height, Float{0.0});
  return overflowInset;
}

void YogaLayoutableShadowNode::layout(LayoutContext layoutContext) {
  // Reading data from a dirtied node does not make sense.
  react_native_assert(!yogaNode_.isDirty());

  auto contentFrame = Rect{};
  for (auto childYogaNode : yogaNode_.getChildren()) {
    auto &childNode = shadowNodeFromContext(childYogaNode);

    // Verifying that the Yoga node belongs to the ShadowNode.
    react_native_assert(&childNode.yogaNode_ == childYogaNode);

    if (childYogaNode->getHasNewLayout()) {
      childYogaNode->setHasNewLayout(false);

      // Reading data from a dirtied node does not make sense.
      react_native_assert(!childYogaNode->isDirty());

      // We must copy layout metrics from Yoga node only once (when the parent
      // node exclusively ownes the child node).
      react_native_assert(childYogaNode->getOwner() == &yogaNode_);

      // We are about to mutate layout metrics of the node.
      childNode.ensureUnsealed();

      auto newLayoutMetrics = layoutMetricsFromYogaNode(*childYogaNode);
      newLayoutMetrics.pointScaleFactor = layoutContext.pointScaleFactor;
      newLayoutMetrics.wasLeftAndRightSwapped =
          layoutContext.swapLeftAndRightInRTL &&
          (newLayoutMetrics.layoutDirection == LayoutDirection::RightToLeft ||
           !CoreFeatures::doNotSwapLeftAndRightOnAndroidInLTR);

      // Child node's layout has changed. When a node is added to
      // `affectedNodes`, onLayout event is called on the component. Comparing
      // `newLayoutMetrics.frame` with `childNode.getLayoutMetrics().frame` to
      // detect if layout has not changed is not advised, please refer to
      // D22999891 for details.
      if (layoutContext.affectedNodes != nullptr) {
        layoutContext.affectedNodes->push_back(&childNode);
      }

      childNode.setLayoutMetrics(newLayoutMetrics);

      if (newLayoutMetrics.displayType != DisplayType::None) {
        childNode.layout(layoutContext);
      }
    }

    auto layoutMetricsWithOverflowInset = childNode.getLayoutMetrics();
    if (layoutMetricsWithOverflowInset.displayType != DisplayType::None) {
      auto viewChildNode = traitCast<ViewShadowNode const *>(&childNode);
      auto hitSlop = viewChildNode != nullptr
          ? viewChildNode->getConcreteProps().hitSlop
          : EdgeInsets{};

      // The contentFrame should always union with existing child node layout +
      // overflowInset. The transform may in a deferred animation and not
      // applied yet.
      contentFrame.unionInPlace(insetBy(
          layoutMetricsWithOverflowInset.frame,
          layoutMetricsWithOverflowInset.overflowInset));
      contentFrame.unionInPlace(
          outsetBy(layoutMetricsWithOverflowInset.frame, hitSlop));

      auto childTransform = childNode.getTransform();
      if (childTransform != Transform::Identity()) {
        // The child node's transform matrix will affect the parent node's
        // contentFrame. We need to union with child node's after transform
        // layout here.
        contentFrame.unionInPlace(insetBy(
            layoutMetricsWithOverflowInset.frame * childTransform,
            layoutMetricsWithOverflowInset.overflowInset * childTransform));
        contentFrame.unionInPlace(outsetBy(
            layoutMetricsWithOverflowInset.frame * childTransform, hitSlop));
      }
    }
  }

  if (yogaNode_.getStyle().overflow() == YGOverflowVisible) {
    // Note that the parent node's overflow layout is NOT affected by its
    // transform matrix. That transform matrix is applied on the parent node as
    // well as all of its child nodes, which won't cause changes on the
    // overflowInset values. A special note on the scale transform -- the scaled
    // layout may look like it's causing overflowInset changes, but it's purely
    // cosmetic and will be handled by pixel density conversion logic later when
    // render the view. The actual overflowInset value is not changed as if the
    // transform is not happening here.
    layoutMetrics_.overflowInset =
        calculateOverflowInset(layoutMetrics_.frame, contentFrame);
  } else {
    layoutMetrics_.overflowInset = {};
  }
}

#pragma mark - Yoga Connectors

YGNode *YogaLayoutableShadowNode::yogaNodeCloneCallbackConnector(
    YGNode * /*oldYogaNode*/,
    YGNode *parentYogaNode,
    int childIndex) {
  SystraceSection s("YogaLayoutableShadowNode::yogaNodeCloneCallbackConnector");

  auto &parentNode = shadowNodeFromContext(parentYogaNode);
  return &parentNode.cloneChildInPlace(childIndex).yogaNode_;
}

YGSize YogaLayoutableShadowNode::yogaNodeMeasureCallbackConnector(
    YGNode *yogaNode,
    float width,
    YGMeasureMode widthMode,
    float height,
    YGMeasureMode heightMode) {
  SystraceSection s(
      "YogaLayoutableShadowNode::yogaNodeMeasureCallbackConnector");

  auto &shadowNode = shadowNodeFromContext(yogaNode);

  auto minimumSize = Size{0, 0};
  auto maximumSize = Size{
      std::numeric_limits<Float>::infinity(),
      std::numeric_limits<Float>::infinity()};

  switch (widthMode) {
    case YGMeasureModeUndefined:
      break;
    case YGMeasureModeExactly:
      minimumSize.width = floatFromYogaFloat(width);
      maximumSize.width = floatFromYogaFloat(width);
      break;
    case YGMeasureModeAtMost:
      maximumSize.width = floatFromYogaFloat(width);
      break;
  }

  switch (heightMode) {
    case YGMeasureModeUndefined:
      break;
    case YGMeasureModeExactly:
      minimumSize.height = floatFromYogaFloat(height);
      maximumSize.height = floatFromYogaFloat(height);
      break;
    case YGMeasureModeAtMost:
      maximumSize.height = floatFromYogaFloat(height);
      break;
  }

  auto size = shadowNode.measureContent(
      threadLocalLayoutContext, {minimumSize, maximumSize});

  return YGSize{
      yogaFloatFromFloat(size.width), yogaFloatFromFloat(size.height)};
}

YogaLayoutableShadowNode &YogaLayoutableShadowNode::shadowNodeFromContext(
    YGNode *yogaNode) {
  return traitCast<YogaLayoutableShadowNode &>(
      *static_cast<ShadowNode *>(yogaNode->getContext()));
}

YGConfig &YogaLayoutableShadowNode::initializeYogaConfig(
    YGConfig &config,
    const YGConfigRef previousConfig) {
  YGConfigSetCloneNodeFunc(
      &config, YogaLayoutableShadowNode::yogaNodeCloneCallbackConnector);
  if (previousConfig != nullptr) {
    YGConfigSetPointScaleFactor(
        &config, YGConfigGetPointScaleFactor(previousConfig));
    YGConfigSetErrata(&config, YGConfigGetErrata(previousConfig));
  }

#ifdef RN_DEBUG_YOGA_LOGGER
  YGConfigSetPrintTreeFlag(&config, true);
#endif
  return config;
}

#pragma mark - RTL left and right swapping

void YogaLayoutableShadowNode::swapStyleLeftAndRight() {
  ensureUnsealed();

  swapLeftAndRightInYogaStyleProps(*this);
  swapLeftAndRightInViewProps(*this);
}

void YogaLayoutableShadowNode::swapLeftAndRightInYogaStyleProps(
    YogaLayoutableShadowNode const &shadowNode) {
  auto yogaStyle = shadowNode.yogaNode_.getStyle();

  YGStyle::Edges const &position = yogaStyle.position();
  YGStyle::Edges const &padding = yogaStyle.padding();
  YGStyle::Edges const &margin = yogaStyle.margin();

  // Swap Yoga node values, position, padding and margin.

  if (yogaStyle.position()[YGEdgeLeft] != YGValueUndefined) {
    yogaStyle.position()[YGEdgeStart] = position[YGEdgeLeft];
    yogaStyle.position()[YGEdgeLeft] = YGValueUndefined;
  }

  if (yogaStyle.position()[YGEdgeRight] != YGValueUndefined) {
    yogaStyle.position()[YGEdgeEnd] = position[YGEdgeRight];
    yogaStyle.position()[YGEdgeRight] = YGValueUndefined;
  }

  if (yogaStyle.padding()[YGEdgeLeft] != YGValueUndefined) {
    yogaStyle.padding()[YGEdgeStart] = padding[YGEdgeLeft];
    yogaStyle.padding()[YGEdgeLeft] = YGValueUndefined;
  }

  if (yogaStyle.padding()[YGEdgeRight] != YGValueUndefined) {
    yogaStyle.padding()[YGEdgeEnd] = padding[YGEdgeRight];
    yogaStyle.padding()[YGEdgeRight] = YGValueUndefined;
  }

  if (yogaStyle.margin()[YGEdgeLeft] != YGValueUndefined) {
    yogaStyle.margin()[YGEdgeStart] = margin[YGEdgeLeft];
    yogaStyle.margin()[YGEdgeLeft] = YGValueUndefined;
  }

  if (yogaStyle.margin()[YGEdgeRight] != YGValueUndefined) {
    yogaStyle.margin()[YGEdgeEnd] = margin[YGEdgeRight];
    yogaStyle.margin()[YGEdgeRight] = YGValueUndefined;
  }

  shadowNode.yogaNode_.setStyle(yogaStyle);
}

void YogaLayoutableShadowNode::swapLeftAndRightInViewProps(
    YogaLayoutableShadowNode const &shadowNode) {
  auto &typedCasting = static_cast<ViewProps const &>(*shadowNode.props_);
  auto &props = const_cast<ViewProps &>(typedCasting);

  // Swap border node values, borderRadii, borderColors and borderStyles.

  if (props.borderRadii.topLeft.has_value()) {
    props.borderRadii.topStart = props.borderRadii.topLeft;
    props.borderRadii.topLeft.reset();
  }

  if (props.borderRadii.bottomLeft.has_value()) {
    props.borderRadii.bottomStart = props.borderRadii.bottomLeft;
    props.borderRadii.bottomLeft.reset();
  }

  if (props.borderRadii.topRight.has_value()) {
    props.borderRadii.topEnd = props.borderRadii.topRight;
    props.borderRadii.topRight.reset();
  }

  if (props.borderRadii.bottomRight.has_value()) {
    props.borderRadii.bottomEnd = props.borderRadii.bottomRight;
    props.borderRadii.bottomRight.reset();
  }

  if (props.borderColors.left.has_value()) {
    props.borderColors.start = props.borderColors.left;
    props.borderColors.left.reset();
  }

  if (props.borderColors.right.has_value()) {
    props.borderColors.end = props.borderColors.right;
    props.borderColors.right.reset();
  }

  if (props.borderStyles.left.has_value()) {
    props.borderStyles.start = props.borderStyles.left;
    props.borderStyles.left.reset();
  }

  if (props.borderStyles.right.has_value()) {
    props.borderStyles.end = props.borderStyles.right;
    props.borderStyles.right.reset();
  }

  YGStyle::Edges const &border = props.yogaStyle.border();

  if (props.yogaStyle.border()[YGEdgeLeft] != YGValueUndefined) {
    props.yogaStyle.border()[YGEdgeStart] = border[YGEdgeLeft];
    props.yogaStyle.border()[YGEdgeLeft] = YGValueUndefined;
  }

  if (props.yogaStyle.border()[YGEdgeRight] != YGValueUndefined) {
    props.yogaStyle.border()[YGEdgeEnd] = border[YGEdgeRight];
    props.yogaStyle.border()[YGEdgeRight] = YGValueUndefined;
  }
}

#pragma mark - Consistency Ensuring Helpers

void YogaLayoutableShadowNode::ensureConsistency() const {
  ensureYogaChildrenLookFine();
  ensureYogaChildrenAlignment();
}

void YogaLayoutableShadowNode::ensureYogaChildrenLookFine() const {
#ifdef REACT_NATIVE_DEBUG
  // Checking that the shapes of Yoga node children object look fine.
  // This is the only heuristic that might produce false-positive results
  // (really broken dangled nodes might look fine). This is useful as an early
  // signal that something went wrong.
  auto &yogaChildren = yogaNode_.getChildren();

  for (auto const &yogaChild : yogaChildren) {
    react_native_assert(yogaChild->getContext());
    react_native_assert(yogaChild->getChildren().size() < 16384);
    if (!yogaChild->getChildren().empty()) {
      react_native_assert(!yogaChild->hasMeasureFunc());
    }
  }
#endif
}

void YogaLayoutableShadowNode::ensureYogaChildrenAlignment() const {
#ifdef REACT_NATIVE_DEBUG
  // If the node is not a leaf node, checking that:
  // - All children are `YogaLayoutableShadowNode` subclasses.
  // - All Yoga children are owned/connected to corresponding children of
  //   this node.

  auto &yogaChildren = yogaNode_.getChildren();
  auto &children = yogaLayoutableChildren_;

  if (getTraits().check(ShadowNodeTraits::Trait::LeafYogaNode)) {
    react_native_assert(yogaChildren.empty());
    return;
  }

  react_native_assert(yogaChildren.size() == children.size());

  for (size_t i = 0; i < children.size(); i++) {
    auto &yogaChild = yogaChildren.at(i);
    auto &child = children.at(i);
    react_native_assert(
        yogaChild->getContext() ==
        traitCast<YogaLayoutableShadowNode const *>(child.get()));
  }
#endif
}

} // namespace facebook::react