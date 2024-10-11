
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

} // namespace facebook::react
