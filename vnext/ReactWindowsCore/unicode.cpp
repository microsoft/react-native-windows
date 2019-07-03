// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "unicode.h"
#include "utilities.h"

#include "windows.h"

#include "stringapiset.h"

#include <cassert>
#include <cstring>
#include <exception>
#include <string>

namespace facebook {
namespace react {
namespace unicode {

// The implementations of the following functions heavily reference the MSDN
// article at https://msdn.microsoft.com/en-us/magazine/mt763237.aspx.

std::wstring utf8ToUtf16(const char *utf8, size_t utf8Len) {
  std::wstring utf16{};

  // A small optimization.
  if (utf8Len == 0) {
    return utf16;
  }

  // Extra parentheses needed here to prevent expanding max as a
  // Windows-specific preprocessor macro.
  if (utf8Len > static_cast<size_t>((std::numeric_limits<int>::max)())) {
    throw std::overflow_error(
        "Length of input string to utf8ToUtf16() must fit into an int.");
  }

  const int utf8Length = static_cast<int>(utf8Len);

  // We do not specify MB_ERR_INVALID_CHARS here, which means that invalid UTF-8
  // characters are replaced with U+FFFD.
  constexpr DWORD flags = 0;

  const int utf16Length = ::MultiByteToWideChar(
      CP_UTF8, // Source string is in UTF-8.
      flags, // Conversion flags.
      utf8, // Source UTF-8 string pointer.
      utf8Length, // Length of the source UTF-8 string, in chars.
      nullptr, // Do not convert during this step, instead, request the size
      0 //   of the destination buffer, in wchar_ts, excluding the
      //   null termination character.
  );

  if (utf16Length == 0) {
    throw facebook::react::unicode::UnicodeConversionException(
        "Cannot get result string length when converting from UTF-8 to UTF-16 "
        "(MultiByteToWideChar failed).",
        GetLastError());
  }

  // Note that because the length of the input UTF-8 string was explicitly
  // passed to MultiByteToWideChar (instead of just passing -1 and asking
  // MultiByteToWideChar to scan the whole input string until a null terminator
  // is found), MultiByteToWideChar won't add an additional null terminator to
  // the result string. Therefore, there's no need to invoke
  // std::wstring::resize with a "utf16Length + 1" value.
  utf16.resize(utf16Length);

  // Convert from UTF-8 to UTF-16
  // Note that MultiByteToWideChar converts the UTF-8 BOM into the UTF-16BE BOM.
  // So we do not have to do anything extra here to ensure correct BOM behavior.
  int result = ::MultiByteToWideChar(
      CP_UTF8, // Source string is in UTF-8.
      flags, // Conversion flags.
      utf8, // Source UTF-8 string pointer.
      utf8Length, // Length of source UTF-8 string, in chars.
      &utf16[0], // Pointer to destination buffer. This is fine because the
      //   the C++11 standard specifies that the elements of a
      //   std::basic_string are stored continuously.
      utf16Length // Size of destination buffer, in wchar_ts.
  );

  if (result == 0) {
    throw facebook::react::unicode::UnicodeConversionException(
        "Cannot convert from UTF-8 to UTF-16 (MultiByteToWideChar failed).",
        GetLastError());
  }

  return utf16;
}

std::wstring utf8ToUtf16(const char *utf8) {
  return utf8ToUtf16(utf8, strlen(utf8));
}

std::wstring utf8ToUtf16(const std::string &utf8) {
  return utf8ToUtf16(utf8.c_str(), utf8.length());
}

#if _HAS_CXX17
std::wstring utf8ToUtf16(const std::string_view &utf8) {
  return utf8ToUtf16(utf8.data(), utf8.length());
}
#endif

std::string utf16ToUtf8(const wchar_t *utf16, size_t utf16Len) {
  std::string utf8{};

  // A small optimization.
  if (utf16Len == 0) {
    return utf8;
  }

  // Extra parentheses needed here to prevent expanding max as a
  // Windows-specific preprocessor macro.
  if (utf16Len > static_cast<size_t>((std::numeric_limits<int>::max)())) {
    throw std::overflow_error(
        "Length of input string to utf16ToUtf8() must fit into an int.");
  }

  const int utf16Length = static_cast<int>(utf16Len);

  // We do not specify WC_ERR_INVALID_CHARS here, which means that invalid
  // UTF-16 characters are replaced with U+FFFD.
  constexpr DWORD flags = 0;

  const int utf8Length = ::WideCharToMultiByte(
      CP_UTF8, // Destination string is in UTF-8.
      flags, // Conversion flags.
      utf16, // Source UTF-16 string pointer.
      utf16Length, // Length of the source UTF-16 string, in wchar_ts.
      nullptr, // Do not convert during this step, instead, request the size
      0, //   of the destination buffer, in chars, excluding the
      //   null termination character.
      nullptr, // WideCharToMultiByte requires the last two parameters to be
      nullptr //   nullptrs when converting to UTF-8.
  );

  if (utf8Length == 0) {
    throw facebook::react::unicode::UnicodeConversionException(
        "Cannot get result string length when converting from UTF-16 to UTF-8 "
        "(WideCharToMultiByte failed).",
        GetLastError());
  }

  // Note that because the length of the input UTF-16 string was explicitly
  // passed to WideCharToMultiByte (instead of just passing -1 and asking
  // WideCharToMultiByte to scan the whole input string until a null terminator
  // is found), WideCharToMultiByte won't add an additional null terminator to
  // the result string. Therefore, there's no need to invoke
  // std::string::resize with a "utf8Length + 1" value.
  utf8.resize(utf8Length);

  // Convert from UTF-8 to UTF-16
  // Note that MultiByteToWideChar converts the UTF-8 BOM into the UTF-16BE BOM.
  // So we do not have to do anything extra here to ensure correct BOM behavior.
  int result = ::WideCharToMultiByte(
      CP_UTF8, // Destination string is in UTF-8.
      flags, // Conversion flags.
      utf16, // Source UTF-16 string pointer.
      utf16Length, // Length of the source UTF-16 string, in wchar_ts.
      &utf8[0], // Pointer to destination buffer. This is fine because the
      //   the C++11 standard specifies that the elements of a
      //   std::basic_string are stored continuously.
      utf8Length, // Size of destination buffer, in chars.
      nullptr, // WideCharToMultiByte requires the last two parameters to be
      nullptr //   nullptrs when converting to UTF-8.
  );

  if (result == 0) {
    throw facebook::react::unicode::UnicodeConversionException(
        "Cannot convert from UTF-16 to UTF-8 (WideCharToMultiByte failed).",
        GetLastError());
  }

  return utf8;
}

std::string utf16ToUtf8(const char16_t *utf16, size_t utf16Len) {
  return utf16ToUtf8(
      utilities::checkedReinterpretCast<const wchar_t *>(utf16), utf16Len);
}

std::string utf16ToUtf8(const wchar_t *utf16) {
  return utf16ToUtf8(utf16, wcslen(utf16));
}

std::string utf16ToUtf8(const char16_t *utf16) {
  return utf16ToUtf8(utf16, std::char_traits<char16_t>::length(utf16));
}

std::string utf16ToUtf8(const std::wstring &utf16) {
  return utf16ToUtf8(utf16.c_str(), utf16.length());
}

std::string utf16ToUtf8(const std::u16string &utf16) {
  return utf16ToUtf8(
      utilities::checkedReinterpretCast<const wchar_t *>(utf16.c_str()),
      utf16.length());
}

#if _HAS_CXX17
std::string utf16ToUtf8(const std::wstring_view &utf16) {
  return utf16ToUtf8(utf16.data(), utf16.length());
}

std::string utf16ToUtf8(const std::u16string_view &utf16) {
  return utf16ToUtf8(
      utilities::checkedReinterpretCast<const wchar_t *>(utf16.data()),
      utf16.length());
}
#endif

} // namespace unicode
} // namespace react
} // namespace facebook
