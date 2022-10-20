// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/attributedstring/AttributedString.h>
#include <react/renderer/attributedstring/ParagraphAttributes.h>
#include <react/renderer/mapbuffer/MapBuffer.h>
#include <react/renderer/textlayoutmanager/TextLayoutManager.h>

namespace facebook::react {

/*
 * State for <WindowsTextInput> component.
 */
class WindowsTextInputState final {
 public:
  int mostRecentEventCount{0};

  /**
   * Stores an opaque cache ID used on the Java side to refer to a specific
   * AttributedString for measurement purposes only.
   */
  int64_t cachedAttributedStringId{0};

  /*
   * All content of <TextInput> component represented as an `AttributedString`.
   * Only set if changed from the React tree's perspective.
   */
  AttributedString attributedString{};

  /*
   * All content of <TextInput> component represented as an `AttributedString`.
   * This stores the previous computed *from the React tree*. This usually
   * doesn't change as the TextInput contents are being updated. If it does
   * change, we need to wipe out current contents of the TextInput and replace
   * with the new value from the tree.
   */
  AttributedString reactTreeAttributedString{};

  /*
   * Represents all visual attributes of a paragraph of text represented as
   * a ParagraphAttributes.
   */
  ParagraphAttributes paragraphAttributes{};

  /**
   * Default TextAttributes used if we need to construct a new Fragment.
   * Only used if text is inserted into an AttributedString with no existing
   * Fragments.
   */
  TextAttributes defaultTextAttributes;

  /**
   * Default parent ShadowView used if we need to construct a new Fragment.
   * Only used if text is inserted into an AttributedString with no existing
   * Fragments.
   */
  ShadowView defaultParentShadowView;

  /**
   * Communicates Android theme padding back to the ShadowNode / Component
   * Descriptor for layout.
   */
  float defaultThemePaddingStart{NAN};
  float defaultThemePaddingEnd{NAN};
  float defaultThemePaddingTop{NAN};
  float defaultThemePaddingBottom{NAN};

  WindowsTextInputState(
      int64_t mostRecentEventCount,
      AttributedString attributedString,
      AttributedString reactTreeAttributedString,
      ParagraphAttributes paragraphAttributes,
      TextAttributes defaultTextAttributes,
      ShadowView defaultParentShadowView,
      float defaultThemePaddingStart,
      float defaultThemePaddingEnd,
      float defaultThemePaddingTop,
      float defaultThemePaddingBottom);

  WindowsTextInputState() = default;
  WindowsTextInputState(WindowsTextInputState const &previousState, folly::dynamic const &data);
  folly::dynamic getDynamic() const;
  MapBuffer getMapBuffer() const;
};

} // namespace facebook::react
