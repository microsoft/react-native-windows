// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Standard Library
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Microsoft::React::Networking {

struct IHttpResource {
  typedef std::unordered_map<std::string, std::string> Headers;

  struct BodyData {
    enum class Type : size_t { Empty, String, Base64, Uri, Form } Type = Type::Empty;
    std::string Data;
  };

  struct Response {
    int64_t StatusCode;
    std::string Url;
    Headers Headers;
    // TODO: Content?
  };

  static std::shared_ptr<IHttpResource> Make() noexcept;

  virtual ~IHttpResource() noexcept {}

  virtual void SendRequest(
      std::string &&method,
      std::string &&url,
      Headers &&headers,
      BodyData &&bodyData,
      std::string &&responseType,
      bool useIncrementalUpdates,
      int64_t timeout,
      bool withCredentials,
      std::function<void(int64_t)> &&callback) noexcept = 0;
  virtual void AbortRequest(int64_t requestId) noexcept = 0;

  virtual void ClearCookies() noexcept = 0;

  virtual void SetOnRequest(std::function<void(int64_t requestId)> &&handler) noexcept = 0; // TODO: Keep???
  virtual void SetOnResponse(std::function<void(int64_t requestId, Response &&response)> &&handler) noexcept = 0;
  virtual void SetOnData(std::function<void(int64_t requestId, std::string &&responseData)> &&handler) noexcept = 0;
  virtual void SetOnError(
      std::function<void(int64_t requestId, std::string &&errorMessage /*, bool isTimeout*/)> &&handler) noexcept = 0;
};

} // namespace Microsoft::React
