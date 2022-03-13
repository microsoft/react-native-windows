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
using Test::DynamicRequest;
using Test::DynamicResponse;

TEST_CLASS (HttpResourceIntegrationTest) {
  TEST_METHOD(RequestGetSucceeds) {
    promise<void> promise;
    string error;
    int statusCode = 0;

    auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5556));
    server->SetOnGet([&promise](const DynamicRequest &request) -> DynamicResponse {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("some resposne content");
      promise.set_value();

      return response;
    });
    server->Start();

    //auto resource = IHttpResource::Make();
    //resource->SetOnResponse([&promise, &statusCode](int64_t, IHttpResource::Response response) {
    //  statusCode = static_cast<int>(response.StatusCode);
    //  promise.set_value();
    //});
    //resource->SetOnError([&promise, &error, &server](int64_t, string &&message) {
    //  error = std::move(message);
    //  promise.set_value();

    //  server->Abort();
    //});
    //resource->SendRequest(
    //    "GET",
    //    "http://localhost:5556",
    //    {} /*header*/,
    //    {} /*bodyData*/,
    //    "text",
    //    false,
    //    1000 /*timeout*/,
    //    false /*withCredentials*/,
    //    [](int64_t) {});

      // Synchronize response.
    promise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(RequestGetHeadersSucceeds) {
    promise<void> promise;
    string error;
    IHttpResource::Response response;

    // HTTP call scope
    //{
      auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5555));
      server->SetOnGet([](const DynamicRequest &request) -> DynamicResponse {
        DynamicResponse response;
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

        server->Abort();
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
    //}

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

  TEST_METHOD(RequestOptionsSucceeds) {
    promise<void> promise;
    string error;
    IHttpResource::Response response;
    string content;

    // HTTP call scope
    //{
      auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5555));
      server->SetOnGet([](const DynamicRequest& request) -> DynamicResponse {
        DynamicResponse response;
        response.result(http::status::ok);
        response.body() = Test::CreateStringResponseBody("Response Body");

        return response;
      });
      server->SetOnOptions([](const DynamicRequest &request) -> DynamicResponse {
        DynamicResponse response;
        response.result(http::status::ok);
        response.set("PreflightName", "PreflightValue");

        return response;
      });
      server->Start();

      auto resource = IHttpResource::Make();
      resource->SetOnResponse([/*&promise, */&response](int64_t, IHttpResource::Response callbackResponse) {
        response = callbackResponse;
      });
      resource->SetOnData([&promise, &content](int64_t, string &&responseData) {
        content = std::move(responseData);
        promise.set_value();
      });
      resource->SetOnError([&promise, &error, &server](int64_t, string &&message) {
        error = std::move(message);
        promise.set_value();

        server->Abort();
      });

      //clang-format off
      resource->SendRequest(
          "OPTIONS",
          "http://localhost:5555",
          {} /*headers*/,
          {} /*bodyData*/,
          "text",
          false,
          1000 /*timeout*/,
          false /*withCredentials*/,
          [](int64_t) {});
      resource->SendRequest(
          "GET",
          "http://localhost:5555",
          {} /*headers*/,
          {} /*bodyData*/,
          "text",
          false,
          1000 /*timeout*/,
          false /*withCredentials*/,
          [](int64_t) {});
      //clang-format on

      server->Stop();
    //}

    promise.get_future().wait();

    Assert::AreEqual({}, error, L"Error encountered");
    for (auto header : response.Headers) {
      if (header.first == "PreflightName") {
        Assert::AreEqual({"PreflightValue"}, header.second, L"Wrong header");
      } else {
        string message = "Unexpected header: [" + header.first + "]=[" + header.second + "]";
        Assert::Fail(Microsoft::Common::Unicode::Utf8ToUtf16(message).c_str());
      }
    }
    Assert::AreEqual({"Response Body"}, content);
  }
};
