
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "CompWindowsTextInputState.h"

#include <react/renderer/components/text/conversions.h>
#include <react/renderer/debug/debugStringConvertibleUtils.h>

#include <utility>

namespace facebook {
namespace react {

CompWindowsTextInputState::CompWindowsTextInputState(
    int64_t mostRecentEventCount,
    AttributedString attributedString,
    AttributedString reactTreeAttributedString,
    ParagraphAttributes paragraphAttributes,
    TextAttributes defaultTextAttributes,
    ShadowView defaultParentShadowView,
    float defaultThemePaddingStart,
    float defaultThemePaddingEnd,
    float defaultThemePaddingTop,
    float defaultThemePaddingBottom)
    : mostRecentEventCount(mostRecentEventCount),
      attributedString(std::move(attributedString)),
      reactTreeAttributedString(std::move(reactTreeAttributedString)),
      paragraphAttributes(std::move(paragraphAttributes)),
      defaultTextAttributes(std::move(defaultTextAttributes)),
      defaultParentShadowView(std::move(defaultParentShadowView)),
      defaultThemePaddingStart(defaultThemePaddingStart),
      defaultThemePaddingEnd(defaultThemePaddingEnd),
      defaultThemePaddingTop(defaultThemePaddingTop),
      defaultThemePaddingBottom(defaultThemePaddingBottom) {}

CompWindowsTextInputState::CompWindowsTextInputState(
    CompWindowsTextInputState const &previousState,
    folly::dynamic const &data)
    : mostRecentEventCount(data.getDefault("mostRecentEventCount", previousState.mostRecentEventCount).getInt()),
      cachedAttributedStringId(data.getDefault("opaqueCacheId", previousState.cachedAttributedStringId).getInt()),
      attributedString(previousState.attributedString),
      reactTreeAttributedString(previousState.reactTreeAttributedString),
      paragraphAttributes(previousState.paragraphAttributes),
      defaultTextAttributes(previousState.defaultTextAttributes),
      defaultParentShadowView(previousState.defaultParentShadowView),
      defaultThemePaddingStart(
          data.getDefault("themePaddingStart", previousState.defaultThemePaddingStart).getDouble()),
      defaultThemePaddingEnd(data.getDefault("themePaddingEnd", previousState.defaultThemePaddingEnd).getDouble()),
      defaultThemePaddingTop(data.getDefault("themePaddingTop", previousState.defaultThemePaddingTop).getDouble()),
      defaultThemePaddingBottom(
          data.getDefault("themePaddingBottom", previousState.defaultThemePaddingBottom).getDouble()){};

} // namespace react
} // namespace facebook
