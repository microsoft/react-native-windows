// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <string>
#include "Unicode.h"
#include "UnicodeTestStrings.h"

using Microsoft::Common::Unicode::Utf16ToUtf8;
using Microsoft::Common::Unicode::Utf8ToUtf16;
using Microsoft::VisualStudio::CppUnitTestFramework::Assert;

namespace Microsoft::React::Test {

TEST_CLASS(UnicodeConversionTest) {
 public:
  TEST_METHOD(PrerequisiteTest) {
    Assert::IsTrue(strlen(SimpleTestStringNoBomUtf8) == 3);
    Assert::IsTrue(wcslen(SimpleTestStringNoBomUtf16) == 3);
    Assert::IsTrue(strlen(SimpleTestStringBomUtf8) == 6);
    Assert::IsTrue(wcslen(SimpleTestStringBomUtf16) == 4);
  }

  TEST_METHOD(Utf8ToUtf16SimpleTestNoBom) {
    Assert::IsTrue(
        Utf8ToUtf16(SimpleTestStringNoBomUtf8, strlen(SimpleTestStringNoBomUtf8)) == SimpleTestStringNoBomUtf16);
  }

  TEST_METHOD(Utf8ToUtf16SimpleTestBom) {
    Assert::IsTrue(Utf8ToUtf16(SimpleTestStringBomUtf8, strlen(SimpleTestStringBomUtf8)) == SimpleTestStringBomUtf16);
  }

  TEST_METHOD(Utf8ToUtf16InvalidCharacterTest) {
    // In UTF-8, cc must the first byte of a two byte sequence, but 22 cannot be
    // the second byte of a two byte sequence (in fact, it is the double quote
    // ASCII character). To prevent over comsumption attacks, we test that 22 is
    // not swallowed/omitted after the string is converted to UTF-16. For more
    // information on overconsumption attacks, see
    // http://websec.github.io/unicode-security-guide/character-transformations/#overconsumption
    constexpr const char *const invalidUtf8 = "\xcc\x22\x3c";
    Assert::IsTrue(Utf8ToUtf16(invalidUtf8) == L"\xfffd" + Utf8ToUtf16(invalidUtf8 + 1));

    // Although ed a3 a9 follows the correct binary format for a three byte
    // UTF-8 sequence, the Unicode code point it encodes is not valid. While
    // converting this string, Utf8ToUtf16 will not realize this until it
    // encounters a9, upon which it will replace ed a3 with U+FFFD. Utf8ToUtf16
    // will then try to interpret a9 on its own and, realizing that a9 can only
    // be the third byte unit of a three byte UTF-8 sequence, will replace it
    // with U+FFFD as well. Hence the result it outputs is U+FFFD U+FFFD.
    constexpr const char *const anotherInvalidUtf8 = "\xed\xa3\xa9";
    Assert::IsTrue(Utf8ToUtf16(anotherInvalidUtf8) == L"\xfffd\xfffd");
  }

  TEST_METHOD(Utf16ToUtf8SimpleTestNoBom) {
    Assert::IsTrue(
        Utf16ToUtf8(SimpleTestStringNoBomUtf16, wcslen(SimpleTestStringNoBomUtf16)) == SimpleTestStringNoBomUtf8);
  }

  TEST_METHOD(Utf16ToUtf8SimpleTestBom) {
    Assert::IsTrue(Utf16ToUtf8(SimpleTestStringBomUtf16, wcslen(SimpleTestStringBomUtf16)) == SimpleTestStringBomUtf8);
  }

  TEST_METHOD(Utf16ToUtf8InvalidCharacterTest) {
    // In Utf-16, D801 must be the first two bytes of a surrogate pair, but 0022
    // cannot be the second two bytes of a surrogate pair (in fact, it is the
    // double quote ASCII character). To prevent over consumption attacks, we
    // test that 0022 is not swallowed/omitted after the string is converted to
    // UTF-8. For more information on overconsumption attaks, see
    // http://websec.github.io/unicode-security-guide/character-transformations/#overconsumption
    constexpr const char16_t *const invalidUtf16 = u"\xD801\x0022";
    Assert::IsTrue(Utf16ToUtf8(invalidUtf16) == "\xef\xbf\xbd\x22");
  }

  TEST_METHOD(SymmetricConversionNoBom) {
    for (size_t i = 0; i < g_utf8TestStrings.size(); ++i) {
      std::wstring utf16 = Utf8ToUtf16(g_utf8TestStrings[i]);
      std::string utf8 = Utf16ToUtf8(utf16);
      Assert::IsTrue(utf8 == g_utf8TestStrings[i]);
    }
  }

  TEST_METHOD(SymmetricConversionTestBom) {
    constexpr const char *utf8Bom = "\xef\xbb\xbf";
    Assert::IsTrue(strlen(utf8Bom) == 3);

    constexpr const wchar_t utf16Bom = L'\xfeff';

    for (size_t i = 0; i < g_utf8TestStrings.size(); ++i) {
      std::string utf8Original = utf8Bom + g_utf8TestStrings[i];
      for (size_t i = 0; i < strlen(utf8Bom); ++i) {
        Assert::IsTrue(utf8Original[i] == utf8Bom[i]);
      }

      std::wstring utf16 = Utf8ToUtf16(utf8Original);
      Assert::IsTrue(utf16[0] == utf16Bom);

      std::string utf8 = Utf16ToUtf8(utf16);
      Assert::IsTrue(utf8 == utf8Original);
    }
  }

 private:
  constexpr static const char *SimpleTestStringNoBomUtf8 = "\x61\x62\x63"; // abc
  constexpr static const wchar_t *SimpleTestStringNoBomUtf16 = L"\x0061\x0062\x0063"; // abc
  constexpr static const char *SimpleTestStringBomUtf8 = "\xef\xbb\xbf\x61\x62\x63"; // <UTF-8 BOM>abc
  constexpr static const wchar_t *SimpleTestStringBomUtf16 = L"\xfeff\x0061\x0062\x0063"; //<UTF-16BE BOM>abc
};

} // namespace Microsoft::React::Test
