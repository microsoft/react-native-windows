// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "HttpResource.h"

#include <Utils.h>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/version.hpp>

using namespace boost::asio::ip;
using namespace boost::beast::http;

using folly::dynamic;
using std::make_unique;
using std::string;
using std::unique_ptr;

using boostecr = boost::system::error_code const &;

namespace facebook {
namespace react {
namespace experimental {

#pragma region HttpResource members

HttpResource::HttpResource() noexcept
    : m_resolver{m_context}, m_socket{m_context} {}

void HttpResource::SendRequest(
    const string &method,
    const string &urlString,
    const Headers &headers,
    dynamic bodyData,
    const string &responseType,
    bool useIncrementalUpdates,
    int64_t timeout,
    std::function<void(int64_t)> &&callback) noexcept {
  // Enforce supported args
  assert(responseType == "text" || responseType == "base64");
  assert(!useIncrementalUpdates);

  // ISS:2306365 - Callback with the requestId

  // Validate verb.
  unique_ptr<Url> url;
  try {
    url = make_unique<Url>(urlString);
  } catch (...) {
    m_errorHandler("Malformed URL");
    return;
  }
  request<string_body> req;
  req.version(11 /*HTTP 1.1*/);
  req.method(string_to_verb(method));
  req.target(url->Target());
  req.set(field::host, url->host); // ISS:2306365 - Determine/append port.
  req.set(field::user_agent, BOOST_BEAST_VERSION_STRING);

  for (const auto &header : headers) {
    // ISS:2306365 - Deal with content-type, content-encoding?
    req.set(header.first, header.second);
  }

  if (!bodyData.empty()) {
    if (!bodyData["string"].empty()) {
    } else if (!bodyData["base64"].empty()) {
    } else if (!bodyData["uri"].empty()) {
      assert(false); // Not implemented.
    } else {
      // Empty request
    }
  }

  m_context.restart();

  // Send/Receive request.
  m_resolver.async_resolve(
      url->host,
      url->port,
      [this, &req](boostecr ec, tcp::resolver::results_type results) {
        if (ec) {
          if (m_errorHandler)
            m_errorHandler(ec.message());
        } else {
          boost::asio::async_connect(
              m_socket,
              results.begin(),
              results.end(),
              [this, &req](boostecr ec, const basic_resolver_iterator<tcp> &) {
                if (ec) {
                  if (m_errorHandler)
                    m_errorHandler(ec.message());
                } else {
                  async_write(
                      m_socket, move(req), [this](boostecr ec, size_t size) {
                        if (ec) {
                          if (m_errorHandler)
                            m_errorHandler(ec.message());
                        } else {
                          if (m_requestHandler)
                            m_requestHandler();

                          async_read(
                              m_socket,
                              m_buffer,
                              m_response,
                              [this](boostecr ec, size_t size) {
                                if (ec) {
                                  if (m_errorHandler)
                                    m_errorHandler(ec.message());
                                } else {
                                  if (m_responseHandler)
                                    m_responseHandler(
                                        boost::beast::buffers_to_string(
                                            m_buffer.data()));

                                  boost::system::error_code bec;
                                  m_socket.shutdown(
                                      tcp::socket::shutdown_both, bec);
                                  if (bec &&
                                      boost::system::errc::not_connected !=
                                          bec) // not_connected may happen. Not
                                               // an actual error.
                                  {
                                    if (m_errorHandler)
                                      m_errorHandler(bec.message());

                                    // ISS:2306365 - Callback?
                                  }
                                }
                              }); // async_read
                        }
                      }); // async_write
                }
              }); // async_connect
        }
      }); // async_resolve

  m_context.run();
}

void HttpResource::AbortRequest() noexcept {
  m_context.stop();

  if (m_socket.is_open()) {
    boost::system::error_code bec;
    m_socket.shutdown(tcp::socket::shutdown_both, bec);
    if (bec && boost::system::errc::not_connected != bec) {
      if (m_errorHandler)
        m_errorHandler(bec.message());
    }
  }
}

void HttpResource::ClearCookies() noexcept {
  assert(false); // Not yet implemented.
}

#pragma region Handler setters

void HttpResource::SetOnRequest(std::function<void()> &&handler) noexcept {
  m_requestHandler = move(handler);
}

void HttpResource::SetOnResponse(
    std::function<void(const std::string &)> &&handler) noexcept {
  m_responseHandler = move(handler);
}

void HttpResource::SetOnError(
    std::function<void(const std::string &)> &&handler) noexcept {
  m_errorHandler = move(handler);
}

#pragma endregion Handler setters

#pragma endregion HttpResource members

} // namespace experimental

#pragma region IHttpResource static members

/*static*/ unique_ptr<IHttpResource> IHttpResource::Make() noexcept {
  return unique_ptr<IHttpResource>(new experimental::HttpResource());
}

#pragma endregion IHttpResource static members

} // namespace react
} // namespace facebook
