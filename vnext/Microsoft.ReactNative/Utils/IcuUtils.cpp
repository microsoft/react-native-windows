// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IcuUtils.h"
#include <icu.h>

namespace Microsoft::ReactNative::IcuUtils {

void UBreakIteratorDeleter::operator()(void *ptr) const noexcept {
  if (ptr) {
    ubrk_close(static_cast<UBreakIterator *>(ptr));
  }
}

WordBreakIterator::WordBreakIterator(const wchar_t *text, int32_t length) noexcept : m_length(length) {
  UErrorCode status = U_ZERO_ERROR;
  auto *iter = ubrk_open(UBRK_WORD, nullptr, reinterpret_cast<const UChar *>(text), length, &status);
  if (U_SUCCESS(status)) {
    m_breakIterator.reset(static_cast<void *>(iter));
  }
}

bool WordBreakIterator::IsValid() const noexcept {
  return m_breakIterator != nullptr;
}

bool WordBreakIterator::GetWordBoundaries(int32_t position, int32_t &outStart, int32_t &outEnd) const noexcept {
  if (!m_breakIterator || position < 0 || position >= m_length) {
    return false;
  }

  auto *iter = static_cast<UBreakIterator *>(m_breakIterator.get());

  int32_t start = ubrk_preceding(iter, position + 1);
  if (start == UBRK_DONE) {
    start = 0;
  }

  int32_t end = ubrk_following(iter, position);
  if (end == UBRK_DONE) {
    end = m_length;
  }

  int32_t ruleStatus = ubrk_getRuleStatus(iter);
  if (ruleStatus == UBRK_WORD_NONE) {
    return false;
  }

  outStart = start;
  outEnd = end;
  return true;
}

bool IsAlphanumeric(UChar32 codePoint) noexcept {
  return u_isalnum(codePoint) != 0;
}

UChar32 GetCodePointAt(const wchar_t *str, int32_t length, int32_t pos) noexcept {
  if (!str || length <= 0 || pos < 0 || pos >= length) {
    return 0;
  }
  UChar32 cp;
  U16_GET(str, 0, pos, length, cp);
  return cp;
}

int32_t MoveToPreviousCodePoint(const wchar_t *str, int32_t pos) noexcept {
  if (!str || pos <= 0) {
    return 0;
  }
  U16_BACK_1(str, 0, pos);
  return pos;
}

int32_t MoveToNextCodePoint(const wchar_t *str, int32_t length, int32_t pos) noexcept {
  if (!str || length <= 0 || pos >= length) {
    return length;
  }
  U16_FWD_1(str, pos, length);
  return pos;
}

} // namespace Microsoft::ReactNative::IcuUtils
