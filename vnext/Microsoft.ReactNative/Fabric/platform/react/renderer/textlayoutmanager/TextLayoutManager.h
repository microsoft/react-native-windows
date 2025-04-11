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
      const AttributedStringBox &attributedStringBox,
      const ParagraphAttributes &paragraphAttributes,
      const TextLayoutContext &layoutContext,
      LayoutConstraints layoutConstraints) const;

  /*
   * Measures lines of `attributedString` using native text rendering
   * infrastructure.
   */
  LinesMeasurements measureLines(
      const AttributedStringBox &attributedStringBox,
      const ParagraphAttributes &paragraphAttributes,
      Size size) const;

  std::shared_ptr<void> getHostTextStorage(
      const AttributedStringBox &attributedStringBox,
      const ParagraphAttributes &paragraphAttributes,
      LayoutConstraints layoutConstraints) const;

  /**
   * Measures an AttributedString on the platform, as identified by some
   * opaque cache ID.
   */
  TextMeasurement measureCachedSpannableById(
      int64_t cacheId,
      const ParagraphAttributes &paragraphAttributes,
      LayoutConstraints layoutConstraints) const;

  /*
   * Calculates baseline of `attributedString` using native text rendering
   * infrastructure.
   */
  Float baseline(AttributedStringBox attributedStringBox, ParagraphAttributes paragraphAttributes, Size size) const;

  /*
   * Returns an opaque pointer to platform-specific TextLayoutManager.
   * Is used on a native views layer to delegate text rendering to the manager.
   */
  void *getNativeTextLayoutManager() const;

  static void GetTextLayout(
      const AttributedStringBox &attributedStringBox,
      const ParagraphAttributes &paragraphAttributes,
      Size size,
      winrt::com_ptr<IDWriteTextLayout> &spTextLayout,
      TextMeasurement::Attachments &attachments) noexcept;

  static void GetTextLayout(
      const AttributedStringBox &attributedStringBox,
      const ParagraphAttributes &paragraphAttributes,
      LayoutConstraints layoutConstraints,
      winrt::com_ptr<IDWriteTextLayout> &spTextLayout) noexcept;

#pragma endregion

 private:
  static winrt::hstring GetTransformedText(const AttributedStringBox &attributedStringBox);
  static void GetTextLayout(
      const AttributedStringBox &attributedStringBox,
      const ParagraphAttributes &paragraphAttributes,
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
