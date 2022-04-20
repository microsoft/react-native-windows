// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <CppUnitTest.h>

#include <Networking/IHttpResource.h>
#include <Networking/OriginPolicy.h>
#include <RuntimeOptions.h>
#include <Test/HttpServer.h>
#include <unicode.h>

// Boost Library
#include <boost/beast/http.hpp>

// Standard Library
#include <future>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace http = boost::beast::http;

using Networking::IHttpResource;
using Networking::OriginPolicy;
using std::make_shared;
using std::promise;
using std::string;
using std::vector;
using Test::DynamicRequest;
using Test::DynamicResponse;
using Test::EmptyResponse;
using Test::HttpServer;
using Test::ResponseWrapper;

TEST_CLASS (HttpResourceIntegrationTest) {

  static uint16_t s_port;

  TEST_METHOD_CLEANUP(MethodCleanup) {
    // Bug in WebSocketServer does not correctly release TCP port between test methods.
    // Using a different por per test for now.
    s_port++;
  }
  TEST_METHOD(RequestGetSucceeds) {
    string url = "http://localhost:" + std::to_string(s_port);

    promise<void> resPromise;
    string error;
    int statusCode = 0;

    auto server = make_shared<HttpServer>(s_port);
    server->Callbacks().OnGet = [&resPromise](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("some response content");

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&resPromise, &statusCode](int64_t, IHttpResource::Response response) {
      statusCode = static_cast<int>(response.StatusCode);
    });
    resource->SetOnData([&resPromise](int64_t, string &&content) { resPromise.set_value(); });
    resource->SetOnError([&resPromise, &error, &server](int64_t, string &&message) {
      error = std::move(message);
      resPromise.set_value();

      server->Stop();
    });
    resource->SendRequest(
        "GET",
        std::move(url),
        {} /*header*/,
        {} /*bodyData*/,
        "text",
        false,
        1000 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});

    // Synchronize response.
    resPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(RequestGetHeadersSucceeds) {
    string url = "http://localhost:" + std::to_string(s_port);

    promise<void> rcPromise;
    string error;
    IHttpResource::Response response;

    auto server = make_shared<HttpServer>(s_port);
    server->Callbacks().OnGet = [](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);

      // Response header
      response.set(http::field::server, "Microsoft::React::Test::HttpServer");
      // Response content header
      response.set(http::field::content_length, "0");
      // Response arbitrary header
      response.set("ResponseHeaderName1", "ResponseHeaderValue1");

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&rcPromise, &response](int64_t, IHttpResource::Response callbackResponse) {
      response = callbackResponse;
      rcPromise.set_value();
    });
    resource->SetOnError([&rcPromise, &error, &server](int64_t, string &&message) {
      error = std::move(message);
      rcPromise.set_value();

      server->Abort();
    });

    //clang-format off
    resource->SendRequest(
        "GET",
        std::move(url),
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

    rcPromise.get_future().wait();
    server->Stop();

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
    string url = "http://localhost:" + std::to_string(s_port);

    promise<void> getResponsePromise;
    promise<void> getDataPromise;
    promise<void> optionsPromise;
    string error;
    IHttpResource::Response getResponse;
    IHttpResource::Response optionsResponse;
    string content;

    auto server = make_shared<HttpServer>(s_port);
    server->Callbacks().OnOptions = [](const DynamicRequest &request) -> ResponseWrapper {
      EmptyResponse response;
      response.result(http::status::partial_content);
      response.set("PreflightName", "PreflightValue");

      return {std::move(response)};
    };
    server->Callbacks().OnGet = [](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("Response Body");

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&getResponse, &getResponsePromise, &optionsResponse, &optionsPromise](
                                int64_t, IHttpResource::Response callbackResponse) {
      if (callbackResponse.StatusCode == static_cast<int64_t>(http::status::ok)) {
        getResponse = callbackResponse;
        getResponsePromise.set_value();
      } else if (callbackResponse.StatusCode == static_cast<int64_t>(http::status::partial_content)) {
        optionsResponse = callbackResponse;
        optionsPromise.set_value();
      }
    });
    resource->SetOnData([&getDataPromise, &content](int64_t, string &&responseData) {
      content = std::move(responseData);

      if (!content.empty())
        getDataPromise.set_value();
    });
    resource->SetOnError(
        [&optionsPromise, &getResponsePromise, &getDataPromise, &error, &server](int64_t, string &&message) {
          error = std::move(message);

          optionsPromise.set_value();
          getResponsePromise.set_value();
          getDataPromise.set_value();

          server->Stop();
        });

    //clang-format off
    resource->SendRequest(
        "OPTIONS",
        string{url},
        {} /*headers*/,
        {} /*bodyData*/,
        "text",
        false,
        1000 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    resource->SendRequest(
        "GET",
        std::move(url),
        {} /*headers*/,
        {} /*bodyData*/,
        "text",
        false,
        1000 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    optionsPromise.get_future().wait();
    getResponsePromise.get_future().wait();
    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
    // TODO: Especialize AreEqual
    Assert::AreEqual(static_cast<size_t>(1), optionsResponse.Headers.size());
    for (auto header : optionsResponse.Headers) {
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

/*static*/ uint16_t HttpResourceIntegrationTest::s_port = 4444;
