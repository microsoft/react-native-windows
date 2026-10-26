
#include <react/renderer/attributedstring/AttributedString.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/attributedstring/PlaceholderAttributedString.h>
#include <react/renderer/attributedstring/TextAttributes.h>
#include <react/renderer/components/text/BaseTextShadowNode.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/core/LayoutContext.h>
#include <react/renderer/core/conversions.h>
#include <react/renderer/textlayoutmanager/TextLayoutContext.h>
#include <react/renderer/textlayoutmanager/TextLayoutManager.h>
#include <react/utils/ContextContainer.h>

namespace facebook::react {

/*
 * `ShadowNode` for <WindowsTextInput> component.
 */
template <const char *concreteComponentName, typename Props, typename EventEmitter, typename State>
class ConcreteTextInputShadowNode
    : public facebook::react::ConcreteViewShadowNode<concreteComponentName, Props, EventEmitter, State> {
 public:
  using Super = facebook::react::ConcreteViewShadowNode<concreteComponentName, Props, EventEmitter, State>;
  using Super::Super;

  static facebook::react::ShadowNodeTraits BaseTraits() {
    auto traits = Super::BaseTraits();
    traits.set(facebook::react::ShadowNodeTraits::Trait::LeafYogaNode);
    traits.set(facebook::react::ShadowNodeTraits::Trait::MeasurableYogaNode);
    traits.set(facebook::react::ShadowNodeTraits::Trait::BaselineYogaNode);
    return traits;
  }

  /*
   * Associates a shared TextLayoutManager with the node.
   * `TextInputShadowNode` uses the manager to measure text content
   * and construct `TextInputState` objects.
   */
  void setTextLayoutManager(std::shared_ptr<const facebook::react::TextLayoutManager> textLayoutManager) {
    Super::ensureUnsealed();
    textLayoutManager_ = std::move(textLayoutManager);
  }

 protected:
  facebook::react::Size measureContent(
      const facebook::react::LayoutContext &layoutContext,
      const facebook::react::LayoutConstraints &layoutConstraints) const override {
    // Layout is called right after measure.
    // Measure is marked as `const`, and `layout` is not; so State can be
    // updated during layout, but not during `measure`. If State is out-of-date
    // in layout, it's too late: measure will have already operated on old
    // State. Thus, we use the same value here that we *will* use in layout to
    // update the state.
    facebook::react::AttributedString attributedString = getMostRecentAttributedString(layoutContext);

    if (attributedString.isEmpty()) {
      attributedString = getPlaceholderAttributedString(layoutContext);
    }

    // BaseTextShadowNode only gets children. We must detect and prepend text
    // value attributes manually.
    if (!Super::getConcreteProps().text.empty()) {
      auto textAttributes = facebook::react::TextAttributes::defaultTextAttributes();
      textAttributes.apply(Super::getConcreteProps().textAttributes);
      auto fragment = facebook::react::AttributedString::Fragment{};
      fragment.string = Super::getConcreteProps().text;
      fragment.textAttributes = textAttributes;
      // If the TextInput opacity is 0 < n < 1, the opacity of the TextInput and
      // text value's background will stack. This is a hack/workaround to prevent
      // that effect.
      fragment.textAttributes.backgroundColor = facebook::react::clearColor();
      fragment.parentShadowView = facebook::react::ShadowView(*this);
      attributedString.prependFragment(std::move(fragment));
    }

    facebook::react::TextLayoutContext textLayoutContext;
    textLayoutContext.pointScaleFactor = layoutContext.pointScaleFactor;

    facebook::react::ParagraphAttributes paragraphAttributes{};
    paragraphAttributes.maximumNumberOfLines = Super::getConcreteProps().multiline ? 0 : 1;
    return textLayoutManager_
        ->measure(
            facebook::react::AttributedStringBox{attributedString},
            paragraphAttributes,
            textLayoutContext,
            layoutConstraints)
        .size;
  }

  void layout(facebook::react::LayoutContext layoutContext) override {
    updateStateIfNeeded(layoutContext);
    Super::layout(layoutContext);
  }

  facebook::react::Float baseline(const facebook::react::LayoutContext &layoutContext, facebook::react::Size size)
      const override {
    // Calculate baseline as 80% of the text height
    return size.height * 0.8f;
  }

  std::shared_ptr<const facebook::react::TextLayoutManager> textLayoutManager_;

  /*
   * Determines the constraints to use while measure the underlying text
   */
  facebook::react::LayoutConstraints getTextConstraints(
      const facebook::react::LayoutConstraints &layoutConstraints) const {
    if (Super::getConcreteProps().multiline) {
      return layoutConstraints;
    } else {
      // A single line TextInput acts as a horizontal scroller of infinitely
      // expandable text, so we want to measure the text as if it is allowed to
      // infinitely expand horizontally, and later clamp to the constraints of the
      // input.
      return facebook::react::LayoutConstraints{
          .minimumSize = layoutConstraints.minimumSize,
          .maximumSize =
              facebook::react::Size{
                  .width = std::numeric_limits<facebook::react::Float>::infinity(),
                  .height = layoutConstraints.maximumSize.height,
              },
          .layoutDirection = layoutConstraints.layoutDirection,
      };
    }
  }

 protected:
  // Overridable method so that the Abi version can maintain user's state
  virtual void updateTextInputState(
      facebook::react::AttributedStringBox attributedStringBox,
      facebook::react::AttributedString &reactTreeAttributedString,
      facebook::react::ParagraphAttributes paragraphAttributes,
      int64_t mostRecentEventCount) = 0;

  /*
   * Creates a `State` object (with `AttributedText` and
   * `TextLayoutManager`) if needed.
   */
  void updateStateIfNeeded(const facebook::react::LayoutContext &layoutContext) {
    Super::ensureUnsealed();
    const auto &stateData = Super::getStateData();
    auto reactTreeAttributedString = getAttributedString(layoutContext);

    // Tree is often out of sync with the value of the TextInput.
    // This is by design - don't change the value of the TextInput in the State,
    // and therefore in Java, unless the tree itself changes.
    if (stateData.reactTreeAttributedString == reactTreeAttributedString) {
      return;
    }

    // If props event counter is less than what we already have in state, skip it
    const auto &props = Super::getConcreteProps();
    if (props.mostRecentEventCount < stateData.mostRecentEventCount) {
      return;
    }

    // Store default TextAttributes in state.
    // In the case where the TextInput is completely empty (no value, no
    // defaultValue, no placeholder, no children) there are therefore no fragments
    // in the AttributedString, and when State is updated, it needs some way to
    // reconstruct a Fragment with default TextAttributes.
    auto defaultTextAttributes = facebook::react::TextAttributes::defaultTextAttributes();
    defaultTextAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
    defaultTextAttributes.apply(Super::getConcreteProps().textAttributes);

    auto newEventCount = stateData.reactTreeAttributedString.isContentEqual(reactTreeAttributedString)
        ? 0
        : Super::getConcreteProps().mostRecentEventCount;
    auto newAttributedString = getMostRecentAttributedString(layoutContext);

    // Even if we're here and updating state, it may be only to update the layout
    // manager If that is the case, make sure we don't update text: pass in the
    // current attributedString unchanged, and pass in zero for the "event count"
    // so no changes are applied There's no way to prevent a state update from
    // flowing to Java, so we just ensure it's a noop in those cases.

    updateTextInputState(
        facebook::react::AttributedStringBox(newAttributedString), reactTreeAttributedString, {}, newEventCount);
  }

  /*
   * Returns a `AttributedString` which represents text content of the node.
   */
  facebook::react::AttributedString getAttributedString(const facebook::react::LayoutContext &layoutContext) const {
    // Use BaseTextShadowNode to get attributed string from children

    auto childTextAttributes = facebook::react::TextAttributes::defaultTextAttributes();
    childTextAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
    childTextAttributes.apply(Super::getConcreteProps().textAttributes);
    // Don't propagate the background color of the TextInput onto the attributed
    // string. Android tries to render shadow of the background alongside the
    // shadow of the text which results in weird artifacts.
    childTextAttributes.backgroundColor = facebook::react::HostPlatformColor::UndefinedColor;

    auto attributedString = facebook::react::AttributedString{};
    auto attachments = facebook::react::BaseTextShadowNode::Attachments{};
    facebook::react::BaseTextShadowNode::buildAttributedString(
        childTextAttributes, *this, attributedString, attachments);
    attributedString.setBaseTextAttributes(childTextAttributes);

    // BaseTextShadowNode only gets children. We must detect and prepend text
    // value attributes manually.
    if (!Super::getConcreteProps().text.empty()) {
      auto textAttributes = facebook::react::TextAttributes::defaultTextAttributes();
      textAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
      textAttributes.apply(Super::getConcreteProps().textAttributes);
      textAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
      auto fragment = facebook::react::AttributedString::Fragment{};
      fragment.string = Super::getConcreteProps().text;
      fragment.textAttributes = textAttributes;
      // If the TextInput opacity is 0 < n < 1, the opacity of the TextInput and
      // text value's background will stack. This is a hack/workaround to prevent
      // that effect.
      fragment.textAttributes.backgroundColor = facebook::react::clearColor();
      fragment.parentShadowView = facebook::react::ShadowView(*this);
      attributedString.prependFragment(std::move(fragment));
    }

    return attributedString;
  }

  /**
   * Get the most up-to-date attributed string for measurement and State.
   */
  facebook::react::AttributedString getMostRecentAttributedString(
      const facebook::react::LayoutContext &layoutContext) const {
    const auto &state = Super::getStateData();

    auto reactTreeAttributedString = getAttributedString(layoutContext);

    // Sometimes the treeAttributedString will only differ from the state
    // not by inherent properties (string or prop attributes), but by the frame of
    // the parent which has changed Thus, we can't directly compare the entire
    // AttributedString
    bool treeAttributedStringChanged =
        !state.reactTreeAttributedString.compareTextAttributesWithoutFrame(reactTreeAttributedString);

    return (!treeAttributedStringChanged ? state.attributedStringBox.getValue() : reactTreeAttributedString);
  }

  facebook::react::AttributedString getPlaceholderAttributedString(
      const facebook::react::LayoutContext &layoutContext) const {
    const auto &props = Super::getConcreteProps();

    facebook::react::AttributedString attributedString;
    attributedString.setBaseTextAttributes(props.textAttributes);
    //[windows
    if (!props.placeholder.empty()) {
      auto textAttributes = facebook::react::TextAttributes::defaultTextAttributes();
      textAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
      textAttributes.apply(props.textAttributes);

      /*
       * These props are applied to `View`, therefore they must not be a part of
       * base text attributes.
       */
      textAttributes.backgroundColor = facebook::react::clearColor();
      textAttributes.opacity = 1;

      attributedString.appendFragment(
          {.string = props.placeholder,
           .textAttributes = textAttributes,
           .parentShadowView = facebook::react::ShadowView(*this)});
    }
    // windows]

    return ensurePlaceholderIfEmpty_DO_NOT_USE(attributedString);
  }
};

} // namespace facebook::react
