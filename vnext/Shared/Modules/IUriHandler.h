// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Folly
#include <folly/dynamic.h>

// Standard Library
#include <string>

namespace Microsoft::React {

/// <summary>
/// Allows to implement a custom fetching process for specific URIs. It is the handler's job to
/// fetch the URI and return the JS body payload.
/// </summary>
struct IUriHandler {
  /// <summary>
  /// Returns if the handler should be used for an URI.
  /// </summary>
  virtual bool Supports(std::string &uri, std::string &responseType) = 0;

  /// <summary>
  /// Fetch the URI and return the JS body payload.
  /// </summary>
  virtual folly::dynamic Fetch(std::string &uri) = 0;
};

} // namespace Microsoft::React
