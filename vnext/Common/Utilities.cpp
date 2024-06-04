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
#include <sstream>

using std::string;
using std::string_view;
using std::wstring_view;
using winrt::array_view;

using winrt::Windows::Security::Cryptography::BinaryStringEncoding;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace Microsoft::React::Utilities {

string DecodeBase64(string_view base64) noexcept {
  typedef array_view<char const> av_t;
  auto bytes = av_t(base64.data(), static_cast<av_t::size_type>(base64.size()));

  using namespace boost::archive::iterators;
  typedef transform_width<binary_from_base64<const char *>, 8, 6> decode_base64;
  std::ostringstream oss;
  std::copy(decode_base64(bytes.cbegin()), decode_base64(bytes.cend()), ostream_iterator<char>(oss));

  return oss.str();
}

// https://www.boost.org/doc/libs/1_76_0/libs/serialization/doc/dataflow.html
string EncodeBase64(string_view text) noexcept {
  typedef array_view<char const> av_t;
  auto bytes = av_t(text.data(), static_cast<av_t::size_type>(text.size()));

  using namespace boost::archive::iterators;
  typedef base64_from_binary<transform_width<const char *, 6, 8>> encode_base64;
  std::ostringstream oss;
  std::copy(encode_base64(bytes.cbegin()), encode_base64(bytes.cend()), ostream_iterator<char>(oss));

  // https://unix.stackexchange.com/questions/631501
  size_t padLength = (4 - (oss.tellp() % 4)) % 4;
  for (size_t i = 0; i < padLength; ++i) {
    oss << '=';
  }

  return oss.str();
}

} // namespace Microsoft::React::Utilities
