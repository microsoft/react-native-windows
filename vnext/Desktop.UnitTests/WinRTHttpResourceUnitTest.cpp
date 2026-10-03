// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking\WinRTHttpResource.h>
#include <Networking\WinRTTypes.h>
#include "WinRTNetworkingMocks.h"

// Windows API
#include <Windows.h>
// Leaving a line so clang-format does not reorder the WinInet include.
#include <WinInet.h>

// Standard Library
#include <atomic>
#include <future>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Windows::Web::Http;

using Microsoft::React::Networking::ResponseOperation;
using Microsoft::React::Networking::WinRTHttpResource;
using winrt::Windows::Foundation::Uri;

namespace Microsoft::React::Test {

namespace {

struct RequestResult {
  int StatusCode{0};
  std::string Content;
  std::string Error;
};

RequestResult SendRequest(
    std::string method,
    WinRTHttpResource::HttpClientFactory clientFactory) {
  auto resource = std::make_shared<WinRTHttpResource>(std::move(clientFactory));
  auto result = std::make_shared<RequestResult>();
  auto completionCount = std::make_shared<std::atomic<int>>(0);
  auto completed = std::make_shared<std::promise<void>>();

  resource->SetOnResponse([result](int64_t, Microsoft::React::Networking::IHttpResource::Response &&response) {
    result->StatusCode = static_cast<int>(response.StatusCode);
  });
  resource->SetOnData([result](int64_t, std::string &&content) { result->Content = std::move(content); });
  resource->SetOnResponseComplete([completionCount, completed](int64_t) {
    if (++(*completionCount) == 1) {
      completed->set_value();
    }
  });
  resource->SetOnError([result, completionCount, completed](int64_t, std::string &&error, bool) {
    result->Error = std::move(error);
    if (++(*completionCount) == 1) {
      completed->set_value();
    }
  });

  resource->SendRequest(
      std::move(method),
      "http://mockserver.rnw/officedev/office-js/issues/4972",
      1, /*requestId*/
      {}, /*headers*/
      {}, /*data*/
      "text",
      false, /*useIncrementalUpdates*/
      0, /*timeout*/
      false, /*withCredentials*/
      [](int64_t) {});

  completed->get_future().wait();
  return *result;
}

HttpClient MakeFailingClient(std::shared_ptr<std::atomic<int>> attempts, HRESULT error) {
  auto filter = winrt::make<MockHttpBaseFilter>();
  filter.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
      [attempts, error](HttpRequestMessage const &) -> ResponseOperation {
    ++(*attempts);
    throw winrt::hresult_error{error};
    co_return nullptr;
  };

  return HttpClient{filter};
}

HttpClient MakeSuccessfulClient(std::shared_ptr<std::atomic<int>> attempts, HttpStatusCode status = HttpStatusCode::Ok) {
  auto filter = winrt::make<MockHttpBaseFilter>();
  filter.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
      [attempts, status](HttpRequestMessage const &request) -> ResponseOperation {
    ++(*attempts);
    HttpResponseMessage response{status};
    response.RequestMessage(request);
    response.Content(HttpStringContent{status == HttpStatusCode::Ok ? L"recovered" : L"server error"});
    co_return response;
  };

  return HttpClient{filter};
}

} // namespace

TEST_CLASS (WinRTHttpResourceUnitTest) {
  TEST_CLASS_INITIALIZE(Initialize) {
    winrt::uninit_apartment();
  }

  TEST_METHOD(GetConnectionFailureCreatesFreshClientAndRetriesOnce) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);

    auto result = SendRequest("GET", [attempts, clientCreations]() {
      auto creation = ++(*clientCreations);
      return creation == 1 ? MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT))
                           : MakeSuccessfulClient(attempts);
    });

    Assert::AreEqual(2, attempts->load());
    Assert::AreEqual(2, clientCreations->load());
    Assert::AreEqual(200, result.StatusCode);
    Assert::AreEqual("recovered", result.Content.c_str());
    Assert::AreEqual("", result.Error.c_str());
  }

  TEST_METHOD(HeadConnectionFailureCreatesFreshClientAndRetriesOnce) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);

    auto result = SendRequest("HEAD", [attempts, clientCreations]() {
      auto creation = ++(*clientCreations);
      return creation == 1 ? MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT))
                           : MakeSuccessfulClient(attempts);
    });

    Assert::AreEqual(2, attempts->load());
    Assert::AreEqual(2, clientCreations->load());
    Assert::AreEqual(200, result.StatusCode);
    Assert::AreEqual("", result.Error.c_str());
  }

  TEST_METHOD(GetRepeatedConnectionFailureStopsAfterOneRetry) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);

    auto result = SendRequest("GET", [attempts, clientCreations]() {
      ++(*clientCreations);
      return MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT));
    });

    Assert::AreEqual(2, attempts->load());
    Assert::AreEqual(2, clientCreations->load());
    Assert::AreNotEqual("", result.Error.c_str());
  }

  TEST_METHOD(PostConnectionFailureDoesNotRetry) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);

    auto result = SendRequest("POST", [attempts, clientCreations]() {
      ++(*clientCreations);
      return MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT));
    });

    Assert::AreEqual(1, attempts->load());
    Assert::AreEqual(1, clientCreations->load());
    Assert::AreNotEqual("", result.Error.c_str());
  }

  TEST_METHOD(CertificateValidationFailureDoesNotRetry) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);

    auto result = SendRequest("GET", [attempts, clientCreations]() {
      ++(*clientCreations);
      return MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_INVALID_CA));
    });

    Assert::AreEqual(1, attempts->load());
    Assert::AreEqual(1, clientCreations->load());
    Assert::AreNotEqual("", result.Error.c_str());
  }

  TEST_METHOD(HttpErrorResponseDoesNotRetry) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);

    auto result = SendRequest("GET", [attempts, clientCreations]() {
      ++(*clientCreations);
      return MakeSuccessfulClient(attempts, HttpStatusCode::ServiceUnavailable);
    });

    Assert::AreEqual(1, attempts->load());
    Assert::AreEqual(1, clientCreations->load());
    Assert::AreEqual(503, result.StatusCode);
    Assert::AreEqual("server error", result.Content.c_str());
    Assert::AreEqual("", result.Error.c_str());
  }
};

} // namespace Microsoft::React::Test
