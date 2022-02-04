// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Standard Libryary
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Microsoft::React {

struct IHttpResource {
  typedef std::unordered_map<std::string, std::string> Headers;

  struct BodyData {
    enum class Type : size_t { Empty, String, Base64, Uri, Form } Type = Type::Empty;
    std::string Data;
  };

  struct Response {
    int64_t StatusCode;
    Headers Headers;
    std::string Url;
  };

  static std::shared_ptr<IHttpResource> Make() noexcept;

  virtual ~IHttpResource() noexcept {}

  virtual void SendRequest(
      const std::string &method,
      const std::string &url,
      const Headers &headers,
      BodyData &&bodyData,
      const std::string &responseType,
      bool useIncrementalUpdates,
      int64_t timeout,
      bool withCredentials,
      std::function<void(int64_t)> &&callback) noexcept = 0;
  virtual void AbortRequest() noexcept = 0;

  virtual void ClearCookies() noexcept = 0;

  virtual void SetOnRequest(std::function<void(int64_t requestId)> &&handler) noexcept = 0;//TODO: Keep???
  virtual void SetOnResponse(std::function<void(int64_t requestId, Response &&response)> &&handler) noexcept = 0;
  virtual void SetOnData(std::function<void(int64_t requestId, std::string &&responseData)> &&handler) noexcept = 0;
  virtual void SetOnError(
      std::function<void(int64_t requestId, std::string &&message /*, bool isTimeout*/)> &&handler) noexcept = 0;
};

} // namespace Microsoft::React
