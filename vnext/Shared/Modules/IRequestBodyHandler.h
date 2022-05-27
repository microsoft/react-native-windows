// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Folly
#include <folly/dynamic.h>

// Standard Library
#include <string>

namespace Microsoft::React {

/// <summary>
/// Allows adding custom handling to build the {@link RequestBody} from the JS body payload.
/// </summary>
struct IRequestBodyHandler {
  /// <summary>
  /// Returns if the handler should be used for a JS body payload.
  /// </summary>
  /// <param name="data">
  /// folly object potentially containing a blob reference.
  /// "blob"  - folly object holding blob metadata. Optional.
  /// </param>
  /// <returns>
  /// true  - <paramref name="data" /> contains a blob reference.
  /// false - <paramref name="data" /> does not contain a blob reference.
  /// </returns>
  virtual bool Supports(folly::dynamic &data) = 0;

  /// <summary>
  /// Returns the {@link RequestBody} for the JS body payload.
  /// </summary>
  /// <param name="data">
  /// Incoming folly object containing the blob metadada.
  /// Structure:
  /// "blob"  - folly object info
  ///   "blobId"  - Blob unique identifier
  ///   "offset"  - Start index to read the blob
  ///   "size"    - Amount of bytes to read from the blob
  /// </param>
  /// <returns>
  /// folly::dynamic object with the following entries:
  /// "type"  - Request content type
  /// "size"  - Amount of bytes
  /// "bytes" - Raw body content
  ///           NOTE: This is an arbitrary key. Pending non-folly structured object to model request body.
  /// </returns>
  virtual folly::dynamic ToRequestBody(folly::dynamic &data, std::string &contentType) = 0;
};

} // namespace Microsoft::React
