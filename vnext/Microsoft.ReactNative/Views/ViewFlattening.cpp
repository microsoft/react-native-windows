// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewFlattening.h"

#include <JSValue.h>
#include <QuirkSettings.h>
#include <Utils/ShadowNodeTypeUtils.h>
#include <unordered_set>

namespace Microsoft::ReactNative::ViewFlattening {

///
/// Helpers
///
///

void AttachChild(INativeUIManagerHost *host, ShadowNodeBase *child);

void AttachChildren(INativeUIManagerHost *host, ShadowNodeBase *node) {
  for (auto childTag : node->m_children) {
    const auto child = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(childTag));
    assert(child && "All child nodes are assumed to exist.");
    AttachChild(host, child);
  }
}

int64_t AttachNativeDescendants(
    INativeUIManagerHost *host,
    ShadowNodeBase *nativeParent,
    ShadowNodeBase *child,
    int64_t nativeIndex) {
  if (!child->IsLayoutOnly()) {
    nativeParent->AddView(*child, nativeIndex);
    return 1;
  } else {
    auto currentIndex = nativeIndex;
    for (auto descendantTag : child->m_children) {
      const auto descendant = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(descendantTag));
      assert(descendant && "All child nodes are assumed to exist.");
      currentIndex += AttachNativeDescendants(host, nativeParent, descendant, currentIndex);
    }
    return currentIndex - nativeIndex;
  }
}

void DetachNativeDescendants(
    INativeUIManagerHost *host,
    ShadowNodeBase *nativeParent,
    ShadowNodeBase *child,
    int64_t nativeIndex,
    bool isUnflattening = false) {
  if (!child->IsLayoutOnly() && !isUnflattening) {
    nativeParent->RemoveChildAt(nativeIndex);
  } else {
    auto currentIndex = nativeIndex;
    for (auto descendantTag : child->m_children) {
      const auto descendant = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(descendantTag));
      assert(descendant && "All child nodes are assumed to exist.");
      // We don't need to increment the native index, because the nodes are removed in order
      DetachNativeDescendants(host, nativeParent, descendant, nativeIndex);
    }
  }
}

int64_t GetNativeIndexForChild(INativeUIManagerHost *host, ShadowNodeBase *node, int64_t childTag) {
  int64_t nativeIndex = 0;
  for (auto siblingTag : node->m_children) {
    if (siblingTag == childTag) {
      break;
    } else {
      const auto sibling = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(siblingTag));
      assert(sibling && "All sibling nodes are assumed to exist.");
      nativeIndex += sibling->IsLayoutOnly() ? sibling->m_nativeChildrenCount : 1;
    }
  }

  return nativeIndex;
}

struct NodeIndexPair {
  ShadowNodeBase *node;
  int64_t index;
};

std::optional<NodeIndexPair>
GetNativeParentAndIndex(INativeUIManagerHost *host, ShadowNodeBase *node, int64_t index = 0) {
  const auto parent = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(node->m_parent));
  if (!parent) {
    return std::nullopt;
  }

  const auto nativeIndex = GetNativeIndexForChild(host, parent, node->m_tag);
  // When transitioning a node from layout-only to materialized, we need to
  // operate as if that parent node is still layout only.
  if (!parent->IsLayoutOnly()) {
    return NodeIndexPair{parent, index + nativeIndex};
  }

  return GetNativeParentAndIndex(host, parent, index + nativeIndex);
}

void UpdateAncestorNativeChildrenCount(INativeUIManagerHost *host, ShadowNodeBase *child, int64_t nativeChildrenCount) {
  if (const auto parent = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(child->m_parent))) {
    parent->m_nativeChildrenCount += nativeChildrenCount;
    if (parent->IsLayoutOnly()) {
      UpdateAncestorNativeChildrenCount(host, parent, nativeChildrenCount);
    }
  }
}

bool IsViewFlatteningEnabled(ShadowNodeBase *node) {
  return winrt::Microsoft::ReactNative::implementation::QuirkSettings::GetIsViewFlatteningEnabled(
      winrt::Microsoft::ReactNative::ReactPropertyBag{node->GetViewManager()->GetReactContext().Properties()});
}

void AttachChild(INativeUIManagerHost *host, ShadowNodeBase *child) {
  const auto nativeParentAndIndex = GetNativeParentAndIndex(host, child);
  // If this node has not yet been attached to a native parent, we can skip
  // this step. Its native descendants will be attached to a native parent when
  // an ancestor is attached to a native parent.
  if (nativeParentAndIndex) {
    auto nativeParent = nativeParentAndIndex->node;
    auto nativeIndex = nativeParentAndIndex->index;

    // If attempting to attach a layout only child to a parent is not a View,
    // we need to materialize the child so it's layout-only children are
    // positioned correctly.
    if (child->IsLayoutOnly() && !IsViewShadowNode(nativeParent)) {
      child->ForceUnflattened();
      AttachChildren(host, child);
    }

    AttachNativeDescendants(host, nativeParent, child, nativeIndex);
  }

  const auto nativeChildrenCount = child->IsLayoutOnly() ? child->m_nativeChildrenCount : 1;
  UpdateAncestorNativeChildrenCount(host, child, nativeChildrenCount);
}

void DetachChild(INativeUIManagerHost *host, ShadowNodeBase *child, bool isUnflattening = false) {
  const auto nativeParentAndIndex = GetNativeParentAndIndex(host, child);

  // If this node has not yet been attached to a native parent, we can skip
  // this step. Its native descendants are not attached to a native parent.
  if (nativeParentAndIndex) {
    auto nativeParent = nativeParentAndIndex->node;
    auto nativeIndex = nativeParentAndIndex->index;
    DetachNativeDescendants(host, nativeParent, child, nativeIndex, isUnflattening);
  }

  const auto nativeChildrenCount = child->IsLayoutOnly() ? child->m_nativeChildrenCount : 1;
  UpdateAncestorNativeChildrenCount(host, child, nativeChildrenCount * -1);
}

///
/// Layout-Only Props
///

const std::unordered_set<std::string> LayoutOnlyProps{
    "alignSelf",
    "alignItems",
    "flex",
    "flexBasis",
    "flexDirection",
    "flexGrow",
    "flexShrink",
    "flexWrap",
    "justifyContent",
    "alignContent",
    "display",

    /* position */
    "position",
    "right",
    "top",
    "bottom",
    "left",
    "start",
    "end",

    /* dimensions */
    "width",
    "height",
    "minWidth",
    "maxWidth",
    "minHeight",
    "maxHeight",

    /* margins */
    "margin",
    "marginVertical",
    "marginHorizontal",
    "marginLeft",
    "marginRight",
    "marginTop",
    "marginBottom",
    "marginStart",
    "marginEnd",

    /* paddings */
    "padding",
    "paddingVertical",
    "paddingHorizontal",
    "paddingLeft",
    "paddingRight",
    "paddingTop",
    "paddingBottom",
    "paddingStart",
    "paddingEnd",
};

///
/// Public Methods
///

void AddView(IViewFlatteningHost *host, ShadowNodeBase *parent, ShadowNodeBase *child, int64_t index) {
  if (!host->IsViewFlatteningEnabled()) {
    parent->AddView(*child, index);
  } else {
    AttachChild(host, child);
  }
}

bool IsLayoutOnly(ShadowNodeBase *node, winrt::Microsoft::ReactNative::JSValueObject const &props) {
  if (!IsViewFlatteningEnabled(node)) {
    return false;
  }

  if (!IsViewShadowNode(node)) {
    return false;
  }

  auto isLayoutOnly = true;
  for (const auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;

    if (LayoutOnlyProps.find(propertyName) == LayoutOnlyProps.end()) {
      if (propertyName == "pointerEvents") {
        const auto pointerEventsValue = propertyValue.AsString();
        isLayoutOnly = (pointerEventsValue == "auto" || pointerEventsValue == "box-none");
      } else if (propertyName == "opacity") {
        isLayoutOnly =
            (propertyValue.IsNull() ||
             std::abs(propertyValue.AsDouble() - 1.0) < std::numeric_limits<double>().epsilon());
      } else if (
          propertyName == "borderWidth" || propertyName == "borderLeftWidth" || propertyName == "borderTopWidth" ||
          propertyName == "borderRightWidth" || propertyName == "borderBottomWidth") {
        isLayoutOnly = propertyValue.AsDouble() < std::numeric_limits<double>().epsilon();
      } else if (propertyName == "overflow") {
        isLayoutOnly = (propertyValue.IsNull() || propertyValue.AsString() == "visible");
      } else if (propertyName == "collapsable") {
        isLayoutOnly = propertyValue.IsNull() || propertyValue.AsBoolean();
      } else {
        isLayoutOnly = false;
      }
    }

    if (!isLayoutOnly) {
      break;
    }
  }

  return isLayoutOnly;
}

void RemoveAllChildren(IViewFlatteningHost *host, ShadowNodeBase *node) {
  if (!node->IsLayoutOnly()) {
    node->removeAllChildren();
  }
}

void RemoveChildAt(IViewFlatteningHost *host, ShadowNodeBase *parent, ShadowNodeBase *child, int64_t index) {
  if (!host->IsViewFlatteningEnabled()) {
    parent->RemoveChildAt(index);
  } else {
    DetachChild(host, child);
  }
}

void UnflattenNode(IViewFlatteningHost *host, ShadowNodeBase *node) {
  // Detach all native children
  DetachChild(host, node, true);

  // Reset native children count, it will be updated again when children are re-attached
  node->m_nativeChildrenCount = 0;

  // Re-attach all native children
  AttachChildren(host, node);

  // Attach self
  AttachChild(host, node);

  // Unflatten layout
  host->UnflattenLayout(node->m_tag);
}

} // namespace Microsoft::ReactNative::ViewFlattening
