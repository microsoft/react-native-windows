// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IHttpResource.h"

#include <Modules/IHttpModuleProxy.h>
#include "IWinRTHttpRequestFactory.h"
#include "WinRTTypes.h"

// Windows API
#include <winrt/Windows.Web.Http.h>

// Standard Library
#include <mutex>

namespace Microsoft::React::Networking {

class WinRTHttpResource : public IHttpResource,
                          public IHttpModuleProxy,
                          public IWinRTHttpRequestFactory,
                          public std::enable_shared_from_this<WinRTHttpResource> {
  winrt::Windows::Web::Http::IHttpClient m_client;
  std::mutex m_mutex;
  std::unordered_map<int64_t, ResponseOperation> m_responses;

  std::function<void(int64_t requestId)> m_onRequestSuccess;
  std::function<void(int64_t requestId, Response &&response)> m_onResponse;
  std::function<void(int64_t requestId, std::string &&responseData)> m_onData;
  std::function<void(int64_t requestId, folly::dynamic &&responseData)> m_onDataDynamic;
  std::function<void(int64_t requestId, std::string &&errorMessage, bool isTimeout)> m_onError;
  std::function<void(int64_t requestId, std::string &&responseData, int64_t progress, int64_t total)>
      m_onIncrementalData;
  std::function<void(int64_t requestId, int64_t progress, int64_t total)> m_onDataProgress;
  std::function<void(int64_t requestId)> m_onComplete;

  // Used for IHttpModuleProxy
  std::weak_ptr<IUriHandler> m_uriHandler;
  std::weak_ptr<IRequestBodyHandler> m_requestBodyHandler;
  std::weak_ptr<IResponseHandler> m_responseHandler;

  void TrackResponse(int64_t requestId, ResponseOperation response) noexcept;

  void UntrackResponse(int64_t requestId) noexcept;

  winrt::fire_and_forget PerformSendRequest(
      winrt::Windows::Web::Http::HttpMethod &&method,
      winrt::Windows::Foundation::Uri &&uri,
      winrt::Windows::Foundation::IInspectable const &args) noexcept;

 public:
  WinRTHttpResource() noexcept;

  WinRTHttpResource(winrt::Windows::Web::Http::IHttpClient &&client) noexcept;

#pragma region IWinRTHttpRequestFactory

  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Web::Http::HttpRequestMessage> CreateRequest(
      winrt::Windows::Web::Http::HttpMethod &&method,
      winrt::Windows::Foundation::Uri &&uri,
      winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::Windows::Foundation::IInspectable>
          props) noexcept override;

#pragma endregion IWinRTHttpRequestFactory

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

  void SetOnRequestSuccess(std::function<void(int64_t requestId)> &&handler) noexcept override;
  void SetOnResponse(std::function<void(int64_t requestId, Response &&response)> &&handler) noexcept override;
  void SetOnData(std::function<void(int64_t requestId, std::string &&responseData)> &&handler) noexcept override;
  void SetOnData(std::function<void(int64_t requestId, folly::dynamic &&responseData)> &&handler) noexcept override;
  void SetOnIncrementalData(
      std::function<void(int64_t requestId, std::string &&responseData, int64_t progress, int64_t total)>
          &&handler) noexcept override;
  void SetOnDataProgress(
      std::function<void(int64_t requestId, int64_t progress, int64_t total)> &&handler) noexcept override;
  void SetOnResponseComplete(std::function<void(int64_t requestId)> &&handler) noexcept override;
  void SetOnError(
      std::function<void(int64_t requestId, std::string &&errorMessage, bool isTimeout)> &&handler) noexcept override;

#pragma endregion IHttpResource

#pragma region IHttpModuleProxy

  void AddUriHandler(std::shared_ptr<IUriHandler> uriHandler) noexcept override;

  void AddRequestBodyHandler(std::shared_ptr<IRequestBodyHandler> requestBodyHandler) noexcept override;

  void AddResponseHandler(std::shared_ptr<IResponseHandler> responseHandler) noexcept override;

#pragma endregion IHttpModuleProxy
};

} // namespace Microsoft::React::Networking
