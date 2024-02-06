// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utilities.h"

using std::string_view;
using std::wstring_view;

namespace Microsoft::React {

constexpr string_view DecodeBase64(string_view&& text) noexcept
{
  return {};
}

constexpr string_view DecodeBase64(wstring_view&& text) noexcept
{
  /*
  * From ImageUtils.cpp
    std::string_view base64String(source.uri.c_str() + start + 1, source.uri.length() - start - 1);
    auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(
        winrt::to_hstring(base64String));

  */

  return {};
}

constexpr string_view EncodeBase64(string_view&& text) noexcept
{
  return {};
}

constexpr string_view EncodeBase64(wstring_view&& text) noexcept
{
  return {};
}

} // namespace Microsoft::React
