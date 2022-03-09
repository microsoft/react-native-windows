// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/attributedstring/AttributedString.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/attributedstring/ParagraphAttributes.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/textlayoutmanager/TextMeasureCache.h>
#include <react/utils/ContextContainer.h>
#include <memory>

namespace facebook {
namespace react {

class TextLayoutManager;

using SharedTextLayoutManager = std::shared_ptr<const TextLayoutManager>;

/*
 * Windows-specific TextLayoutManager.
 */
class TextLayoutManager {
 public:
  using Shared = std::shared_ptr<const TextLayoutManager>;

#pragma region required interface from core cxx cross platform impl
  TextLayoutManager(const ContextContainer::Shared &contextContainer) : m_contextContainer(contextContainer){};
  ~TextLayoutManager();

  /*
   * Measures `attributedStringBox` using native text rendering infrastructure.
   */
  TextMeasurement measure(
      AttributedStringBox attributedStringBox,
      ParagraphAttributes paragraphAttributes,
      LayoutConstraints layoutConstraints) const;

  /*
   * Measures lines of `attributedString` using native text rendering
   * infrastructure.
   */
  LinesMeasurements measureLines(AttributedString attributedString, ParagraphAttributes paragraphAttributes, Size size)
      const;

  /*
   * Returns an opaque pointer to platform-specific TextLayoutManager.
   * Is used on a native views layer to delegate text rendering to the manager.
   */
  void *getNativeTextLayoutManager() const;

#pragma endregion

 private:
  ContextContainer::Shared m_contextContainer;
};

} // namespace react
} // namespace facebook
