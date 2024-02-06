// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utilities.h"

// Boost Library
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/transform_width.hpp>

// Windows API
#include <winrt/Windows.Security.Cryptography.h>

// Standard Library
using std::string;
using std::string_view;
using std::wstring_view;
using winrt::array_view;

using winrt::Windows::Security::Cryptography::BinaryStringEncoding;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace Microsoft::React::Utilities {

string_view DecodeBase64(string_view&& text) noexcept
{
  return {};
}

string_view DecodeBase64(wstring_view&& text) noexcept
{
  return {};
}

string EncodeBase64(string_view&& text) noexcept
{
  /*
  // https://unix.stackexchange.com/questions/631501
  auto padLength = 4 - (oss.tellp() % 4);
  for (auto i = 0; i < padLength; ++i) {
    result += '=';
  }
  */
  typedef array_view<char const> av_t;
  auto bytes = av_t(text.data(), static_cast<av_t::size_type>(text.size()));

  using namespace boost::archive::iterators;
  typedef base64_from_binary<transform_width<const char*, 6, 8>> encode_base64;
  std::ostringstream oss;
  std::copy(encode_base64(bytes.cbegin()), encode_base64(bytes.cend()), ostream_iterator<char>(oss));

  return oss.str();
}

string EncodeBase64(wstring_view&& text) noexcept
{
  //TODO: Confirm encoding
  auto buffer = CryptographicBuffer::ConvertStringToBinary(std::move(text), BinaryStringEncoding::Utf8);
  auto encoded = CryptographicBuffer::EncodeToBase64String(buffer);

  return winrt::to_string(encoded);
}

} // namespace Microsoft::React::Utilities
