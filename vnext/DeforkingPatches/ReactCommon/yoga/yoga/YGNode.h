/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the LICENSE
 * file in the root directory of this source tree.
 */
#pragma once
#include <cstdint>
#include <stdio.h>
#include "Bitfield.h"
#include "CompactValue.h"
#include "YGConfig.h"
#include "YGLayout.h"
#include "YGStyle.h"
#include "YGMacros.h"
#include "Yoga-internal.h"

YGConfigRef __cdecl YGConfigGetDefault();

struct YGNode {
  using MeasureWithContextFn =
      YGSize (__cdecl *)(YGNode*, float, YGMeasureMode, float, YGMeasureMode, void*);
  using BaselineWithContextFn = float (__cdecl *)(YGNode*, float, float, void*);
  using PrintWithContextFn = void (__cdecl *)(YGNode*, void*);

private:
  static constexpr size_t hasNewLayout_ = 0;
  static constexpr size_t isReferenceBaseline_ = 1;
  static constexpr size_t isDirty_ = 2;
  static constexpr size_t nodeType_ = 3;
  static constexpr size_t measureUsesContext_ = 4;
  static constexpr size_t baselineUsesContext_ = 5;
  static constexpr size_t printUsesContext_ = 6;
  static constexpr size_t useWebDefaults_ = 7;

  void* context_ = nullptr;
  using Flags = facebook::yoga::
      Bitfield<uint8_t, bool, bool, bool, YGNodeType, bool, bool, bool, bool>;
  Flags flags_ =
      {true, false, false, YGNodeTypeDefault, false, false, false, false};
  uint8_t reserved_ = 0;
  union {
    YGMeasureFunc noContext;
    MeasureWithContextFn withContext;
  } measure_ = {nullptr};
  union {
    YGBaselineFunc noContext;
    BaselineWithContextFn withContext;
  } baseline_ = {nullptr};
  union {
    YGPrintFunc noContext;
    PrintWithContextFn withContext;
  } print_ = {nullptr};
  YGDirtiedFunc dirtied_ = nullptr;
  YGStyle style_ = {};
  YGLayout layout_ = {};
  uint32_t lineIndex_ = 0;
  YGNodeRef owner_ = nullptr;
  YGVector children_ = {};
  YGConfigRef config_;
  std::array<YGValue, 2> resolvedDimensions_ = {
      {YGValueUndefined, YGValueUndefined}};

  YGFloatOptional __cdecl relativePosition(
      const YGFlexDirection axis,
      const float axisSize) const;

  void __cdecl setMeasureFunc(decltype(measure_));
  void __cdecl setBaselineFunc(decltype(baseline_));

  void useWebDefaults() {
    flags_.at<useWebDefaults_>() = true;
    style_.flexDirection() = YGFlexDirectionRow;
    style_.alignContent() = YGAlignStretch;
  }

  // DANGER DANGER DANGER!
  // If the the node assigned to has children, we'd either have to deallocate
  // them (potentially incorrect) or ignore them (danger of leaks). Only ever
  // use this after checking that there are no children.
  // DO NOT CHANGE THE VISIBILITY OF THIS METHOD!
  YGNode& operator=(YGNode&&) = default;

  using CompactValue = facebook::yoga::detail::CompactValue;

public:
  YGNode() : YGNode{YGConfigGetDefault()} {}
  explicit YGNode(const YGConfigRef config) : config_{config} {
    if (config->useWebDefaults) {
      useWebDefaults();
    }
  };
  ~YGNode() = default; // cleanup of owner/children relationships in YGNodeFree

  YGNode(YGNode&&);

  // Does not expose true value semantics, as children are not cloned eagerly.
  // Should we remove this?
  YGNode(const YGNode& node) = default;

  // for RB fabric
  YGNode(const YGNode& node, YGConfigRef config);

  // assignment means potential leaks of existing children, or alternatively
  // freeing unowned memory, double free, or freeing stack memory.
  YGNode& operator=(const YGNode&) = delete;

  // Getters
  void* __cdecl getContext() const { return context_; }

  uint8_t& __cdecl reserved() { return reserved_; }
  uint8_t __cdecl reserved() const { return reserved_; }

  void __cdecl print(void*);

  bool __cdecl getHasNewLayout() const { return flags_.at<hasNewLayout_>(); }

  YGNodeType __cdecl getNodeType() const { return flags_.at<nodeType_>(); }

  bool __cdecl hasMeasureFunc() const noexcept { return measure_.noContext != nullptr; }

  YGSize __cdecl measure(float, YGMeasureMode, float, YGMeasureMode, void*);

  bool __cdecl hasBaselineFunc() const noexcept {
    return baseline_.noContext != nullptr;
  }

  float __cdecl baseline(float width, float height, void* layoutContext);

  YGDirtiedFunc __cdecl getDirtied() const { return dirtied_; }

  // For Performance reasons passing as reference.
  YGStyle& __cdecl getStyle() { return style_; }

  const YGStyle& __cdecl getStyle() const { return style_; }

  // For Performance reasons passing as reference.
  YGLayout& __cdecl getLayout() { return layout_; }

  const YGLayout& __cdecl getLayout() const { return layout_; }

  uint32_t __cdecl getLineIndex() const { return lineIndex_; }

  bool __cdecl isReferenceBaseline() { return flags_.at<isReferenceBaseline_>(); }

  // returns the YGNodeRef that owns this YGNode. An owner is used to identify
  // the YogaTree that a YGNode belongs to. This method will return the parent
  // of the YGNode when a YGNode only belongs to one YogaTree or nullptr when
  // the YGNode is shared between two or more YogaTrees.
  YGNodeRef __cdecl getOwner() const { return owner_; }

  // Deprecated, use getOwner() instead.
  YGNodeRef __cdecl getParent() const { return getOwner(); }

  const YGVector& __cdecl getChildren() const { return children_; }

  // Applies a callback to all children, after cloning them if they are not
  // owned.
  template <typename T>
  void __cdecl iterChildrenAfterCloningIfNeeded(T callback, void* cloneContext) {
    int i = 0;
    for (YGNodeRef& child : children_) {
      if (child->getOwner() != this) {
        child = config_->cloneNode(child, this, i, cloneContext);
        child->setOwner(this);
      }
      i += 1;

      callback(child, cloneContext);
    }
  }

  YGNodeRef __cdecl getChild(uint32_t index) const { return children_.at(index); }

  YGConfigRef __cdecl getConfig() const { return config_; }

  bool __cdecl isDirty() const { return flags_.at<isDirty_>(); }

  std::array<YGValue, 2> __cdecl getResolvedDimensions() const {
    return resolvedDimensions_;
  }

  YGValue __cdecl getResolvedDimension(int index) const {
    return resolvedDimensions_[index];
  }

  // Methods related to positions, margin, padding and border
  YGFloatOptional __cdecl getLeadingPosition(
      const YGFlexDirection axis,
      const float axisSize) const;
  bool __cdecl isLeadingPositionDefined(const YGFlexDirection axis) const;
  bool __cdecl isTrailingPosDefined(const YGFlexDirection axis) const;
  YGFloatOptional __cdecl getTrailingPosition(
      const YGFlexDirection axis,
      const float axisSize) const;
  YGFloatOptional __cdecl getLeadingMargin(
      const YGFlexDirection axis,
      const float widthSize) const;
  YGFloatOptional __cdecl getTrailingMargin(
      const YGFlexDirection axis,
      const float widthSize) const;
  float __cdecl getLeadingBorder(const YGFlexDirection flexDirection) const;
  float __cdecl getTrailingBorder(const YGFlexDirection flexDirection) const;
  YGFloatOptional __cdecl getLeadingPadding(
      const YGFlexDirection axis,
      const float widthSize) const;
  YGFloatOptional __cdecl getTrailingPadding(
      const YGFlexDirection axis,
      const float widthSize) const;
  YGFloatOptional __cdecl getLeadingPaddingAndBorder(
      const YGFlexDirection axis,
      const float widthSize) const;
  YGFloatOptional __cdecl getTrailingPaddingAndBorder(
      const YGFlexDirection axis,
      const float widthSize) const;
  YGFloatOptional __cdecl getMarginForAxis(
      const YGFlexDirection axis,
      const float widthSize) const;
  // Setters

  void __cdecl setContext(void* context) { context_ = context; }

  void __cdecl setPrintFunc(YGPrintFunc printFunc) {
    print_.noContext = printFunc;
    flags_.at<printUsesContext_>() = false;
  }
  void __cdecl setPrintFunc(PrintWithContextFn printFunc) {
    print_.withContext = printFunc;
    flags_.at<printUsesContext_>() = true;
  }
  void __cdecl setPrintFunc(std::nullptr_t) { setPrintFunc(YGPrintFunc{nullptr}); }

  void __cdecl setHasNewLayout(bool hasNewLayout) {
    flags_.at<hasNewLayout_>() = hasNewLayout;
  }

  void __cdecl setNodeType(YGNodeType nodeType) { flags_.at<nodeType_>() = nodeType; }

  void __cdecl setMeasureFunc(YGMeasureFunc measureFunc);
  void __cdecl setMeasureFunc(MeasureWithContextFn);
  void __cdecl setMeasureFunc(std::nullptr_t) {
    return setMeasureFunc(YGMeasureFunc{nullptr});
  }

  void __cdecl setBaselineFunc(YGBaselineFunc baseLineFunc) {
    flags_.at<baselineUsesContext_>() = false;
    baseline_.noContext = baseLineFunc;
  }
  void __cdecl setBaselineFunc(BaselineWithContextFn baseLineFunc) {
    flags_.at<baselineUsesContext_>() = true;
    baseline_.withContext = baseLineFunc;
  }
  void __cdecl setBaselineFunc(std::nullptr_t) {
    return setBaselineFunc(YGBaselineFunc{nullptr});
  }

  void __cdecl setDirtiedFunc(YGDirtiedFunc dirtiedFunc) { dirtied_ = dirtiedFunc; }

  void __cdecl setStyle(const YGStyle& style) { style_ = style; }

  void __cdecl setLayout(const YGLayout& layout) { layout_ = layout; }

  void __cdecl setLineIndex(uint32_t lineIndex) { lineIndex_ = lineIndex; }

  void __cdecl setIsReferenceBaseline(bool isReferenceBaseline) {
    flags_.at<isReferenceBaseline_>() = isReferenceBaseline;
  }

  void __cdecl setOwner(YGNodeRef owner) { owner_ = owner; }

  void __cdecl setChildren(const YGVector& children) { children_ = children; }

  // TODO: rvalue override for setChildren

  YG_DEPRECATED void __cdecl setConfig(YGConfigRef config) { config_ = config; }

  void __cdecl setDirty(bool isDirty);
  void __cdecl setLayoutLastOwnerDirection(YGDirection direction);
  void __cdecl setLayoutComputedFlexBasis(const YGFloatOptional computedFlexBasis);
  void __cdecl setLayoutComputedFlexBasisGeneration(
      uint32_t computedFlexBasisGeneration);
  void __cdecl setLayoutMeasuredDimension(float measuredDimension, int index);
  void __cdecl setLayoutHadOverflow(bool hadOverflow);
  void __cdecl setLayoutDimension(float dimension, int index);
  void __cdecl setLayoutDirection(YGDirection direction);
  void __cdecl setLayoutMargin(float margin, int index);
  void __cdecl setLayoutBorder(float border, int index);
  void __cdecl setLayoutPadding(float padding, int index);
  void __cdecl setLayoutPosition(float position, int index);
  void __cdecl setPosition(
      const YGDirection direction,
      const float mainSize,
      const float crossSize,
      const float ownerWidth);
  void __cdecl setLayoutDoesLegacyFlagAffectsLayout(bool doesLegacyFlagAffectsLayout);
  void __cdecl setLayoutDidUseLegacyFlag(bool didUseLegacyFlag);
  void __cdecl markDirtyAndPropogateDownwards();

  // Other methods
  YGValue __cdecl marginLeadingValue(const YGFlexDirection axis) const;
  YGValue __cdecl marginTrailingValue(const YGFlexDirection axis) const;
  YGValue __cdecl resolveFlexBasisPtr() const;
  void __cdecl resolveDimension();
  YGDirection __cdecl resolveDirection(const YGDirection ownerDirection);
  void __cdecl clearChildren();
  /// Replaces the occurrences of oldChild with newChild
  void __cdecl replaceChild(YGNodeRef oldChild, YGNodeRef newChild);
  void __cdecl replaceChild(YGNodeRef child, uint32_t index);
  void __cdecl insertChild(YGNodeRef child, uint32_t index);
  /// Removes the first occurrence of child
  bool __cdecl removeChild(YGNodeRef child);
  void __cdecl removeChild(uint32_t index);

  void __cdecl cloneChildrenIfNeeded(void*);
  void __cdecl markDirtyAndPropogate();
  float __cdecl resolveFlexGrow() const;
  float __cdecl resolveFlexShrink() const;
  bool __cdecl isNodeFlexible();
  bool __cdecl didUseLegacyFlag();
  bool __cdecl isLayoutTreeEqualToNode(const YGNode& node) const;
  void __cdecl reset();
};
