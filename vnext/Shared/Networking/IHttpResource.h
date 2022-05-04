// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Folly
#include <folly/dynamic.h>

// Standard Library
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Microsoft::React::Networking {

struct IHttpResource {
  typedef std::unordered_map<std::string, std::string> Headers;

  // TODO: Implement Form data
  struct BodyData {
    enum class Type : size_t { Empty, String, Base64, Uri, Form } Type = Type::Empty;
    std::string Data;
  };

  struct Response {
    int64_t StatusCode;
    std::string Url;
    Headers Headers;
  };

  static std::shared_ptr<IHttpResource> Make() noexcept;

  virtual ~IHttpResource() noexcept {}

  /// <summary>
  /// Initiates an HTTP request.
  /// </summary>
  /// <param name="method">
  /// HTTP verb to send in de request.
  /// GET | POST | PUT | DELETE | OPTIONS
  /// </param>
  /// <param name="url">
  /// Server/service remote endpoint to send the request to.
  /// </param>
  /// <param name="requestId">
  /// Request unique identifier.
  /// </param>
  /// <param name="headers">
  /// HTTP request header map.
  /// </param>
  /// <param name="data">
  /// Dynamic map containing request payload.
  /// The payload may be an empty request body or one of the following:
  /// "string"  - UTF-8 string payload
  /// "base64"  - Base64-encoded data string
  /// "uri"     - URI data reference
  /// "form"    - Form-encoded data
  /// </param>
  /// <param name="responseType">
  /// text | binary | blob
  /// </param>
  /// <param name="useIncrementalUpdates">
  /// Response body to be retrieved in several iterations.
  /// </param>
  /// <param name="timeout">
  /// Request timeout in miliseconds.
  /// </param>
  /// <param name="withCredentials">
  /// Allow including credentials in request.
  /// </param>
  virtual void SendRequest(
      std::string &&method,
      std::string &&url,
      int64_t requestId,
      Headers &&headers,
      folly::dynamic &&data,
      std::string &&responseType,
      bool useIncrementalUpdates,
      int64_t timeout,
      bool withCredentials,
      std::function<void(int64_t)> &&callback) noexcept = 0;
  virtual void AbortRequest(int64_t requestId) noexcept = 0;

  virtual void ClearCookies() noexcept = 0;

  virtual void SetOnRequest(std::function<void(int64_t requestId)> &&handler) noexcept = 0;
  virtual void SetOnResponse(std::function<void(int64_t requestId, Response &&response)> &&handler) noexcept = 0;
  virtual void SetOnData(std::function<void(int64_t requestId, std::string &&responseData)> &&handler) noexcept = 0;
  virtual void SetOnError(
      std::function<void(int64_t requestId, std::string &&errorMessage /*, bool isTimeout*/)> &&handler) noexcept = 0;
};

} // namespace Microsoft::React::Networking
