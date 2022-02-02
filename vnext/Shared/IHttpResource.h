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

  virtual void SetOnRequest(std::function<void()> &&handler) noexcept = 0;
  virtual void SetOnResponse(std::function<void(const std::string &)> &&handler) noexcept = 0;
  virtual void SetOnError(std::function<void(const std::string &)> &&handler) noexcept = 0;
};

} // namespace Microsoft::React
