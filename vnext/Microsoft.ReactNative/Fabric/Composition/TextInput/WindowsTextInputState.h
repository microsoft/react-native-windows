/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/attributedstring/ParagraphAttributes.h>
#include <react/renderer/textlayoutmanager/TextLayoutManager.h>

#include <folly/dynamic.h>
#include <react/renderer/mapbuffer/MapBuffer.h>
#include <react/renderer/mapbuffer/MapBufferBuilder.h> // [windows]

namespace facebook::react {

/*
 * State for <TextInput> component.
 */
class WindowsTextInputState final {
 public:
  WindowsTextInputState() = default;

  WindowsTextInputState(
      AttributedStringBox attributedStringBox,
      AttributedString reactTreeAttributedString,
      ParagraphAttributes paragraphAttributes,
      int64_t mostRecentEventCount)
      : attributedStringBox(std::move(attributedStringBox)),
        reactTreeAttributedString(std::move(reactTreeAttributedString)),
        paragraphAttributes(std::move(paragraphAttributes)),
        mostRecentEventCount(mostRecentEventCount)
  {
  }

<<<<<<< Upstream
  AndroidTextInputState(const AndroidTextInputState &previousState, const folly::dynamic &data)
=======
  WindowsTextInputState(const WindowsTextInputState &previousState, const folly::dynamic &data)
>>>>>>> Override
      : attributedStringBox(previousState.attributedStringBox),
        reactTreeAttributedString(previousState.reactTreeAttributedString),
        paragraphAttributes(previousState.paragraphAttributes),
        mostRecentEventCount(data.getDefault("mostRecentEventCount", previousState.mostRecentEventCount).getInt()),
<<<<<<< Upstream
        cachedAttributedStringId(data.getDefault("opaqueCacheId", previousState.cachedAttributedStringId).getInt())
  {
  }
=======
        cachedAttributedStringId(data.getDefault("opaqueCacheId", previousState.cachedAttributedStringId).getInt()) {}
>>>>>>> Override

  folly::dynamic getDynamic() const;
  MapBuffer getMapBuffer() const;

  /*
   * All content of <TextInput> component.
   */
  AttributedStringBox attributedStringBox;

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
  ParagraphAttributes paragraphAttributes;

  int64_t mostRecentEventCount{0};

  /**
   * Stores an opaque cache ID used on the Java side to refer to a specific
   * AttributedString for measurement purposes only.
   */
  int64_t cachedAttributedStringId{0};
};

} // namespace facebook::react
