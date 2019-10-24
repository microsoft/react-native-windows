// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/JSBigString.h>
#include "AbiSafe.h"

namespace facebook {
namespace react {

// JSBigString implementation that wraps up AbiString.
struct JSBigAbiString final : JSBigString {
  // Use Make method to ensure JSBigAbiString creation and deletion in the same
  // DLL.
  static std::unique_ptr<const JSBigAbiString> Make(AbiSafe::AbiStringPtr &&abiString) noexcept;

  JSBigAbiString(AbiSafe::AbiStringPtr &&abiString) noexcept;

  bool isAscii() const override;
  const char *c_str() const override;
  size_t size() const override;

 private:
  void EnsureStringLoaded() const noexcept;

 private:
  AbiSafe::AbiStringPtr m_abiString;
  AbiSafe::AbiCharSpan m_charSpan{};
};

} // namespace react
} // namespace facebook
