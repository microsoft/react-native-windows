/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <yoga/Yoga.h>

#include <yoga/algorithm/Cache.h>
#include <yoga/algorithm/CalculateLayout.h>
#include <yoga/algorithm/PixelGrid.h>
#include <yoga/debug/AssertFatal.h>
#include <yoga/debug/Log.h>
#include <yoga/debug/NodeToString.h>
#include <yoga/event/event.h>
#include <yoga/node/Node.h>
#include <yoga/enums/Edge.h>

using namespace facebook;
using namespace facebook::yoga;

void* YGNodeGetContext(YGNodeConstRef node) {
  return resolveRef(node)->getContext();
}

void YGNodeSetContext(YGNodeRef node, void* context) {
  return resolveRef(node)->setContext(context);
}

YGConfigConstRef YGNodeGetConfig(YGNodeRef node) {
  return resolveRef(node)->getConfig();
}

void YGNodeSetConfig(YGNodeRef node, YGConfigRef config) {
  resolveRef(node)->setConfig(resolveRef(config));
}

bool YGNodeHasMeasureFunc(YGNodeConstRef node) {
  return resolveRef(node)->hasMeasureFunc();
}

void YGNodeSetMeasureFunc(YGNodeRef node, YGMeasureFunc measureFunc) {
  resolveRef(node)->setMeasureFunc(measureFunc);
}

bool YGNodeHasBaselineFunc(YGNodeConstRef node) {
  return resolveRef(node)->hasBaselineFunc();
}

void YGNodeSetBaselineFunc(YGNodeRef node, YGBaselineFunc baselineFunc) {
  resolveRef(node)->setBaselineFunc(baselineFunc);
}

YGDirtiedFunc YGNodeGetDirtiedFunc(YGNodeConstRef node) {
  return resolveRef(node)->getDirtiedFunc();
}

void YGNodeSetDirtiedFunc(YGNodeRef node, YGDirtiedFunc dirtiedFunc) {
  resolveRef(node)->setDirtiedFunc(dirtiedFunc);
}

bool YGNodeGetHasNewLayout(YGNodeConstRef node) {
  return resolveRef(node)->getHasNewLayout();
}

void YGConfigSetPrintTreeFlag(YGConfigRef config, bool enabled) {
  resolveRef(config)->setShouldPrintTree(enabled);
}

void YGNodeSetHasNewLayout(YGNodeRef node, bool hasNewLayout) {
  resolveRef(node)->setHasNewLayout(hasNewLayout);
}

YGNodeType YGNodeGetNodeType(YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getNodeType());
}

void YGNodeSetNodeType(YGNodeRef node, YGNodeType nodeType) {
  return resolveRef(node)->setNodeType(scopedEnum(nodeType));
}

bool YGNodeIsDirty(YGNodeConstRef node) {
  return resolveRef(node)->isDirty();
}

YGNodeRef YGNodeNewWithConfig(const YGConfigConstRef config) {
  auto* node = new yoga::Node{resolveRef(config)};
  yoga::assertFatal(
      config != nullptr, "Tried to construct YGNode with null config");
  Event::publish<Event::NodeAllocation>(node, {config});

  return node;
}

YGConfigConstRef YGConfigGetDefault() {
  return &yoga::Config::getDefault();
}

YGNodeRef YGNodeNew(void) {
  return YGNodeNewWithConfig(YGConfigGetDefault());
}

YGNodeRef YGNodeClone(YGNodeConstRef oldNodeRef) {
  auto oldNode = resolveRef(oldNodeRef);
  const auto node = new yoga::Node(*oldNode);
  yoga::assertFatalWithConfig(
      oldNode->getConfig(),
      node != nullptr,
      "Could not allocate memory for node");
  Event::publish<Event::NodeAllocation>(node, {node->getConfig()});
  node->setOwner(nullptr);
  return node;
}

void YGNodeFree(const YGNodeRef nodeRef) {
  const auto node = resolveRef(nodeRef);

  if (auto owner = node->getOwner()) {
    owner->removeChild(node);
    node->setOwner(nullptr);
  }

  const size_t childCount = node->getChildCount();
  for (size_t i = 0; i < childCount; i++) {
    auto child = node->getChild(i);
    child->setOwner(nullptr);
  }

  node->clearChildren();

  Event::publish<Event::NodeDeallocation>(node, {YGNodeGetConfig(node)});
  delete resolveRef(node);
}

void YGNodeFinalize(const YGNodeRef node) {
  Event::publish<Event::NodeDeallocation>(node, {YGNodeGetConfig(node)});
  delete resolveRef(node);
}

void YGNodeFreeRecursive(YGNodeRef rootRef) {
  const auto root = resolveRef(rootRef);

  size_t skipped = 0;
  while (root->getChildCount() > skipped) {
    const auto child = root->getChild(skipped);
    if (child->getOwner() != root) {
      // Don't free shared nodes that we don't own.
      skipped += 1;
    } else {
      YGNodeRemoveChild(root, child);
      YGNodeFreeRecursive(child);
    }
  }
  YGNodeFree(root);
}

void YGNodeReset(YGNodeRef node) {
  resolveRef(node)->reset();
}

YGConfigRef YGConfigNew(void) {
  return new yoga::Config(getDefaultLogger());
}

void YGConfigFree(const YGConfigRef config) {
  delete resolveRef(config);
}

void YGNodeSetIsReferenceBaseline(YGNodeRef nodeRef, bool isReferenceBaseline) {
  const auto node = resolveRef(nodeRef);
  if (node->isReferenceBaseline() != isReferenceBaseline) {
    node->setIsReferenceBaseline(isReferenceBaseline);
    node->markDirtyAndPropagate();
  }
}

bool YGNodeIsReferenceBaseline(YGNodeConstRef node) {
  return resolveRef(node)->isReferenceBaseline();
}

void YGNodeInsertChild(
    const YGNodeRef ownerRef,
    const YGNodeRef childRef,
    const size_t index) {
  auto owner = resolveRef(ownerRef);
  auto child = resolveRef(childRef);

  yoga::assertFatalWithNode(
      owner,
      child->getOwner() == nullptr,
      "Child already has a owner, it must be removed first.");

  yoga::assertFatalWithNode(
      owner,
      !owner->hasMeasureFunc(),
      "Cannot add child: Nodes with measure functions cannot have children.");

  owner->insertChild(child, index);
  child->setOwner(owner);
  owner->markDirtyAndPropagate();
}

void YGNodeSwapChild(
    const YGNodeRef ownerRef,
    const YGNodeRef childRef,
    const size_t index) {
  auto owner = resolveRef(ownerRef);
  auto child = resolveRef(childRef);

  owner->replaceChild(child, index);
  child->setOwner(owner);
}

void YGNodeRemoveChild(
    const YGNodeRef ownerRef,
    const YGNodeRef excludedChildRef) {
  auto owner = resolveRef(ownerRef);
  auto excludedChild = resolveRef(excludedChildRef);

  if (owner->getChildCount() == 0) {
    // This is an empty set. Nothing to remove.
    return;
  }

  // Children may be shared between parents, which is indicated by not having an
  // owner. We only want to reset the child completely if it is owned
  // exclusively by one node.
  auto childOwner = excludedChild->getOwner();
  if (owner->removeChild(excludedChild)) {
    if (owner == childOwner) {
      excludedChild->setLayout({}); // layout is no longer valid
      excludedChild->setOwner(nullptr);
    }
    owner->markDirtyAndPropagate();
  }
}

void YGNodeRemoveAllChildren(const YGNodeRef ownerRef) {
  auto owner = resolveRef(ownerRef);

  const size_t childCount = owner->getChildCount();
  if (childCount == 0) {
    // This is an empty set already. Nothing to do.
    return;
  }
  auto* firstChild = owner->getChild(0);
  if (firstChild->getOwner() == owner) {
    // If the first child has this node as its owner, we assume that this child
    // set is unique.
    for (size_t i = 0; i < childCount; i++) {
      yoga::Node* oldChild = owner->getChild(i);
      oldChild->setLayout({}); // layout is no longer valid
      oldChild->setOwner(nullptr);
    }
    owner->clearChildren();
    owner->markDirtyAndPropagate();
    return;
  }
  // Otherwise, we are not the owner of the child set. We don't have to do
  // anything to clear it.
  owner->setChildren({});
  owner->markDirtyAndPropagate();
}

void YGNodeSetChildren(
    const YGNodeRef ownerRef,
    const YGNodeRef* childrenRefs,
    const size_t count) {
  auto owner = resolveRef(ownerRef);
  auto children = reinterpret_cast<yoga::Node* const*>(childrenRefs);

  if (!owner) {
    return;
  }

  const std::vector<yoga::Node*> childrenVector = {children, children + count};
  if (childrenVector.size() == 0) {
    if (owner->getChildCount() > 0) {
      for (auto* child : owner->getChildren()) {
        child->setLayout({});
        child->setOwner(nullptr);
      }
      owner->setChildren({});
      owner->markDirtyAndPropagate();
    }
  } else {
    if (owner->getChildCount() > 0) {
      for (auto* oldChild : owner->getChildren()) {
        // Our new children may have nodes in common with the old children. We
        // don't reset these common nodes.
        if (std::find(childrenVector.begin(), childrenVector.end(), oldChild) ==
            childrenVector.end()) {
          oldChild->setLayout({});
          oldChild->setOwner(nullptr);
        }
      }
    }
    owner->setChildren(childrenVector);
    for (yoga::Node* child : childrenVector) {
      child->setOwner(owner);
    }
    owner->markDirtyAndPropagate();
  }
}

YGNodeRef YGNodeGetChild(const YGNodeRef nodeRef, const size_t index) {
  const auto node = resolveRef(nodeRef);

  if (index < node->getChildren().size()) {
    return node->getChild(index);
  }
  return nullptr;
}

size_t YGNodeGetChildCount(const YGNodeConstRef node) {
  return resolveRef(node)->getChildren().size();
}

YGNodeRef YGNodeGetOwner(const YGNodeRef node) {
  return resolveRef(node)->getOwner();
}

YGNodeRef YGNodeGetParent(const YGNodeRef node) {
  return resolveRef(node)->getOwner();
}

void YGNodeMarkDirty(const YGNodeRef nodeRef) {
  const auto node = resolveRef(nodeRef);

  yoga::assertFatalWithNode(
      node,
      node->hasMeasureFunc(),
      "Only leaf nodes with custom measure functions "
      "should manually mark themselves as dirty");

  node->markDirtyAndPropagate();
}

void YGNodeCopyStyle(
    const YGNodeRef dstNodeRef,
    const YGNodeConstRef srcNodeRef) {
  auto dstNode = resolveRef(dstNodeRef);
  auto srcNode = resolveRef(srcNodeRef);

  if (!(dstNode->getStyle() == srcNode->getStyle())) {
    dstNode->setStyle(srcNode->getStyle());
    dstNode->markDirtyAndPropagate();
  }
}

float YGNodeStyleGetFlexGrow(const YGNodeConstRef nodeRef) {
  const auto node = resolveRef(nodeRef);
  return node->getStyle().flexGrow().isUndefined()
      ? Style::DefaultFlexGrow
      : node->getStyle().flexGrow().unwrap();
}

float YGNodeStyleGetFlexShrink(const YGNodeConstRef nodeRef) {
  const auto node = resolveRef(nodeRef);
  return node->getStyle().flexShrink().isUndefined()
      ? (node->getConfig()->useWebDefaults() ? Style::WebDefaultFlexShrink
                                             : Style::DefaultFlexShrink)
      : node->getStyle().flexShrink().unwrap();
}

namespace {

template <auto GetterT, auto SetterT, typename ValueT>
void updateStyle(YGNodeRef node, ValueT value) {
  auto& style = resolveRef(node)->getStyle();
  if ((style.*GetterT)() != value) {
    (style.*SetterT)(value);
    resolveRef(node)->markDirtyAndPropagate();
  }
}

template <auto GetterT, auto SetterT, typename IdxT, typename ValueT>
void updateStyle(YGNodeRef node, IdxT idx, ValueT value) {
  auto& style = resolveRef(node)->getStyle();
  if ((style.*GetterT)(idx) != value) {
    (style.*SetterT)(idx, value);
    resolveRef(node)->markDirtyAndPropagate();
  }
}

} // namespace

void YGNodeStyleSetDirection(const YGNodeRef node, const YGDirection value) {
  updateStyle<&Style::direction, &Style::setDirection>(node, scopedEnum(value));
}
YGDirection YGNodeStyleGetDirection(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().direction());
}

void YGNodeStyleSetFlexDirection(
    const YGNodeRef node,
    const YGFlexDirection flexDirection) {
  updateStyle<&Style::flexDirection, &Style::setFlexDirection>(
      node, scopedEnum(flexDirection));
}
YGFlexDirection YGNodeStyleGetFlexDirection(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().flexDirection());
}

void YGNodeStyleSetJustifyContent(
    const YGNodeRef node,
    const YGJustify justifyContent) {
   updateStyle<&Style::justifyContent, &Style::setJustifyContent>(
      node, scopedEnum(justifyContent));
}
YGJustify YGNodeStyleGetJustifyContent(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().justifyContent());
}

void YGNodeStyleSetAlignContent(
    const YGNodeRef node,
    const YGAlign alignContent) {
  updateStyle<&Style::alignContent, &Style::setAlignContent>(
      node, scopedEnum(alignContent));
}
YGAlign YGNodeStyleGetAlignContent(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().alignContent());
}

void YGNodeStyleSetAlignItems(const YGNodeRef node, const YGAlign alignItems) {
   updateStyle<&Style::alignItems, &Style::setAlignItems>(
      node, scopedEnum(alignItems));
}
YGAlign YGNodeStyleGetAlignItems(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().alignItems());
}

void YGNodeStyleSetAlignSelf(const YGNodeRef node, const YGAlign alignSelf) {
  updateStyle<&Style::alignSelf, &Style::setAlignSelf>(
      node, scopedEnum(alignSelf));
}
YGAlign YGNodeStyleGetAlignSelf(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().alignSelf());
}

void YGNodeStyleSetPositionType(
    const YGNodeRef node,
    const YGPositionType positionType) {
  updateStyle<&Style::positionType, &Style::setPositionType>(
      node, scopedEnum(positionType));
}
YGPositionType YGNodeStyleGetPositionType(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().positionType());
}

void YGNodeStyleSetFlexWrap(const YGNodeRef node, const YGWrap flexWrap) {
  updateStyle<&Style::flexWrap, &Style::setFlexWrap>(
      node, scopedEnum(flexWrap));
}
YGWrap YGNodeStyleGetFlexWrap(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().flexWrap());
}

void YGNodeStyleSetOverflow(const YGNodeRef node, const YGOverflow overflow) {
  updateStyle<&Style::overflow, &Style::setOverflow>(
      node, scopedEnum(overflow));
}
YGOverflow YGNodeStyleGetOverflow(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().overflow());
}

void YGNodeStyleSetDisplay(const YGNodeRef node, const YGDisplay display) {
  updateStyle<&Style::display, &Style::setDisplay>(node, scopedEnum(display));
}
YGDisplay YGNodeStyleGetDisplay(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getStyle().display());
}

void YGNodeStyleSetFlex(const YGNodeRef node, const float flex) {
  updateStyle<&Style::flex, &Style::setFlex>(node, FloatOptional{flex});
}

float YGNodeStyleGetFlex(const YGNodeConstRef nodeRef) {
  const auto node = resolveRef(nodeRef);
  return node->getStyle().flex().isUndefined()
      ? YGUndefined
      : node->getStyle().flex().unwrap();
}

void YGNodeStyleSetFlexGrow(const YGNodeRef node, const float flexGrow) {
  updateStyle<&Style::flexGrow, &Style::setFlexGrow>(
      node, FloatOptional{flexGrow});
}

void YGNodeStyleSetFlexShrink(const YGNodeRef node, const float flexShrink) {
  updateStyle<&Style::flexShrink, &Style::setFlexShrink>(
      node, FloatOptional{flexShrink});
}

YGValue YGNodeStyleGetFlexBasis(const YGNodeConstRef node) {
  YGValue flexBasis = resolveRef(node)->getStyle().flexBasis();
  if (flexBasis.unit == YGUnitUndefined || flexBasis.unit == YGUnitAuto) {
    flexBasis.value = YGUndefined;
  }
  return flexBasis;
}

void YGNodeStyleSetFlexBasis(const YGNodeRef node, const float flexBasis) {
  updateStyle<&Style::flexBasis, &Style::setFlexBasis>(
      node, value::points(flexBasis));
}

void YGNodeStyleSetFlexBasisPercent(
    const YGNodeRef node,
    const float flexBasisPercent) {
  updateStyle<&Style::flexBasis, &Style::setFlexBasis>(
      node, value::percent(flexBasisPercent));
}

void YGNodeStyleSetFlexBasisAuto(const YGNodeRef node) {
  updateStyle<&Style::flexBasis, &Style::setFlexBasis>(node, value::ofAuto());
}

void YGNodeStyleSetPosition(YGNodeRef node, YGEdge edge, float points) {
   updateStyle<&Style::position, &Style::setPosition>(
      node, scopedEnum(edge), value::points(points));
}
void YGNodeStyleSetPositionPercent(YGNodeRef node, YGEdge edge, float percent) {
  updateStyle<&Style::position, &Style::setPosition>(
      node, scopedEnum(edge), value::percent(percent));
}
YGValue YGNodeStyleGetPosition(YGNodeConstRef node, YGEdge edge) {
  return resolveRef(node)->getStyle().position(scopedEnum(edge));
}

void YGNodeStyleSetMargin(YGNodeRef node, YGEdge edge, float points) {
   updateStyle<&Style::margin, &Style::setMargin>(
      node, scopedEnum(edge), value::points(points));
}
void YGNodeStyleSetMarginPercent(YGNodeRef node, YGEdge edge, float percent) {
  updateStyle<&Style::margin, &Style::setMargin>(
      node, scopedEnum(edge), value::percent(percent));
}
void YGNodeStyleSetMarginAuto(YGNodeRef node, YGEdge edge) {
   updateStyle<&Style::margin, &Style::setMargin>(
      node, scopedEnum(edge), value::ofAuto());
}
YGValue YGNodeStyleGetMargin(YGNodeConstRef node, YGEdge edge) {
  return resolveRef(node)->getStyle().margin(scopedEnum(edge));
}

void YGNodeStyleSetPadding(YGNodeRef node, YGEdge edge, float points) {
  updateStyle<&Style::padding, &Style::setPadding>(
      node, scopedEnum(edge), value::points(points));
}
void YGNodeStyleSetPaddingPercent(YGNodeRef node, YGEdge edge, float percent) {
   updateStyle<&Style::padding, &Style::setPadding>(
      node, scopedEnum(edge), value::percent(percent));
}
YGValue YGNodeStyleGetPadding(YGNodeConstRef node, YGEdge edge) {
  return resolveRef(node)->getStyle().padding(scopedEnum(edge));
}

void YGNodeStyleSetBorder(
    const YGNodeRef node,
    const YGEdge edge,
    const float border) {
  updateStyle<&Style::border, &Style::setBorder>(node, scopedEnum(edge), value::points(border));
}

float YGNodeStyleGetBorder(const YGNodeConstRef node, const YGEdge edge) {
  auto border = resolveRef(node)->getStyle().border(scopedEnum(edge));
  if (border.isUndefined() || border.isAuto()) {
    return YGUndefined;
  }

  return static_cast<YGValue>(border).value;
}

void YGNodeStyleSetGap(
    const YGNodeRef node,
    const YGGutter gutter,
    const float gapLength) {
  updateStyle<&Style::gap, &Style::setGap>(
      node, scopedEnum(gutter), value::points(gapLength));
}

float YGNodeStyleGetGap(const YGNodeConstRef node, const YGGutter gutter) {
  auto gapLength = resolveRef(node)->getStyle().gap(scopedEnum(gutter));
  if (gapLength.isUndefined() || gapLength.isAuto()) {
    return YGUndefined;
  }

  return static_cast<YGValue>(gapLength).value;
}

// Yoga specific properties, not compatible with flexbox specification

float YGNodeStyleGetAspectRatio(const YGNodeConstRef node) {
  const FloatOptional op = resolveRef(node)->getStyle().aspectRatio();
  return op.isUndefined() ? YGUndefined : op.unwrap();
}

void YGNodeStyleSetAspectRatio(const YGNodeRef node, const float aspectRatio) {
  updateStyle<&Style::aspectRatio, &Style::setAspectRatio>(
      node, FloatOptional{aspectRatio});
}

void YGNodeStyleSetWidth(YGNodeRef node, float points) {
  updateStyle<&Style::dimension, &Style::setDimension>(
      node, Dimension::Width, value::points(points));
}
void YGNodeStyleSetWidthPercent(YGNodeRef node, float percent) {
  updateStyle<&Style::dimension, &Style::setDimension>(
      node, Dimension::Width, value::percent(percent));
}
void YGNodeStyleSetWidthAuto(YGNodeRef node) {
   updateStyle<&Style::dimension, &Style::setDimension>(
      node, Dimension::Width, value::ofAuto());
}
YGValue YGNodeStyleGetWidth(YGNodeConstRef node) {
  return resolveRef(node)->getStyle().dimension(Dimension::Width);
}

void YGNodeStyleSetHeight(YGNodeRef node, float points) {
  updateStyle<&Style::dimension, &Style::setDimension>(
      node, Dimension::Height, value::points(points));
}

void YGNodeStyleSetHeightPercent(YGNodeRef node, float percent) {
  updateStyle<&Style::dimension, &Style::setDimension>(
      node, Dimension::Height, value::percent(percent));
}

void YGNodeStyleSetHeightAuto(YGNodeRef node) {
  updateStyle<&Style::dimension, &Style::setDimension>(
      node, Dimension::Height, value::ofAuto());
}
YGValue YGNodeStyleGetHeight(YGNodeConstRef node) {
  return resolveRef(node)->getStyle().dimension(Dimension::Height);
}

void YGNodeStyleSetMinWidth(const YGNodeRef node, const float minWidth) {
  updateStyle<&Style::minDimension, &Style::setMinDimension>(
      node, Dimension::Width, value::points(minWidth));
}

void YGNodeStyleSetMinWidthPercent(const YGNodeRef node, const float minWidth) {
  updateStyle<&Style::minDimension, &Style::setMinDimension>(
      node, Dimension::Width, value::percent(minWidth));
}
YGValue YGNodeStyleGetMinWidth(const YGNodeConstRef node) {
  return resolveRef(node)->getStyle().minDimension(Dimension::Width);
}

void YGNodeStyleSetMinHeight(const YGNodeRef node, const float minHeight) {
  updateStyle<&Style::minDimension, &Style::setMinDimension>(
      node, Dimension::Height, value::points(minHeight));
}

void YGNodeStyleSetMinHeightPercent(
    const YGNodeRef node,
    const float minHeight) {
  updateStyle<&Style::minDimension, &Style::setMinDimension>(
      node, Dimension::Height, value::percent(minHeight));
}
YGValue YGNodeStyleGetMinHeight(const YGNodeConstRef node) {
  return resolveRef(node)->getStyle().minDimension(Dimension::Height);
}

void YGNodeStyleSetMaxWidth(const YGNodeRef node, const float maxWidth) {
  updateStyle<&Style::maxDimension, &Style::setMaxDimension>(
      node, Dimension::Width, value::points(maxWidth));
}

void YGNodeStyleSetMaxWidthPercent(const YGNodeRef node, const float maxWidth) {
  updateStyle<&Style::maxDimension, &Style::setMaxDimension>(
      node, Dimension::Width, value::percent(maxWidth));
}
YGValue YGNodeStyleGetMaxWidth(const YGNodeConstRef node) {
  return resolveRef(node)->getStyle().maxDimension(Dimension::Width);
}

void YGNodeStyleSetMaxHeight(const YGNodeRef node, const float maxHeight) {
  updateStyle<&Style::maxDimension, &Style::setMaxDimension>(
      node, Dimension::Height, value::points(maxHeight));
}

void YGNodeStyleSetMaxHeightPercent(
    const YGNodeRef node,
    const float maxHeight) {
  updateStyle<&Style::maxDimension, &Style::setMaxDimension>(
      node, Dimension::Height, value::percent(maxHeight));
}
YGValue YGNodeStyleGetMaxHeight(const YGNodeConstRef node) {
  return resolveRef(node)->getStyle().maxDimension(Dimension::Height);
}

namespace {

template <auto LayoutMember>
float getResolvedLayoutProperty(const YGNodeConstRef nodeRef, const Edge edge) {
  const auto node = resolveRef(nodeRef);
  yoga::assertFatalWithNode(
      node,
      edge <= Edge::End,
      "Cannot get layout properties of multi-edge shorthands");

  if (edge == Edge::Start) {
    if (node->getLayout().direction() == Direction::RTL) {
      return (node->getLayout().*LayoutMember)(Edge::Right);
    } else {
      return (node->getLayout().*LayoutMember)(Edge::Left);
    }
  }

  if (edge == Edge::End) {
    if (node->getLayout().direction() == Direction::RTL) {
      return (node->getLayout().*LayoutMember)(Edge::Left);
    } else {
      return (node->getLayout().*LayoutMember)(Edge::Right);
    }
  }

  return (node->getLayout().*LayoutMember)(edge);
}

} // namespace

float YGNodeLayoutGetLeft(const YGNodeConstRef node) {
  return resolveRef(node)->getLayout().position(Edge::Left);
}

float YGNodeLayoutGetTop(const YGNodeConstRef node) {
  return resolveRef(node)->getLayout().position(Edge::Top);
}

float YGNodeLayoutGetRight(const YGNodeConstRef node) {
  return resolveRef(node)->getLayout().position(Edge::Right);
}

float YGNodeLayoutGetBottom(const YGNodeConstRef node) {
  return resolveRef(node)->getLayout().position(Edge::Bottom);
}

float YGNodeLayoutGetWidth(const YGNodeConstRef node) {
  return resolveRef(node)->getLayout().dimension(Dimension::Width);
}

float YGNodeLayoutGetHeight(const YGNodeConstRef node) {
  return resolveRef(node)->getLayout().dimension(Dimension::Height);
}

YGDirection YGNodeLayoutGetDirection(const YGNodeConstRef node) {
  return unscopedEnum(resolveRef(node)->getLayout().direction());
}

bool YGNodeLayoutGetHadOverflow(const YGNodeConstRef node) {
  return resolveRef(node)->getLayout().hadOverflow();
}

float YGNodeLayoutGetMargin(YGNodeConstRef node, YGEdge edge) {
  return getResolvedLayoutProperty<&LayoutResults::margin>(
      node, scopedEnum(edge));
}

float YGNodeLayoutGetBorder(YGNodeConstRef node, YGEdge edge) {
  return getResolvedLayoutProperty<&LayoutResults::border>(
      node, scopedEnum(edge));
}

float YGNodeLayoutGetPadding(YGNodeConstRef node, YGEdge edge) {
  return getResolvedLayoutProperty<&LayoutResults::padding>(
      node, scopedEnum(edge));
}

#ifdef DEBUG
void YGNodePrint(const YGNodeConstRef node, const YGPrintOptions options) {
  yoga::print(resolveRef(node), scopedEnum(options));
}
#endif

void YGConfigSetLogger(const YGConfigRef config, YGLogger logger) {
  if (logger != nullptr) {
    resolveRef(config)->setLogger(logger);
  } else {
    resolveRef(config)->setLogger(getDefaultLogger());
  }
}

void YGConfigSetPointScaleFactor(
    const YGConfigRef config,
    const float pixelsInPoint) {
  yoga::assertFatalWithConfig(
      resolveRef(config),
      pixelsInPoint >= 0.0f,
      "Scale factor should not be less than zero");

  // We store points for Pixel as we will use it for rounding
  if (pixelsInPoint == 0.0f) {
    // Zero is used to skip rounding
    resolveRef(config)->setPointScaleFactor(0.0f);
  } else {
    resolveRef(config)->setPointScaleFactor(pixelsInPoint);
  }
}

float YGConfigGetPointScaleFactor(const YGConfigConstRef config) {
  return resolveRef(config)->getPointScaleFactor();
}

float YGRoundValueToPixelGrid(
    const double value,
    const double pointScaleFactor,
    const bool forceCeil,
    const bool forceFloor) {
  return yoga::roundValueToPixelGrid(
      value, pointScaleFactor, forceCeil, forceFloor);
}

void YGConfigSetExperimentalFeatureEnabled(
    const YGConfigRef config,
    const YGExperimentalFeature feature,
    const bool enabled) {
  resolveRef(config)->setExperimentalFeatureEnabled(
      scopedEnum(feature), enabled);
}

bool YGConfigIsExperimentalFeatureEnabled(
    const YGConfigConstRef config,
    const YGExperimentalFeature feature) {
  return resolveRef(config)->isExperimentalFeatureEnabled(scopedEnum(feature));
}

void YGConfigSetUseWebDefaults(const YGConfigRef config, const bool enabled) {
  resolveRef(config)->setUseWebDefaults(enabled);
}

bool YGConfigGetUseWebDefaults(const YGConfigConstRef config) {
  return resolveRef(config)->useWebDefaults();
}

void YGConfigSetContext(const YGConfigRef config, void* context) {
  resolveRef(config)->setContext(context);
}

void* YGConfigGetContext(const YGConfigConstRef config) {
  return resolveRef(config)->getContext();
}

void YGConfigSetErrata(YGConfigRef config, YGErrata errata) {
  resolveRef(config)->setErrata(scopedEnum(errata));
}

YGErrata YGConfigGetErrata(YGConfigConstRef config) {
  return unscopedEnum(resolveRef(config)->getErrata());
}

void YGConfigSetCloneNodeFunc(
    const YGConfigRef config,
    const YGCloneNodeFunc callback) {
  resolveRef(config)->setCloneNodeCallback(callback);
}

// TODO: This should not be part of the public API. Remove after removing
// ComponentKit usage of it.
bool YGNodeCanUseCachedMeasurement(
    YGMeasureMode widthMode,
    float availableWidth,
    YGMeasureMode heightMode,
    float availableHeight,
    YGMeasureMode lastWidthMode,
    float lastAvailableWidth,
    YGMeasureMode lastHeightMode,
    float lastAvailableHeight,
    float lastComputedWidth,
    float lastComputedHeight,
    float marginRow,
    float marginColumn,
    YGConfigRef config) {
  return yoga::canUseCachedMeasurement(
      sizingMode(scopedEnum(widthMode)),
      availableWidth,
      sizingMode(scopedEnum(heightMode)),
      availableHeight,
      sizingMode(scopedEnum(lastWidthMode)),
      lastAvailableWidth,
      sizingMode(scopedEnum(lastHeightMode)),
      lastAvailableHeight,
      lastComputedWidth,
      lastComputedHeight,
      marginRow,
      marginColumn,
      resolveRef(config));
}

void YGNodeCalculateLayout(
    const YGNodeRef node,
    const float ownerWidth,
    const float ownerHeight,
    const YGDirection ownerDirection) {
  yoga::calculateLayout(
      resolveRef(node), ownerWidth, ownerHeight, scopedEnum(ownerDirection));
}
