// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Folly/dynamic.h>

namespace Microsoft::React {

struct IHttpResource {
  typedef std::map<std::string, std::string> Headers;

  static std::unique_ptr<IHttpResource> Make() noexcept;

  virtual ~IHttpResource() noexcept {}

  virtual void SendRequest(
      const std::string &method,
      const std::string &url,
      const Headers &headers,
      folly::dynamic bodyData, // ISS:2365799 - Make non-folly.
      const std::string &responseType,
      bool useIncrementalUpdates,
      std::int64_t timeout,
      std::function<void(int64_t)> &&callback) noexcept = 0;
  virtual void AbortRequest() noexcept = 0;
  virtual void ClearCookies() noexcept = 0;

  virtual void SetOnRequest(std::function<void()> &&handler) noexcept = 0;
  virtual void SetOnResponse(
      std::function<void(const std::string &)> &&handler) noexcept = 0;
  virtual void SetOnError(
      std::function<void(const std::string &)> &&handler) noexcept = 0;
};

} // namespace Microsoft::React
