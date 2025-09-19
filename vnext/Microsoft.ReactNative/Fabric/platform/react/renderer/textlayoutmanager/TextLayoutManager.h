// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <dwrite.h>
#include <react/renderer/attributedstring/AttributedString.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/attributedstring/ParagraphAttributes.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/textlayoutmanager/TextLayoutContext.h>
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
  ~TextLayoutManager() = default;

  /*
   * Measures `attributedStringBox` using native text rendering infrastructure.
   */
  TextMeasurement measure(
      AttributedStringBox attributedStringBox,
      ParagraphAttributes paragraphAttributes,
      const TextLayoutContext &layoutContext,
      LayoutConstraints layoutConstraints,
      std::shared_ptr<void> /* hostTextStorage */) const;

  /*
   * Measures lines of `attributedString` using native text rendering
   * infrastructure.
   */
  LinesMeasurements measureLines(AttributedString attributedString, ParagraphAttributes paragraphAttributes, Size size)
      const;

  std::shared_ptr<void> getHostTextStorage(
      AttributedString attributedString,
      ParagraphAttributes paragraphAttributes,
      LayoutConstraints layoutConstraints) const;

  /**
   * Measures an AttributedString on the platform, as identified by some
   * opaque cache ID.
   */
  TextMeasurement measureCachedSpannableById(
      int64_t cacheId,
      ParagraphAttributes const &paragraphAttributes,
      LayoutConstraints layoutConstraints) const;

  /*
   * Returns an opaque pointer to platform-specific TextLayoutManager.
   * Is used on a native views layer to delegate text rendering to the manager.
   */
  void *getNativeTextLayoutManager() const;

  static void GetTextLayout(
      AttributedStringBox attributedStringBox,
      ParagraphAttributes paragraphAttributes,
      LayoutConstraints layoutConstraints,
      winrt::com_ptr<IDWriteTextLayout> &spTextLayout) noexcept;

  static void GetTextLayoutByAdjustingFontSizeToFit(
      AttributedStringBox attributedStringBox,
      const ParagraphAttributes &paragraphAttributes,
      LayoutConstraints layoutConstraints,
      winrt::com_ptr<IDWriteTextLayout> &spTextLayout,
      TextMeasurement::Attachments &attachments) noexcept;

#pragma endregion

 private:
  static winrt::hstring GetTransformedText(AttributedStringBox const &attributedStringBox);
  static void GetTextLayout(
      AttributedStringBox attributedString,
      ParagraphAttributes paragraphAttributes,
      Size size,
      winrt::com_ptr<IDWriteTextLayout> &spTextLayout) noexcept;

  ContextContainer::Shared m_contextContainer;
#pragma warning(push)
#pragma warning(disable : 5028)
  TextMeasureCache m_measureCache{};
#pragma warning(pop)
};

} // namespace react
} // namespace facebook
