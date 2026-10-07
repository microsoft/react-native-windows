// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <CppRuntimeOptions.h>
#include <Networking\WinRTHttpResource.h>
#include <Networking\WinRTTypes.h>
#include "WinRTNetworkingMocks.h"

// Windows API
#include <Windows.h>
// Leaving a line so clang-format does not reorder the WinInet include.
#include <WinInet.h>

// Standard Library
#include <atomic>
#include <chrono>
#include <future>
#include <thread>

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
  bool IsTimeout{false};
};

RequestResult SendRequest(
    std::string method,
    WinRTHttpResource::HttpClientFactory clientFactory,
    int64_t timeout = 0) {
  auto initialClient = clientFactory();
  auto resource =
      std::make_shared<WinRTHttpResource>(std::move(initialClient), std::move(clientFactory));
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
  resource->SetOnError([result, completionCount, completed](int64_t, std::string &&error, bool isTimeout) {
    result->Error = std::move(error);
    result->IsTimeout = isTimeout;
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
      timeout,
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

HttpClient MakeDelayedFailingClient(
    std::shared_ptr<std::atomic<int>> attempts,
    HRESULT error,
    std::chrono::milliseconds delay) {
  auto filter = winrt::make<MockHttpBaseFilter>();
  filter.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
      [attempts, error, delay](HttpRequestMessage const &) -> ResponseOperation {
    ++(*attempts);
    co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{delay.count() * 10000});
    throw winrt::hresult_error{error};
    co_return nullptr;
  };

  return HttpClient{filter};
}

HttpClient MakeDelayedSuccessfulClient(
    std::shared_ptr<std::atomic<int>> attempts,
    std::chrono::milliseconds delay) {
  auto filter = winrt::make<MockHttpBaseFilter>();
  filter.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
      [attempts, delay](HttpRequestMessage const &request) -> ResponseOperation {
    ++(*attempts);
    co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{delay.count() * 10000});
    HttpResponseMessage response{HttpStatusCode::Ok};
    response.RequestMessage(request);
    response.Content(HttpStringContent{L"recovered"});
    co_return response;
  };

  return HttpClient{filter};
}

} // namespace

TEST_CLASS (WinRTHttpResourceUnitTest) {
  TEST_CLASS_INITIALIZE(Initialize) {
    winrt::uninit_apartment();
  }

  TEST_METHOD_CLEANUP(MethodCleanup) {
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", false);
  }

  TEST_METHOD(RetryDisabledPreservesConnectionFailure) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);

    auto result = SendRequest("GET", [attempts, clientCreations]() {
      ++(*clientCreations);
      return MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT));
    });

    Assert::AreEqual(1, attempts->load());
    Assert::AreEqual(1, clientCreations->load());
    Assert::AreNotEqual("", result.Error.c_str());
  }

  TEST_METHOD(GetConnectionFailureCreatesFreshClientAndRetriesOnce) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

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
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

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
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

    auto result = SendRequest("GET", [attempts, clientCreations]() {
      ++(*clientCreations);
      return MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT));
    });

    Assert::AreEqual(2, attempts->load());
    Assert::AreEqual(2, clientCreations->load());
    Assert::AreNotEqual("", result.Error.c_str());
  }

  TEST_METHOD(GetInternetTimeoutCreatesFreshClientAndRetriesOnce) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

    auto result = SendRequest("GET", [attempts, clientCreations]() {
      auto creation = ++(*clientCreations);
      return creation == 1 ? MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_TIMEOUT))
                           : MakeSuccessfulClient(attempts);
    });

    Assert::AreEqual(2, attempts->load());
    Assert::AreEqual(2, clientCreations->load());
    Assert::AreEqual(200, result.StatusCode);
    Assert::AreEqual("recovered", result.Content.c_str());
    Assert::AreEqual("", result.Error.c_str());
  }

  TEST_METHOD(PostConnectionFailureDoesNotRetry) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

    auto result = SendRequest("POST", [attempts, clientCreations]() {
      ++(*clientCreations);
      return MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT));
    });

    Assert::AreEqual(1, attempts->load());
    Assert::AreEqual(1, clientCreations->load());
    Assert::AreNotEqual("", result.Error.c_str());
  }

  TEST_METHOD(MixedCaseGetConnectionFailureCreatesFreshClientAndRetriesOnce) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

    auto result = SendRequest("gEt", [attempts, clientCreations]() {
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

  TEST_METHOD(RetryUsesRemainingRequestTimeout) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

    auto start = std::chrono::steady_clock::now();
    auto result = SendRequest(
        "GET",
        [attempts, clientCreations]() {
          auto creation = ++(*clientCreations);
          return creation == 1
              ? MakeDelayedFailingClient(
                    attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT), std::chrono::milliseconds{200})
              : MakeDelayedSuccessfulClient(attempts, std::chrono::milliseconds{1000});
        },
        800);
    auto elapsed = std::chrono::steady_clock::now() - start;

    Assert::AreEqual(2, attempts->load());
    Assert::AreEqual(2, clientCreations->load());
    Assert::IsTrue(result.IsTimeout);
    Assert::AreNotEqual("", result.Error.c_str());
    Assert::IsTrue(elapsed < std::chrono::milliseconds{1200});
  }

  TEST_METHOD(AbortDuringClientReplacementDoesNotDeliverRetryResponse) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);
    auto callbackCount = std::make_shared<std::atomic<int>>(0);
    auto replacementStarted = std::make_shared<std::promise<void>>();
    auto allowReplacement = std::make_shared<std::promise<void>>();
    auto allowReplacementFuture = allowReplacement->get_future().share();
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

    WinRTHttpResource::HttpClientFactory clientFactory =
        [attempts, clientCreations, replacementStarted, allowReplacementFuture]() {
          auto creation = ++(*clientCreations);
          if (creation == 1) {
            return MakeFailingClient(attempts, HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT));
          }

          replacementStarted->set_value();
          allowReplacementFuture.wait();
          return MakeSuccessfulClient(attempts);
        };

    auto initialClient = clientFactory();
    auto resource =
        std::make_shared<WinRTHttpResource>(std::move(initialClient), std::move(clientFactory));
    resource->SetOnResponse([callbackCount](int64_t, Microsoft::React::Networking::IHttpResource::Response &&) {
      ++(*callbackCount);
    });
    resource->SetOnData([callbackCount](int64_t, std::string &&) { ++(*callbackCount); });
    resource->SetOnResponseComplete([callbackCount](int64_t) { ++(*callbackCount); });
    resource->SetOnError([callbackCount](int64_t, std::string &&, bool) { ++(*callbackCount); });

    resource->SendRequest(
        "GET",
        "http://mockserver.rnw/officedev/office-js/issues/4972",
        1, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text",
        false, /*useIncrementalUpdates*/
        0, /*timeout*/
        false, /*withCredentials*/
        [](int64_t) {});

    replacementStarted->get_future().wait();
    resource->AbortRequest(1);
    allowReplacement->set_value();
    std::this_thread::sleep_for(std::chrono::milliseconds{50});

    Assert::AreEqual(1, attempts->load());
    Assert::AreEqual(2, clientCreations->load());
    Assert::AreEqual(0, callbackCount->load());
  }

  TEST_METHOD(ConcurrentFailuresShareOneReplacementClient) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);
    auto completions = std::make_shared<std::atomic<int>>(0);
    auto completed = std::make_shared<std::promise<void>>();
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

    WinRTHttpResource::HttpClientFactory clientFactory = [attempts, clientCreations]() {
      auto creation = ++(*clientCreations);
      if (creation > 1) {
        return MakeSuccessfulClient(attempts);
      }

      auto filter = winrt::make<MockHttpBaseFilter>();
      filter.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
          [attempts](HttpRequestMessage const &) -> ResponseOperation {
        ++(*attempts);
        while (attempts->load() < 2) {
          co_await winrt::resume_after(winrt::Windows::Foundation::TimeSpan{10000});
        }
        throw winrt::hresult_error{HRESULT_FROM_WIN32(ERROR_INTERNET_CANNOT_CONNECT)};
        co_return nullptr;
      };
      return HttpClient{filter};
    };

    auto initialClient = clientFactory();
    auto resource =
        std::make_shared<WinRTHttpResource>(std::move(initialClient), std::move(clientFactory));
    resource->SetOnResponse([](int64_t, Microsoft::React::Networking::IHttpResource::Response &&) {});
    resource->SetOnData([](int64_t, std::string &&) {});
    resource->SetOnResponseComplete([completions, completed](int64_t) {
      if (++(*completions) == 2) {
        completed->set_value();
      }
    });
    resource->SetOnError([completions, completed](int64_t, std::string &&, bool) {
      if (++(*completions) == 2) {
        completed->set_value();
      }
    });

    for (int64_t requestId = 1; requestId <= 2; ++requestId) {
      resource->SendRequest(
          "GET",
          "http://mockserver.rnw/officedev/office-js/issues/4972",
          requestId,
          {}, /*headers*/
          {}, /*data*/
          "text",
          false, /*useIncrementalUpdates*/
          0, /*timeout*/
          false, /*withCredentials*/
          [](int64_t) {});
    }

    completed->get_future().wait();

    Assert::AreEqual(4, attempts->load());
    Assert::AreEqual(2, clientCreations->load());
    Assert::AreEqual(2, completions->load());
  }

  TEST_METHOD(CertificateValidationFailureDoesNotRetry) {
    auto attempts = std::make_shared<std::atomic<int>>(0);
    auto clientCreations = std::make_shared<std::atomic<int>>(0);
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

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
    Microsoft::React::SetRuntimeOptionBool("Http.RetryOnTransientNetworkError", true);

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
