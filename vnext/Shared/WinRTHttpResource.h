// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IHttpResource.h>

// Windows API
#include <winrt/Windows.Web.Http.h>
// Standard Library

namespace Microsoft::React {

class WinRTHttpResource : public IHttpResource, public std::enable_shared_from_this<WinRTHttpResource> {

  //TODO: Make non-trivial args r-value??
  winrt::fire_and_forget PerformSendRequest(/*TODO: shared self?,*/
    winrt::Windows::Web::Http::HttpClient client,
    winrt::Windows::Web::Http::HttpRequestMessage,
    bool textResponse
    /*, requestId?*/) noexcept;

public:
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
  void AbortRequest() noexcept override;

  void ClearCookies() noexcept override;

  void SetOnRequest(std::function<void()> &&handler) noexcept override;
  void SetOnResponse(std::function<void(const std::string &)> &&handler) noexcept override;
  void SetOnError(std::function<void(const std::string &)> &&handler) noexcept override;
};

} // namespace
