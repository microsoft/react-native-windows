
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WindowsTextInputState.h"

#include <react/renderer/components/text/conversions.h>
#include <react/renderer/debug/debugStringConvertibleUtils.h>
#include <react/renderer/mapbuffer/MapBuffer.h>
#include <react/renderer/mapbuffer/MapBufferBuilder.h>

#include <utility>

namespace facebook::react {

WindowsTextInputState::WindowsTextInputState(
    int64_t mostRecentEventCount,
    AttributedString attributedString,
    AttributedString reactTreeAttributedString,
    ParagraphAttributes paragraphAttributes,
    double defaultThemePaddingStart,
    double defaultThemePaddingEnd,
    double defaultThemePaddingTop,
    double defaultThemePaddingBottom)
    : mostRecentEventCount(mostRecentEventCount),
      attributedString(std::move(attributedString)),
      reactTreeAttributedString(std::move(reactTreeAttributedString)),
      paragraphAttributes(std::move(paragraphAttributes)),
      defaultThemePaddingStart(defaultThemePaddingStart),
      defaultThemePaddingEnd(defaultThemePaddingEnd),
      defaultThemePaddingTop(defaultThemePaddingTop),
      defaultThemePaddingBottom(defaultThemePaddingBottom) {}

WindowsTextInputState::WindowsTextInputState(const WindowsTextInputState &previousState, const folly::dynamic &data)
    : mostRecentEventCount(data.getDefault("mostRecentEventCount", previousState.mostRecentEventCount).getInt()),
      cachedAttributedStringId(data.getDefault("opaqueCacheId", previousState.cachedAttributedStringId).getInt()),
      attributedString(previousState.attributedString),
      reactTreeAttributedString(previousState.reactTreeAttributedString),
      paragraphAttributes(previousState.paragraphAttributes),
      defaultThemePaddingStart(
          data.getDefault("themePaddingStart", previousState.defaultThemePaddingStart).getDouble()),
      defaultThemePaddingEnd(data.getDefault("themePaddingEnd", previousState.defaultThemePaddingEnd).getDouble()),
      defaultThemePaddingTop(data.getDefault("themePaddingTop", previousState.defaultThemePaddingTop).getDouble()),
      defaultThemePaddingBottom(
          data.getDefault("themePaddingBottom", previousState.defaultThemePaddingBottom).getDouble()){};

} // namespace facebook::react
