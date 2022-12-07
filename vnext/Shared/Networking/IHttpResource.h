// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Folly
#include <folly/dynamic.h>

// Windows API
#include <winrt/Windows.Foundation.h>

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

  static std::shared_ptr<IHttpResource> Make(
      winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept;

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
  /// Note: A value of 0 means no timeout. The resource will await the response indefinitely.
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

  /// <summary>
  /// Sets a function to be invoked when a request has been successfully responded.
  /// </summary>
  /// <param name="handler">
  ///
  /// Parameters:
  ///   <param name="requestId">
  ///   Unique number identifying the HTTP request
  ///   </param>
  /// </param>
  virtual void SetOnRequestSuccess(std::function<void(int64_t requestId)> &&handler) noexcept = 0;

  /// <summary>
  /// Sets a function to be invoked when a response arrives and its headers are received.
  /// </summary>
  /// <param name="handler">
  ///
  /// Parameters:
  ///   <param name="requestId">
  ///   Unique number identifying the HTTP request
  ///   </param>
  ///   <param name="response">
  ///   Object containing basic response data
  ///   </param>
  /// </param>
  virtual void SetOnResponse(std::function<void(int64_t requestId, Response &&response)> &&handler) noexcept = 0;

  /// <summary>
  /// Sets a function to be invoked when response content data has been received.
  /// </summary>
  /// <param name="handler">
  ///
  /// Parameters:
  ///   <param name="requestId">
  ///   Unique number identifying the HTTP request
  ///   </param>
  ///   <param name="responseData">
  ///   Response content payload (plain text or Base64-encoded)
  ///   </param>
  /// </param>
  virtual void SetOnData(std::function<void(int64_t requestId, std::string &&responseData)> &&handler) noexcept = 0;

  /// <summary>
  /// Sets a function to be invoked when response content data has been received.
  /// </summary>
  /// <param name="handler">
  ///
  /// Parameters:
  ///   <param name="requestId">
  ///   Unique number identifying the HTTP request
  ///   </param>
  ///   <param name="responseData">
  ///   Structured response content payload (i.e. Blob data)
  ///   </param>
  /// </param>
  virtual void SetOnData(std::function<void(int64_t requestId, folly::dynamic &&responseData)> &&handler) noexcept = 0;

  /// <summary>
  /// Sets a function to be invoked when a response content increment has been received.
  /// </summary>
  /// <remarks>
  /// The handler set by this method will only be called if the request sets the incremental updates flag.
  /// The handler is also mutually exclusive with those set by `SetOnData`, which are used for one pass, non-incremental
  /// updates.
  /// </remarks>
  /// <param name="handler">
  ///
  /// Parameters:
  ///   <param name="requestId">
  ///   Unique number identifying the HTTP request
  ///   </param>
  ///   <param name="responseData">
  ///   Partial response content data increment (non-accumulative)
  ///   </param>
  ///   <param name="progress">
  ///   Number of bytes received so far
  ///   </param>
  ///   <param name="total">
  ///   Number of total bytes to receive
  ///   </param>
  /// </param>
  virtual void SetOnIncrementalData(
      std::function<void(int64_t requestId, std::string &&responseData, int64_t progress, int64_t total)>
          &&handler) noexcept = 0;

  /// <summary>
  /// Sets a function to be invoked when response content download progress is reported.
  /// </summary>
  /// <param name="handler">
  ///
  /// Parameters:
  ///   <param name="requestId">
  ///   Unique number identifying the HTTP request
  ///   </param>
  ///   <param name="progress">
  ///   Number of bytes received so far
  ///   </param>
  ///   <param name="total">
  ///   Number of total bytes to receive
  ///   </param>
  /// </param>
  virtual void SetOnDataProgress(
      std::function<void(int64_t requestId, int64_t progress, int64_t total)> &&handler) noexcept = 0;

  /// <summary>
  /// Sets a function to be invoked when a response has been fully handled (either succeeded or failed).
  /// </summary>
  /// <param name="handler">
  ///
  /// Parameters:
  ///   <param name="requestId">
  ///   Unique number identifying the HTTP request
  ///   </param>
  /// </param>
  virtual void SetOnResponseComplete(std::function<void(int64_t requestId)> &&handler) noexcept = 0;

  /// <summary>
  /// Sets a function to be invoked when an error condition is found.
  /// </summary>
  /// <remarks>
  /// The handler's purpose is not to report any given HTTP error status (i.e. 403, 501).
  /// It is meant to report application errors when executing HTTP requests.
  /// </remarks>
  /// <param name="handler">
  ///
  /// Parameters:
  ///   <param name="requestId">
  ///   Unique number identifying the HTTP request
  ///   </param>
  /// </param>
  virtual void SetOnError(
      std::function<void(int64_t requestId, std::string &&errorMessage, bool isTimeout)> &&handler) noexcept = 0;
};

} // namespace Microsoft::React::Networking
