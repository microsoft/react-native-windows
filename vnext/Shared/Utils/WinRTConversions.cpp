// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WinRTConversions.h"

// Standard Library
#include <sstream>

namespace Microsoft::React::Utilities {

std::string HResultToString(winrt::hresult_error const &e) {
  std::stringstream stream;
  stream << "[0x" << std::hex << e.code() << "] " << winrt::to_string(e.message());

  return stream.str();
}

std::string HResultToString(winrt::hresult &&result) {
  return HResultToString(winrt::hresult_error(std::move(result), winrt::hresult_error::from_abi));
}

} // namespace Microsoft::React::Utilities
