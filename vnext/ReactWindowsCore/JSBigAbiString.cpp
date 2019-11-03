// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "JSBigAbiString.h"

namespace facebook {
namespace react {

/*static*/ std::unique_ptr<const JSBigAbiString> JSBigAbiString::Make(AbiSafe::AbiStringPtr &&abiString) noexcept {
  return std::make_unique<const JSBigAbiString>(std::move(abiString));
}

JSBigAbiString::JSBigAbiString(AbiSafe::AbiStringPtr &&abiString) noexcept : m_abiString{std::move(abiString)} {}

bool JSBigAbiString::isAscii() const {
  return false;
}

const char *JSBigAbiString::c_str() const {
  EnsureStringLoaded();
  return m_charSpan.Data;
}

size_t JSBigAbiString::size() const {
  EnsureStringLoaded();
  return m_charSpan.Size;
}

void JSBigAbiString::EnsureStringLoaded() const noexcept {
  if (!m_charSpan.Data) {
    const_cast<JSBigAbiString *>(this)->m_charSpan = m_abiString->GetSpan();
  }
}

} // namespace react
} // namespace facebook
