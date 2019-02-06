// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "UnicodeConversion.h"
#include <jschelpers\Unicode.h>

#include "windows.h"
#include "stringapiset.h"

#include <string>
#include <exception>

namespace facebook {
namespace react {

namespace UnicodeConversion {

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

std::wstring Utf8ToUtf16(const std::string& utf8)
{
  return Utf8ToUtf16(utf8.c_str(), utf8.length());
}

std::string Utf16ToUtf8(const wchar_t* utf16, size_t utf16Len)
{
  static_assert(sizeof(wchar_t) == sizeof(uint16_t), "Doing a reinterpret_cast, so they had better match");
  return facebook::react::unicode::utf16toUTF8(reinterpret_cast<const uint16_t*>(utf16), utf16Len);
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
