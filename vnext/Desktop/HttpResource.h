// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IHttpResource.h>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http.hpp>

namespace Microsoft::React::Experimental {

class HttpResource : public IHttpResource {
  boost::asio::io_context m_context;
  boost::asio::ip::tcp::resolver m_resolver;
  boost::asio::ip::tcp::socket m_socket;

  // TODO: Remove?
  boost::beast::flat_buffer m_buffer;
  boost::beast::http::request<boost::beast::http::empty_body> m_request;
  boost::beast::http::response<boost::beast::http::string_body> m_response;

  std::function<void()> m_requestHandler;
  std::function<void(const std::string &)> m_responseHandler;
  std::function<void(const std::string &)> m_errorHandler;

 public:
  HttpResource() noexcept;

#pragma region IHttpResource members

  void SendRequest(
      const std::string &method,
      const std::string &url,
      const Headers &headers,
      folly::dynamic bodyData,
      const std::string &responseType,
      bool useIncrementalUpdates,
      std::int64_t timeout,
      std::function<void(int64_t)> &&callback) noexcept override;
  void AbortRequest() noexcept override;
  void ClearCookies() noexcept override;

  void SetOnRequest(std::function<void()> &&handler) noexcept override;
  void SetOnResponse(std::function<void(const std::string &)> &&handler) noexcept override;
  void SetOnError(std::function<void(const std::string &)> &&handler) noexcept override;

#pragma endregion
};

} // namespace Microsoft::React::Experimental
