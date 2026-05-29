// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IcuUtils.h"

#include <cwctype>

#include <winrt/Windows.Data.Text.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <icu.h>

namespace Microsoft::ReactNative::IcuUtils {

namespace {

// icu.dll ships in Windows 10 1903+. Desktop.DLL delay-loads it so that the
// react-native-win32.dll can still load on older Windows versions (e.g. 1809).
// Probe once per process to decide whether ICU calls are safe.
bool IsIcuDllAvailable() noexcept {
  static const bool s_available = []() noexcept {
    const HMODULE hIcu = ::LoadLibraryExW(L"icu.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    return hIcu != nullptr;
  }();
  return s_available;
}

std::vector<std::pair<int32_t, int32_t>> ComputeBoundariesWithWinrt(const wchar_t *text, int32_t length) noexcept {
  std::vector<std::pair<int32_t, int32_t>> boundaries;
  try {
    winrt::Windows::Data::Text::WordsSegmenter segmenter{L""};
    const winrt::hstring src{text, static_cast<uint32_t>(length)};
    for (const auto &token : segmenter.GetTokens(src)) {
      const auto seg = token.SourceTextSegment();
      const int32_t start = static_cast<int32_t>(seg.StartPosition);
      const int32_t end = start + static_cast<int32_t>(seg.Length);
      boundaries.emplace_back(start, end);
    }
  } catch (const winrt::hresult_error &) {
    boundaries.clear();
  }
  return boundaries;
}

bool IsAlphanumericFallback(UChar32 codePoint) noexcept {
  // Treat supplementary-plane code points as non-alphanumeric in the fallback;
  // this matches the degraded behavior expected when ICU is unavailable.
  if (codePoint < 0 || codePoint > 0xFFFF) {
    return false;
  }
  return std::iswalnum(static_cast<wint_t>(codePoint)) != 0;
}

} // namespace

void UBreakIteratorDeleter::operator()(void *ptr) const noexcept {
  if (ptr) {
    ubrk_close(static_cast<UBreakIterator *>(ptr));
  }
}

WordBreakIterator::WordBreakIterator(const wchar_t *text, int32_t length) noexcept : m_length(length) {
  if (!text || length <= 0) {
    return;
  }

  if (IsIcuDllAvailable()) {
    UErrorCode status = U_ZERO_ERROR;
    auto *iter = ubrk_open(UBRK_WORD, nullptr, reinterpret_cast<const UChar *>(text), length, &status);
    if (U_SUCCESS(status)) {
      m_breakIterator.reset(static_cast<void *>(iter));
    }
    return;
  }

  m_winrtBoundaries = ComputeBoundariesWithWinrt(text, length);
}

bool WordBreakIterator::IsValid() const noexcept {
  return m_breakIterator != nullptr || !m_winrtBoundaries.empty();
}

bool WordBreakIterator::GetWordBoundaries(int32_t position, int32_t &outStart, int32_t &outEnd) const noexcept {
  if (position < 0 || position >= m_length) {
    return false;
  }

  if (m_breakIterator) {
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

  for (const auto &[start, end] : m_winrtBoundaries) {
    if (position >= start && position < end) {
      outStart = start;
      outEnd = end;
      return true;
    }
  }
  return false;
}

bool IsAlphanumeric(UChar32 codePoint) noexcept {
  if (IsIcuDllAvailable()) {
    return u_isalnum(codePoint) != 0;
  }
  return IsAlphanumericFallback(codePoint);
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
