// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "UnicodeConversion.h"

#include "windows.h"
#include "stringapiset.h"

#include <string>
#include <exception>
#include <stdexcept>

namespace facebook {
namespace react {

namespace UnicodeConversion {

const uint16_t kUtf8OneByteBoundary       = 0x80;
const uint16_t kUtf8TwoBytesBoundary      = 0x800;
const uint16_t kUtf16HighSubLowBoundary   = 0xD800;
const uint16_t kUtf16HighSubHighBoundary  = 0xDC00;
const uint16_t kUtf16LowSubHighBoundary   = 0xE000;

std::wstring Utf8ToUtf16(const char* utf8, size_t utf8Len)
{
  std::wstring utf16{};

  if (utf8Len == 0)
  {
    return utf16;
  }

  // Extra parentheses needed here to prevent expanding max as a
  // Windows-specific preprocessor macro.
  if (utf8Len > static_cast<size_t>((std::numeric_limits<int>::max)()))
  {
    throw std::overflow_error("Input string too long: size_t-length doesn't fit into int.");
  }

  const int utf8Length = static_cast<int>(utf8Len);

  // Fail if an invalid UTF-8 character is encountered in the input string.
  constexpr DWORD flags = MB_ERR_INVALID_CHARS;

  const int utf16Length = ::MultiByteToWideChar(
    CP_UTF8,       // Source string is in UTF-8.
    flags,         // Conversion flags.
    utf8,          // Source UTF-8 string pointer.
    utf8Length,    // Length of the source UTF-8 string, in chars.
    nullptr,       // Do not convert during this step, instead
    0              //   request size of destination buffer, in wchar_ts.
  );

  if (utf16Length == 0)
  {
    throw std::runtime_error("Cannot get result string length when converting from UTF-8 to UTF-16 (MultiByteToWideChar failed).");
  }

  utf16.resize(utf16Length);

  // Convert from UTF-8 to UTF-16
  // Note that MultiByteToWideChar converts the UTF-8 BOM into the UTF-16BE BOM.
  int result = ::MultiByteToWideChar(
    CP_UTF8,       // Source string is in UTF-8.
    flags,         // Conversion flags.
    utf8,          // Source UTF-8 string pointer.
    utf8Length,    // Length of source UTF-8 string, in chars.
    &utf16[0],     // Pointer to destination buffer.
    utf16Length    // Size of destination buffer, in wchar_ts.
  );

  if (result == 0)
  {
    throw std::runtime_error("Cannot convert from UTF-8 to UTF-16 (MultiByteToWideChar failed).");
  }

  return utf16;
}

// Calculate how many bytes are needed to convert an UTF16 string into UTF8
// UTF16 string
size_t Utf16toUtf8Length(const wchar_t* utf16String, size_t utf16StringLen) {
  if (!utf16String || utf16StringLen == 0) {
    return 0;
  }

  uint32_t utf8StringLen = 0;
  auto utf16StringEnd = utf16String + utf16StringLen;
  auto idx16 = utf16String;
  while (idx16 < utf16StringEnd) {
    auto ch = *idx16++;
    if (ch < kUtf8OneByteBoundary) {
      utf8StringLen++;
    } else if (ch < kUtf8TwoBytesBoundary) {
      utf8StringLen += 2;
    } else if (
        (ch >= kUtf16HighSubLowBoundary) && (ch < kUtf16HighSubHighBoundary) &&
        (idx16 < utf16StringEnd) &&
        (*idx16 >= kUtf16HighSubHighBoundary) && (*idx16 < kUtf16LowSubHighBoundary)) {
      utf8StringLen += 4;
      idx16++;
    } else {
      utf8StringLen += 3;
    }
  }

  return utf8StringLen;
}

std::string Utf16ToUtf8(const wchar_t* utf16String, size_t utf16StringLen) {
  if (!utf16String || utf16StringLen <= 0) {
    return "";
  }

  std::string utf8String(Utf16toUtf8Length(utf16String, utf16StringLen), '\0');
  auto idx8 = utf8String.begin();
  auto idx16 = utf16String;
  auto utf16StringEnd = utf16String + utf16StringLen;
  while (idx16 < utf16StringEnd) {
    auto ch = *idx16++;
    if (ch < kUtf8OneByteBoundary) {
      *idx8++ = (ch & 0x7F);
    } else if (ch < kUtf8TwoBytesBoundary) {
#ifdef _MSC_VER
      #pragma warning(suppress: 4244)
      *idx8++ = 0b11000000 | (ch >> 6);
#else
      *idx8++ = 0b11000000 | (ch >> 6);
#endif
      *idx8++ = 0b10000000 | (ch & 0x3F);
    } else if (
        (ch >= kUtf16HighSubLowBoundary) && (ch < kUtf16HighSubHighBoundary) &&
        (idx16 < utf16StringEnd) &&
        (*idx16 >= kUtf16HighSubHighBoundary) && (*idx16 < kUtf16LowSubHighBoundary)) {
      auto ch2 = *idx16++;
      uint8_t trunc_byte = (((ch >> 6) & 0x0F) + 1);
      *idx8++ = 0b11110000 | (trunc_byte >> 2);
      *idx8++ = 0b10000000 | ((trunc_byte & 0x03) << 4) | ((ch >> 2) & 0x0F);
      *idx8++ = 0b10000000 | ((ch & 0x03) << 4) | ((ch2 >> 6) & 0x0F);
      *idx8++ = 0b10000000 | (ch2 & 0x3F);
    } else {
      *idx8++ = 0b11100000 | (ch >> 12);
      *idx8++ = 0b10000000 | ((ch >> 6) & 0x3F);
      *idx8++ = 0b10000000 | (ch & 0x3F);
    }
  }

  return utf8String;
}

std::wstring Utf8ToUtf16(const std::string& utf8)
{
  return Utf8ToUtf16(utf8.c_str(), utf8.length());
}

std::string Utf16ToUtf8(const std::wstring& utf16)
{
  return Utf16ToUtf8(utf16.c_str(), utf16.length());
}

#if _HAS_CXX17
std::string Utf16ToUtf8(const std::wstring_view& utf16)
{
  return Utf16ToUtf8(utf16.data(), utf16.length());
}
#endif

} // namespace UnicodeConversion

} // namespace react
} // namespace facebook
