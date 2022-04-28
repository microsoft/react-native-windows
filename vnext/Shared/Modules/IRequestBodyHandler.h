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
  virtual bool Supports(folly::dynamic &data) = 0;

  /// <summary>
  /// Returns the {@link RequestBody} for the JS body payload.
  /// </summary>
  virtual void * /*RequestBody*/ ToRequestBody(folly::dynamic &data, std::string &contentType) = 0;
};

} // namespace Microsoft::React
