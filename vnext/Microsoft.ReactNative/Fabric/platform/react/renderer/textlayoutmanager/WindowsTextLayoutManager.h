// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#pragma warning(push)
#pragma warning(disable : 5028) // 'Node': Alignment specified in prior declaration (1) not specified in definition
#include <react/renderer/textlayoutmanager/TextLayoutManager.h>
#pragma warning(pop)

#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/attributedstring/ParagraphAttributes.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/textlayoutmanager/TextLayoutContext.h>
#include <react/renderer/textlayoutmanager/TextMeasureCache.h>
#include <react/utils/ContextContainer.h>
#include <memory>

namespace facebook::react {

class WindowsTextLayoutManager : public TextLayoutManager {
 public:
  WindowsTextLayoutManager(const ContextContainer::Shared &contextContainer);
  virtual ~WindowsTextLayoutManager() = default;

  /*
   * Not copyable.
   */
  WindowsTextLayoutManager(const WindowsTextLayoutManager &) = delete;
  WindowsTextLayoutManager &operator=(const WindowsTextLayoutManager &) = delete;

  /*
   * Not movable.
   */
  WindowsTextLayoutManager(WindowsTextLayoutManager &&) = delete;
  WindowsTextLayoutManager &operator=(WindowsTextLayoutManager &&) = delete;

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

  static void GetTextLayoutByAdjustingFontSizeToFit(
      AttributedStringBox attributedStringBox,
      const ParagraphAttributes &paragraphAttributes,
      LayoutConstraints layoutConstraints,
      winrt::com_ptr<IDWriteTextLayout> &spTextLayout,
      TextMeasurement::Attachments &attachments,
      float minimumFontScale) noexcept;

 private:
  static winrt::hstring GetTransformedText(const AttributedStringBox &attributedStringBox);
  static void GetTextLayout(
      const AttributedStringBox &attributedStringBox,
      const ParagraphAttributes &paragraphAttributes,
      Size size,
      winrt::com_ptr<IDWriteTextLayout> &spTextLayout) noexcept;
};

} // namespace facebook::react
