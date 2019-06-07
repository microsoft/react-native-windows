// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "../ReactWindowsCore/unicode.h"
#include "UnicodeTestStrings.h"
#include <CppUnitTest.h>
#include <string>

using facebook::react::unicode::utf8ToUtf16;
using facebook::react::unicode::utf16ToUtf8;
using Microsoft::VisualStudio::CppUnitTestFramework::Assert;

TEST_CLASS(UnicodeConversionTest)
{
public:

  TEST_METHOD(PrerequisiteTest)
  {
    Assert::IsTrue(strlen(SimpleTestStringNoBomUtf8) == 3);
    Assert::IsTrue(wcslen(SimpleTestStringNoBomUtf16) == 3);
    Assert::IsTrue(strlen(SimpleTestStringBomUtf8) == 6);
    Assert::IsTrue(wcslen(SimpleTestStringBomUtf16) == 4);
  }

  TEST_METHOD(utf8ToUtf16SimpleTestNoBom)
  {
    Assert::IsTrue(utf8ToUtf16(SimpleTestStringNoBomUtf8, strlen(SimpleTestStringNoBomUtf8)) == SimpleTestStringNoBomUtf16);
  }

  TEST_METHOD(utf8ToUtf16SimpleTestBom)
  {
    Assert::IsTrue(utf8ToUtf16(SimpleTestStringBomUtf8, strlen(SimpleTestStringBomUtf8)) == SimpleTestStringBomUtf16);
  }

  TEST_METHOD(utf8ToUtf16InvalidCharacterTest)
  {
    constexpr const char* const invalidUtf8 = "\xed\xa3\xa9";

    // The binary representation for invalidUtf8 is
    //     11101101 10100011 10101001
    // which idicates that all three bytes is used to encode one (invalid)
    // character. One would expect MultiByteToWideChar to output u"\xfffd"
    // (U+FFFD encodede on UTF-16) when given invalidUtf8 as input, but oddly, it
    // outputs u"\xfffd\xfffd". However, the reason for this behavior is
    // irrelevant, as what we care about is that invalid UTF-8 characters do not
    // cause utf8ToUtf16() to throw any exception.

    std::wstring replacementUtf16{ reinterpret_cast<const wchar_t*>(u"\xfffd\xfffd") };

    Assert::IsTrue(utf8ToUtf16(invalidUtf8) == replacementUtf16);
  }

  TEST_METHOD(utf16ToUtf8SimpleTestNoBom)
  {
    Assert::IsTrue(utf16ToUtf8(SimpleTestStringNoBomUtf16, wcslen(SimpleTestStringNoBomUtf16)) == SimpleTestStringNoBomUtf8);
  }

  TEST_METHOD(utf16ToUtf8SimpleTestBom)
  {
    Assert::IsTrue(utf16ToUtf8(SimpleTestStringBomUtf16, wcslen(SimpleTestStringBomUtf16)) == SimpleTestStringBomUtf8);
  }

  TEST_METHOD(utf16ToUtf8InvalidCharacterTest)
  {
    constexpr const char16_t* const invalidUtf16 = u"\xd8e9";

    // This is U+FFFD encoded in UTF-8.
    std::string replacementUtf8{ "\xef\xbf\xbd" };

    Assert::IsTrue(utf16ToUtf8(invalidUtf16) == replacementUtf8);
  }

  TEST_METHOD(SymmetricConversionNoBom)
  {
    for (size_t i = 0; i < g_utf8TestStrings.size(); ++i)
    {
      std::wstring utf16 = utf8ToUtf16(g_utf8TestStrings[i]);
      std::string utf8 = utf16ToUtf8(utf16);
      Assert::IsTrue(utf8 == g_utf8TestStrings[i]);
    }
  }

  TEST_METHOD(SymmetricConversionTestBom)
  {
    constexpr const char* utf8Bom = "\xef\xbb\xbf";
    Assert::IsTrue(strlen(utf8Bom) == 3);

    constexpr const wchar_t utf16Bom = L'\xfeff';

    for (size_t i = 0; i < g_utf8TestStrings.size(); ++i)
    {
      std::string utf8Original = utf8Bom + g_utf8TestStrings[i];
      for (size_t i = 0; i < strlen(utf8Bom); ++i)
      {
        Assert::IsTrue(utf8Original[i] == utf8Bom[i]);
      }

      std::wstring utf16 = utf8ToUtf16(utf8Original);
      Assert::IsTrue(utf16[0] == utf16Bom);

      std::string utf8 = utf16ToUtf8(utf16);
      Assert::IsTrue(utf8 == utf8Original);
    }
  }

private:

  constexpr static const char* SimpleTestStringNoBomUtf8 = "\x61\x62\x63"; // abc
  constexpr static const wchar_t* SimpleTestStringNoBomUtf16 = L"\x0061\x0062\x0063"; // abc
  constexpr static const char* SimpleTestStringBomUtf8 = "\xef\xbb\xbf\x61\x62\x63"; // <UTF-8 BOM>abc
  constexpr static const wchar_t* SimpleTestStringBomUtf16 = L"\xfeff\x0061\x0062\x0063"; //<UTF-16BE BOM>abc
};
