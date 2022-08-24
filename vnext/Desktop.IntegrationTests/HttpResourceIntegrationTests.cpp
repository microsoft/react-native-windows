// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <CppUnitTest.h>

#include <Networking/IHttpResource.h>
#include <Networking/OriginPolicy.h>
#include <Test/HttpServer.h>
#include <unicode.h>

// Boost Library
#include <boost/beast/http.hpp>

// Standard Library
#include <future>

#include <WinInet.h>
#include <winrt/base.h>

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

struct RequestContext
{
  HINTERNET Session = NULL;
  HINTERNET Connection = NULL;
  int64_t StatusCode = 0;
  HANDLE Event = CreateEvent(NULL, FALSE, FALSE, L"ResponseEvent");
};

void ContextCallback(
    HINTERNET hInternet,
    DWORD_PTR dwContext,
    DWORD dwInternetStatus,
    LPVOID lpvStatusInformation,
    DWORD dwStatusInformationLength) {

  auto context = (RequestContext *)dwContext;
  DWORD dwError = 0;
  LPVOID lpMsgBuf;
  LPVOID lpDisplayBuf;
  winrt::hstring msgErr;
  auto osta = NULL;
  LPINTERNET_ASYNC_RESULT statusInfo = NULL;

  switch (dwInternetStatus) {
    case INTERNET_STATUS_RESOLVING_NAME:
      break;
    case INTERNET_STATUS_NAME_RESOLVED:
      break;
    case INTERNET_STATUS_CONNECTING_TO_SERVER:
      break;
    case INTERNET_STATUS_CONNECTED_TO_SERVER:
      break;
    case INTERNET_STATUS_COOKIE_SENT:
      break;
    case INTERNET_STATUS_COOKIE_RECEIVED:
      break;
    case INTERNET_STATUS_COOKIE_HISTORY: {
      // ExitSwitch:
      break;
    case INTERNET_STATUS_CLOSING_CONNECTION:
      break;
    case INTERNET_STATUS_CONNECTION_CLOSED:
      break;
    case INTERNET_STATUS_HANDLE_CLOSING:
      // Signal the event for closing the handle
      // only for the Request Handle
      // if (appContext) {
      //  SetEvent(appContext->hEvent);
      //}
      break;
    case INTERNET_STATUS_HANDLE_CREATED:
      // Verify we've a valid pointer
      if (lpvStatusInformation) {
        // fprintf(stderr, "Handle %x created\n", ((LPINTERNET_ASYNC_RESULT)lpvStatusInformation)->dwResult);
      }
      break;
    case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
      break;
    case INTERNET_STATUS_RECEIVING_RESPONSE:
      break;
    case INTERNET_STATUS_RESPONSE_RECEIVED:
      //// Verify we've a valid pointer with the correct size
      // if (lpvStatusInformation && dwStatusInformationLength == sizeof(DWORD)) {
      //   dwBytes = *((LPDWORD)lpvStatusInformation);
      //   fprintf(stderr, "Status: Response Received (%d Bytes)\n", dwBytes);
      // } else {
      //   fprintf(stderr, "Response Received: lpvStatusInformation not valid\n");
      // }
      break;
    case INTERNET_STATUS_REDIRECT:
      break;
    case INTERNET_STATUS_REQUEST_COMPLETE:
    // See https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/web/Wininet/Async/async.c#L389

      // check for error first
      dwError = ((LPINTERNET_ASYNC_RESULT)lpvStatusInformation)->dwError;
      statusInfo = (LPINTERNET_ASYNC_RESULT)lpvStatusInformation;

      if (dwError != ERROR_SUCCESS) {
        msgErr = winrt::hresult_error(HRESULT_FROM_WIN32(dwError), winrt::hresult_error::from_abi).message();
        if (dwError == ERROR_HTTP_REDIRECT_NEEDS_CONFIRMATION) {
          osta = ERROR_HTTP_REDIRECT_NEEDS_CONFIRMATION;
        }

        exit(1);
      }

      SetEvent(context->Event);
      break;
    case INTERNET_STATUS_REQUEST_SENT:
      // Verify we've a valid pointer with the correct size
      if (lpvStatusInformation && dwStatusInformationLength == sizeof(DWORD)) {
        // dwBytes = *((LPDWORD)lpvStatusInformation);
      } else {
        fprintf(stderr, "Request sent: lpvStatusInformation not valid\n");
      }
      break;
    case INTERNET_STATUS_DETECTING_PROXY:
      break;
    case INTERNET_STATUS_SENDING_REQUEST:
      break;
    case INTERNET_STATUS_STATE_CHANGE:
      break;
    case INTERNET_STATUS_P3P_HEADER:
      break;
    default:
      break;
    }
  }
}

namespace Microsoft::React::Test {

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
        0, /*requestId*/
        {}, /*header*/
        {}, /*data*/
        "text",
        false,
        0 /*timeout*/,
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
        0, /*requestId*/
        {
            {"Content-Type", "application/json"},
            {"Content-Encoding", "ASCII"},
            {"name3", "value3"},
            {"name4", "value4"},
        },
        {}, /*data*/
        "text",
        false,
        0 /*timeout*/,
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

    resource->SendRequest("GET", "http://nonexistinghost", 0, {}, {}, "text", false, 0, false, [](int64_t) {});

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
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text",
        false,
        1000 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text",
        false,
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    optionsPromise.get_future().wait();
    getResponsePromise.get_future().wait();
    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
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

  TEST_METHOD(SimpleRedirectGetSucceeds) {
    auto port1 = s_port;
    auto port2 = ++s_port;
    string url = "http://localhost:" + std::to_string(port1);

    promise<void> responsePromise;
    promise<void> contentPromise;
    IHttpResource::Response responseResult;
    string content;
    string error;

    auto server1 = make_shared<HttpServer>(port1);
    server1->Callbacks().OnGet = [port2](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::moved_permanently);
      response.set(http::field::location, {"http://localhost:" + std::to_string(port2)});

      return {std::move(response)};
    };
    auto server2 = make_shared<HttpServer>(port2);
    server2->Callbacks().OnGet = [](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("Redirect Content");

      return {std::move(response)};
    };

    server1->Start();
    server2->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&responseResult, &responsePromise](int64_t, IHttpResource::Response response) {
      if (response.StatusCode == static_cast<int64_t>(http::status::ok)) {
        responseResult = response;
        responsePromise.set_value();
      }
    });
    resource->SetOnData([&contentPromise, &content](int64_t, string &&responseData) {
      content = std::move(responseData);

      if (!content.empty())
        contentPromise.set_value();
    });
    resource->SetOnError([&responsePromise, &contentPromise, &error, &server1](int64_t, string &&message) {
      error = std::move(message);

      responsePromise.set_value();
      contentPromise.set_value();
    });

    //clang-format off
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text",
        false, /*useIncrementalUpdates*/
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    responsePromise.get_future().wait();
    contentPromise.get_future().wait();

    server2->Stop();
    server1->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(200), responseResult.StatusCode);
    Assert::AreEqual({"Redirect Content"}, content);
  }

  TEST_METHOD(SimpleRedirectPatchSucceeds) {
    auto port1 = s_port;
    auto port2 = ++s_port;
    string url = "http://localhost:" + std::to_string(port1);

    promise<void> responsePromise;
    promise<void> contentPromise;
    IHttpResource::Response responseResult;
    string content;
    string error;

    auto server1 = make_shared<HttpServer>(port1);
    server1->Callbacks().OnPatch = [port2](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::moved_permanently);
      response.set(http::field::location, {"http://localhost:" + std::to_string(port2)});

      return {std::move(response)};
    };
    auto server2 = make_shared<HttpServer>(port2);
    server2->Callbacks().OnPatch = [](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("Redirect Content");

      return {std::move(response)};
    };

    server1->Start();
    server2->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&responseResult, &responsePromise](int64_t, IHttpResource::Response response) {
      if (response.StatusCode == static_cast<int64_t>(http::status::ok)) {
        responseResult = response;
        responsePromise.set_value();
      }
    });
    resource->SetOnData([&contentPromise, &content](int64_t, string &&responseData) {
      content = std::move(responseData);

      if (!content.empty())
        contentPromise.set_value();
    });
    resource->SetOnError([&responsePromise, &contentPromise, &error, &server1](int64_t, string &&message) {
      error = std::move(message);

      responsePromise.set_value();
      contentPromise.set_value();
    });

    //clang-format off
    resource->SendRequest(
        "PATCH",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text",
        false, /*useIncrementalUpdates*/
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    responsePromise.get_future().wait();
    contentPromise.get_future().wait();

    server2->Stop();
    server1->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(200), responseResult.StatusCode);
    Assert::AreEqual({"Redirect Content"}, content);
  }

  TEST_METHOD(SimpleRedirectWinInetSucceeds) {
    auto port1 = s_port;
    auto port2 = ++s_port;
    string url = "http://localhost:" + std::to_string(port1);

    promise<void> responsePromise;
    promise<void> contentPromise;
    IHttpResource::Response responseResult;
    string content;
    string error;

    RequestContext context;

    auto responseFunc1 = [port2](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::moved_permanently);
      response.set(http::field::location, {"http://localhost:" + std::to_string(port2)});

      return {std::move(response)};
    };
    auto responseFunc2 = [](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("Redirect Content");

      return {std::move(response)};
    };

    auto server1 = make_shared<HttpServer>(port1);
    auto server2 = make_shared<HttpServer>(port2);

    server1->Callbacks().OnGet = responseFunc1;
    server1->Callbacks().OnPatch = responseFunc1;
    server2->Callbacks().OnGet = responseFunc2;
    server2->Callbacks().OnPatch = responseFunc2;


    server1->Start();
    server2->Start();

    const wchar_t *acceptTypes[] = {L"text/*", NULL};
    auto hSession = InternetOpen(L"MyUserAgent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, INTERNET_FLAG_ASYNC);
    if (!hSession) {
      Assert::Fail(L"Couldn't start session");
    }

    auto hCallback = InternetSetStatusCallback(hSession, (INTERNET_STATUS_CALLBACK)ContextCallback);
    if (hCallback == INTERNET_INVALID_STATUS_CALLBACK) {
      Assert::Fail(L"Failed to set callback");
    }

    auto hConnect = InternetConnect(
        hSession, L"localhost", static_cast<INTERNET_PORT>(port1), NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD_PTR)&context);
    if (!hConnect) {
      Assert::Fail(L"Couldn't connect");
    }

    auto hRequest = HttpOpenRequest(
      hConnect, L"PATCH", L"/", NULL, NULL, acceptTypes,
        INTERNET_FLAG_RELOAD
      | INTERNET_FLAG_NO_CACHE_WRITE,
        (DWORD_PTR)&context);
    if (!hRequest) {
      Assert::Fail(L"Couldn't send request");
    }

    auto bRequestSuccess = HttpSendRequest(hRequest, NULL /*headers*/, 0, NULL /*content*/, 0);
    auto lErr = GetLastError();
    if (!bRequestSuccess && lErr != ERROR_IO_PENDING) {
      Assert::Fail(L"Failed to send");
    }

    WaitForSingleObject(context.Event, INFINITE);
    // responsePromise.get_future().wait();
    // contentPromise.get_future().wait();

    server2->Stop();
    server1->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(200), responseResult.StatusCode);
    Assert::AreEqual({"Redirect Content"}, content);
  }

  TEST_METHOD(TimeoutSucceeds) {
    auto port = s_port;
    string url = "http://localhost:" + std::to_string(port);

    promise<void> getPromise;
    string error;
    int statusCode = 0;

    auto server = std::make_shared<HttpServer>(s_port);
    server->Callbacks().OnGet = [](const DynamicRequest &) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);

      // Hold response to test client timeout
      promise<void> timer;
      timer.get_future().wait_for(std::chrono::milliseconds(2000));

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&getPromise, &statusCode](int64_t, IHttpResource::Response response) {
      statusCode = static_cast<int>(response.StatusCode);
      getPromise.set_value();
    });
    resource->SetOnError([&getPromise, &error](int64_t, string &&errorMessage) {
      error = std::move(errorMessage);
      getPromise.set_value();
    });
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text", /*responseType*/
        false, /*useIncrementalUpdates*/
        6000, /*timeout*/
        false, /*withCredentials*/
        [](int64_t) {} /*callback*/);

    getPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(TimeoutFails) {
    auto port = s_port;
    string url = "http://localhost:" + std::to_string(port);

    promise<void> getPromise;
    string error;
    int statusCode = 0;

    auto server = std::make_shared<HttpServer>(s_port);
    server->Callbacks().OnGet = [](const DynamicRequest &) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);

      // Hold response to test client timeout
      promise<void> timer;
      timer.get_future().wait_for(std::chrono::milliseconds(4000));

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&getPromise, &statusCode](int64_t, IHttpResource::Response response) {
      statusCode = static_cast<int>(response.StatusCode);
      getPromise.set_value();
    });
    resource->SetOnError([&getPromise, &error](int64_t, string &&errorMessage) {
      error = std::move(errorMessage);
      getPromise.set_value();
    });
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text", /*responseType*/
        false, /*useIncrementalUpdates*/
        2000, /*timeout*/
        false, /*withCredentials*/
        [](int64_t) {} /*callback*/);

    getPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({"[0x800705b4] This operation returned because the timeout period expired."}, error);
    Assert::AreEqual(0, statusCode);
  }
};

/*static*/ uint16_t HttpResourceIntegrationTest::s_port = 4444;

} // namespace Microsoft::React::Test
