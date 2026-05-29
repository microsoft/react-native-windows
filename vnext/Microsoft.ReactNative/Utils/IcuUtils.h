// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

// ICU utilities wrapped in a namespace to avoid UChar naming conflicts with Folly's FBString.
// Folly has a template parameter named 'UChar' which conflicts with ICU's global UChar typedef.
namespace Microsoft::ReactNative::IcuUtils {

using UChar32 = int32_t;

struct UBreakIteratorDeleter {
  void operator()(void *ptr) const noexcept;
};

class WordBreakIterator {
 public:
  WordBreakIterator(const wchar_t *text, int32_t length) noexcept;

  WordBreakIterator(const WordBreakIterator &) = delete;
  WordBreakIterator &operator=(const WordBreakIterator &) = delete;
  WordBreakIterator(WordBreakIterator &&) = default;
  WordBreakIterator &operator=(WordBreakIterator &&) = default;

  bool IsValid() const noexcept;

  bool GetWordBoundaries(int32_t position, int32_t &outStart, int32_t &outEnd) const noexcept;

 private:
  std::unique_ptr<void, UBreakIteratorDeleter> m_breakIterator{nullptr};
  // Populated when icu.dll is unavailable (Windows < 1903); produced by
  // Windows.Data.Text.WordsSegmenter, sorted as [start, end) ranges.
  std::vector<std::pair<int32_t, int32_t>> m_winrtBoundaries;
  int32_t m_length = 0;
};

bool IsAlphanumeric(UChar32 codePoint) noexcept;
UChar32 GetCodePointAt(const wchar_t *str, int32_t length, int32_t pos) noexcept;
int32_t MoveToPreviousCodePoint(const wchar_t *str, int32_t pos) noexcept;
int32_t MoveToNextCodePoint(const wchar_t *str, int32_t length, int32_t pos) noexcept;

} // namespace Microsoft::ReactNative::IcuUtils
