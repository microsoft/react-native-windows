// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <CppUnitTest.h>
#include <IHttpResource.h>
#include <Test/HttpServer.h>
#include <boost/beast/http.hpp>
#include <unicode.h>

// Standard Library
#include <future>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace http = boost::beast::http;

using std::promise;
using std::string;
using std::vector;

TEST_CLASS (HttpResourceIntegrationTest) {
  TEST_METHOD(RequestGetSucceeds) {
    promise<void> getPromise;
    int statusCode = 0;

    // HTTP call scope
    {
      auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5555));
      server->SetOnGet([](const http::request<http::string_body> &request) -> http::response<http::dynamic_body> {
        http::response<http::dynamic_body> response;
        response.result(http::status::ok);

        return response;
      });
      server->Start();

      auto resource = IHttpResource::Make();
      resource->SetOnResponse([&getPromise, &statusCode](int64_t, IHttpResource::Response response) {
        statusCode = static_cast<int>(response.StatusCode);
        getPromise.set_value();
      });
      resource->SendRequest(
          "GET",
          "http://localhost:5555",
          {} /*header*/,
          {} /*bodyData*/,
          "text",
          false,
          1000 /*timeout*/,
          false /*withCredentials*/,
          [](int64_t) {});

      server->Stop();
    }
    // Synchronize response.
    getPromise.get_future().wait();

    Assert::AreEqual(200, statusCode);
  }

  // TODO: Hangs! Likely headers make server not send response.
  TEST_METHOD(RequestGetHeadersSucceeds) {
    promise<void> promise;
    string error;
    IHttpResource::Response response;

    // HTTP call scope
    {
      auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5555));
      server->SetOnGet([](const http::request<http::string_body> &request) -> http::response<http::dynamic_body> {
        http::response<http::dynamic_body> response;
        response.result(http::status::ok);

        // Response header
        response.set(http::field::server, "Microsoft::React::Test::HttpServer");
        // Response content header
        response.set(http::field::content_length, "0");
        // Response arbitrary header
        response.set("ResponseHeaderName1", "ResponseHeaderValue1");

        return response;
      });
      server->Start();

      auto resource = IHttpResource::Make();
      resource->SetOnResponse([&promise, &response](int64_t, IHttpResource::Response callbackResponse) {
        response = callbackResponse;
        promise.set_value();
      });
      resource->SetOnError([&promise, &error, &server](int64_t, string &&message) {
        error = std::move(message);
        promise.set_value();

        server->Stop(true /*abort*/);
      });

      //clang-format off
      resource->SendRequest(
          "GET",
          "http://localhost:5555",
          {
              {"Content-Type", "application/json"},
              {"Content-Encoding", "ASCII"},
              {"name3", "value3"},
              {"name4", "value4"},
          },
          {} /*bodyData*/,
          "text",
          false,
          1000 /*timeout*/,
          false /*withCredentials*/,
          [](int64_t) {});
      //clang-format on

      server->Stop();
    }

    promise.get_future().wait();

    Assert::AreEqual({}, error, L"Error encountered");
    for (auto header : response.Headers) {
      if (header.first == "Server") {
        Assert::AreEqual({"Microsoft::React::Test::HttpServer"}, header.second, L"Wrong header");
      } else if (header.first == "Content-Length") {
        Assert::AreEqual({"0"}, header.second, L"Wrong header");
      } else if (header.first == "ResponseHeaderName1") {
        Assert::AreEqual({"ResponseHeaderValue1"}, header.second, L"Wrong header");
      } else {
        string message = "Unexpected header: [" + header.first + "]=[" + header.second + "]";
        Assert::Fail(Microsoft::Common::Unicode::Utf8ToUtf16(message).c_str());
      }
    }
  }

  TEST_METHOD(RequestGetFails) {
    string error;
    promise<void> promise;

    auto resource = IHttpResource::Make();
    resource->SetOnError([&error, &promise](int64_t, string &&message) {
      error = message;
      promise.set_value();
    });

    resource->SendRequest("GET", "http://nonexistinghost", {}, {}, "text", false, 1000, false, [](int64_t) {});

    promise.get_future().wait();

    Logger::WriteMessage(error.c_str());
    Assert::AreNotEqual(string{}, error);
  }
};
