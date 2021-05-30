// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::string;

namespace Microsoft::React::Test {

// We turn clang format off here because it does not work with some of the
// test macros.
// clang-format off

TEST_CLASS(UtilsTest) {

  void ExpectUrl(
      string urlString,
      string protocol,
      string host,
      string port = "",
      string path = "/",
      string query = "") {

    Url url(std::move(urlString));

    Assert::AreEqual(protocol, url.scheme);
    Assert::AreEqual(host, url.host);
    Assert::AreEqual(port, url.port);
    Assert::AreEqual(path, url.path);
    Assert::AreEqual(query, url.queryString);
  }

#pragma region Url Tests

  TEST_METHOD(UtilsTest_ValidProtocols) {
    string protocols[4]{"http", "https", "ws", "wss"};
    for (auto protocol : protocols) {
      ExpectUrl(string(protocol + "://internal"), protocol, "internal");
    }
  }

  TEST_METHOD(UtilsTest_IntraHost) {
    ExpectUrl("ws://internal", "ws", "internal");
  }

  TEST_METHOD(UtilsTest_IntraHostTrailing) {
    ExpectUrl("ws://internal/", "ws", "internal");
  }

  TEST_METHOD(UtilsTest_IntraHostQueryLeading) {
    ExpectUrl("ws://internal?", "ws", "internal");
  }

  TEST_METHOD(UtilsTest_IntraHostTrailingQueryLeading) {
    ExpectUrl("ws://internal/?", "ws", "internal");
  }

  TEST_METHOD(UtilsTest_NormalHostQueryLeading) {
    ExpectUrl("ws://example.com?", "ws", "example.com");
  }

  TEST_METHOD(UtilsTest_IntraPort) {
    ExpectUrl("ws://internal:5000", "ws", "internal", "5000");
  }

  TEST_METHOD(UtilsTest_NormalPort) {
    ExpectUrl("ws://example.com:443", "ws", "example.com", "443");
  }

  TEST_METHOD(UtilsTest_PortPath) {
    ExpectUrl("ws://example.com:5000/ws", "ws", "example.com", "5000", "/ws");
  }

  TEST_METHOD(UtilsTest_Query) {
    ExpectUrl(
        "ws://example.com?a=1&b=2", "ws", "example.com", "", "/", "a=1&b=2");
  }

  TEST_METHOD(UtilsTest_TrailingPathQuery) {
    ExpectUrl(
        "ws://example.com/?a=1&b=2", "ws", "example.com", "", "/", "a=1&b=2");
  }

  TEST_METHOD(UtilsTest_HyphenHostInternal) {
    ExpectUrl("wss://-my-hyphened-host--", "wss", "-my-hyphened-host--");
  }

  TEST_METHOD(UtilsTest_NestedPathTrailingSlashLeadingQuestionMark) {
    ExpectUrl(
        "ws://example.com/the/nested/path/?",
        "ws",
        "example.com",
        "",
        "/the/nested/path/");
  }

  TEST_METHOD(UtilsTest_NestedSubdomain) {
    ExpectUrl(
        "ws://nested.sub.domain.of.example.com",
        "ws",
        "nested.sub.domain.of.example.com");
  }

#pragma region Url Negative Tests

  TEST_METHOD(UtilsTest_EmptyStringFails) {
    Assert::ExpectException<std::exception>([]() { Url(""); });
  }

  TEST_METHOD(UtilsTest_WrongProtocol) {
    Assert::ExpectException<std::exception>([]() { Url("foos://internal"); });
  }

  TEST_METHOD(UtilsTest_BadCharsInPort) {
    Assert::ExpectException<std::exception>([]() { Url("ws://internal:50O0"); });
  }

  TEST_METHOD(UtilsTest_SpacesInProtocol) {
    Assert::ExpectException<std::exception>([]() { Url(" ws://internal"); });
  }

#pragma endregion

#pragma endregion
};

// clang-format on

} // namespace Microsoft::React::Test
