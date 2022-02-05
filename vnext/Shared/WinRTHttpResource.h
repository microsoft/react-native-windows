// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IHttpResource.h>

// Windows API
#include <winrt/Windows.Web.Http.h>

// Standard Library
#include <mutex>

namespace Microsoft::React {

class WinRTHttpResource : public IHttpResource, public std::enable_shared_from_this<WinRTHttpResource> {

  typedef winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
      ResponseType;

  static int64_t s_lastRequestId;

  winrt::Windows::Web::Http::IHttpClient m_client;
  std::mutex m_mutex;
  std::unordered_map<int64_t, ResponseType> m_requests;

  std::function<void(int64_t requestId)> m_onRequest;
  std::function<void(int64_t requestId, Response &&response)> m_onResponse;
  std::function<void(int64_t requestId, std::string &&responseData)> m_onData;
  std::function<void(int64_t requestId, std::string &&message /*, bool isTimeout*/)> m_onError;

  void AddRequest(int64_t requestId, ResponseType response) noexcept;

  void RemoveRequest(int64_t requestId) noexcept;

  //TODO: Make non-trivial args r-value??
  winrt::fire_and_forget PerformSendRequest(/*TODO: shared self?,*/
    int64_t requestId,
    winrt::Windows::Web::Http::HttpRequestMessage request,
    bool textResponse
    /*, requestId?*/) noexcept;

public:
  WinRTHttpResource() noexcept;

  WinRTHttpResource(winrt::Windows::Web::Http::IHttpClient client) noexcept;

#pragma region IHttpResource

  void SendRequest(
      const std::string &method,
      const std::string &url,
      const Headers &headers,
      BodyData &&bodyData,
      const std::string &responseType,
      bool useIncrementalUpdates,
      int64_t timeout,
      bool withCredentials,
      std::function<void(int64_t)> &&callback) noexcept override;
  void AbortRequest(int64_t requestId) noexcept override;
  void ClearCookies() noexcept override;

#pragma endregion IHttpResource

  void SetOnRequest(std::function<void(int64_t requestId)> &&handler) noexcept override;
  void SetOnResponse(std::function<void(int64_t requestId, Response&& response)> &&handler) noexcept override;
  void SetOnData(std::function<void(int64_t requestId, std::string&& responseData)> &&handler) noexcept override;
  void SetOnError(
      std::function<void(int64_t requestId, std::string &&message /*, bool isTimeout*/)> &&handler) noexcept override;
};

} // namespace
