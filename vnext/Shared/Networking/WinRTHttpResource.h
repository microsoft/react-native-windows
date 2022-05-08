// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IHttpResource.h"

#include "WinRTTypes.h"

// Windows API
#include <winrt/Windows.Web.Http.h>

// Standard Library
#include <mutex>

namespace Microsoft::React::Networking {

class WinRTHttpResource : public IHttpResource, public std::enable_shared_from_this<WinRTHttpResource> {
  winrt::Windows::Web::Http::IHttpClient m_client;
  std::mutex m_mutex;
  std::unordered_map<int64_t, ResponseOperation> m_responses;
  winrt::Windows::Foundation::IInspectable m_inspectableProperties;

  std::function<void(int64_t requestId)> m_onRequest;
  std::function<void(int64_t requestId, Response &&response)> m_onResponse;
  std::function<void(int64_t requestId, std::string &&responseData)> m_onData;
  std::function<void(int64_t requestId, std::string &&errorMessage /*, bool isTimeout*/)> m_onError;

  void TrackResponse(int64_t requestId, ResponseOperation response) noexcept;

  void UntrackResponse(int64_t requestId) noexcept;

  winrt::fire_and_forget PerformSendRequest(
      winrt::Windows::Web::Http::HttpRequestMessage &&request,
      winrt::Windows::Foundation::IInspectable const &args) noexcept;

 public:
  WinRTHttpResource() noexcept;

  WinRTHttpResource(winrt::Windows::Web::Http::IHttpClient &&client) noexcept;

  WinRTHttpResource(
      winrt::Windows::Web::Http::IHttpClient &&client,
      winrt::Windows::Foundation::IInspectable inspectableProperties) noexcept;

#pragma region IHttpResource

  void SendRequest(
      std::string &&method,
      std::string &&url,
      int64_t requestId,
      Headers &&headers,
      folly::dynamic &&data,
      std::string &&responseType,
      bool useIncrementalUpdates,
      int64_t timeout,
      bool withCredentials,
      std::function<void(int64_t)> &&callback) noexcept override;
  void AbortRequest(int64_t requestId) noexcept override;
  void ClearCookies() noexcept override;

#pragma endregion IHttpResource

  void SetOnRequest(std::function<void(int64_t requestId)> &&handler) noexcept override;
  void SetOnResponse(std::function<void(int64_t requestId, Response &&response)> &&handler) noexcept override;
  void SetOnData(std::function<void(int64_t requestId, std::string &&responseData)> &&handler) noexcept override;
  void SetOnError(std::function<void(int64_t requestId, std::string &&errorMessage /*, bool isTimeout*/)>
                      &&handler) noexcept override;
};

} // namespace Microsoft::React::Networking
