// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <Utils.h>
#include <utilities.h>

// Windows API
#include <winrt/base.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::string;
using std::string_view;

namespace Microsoft::React::Test {

TEST_CLASS (UtilsTest) {
 public:

// We turn clang format off here because it does not work with some of the
// test macros.
// clang-format off

#pragma region Base64 Tests

  TEST_METHOD(EncodeStdStringToBase64Succeeds)
  {
    string messages[]
    {
      "",
      "a",
      "ab",
      "abc",
      "abcd",
      "abcde",
      "abcdef",
      "abcdefg",
      "abcdefgh",
      "abcdefghi",
      "abcdefghij",
      "abcdefghijk",
      "abcdefghijkl"
    };

    // Computed using [System.Convert]::ToBase64String
    constexpr const char* expected[] =
    {
      "",
      "YQ==",
      "YWI=",
      "YWJj",
      "YWJjZA==",
      "YWJjZGU=",
      "YWJjZGVm",
      "YWJjZGVmZw==",
      "YWJjZGVmZ2g=",
      "YWJjZGVmZ2hp",
      "YWJjZGVmZ2hpag==",
      "YWJjZGVmZ2hpams=",
      "YWJjZGVmZ2hpamts"
    };

    for (auto i = 0; i < sizeof(messages)/sizeof(string); ++i)
    {
      auto actual = Utilities::EncodeBase64(string_view(messages[i]));
      Assert::AreEqual(expected[i], actual.data());
    }
  }

  TEST_METHOD(EncodeArrayViewToBase64Succeeds)
  {
    //[System.Convert]::ToBase64String([byte[]](0,1,2,3,4))
    constexpr const char* expected = "AAECAwQ=";

    auto input = std::vector<uint8_t>{ 0, 1, 2, 3, 4 };
    auto bytes = winrt::array_view<const uint8_t>(input.data(), input.data() + input.size());
    auto chars = reinterpret_cast<const char*>(bytes.data());
    auto view = string_view(chars, bytes.size());

    auto actual = Utilities::EncodeBase64(view);
    Assert::AreEqual(expected, actual.c_str());
  }

  TEST_METHOD(DecodeStdStringFromBase64Succeeds)
  {
    constexpr const char* messages[] =
    {
      "",
      "YQ==",
      "YWI=",
      "YWJj",
      "YWJjZA==",
      "YWJjZGU=",
      "YWJjZGVm",
      "YWJjZGVmZw==",
      "YWJjZGVmZ2g=",
      "YWJjZGVmZ2hp",
      "YWJjZGVmZ2hpag==",
      "YWJjZGVmZ2hpams=",
      "YWJjZGVmZ2hpamts"
    };

    constexpr const char* expected[]
    {
      "",
      "a",
      "ab",
      "abc",
      "abcd",
      "abcde",
      "abcdef",
      "abcdefg",
      "abcdefgh",
      "abcdefghi",
      "abcdefghij",
      "abcdefghijk",
      "abcdefghijkl"
    };

    for (auto i = 0; i < sizeof(messages) / sizeof(string); ++i)
    {
      auto actual = Utilities::DecodeBase64(string_view(messages[i]));
      Assert::AreEqual(expected[i], actual.data());
    }
  }

#pragma endregion Base64 Tests

#pragma region FormatString Tests

  TEST_METHOD(UtilsTest_StringFormat_Simple)
  {
    std::string result = Microsoft::React::FormatString("Hello, %s!", "World");
    Assert::AreEqual("Hello, World!", result.c_str());
  }

  TEST_METHOD(UtilsTest_StringFormat_Integer)
  {
    std::string result = Microsoft::React::FormatString("Port: %d", 8081);
    Assert::AreEqual("Port: 8081", result.c_str());
  }

  TEST_METHOD(UtilsTest_StringFormat_Multiple)
  {
    std::string result = Microsoft::React::FormatString("%s:%d", "localhost", 8081);
    Assert::AreEqual("localhost:8081", result.c_str());
  }

  TEST_METHOD(UtilsTest_StringFormat_Complex)
  {
    std::string result = Microsoft::React::FormatString(
      "http://%s/%s.bundle?platform=%s&dev=%s&hot=%s",
      "localhost:8081",
      "index",
      "windows",
      "true",
      "false");
    Assert::AreEqual(
      "http://localhost:8081/index.bundle?platform=windows&dev=true&hot=false",
      result.c_str());
  }

  TEST_METHOD(UtilsTest_StringFormat_EmptyString)
  {
    std::string result = Microsoft::React::FormatString("");
    Assert::AreEqual("", result.c_str());
  }

  TEST_METHOD(UtilsTest_StringFormat_NullPtr)
  {
    std::string result = Microsoft::React::FormatString(nullptr);
    Assert::AreEqual("", result.c_str());
  }

  TEST_METHOD(UtilsTest_StringFormat_NoArgs)
  {
    std::string result = Microsoft::React::FormatString("no args here");
    Assert::AreEqual("no args here", result.c_str());
  }

  TEST_METHOD(UtilsTest_StringFormat_LargeString)
  {
    std::string longString(1000, 'a');
    std::string result = Microsoft::React::FormatString("%s", longString.c_str());
    Assert::AreEqual(longString.c_str(), result.c_str());
  }

  TEST_METHOD(UtilsTest_StringFormat_MixedTypes)
  {
    std::string result = Microsoft::React::FormatString(
      "Int: %d, Uint: %u, Hex: %x, String: %s, Float: %.2f",
      -42,
      42u,
      255,
      "test",
      3.14159);
    Assert::AreEqual("Int: -42, Uint: 42, Hex: ff, String: test, Float: 3.14", result.c_str());
  }

  TEST_METHOD(UtilsTest_StringFormat_SpecialChars)
  {
    std::string result = Microsoft::React::FormatString("100%% complete");
    Assert::AreEqual("100% complete", result.c_str());
  }

#pragma endregion FormatString Tests
};

// clang-format on

} // namespace Microsoft::React::Test
