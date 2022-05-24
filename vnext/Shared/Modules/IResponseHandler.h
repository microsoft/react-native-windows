#pragma once

// Folly
#include <folly/dynamic.h>

// Standard Library
#include <string>
#include <vector>

namespace Microsoft::React {

/// <summary>
/// Allows adding custom handling to build the JS body payload from the {@link ResponseBody}.
/// </summary>
struct IResponseHandler {
  /// <summary>
  /// Returns if the handler should be used for a response type.
  /// </summary>
  virtual bool Supports(std::string &responseType) = 0;

  /// <summary>
  /// Returns the JS body payload for the {@link ResponseBody}.
  /// </summary>
  virtual folly::dynamic ToResponseData(std::vector<uint8_t> &&content) = 0;
};

} // namespace Microsoft::React
