// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <exception>
#include <string>
#include <stdexcept>
#if _HAS_CXX17
#include <string_view>
#endif

namespace facebook {
namespace react {
namespace unicode {

// All functions in this header offer the strong exception safety guarantee and
// may throw the following exceptions:
//   - std::bad_alloc,
//   - std::overflow_error, and
//   - UnicodeConversionException (defined below).
//
class UnicodeConversionException : public std::runtime_error
{
public:
  UnicodeConversionException(const char* const message, uint32_t errorCode)
    : std::runtime_error(message)
    , m_errorCode(errorCode)
  {}

  inline uint32_t ErrorCode() const
  {
    return m_errorCode;
  }

private:
  // The error code returned by GetLastError().
  uint32_t m_errorCode;
};

// The following functions convert UTF-8 strings to UTF-16BE strings.
//
// If the input UTF-8 string begins with the UTF-8 Byte Order Mark (BOM) (0xef
// 0xbb 0xbf), then the output UTF-16BE string will begin with the UTF-16BE BOM
// (0xfeff). For example, "<UTF-8 BOM>abc" (0xef 0xbb 0xbf 0x61 0x62 0x63) is
// converted to "<UTF-16BE BOM>abc" (0xfeff 0x0061 0x0062 0x0063).
//
// If the input UTF-8 string omits the UTF-8 BOM, then the output UTF-16BE
// string will also omit the UTF16-BE BOM. For example, "abc" (0x61 0x62 0x63)
// is converted to "abc" (0x0061 0x0062 0x0063).
//
// For (1), utf8 does not have to be null terminated, and utf8Len must reflect
// the length of utf8, without the null terminator if it has one. The behavior
// is undefined otherwise.
//
// For (2), utf8 must be null terminated. The behavior is undefined otherwise.
//
/* (1) */ std::wstring __stdcall utf8ToUtf16(const char* utf8, size_t utf8Len);
/* (2) */ std::wstring __stdcall utf8ToUtf16(const char* utf8);
/* (3) */ std::wstring __stdcall utf8ToUtf16(const std::string& utf8);
#if _HAS_CXX17
/* (4) */ std::wstring __stdcall utf8ToUtf16(const std::string_view& utf8);
#endif

// The following functions convert UTF-16BE strings to UTF-8 strings. Their
// behaviors mirror those of the above utf8ToUtf16 functions.
//
// For (1) and (2), utf16 does not have to be null terminated, and utf16Len must
// reflect the length of utf16, without the null terminator if it has one. The
// behavior is undefined otherwise.
//
// For (3) and (4), utf16 must be null terminated. The behavior is undefined
// otherwise.
//
/* (1) */ std::string __stdcall utf16ToUtf8(const wchar_t* utf16, size_t utf16Len);
/* (2) */ std::string __stdcall utf16ToUtf8(const char16_t* utf16, size_t utf16Len);
/* (3) */ std::string __stdcall utf16ToUtf8(const wchar_t* utf16);
/* (4) */ std::string __stdcall utf16ToUtf8(const char16_t* utf16);
/* (5) */ std::string __stdcall utf16ToUtf8(const std::wstring& utf16);
/* (6) */ std::string __stdcall utf16ToUtf8(const std::u16string& utf16);
#if _HAS_CXX17
/* (7) */ std::string __stdcall utf16ToUtf8(const std::wstring_view& utf16);
/* (8) */ std::string __stdcall utf16ToUtf8(const std::u16string_view& utf16);
#endif

} // namespace utilities
} // namespace react
} // namespace facebook
