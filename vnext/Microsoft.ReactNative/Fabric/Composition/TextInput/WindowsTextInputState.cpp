
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WindowsTextInputState.h"

#ifdef ANDROID
#include <react/renderer/attributedstring/conversions.h>
#include <react/renderer/components/text/ParagraphState.h>
#endif

namespace facebook::react {

WindowsTextInputState::WindowsTextInputState(
    AttributedStringBox attributedStringBox,
    AttributedString reactTreeAttributedString,
    ParagraphAttributes paragraphAttributes,
    int64_t mostRecentEventCount)
    : attributedStringBox(std::move(attributedStringBox)),
      reactTreeAttributedString(std::move(reactTreeAttributedString)),
      paragraphAttributes(std::move(paragraphAttributes)),
      mostRecentEventCount(mostRecentEventCount) {}

} // namespace facebook::react
