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
  /// Returns whether the handler should be used for a given URI.
  /// </summary>
  virtual bool Supports(std::string &uri, std::string &responseType) = 0;

  /// <summary>
  /// Fetch the URI and return the JS body payload.
  /// </summary>
  /// <returns>
  /// Blob representation in a dynamic object with the folliwing structure:
  /// "blobId"      - Blob unique identifier
  /// "offset"      - Blob segment starting offset
  /// "size"        - Number of bytes fetched from blob
  /// "name"        - File name obtained from the URI
  /// "lastModified - Last write to local file in milliseconds
  virtual folly::dynamic Fetch(std::string &uri) = 0;
};

} // namespace Microsoft::React
