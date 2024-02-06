// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utilities.h"

// Boost Library
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
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

string DecodeBase64(string_view &&base64) noexcept {
  typedef array_view<char const> av_t;
  auto bytes = av_t(base64.data(), static_cast<av_t::size_type>(base64.size()));

  using namespace boost::archive::iterators;
  typedef transform_width<binary_from_base64<const char *>, 8, 6> decode_base64;
  std::ostringstream oss;
  std::copy(decode_base64(bytes.cbegin()), decode_base64(bytes.cend()), ostream_iterator<char>(oss));

  return oss.str();
}

string EncodeBase64(string_view &&text) noexcept {
  typedef array_view<char const> av_t;
  auto bytes = av_t(text.data(), static_cast<av_t::size_type>(text.size()));

  using namespace boost::archive::iterators;
  typedef base64_from_binary<transform_width<const char *, 6, 8>> encode_base64;
  std::ostringstream oss;
  std::copy(encode_base64(bytes.cbegin()), encode_base64(bytes.cend()), ostream_iterator<char>(oss));

  // https://unix.stackexchange.com/questions/631501
  auto padLength = (4 - (oss.tellp() % 4)) % 4;
  for (auto i = 0; i < padLength; ++i) {
    oss << '=';
  }

  return oss.str();
}

string EncodeBase64(wstring_view &&text) noexcept {
  // TODO: Confirm encoding
  auto buffer = CryptographicBuffer::ConvertStringToBinary(std::move(text), BinaryStringEncoding::Utf8);
  auto encoded = CryptographicBuffer::EncodeToBase64String(buffer);

  return winrt::to_string(encoded);
}

} // namespace Microsoft::React::Utilities
