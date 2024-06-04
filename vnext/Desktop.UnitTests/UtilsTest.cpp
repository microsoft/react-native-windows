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

// We turn clang format off here because it does not work with some of the
// test macros.
// clang-format off

TEST_CLASS(UtilsTest)
{

  void ExpectUrl(
    string urlString,
    string protocol,
    string host,
    string port = "",
    string path = "/",
    string query = "")
  {

    Url url(std::move(urlString));

    Assert::AreEqual(protocol, url.scheme);
    Assert::AreEqual(host, url.host);
    Assert::AreEqual(port, url.port);
    Assert::AreEqual(path, url.path);
    Assert::AreEqual(query, url.queryString);
  }

#pragma region Url Tests

  TEST_METHOD(UtilsTest_ValidProtocols)
  {
    string protocols[4] { "http", "https", "ws", "wss" };
    for (auto protocol : protocols)
    {
      ExpectUrl(string(protocol + "://internal"), protocol, "internal");
    }
  }

  TEST_METHOD(UtilsTest_IntraHost)
  {
    ExpectUrl("ws://internal", "ws", "internal");
  }

  TEST_METHOD(UtilsTest_IntraHostTrailing)
  {
    ExpectUrl("ws://internal/", "ws", "internal");
  }

  TEST_METHOD(UtilsTest_IntraHostQueryLeading)
  {
    ExpectUrl("ws://internal?", "ws", "internal");
  }

  TEST_METHOD(UtilsTest_IntraHostTrailingQueryLeading)
  {
    ExpectUrl("ws://internal/?", "ws", "internal");
  }

  TEST_METHOD(UtilsTest_NormalHostQueryLeading)
  {
    ExpectUrl("ws://example.com?", "ws", "example.com");
  }

  TEST_METHOD(UtilsTest_IntraPort)
  {
    ExpectUrl("ws://internal:5000", "ws", "internal", "5000");
  }

  TEST_METHOD(UtilsTest_NormalPort)
  {
    ExpectUrl("ws://example.com:443", "ws", "example.com", "443");
  }

  TEST_METHOD(UtilsTest_PortPath)
  {
    ExpectUrl("ws://example.com:5000/ws", "ws", "example.com", "5000", "/ws");
  }

  TEST_METHOD(UtilsTest_Query)
  {
    ExpectUrl(
      "ws://example.com?a=1&b=2", "ws", "example.com", "", "/", "a=1&b=2");
  }

  TEST_METHOD(UtilsTest_TrailingPathQuery)
  {
    ExpectUrl(
      "ws://example.com/?a=1&b=2", "ws", "example.com", "", "/", "a=1&b=2");
  }

  TEST_METHOD(UtilsTest_HyphenHostInternal)
  {
    ExpectUrl("wss://-my-hyphened-host--", "wss", "-my-hyphened-host--");
  }

  TEST_METHOD(UtilsTest_NestedPathTrailingSlashLeadingQuestionMark)
  {
    ExpectUrl(
      "ws://example.com/the/nested/path/?",
      "ws",
      "example.com",
      "",
      "/the/nested/path/");
  }

  TEST_METHOD(UtilsTest_NestedSubdomain)
  {
    ExpectUrl(
      "ws://nested.sub.domain.of.example.com",
      "ws",
      "nested.sub.domain.of.example.com");
  }

#pragma region Url Negative Tests

  TEST_METHOD(UtilsTest_EmptyStringFails)
  {
    Assert::ExpectException<std::exception>([]()
    {
      Url("");
    });
  }

  TEST_METHOD(UtilsTest_WrongProtocol)
  {
    Assert::ExpectException<std::exception>([]()
    {
      Url("foos://internal");
    });
  }

  TEST_METHOD(UtilsTest_BadCharsInPort)
  {
    Assert::ExpectException<std::exception>([]()
    {
      Url("ws://internal:50O0");
    });
  }

  TEST_METHOD(UtilsTest_SpacesInProtocol)
  {
    Assert::ExpectException<std::exception>([]()
    {
      Url(" ws://internal");
    });
  }

#pragma endregion

#pragma endregion

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
};

// clang-format on

} // namespace Microsoft::React::Test
