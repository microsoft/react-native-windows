// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <string>

namespace facebook {
namespace react {

namespace UnicodeConversion {

/* The following functions are used to convert UTF-8 strings to UTF-16BE strings.
 *
 * If the input UTF-8 string begins with the UTF-8 Byte Order Mark (BOM)
 * (0xef 0xbb 0xbf), then the output UTF-16BE string will begin with the UTF-16BE
 * BOM (0xfeff). For example, "<UTF-8 BOM>﻿abc" (0xef 0xbb 0xbf 0x61 0x62 0x63) is
 * converted to "<UTF-16BE BOM>abc" (0xfeff 0x0061 0x0062 0x0063).
 *
 * If the input UTF-8 string omits the UTF-8 BOM, then the output UTF-16BE string
 * will also omit the UTF16-BE BOM. For example, "abc" (0x61 0x62 0x63) is
 * converted to "abc" (0x0061 0x0062 0x0063).
 */
std::wstring Utf8ToUtf16(const char* utf8, size_t utf8Len);
std::wstring Utf8ToUtf16(const std::string& utf8);

/* The following functions are used to convert UTF-16BE strings to UTF-8 strings.
 *
 * If the input UTF-16BE string begins with the UTF-16BE Byte Order Mark (BOM)
 * (0xfeff), then the output UTF-8 string will begin with the UTF-8
 * BOM (0xef 0xbb 0xbf). For example, "<UTF-16BE BOM>﻿abc" (0xfeff 0x0061 0x0062 0x0063)
 * is converted to "<UTF-8 BOM>﻿abc" (0xef 0xbb 0xbf 0x61 0x62 0x63).
 *
 * If the input UTF-16BE string omits the UTF-16BE BOM, then the output UTF-8 string
 * will also omit the UTF8 BOM. For example, "abc" (0x0061 0x0062 0x0063) is
 * converted to "abc" (0x61 0x62 0x63).
 */
std::string Utf16ToUtf8(const wchar_t* utf16, size_t utf16Len);
std::string Utf16ToUtf8(const std::wstring& utf16);
#if _HAS_CXX17
std::string Utf16ToUtf8(const std::wstring_view& utf16);
#endif

} // namespace UnicodeConversion
} // namespace react
} // namespace facebook
