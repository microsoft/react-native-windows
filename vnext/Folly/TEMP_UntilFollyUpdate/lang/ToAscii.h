/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstring>

#include <folly/ConstexprMath.h>
#include <folly/Likely.h>
#include <folly/Portability.h>
#include <folly/Utility.h>
#include <folly/lang/Align.h>
#include <folly/lang/CArray.h>
#include <folly/portability/Builtins.h>

namespace folly {

//  to_ascii_alphabet
//
//  Used implicity by to_ascii_lower and to_ascii_upper below.
//
//  This alphabet translates digits to 0-9,a-z or 0-9,A-Z. The largest supported
//  base is 36; operator() presumes an argument less than that.
//
//  Alternative alphabets may be used with to_ascii_with provided they match
//  the constructibility/destructibility and the interface of this one.
template <bool Upper>
struct to_ascii_alphabet {
  //  operator()
  //
  //  Translates a single digit to 0-9,a-z or 0-9,A-Z.
  //
  //  async-signal-safe
  constexpr char operator()(uint8_t b) const {
    return b < 10 ? '0' + b : (Upper ? 'A' : 'a') + (b - 10);
  }
};
using to_ascii_alphabet_lower = to_ascii_alphabet<false>;
using to_ascii_alphabet_upper = to_ascii_alphabet<true>;

//  to_ascii_size_max
//  [Windows] - Replaced logic that required data with values just for the
//  bases required to compile.
//
//  The maximum size buffer that might be required to hold the ascii-encoded
//  representation of any value of unsigned type I in base Base.
//
//  In base 10, u64 requires at most 20 bytes, u32 at most 10, u16 at most 5,
//  and u8 at most 3.
/*
template <uint64_t Base, typename I>
FOLLY_INLINE_VARIABLE constexpr size_t to_ascii_size_max =
  detail::to_ascii_powers<Base, I>::size;
*/
//  to_ascii_size_max_decimal
//
//  An alias to to_ascii_size_max<10>.
template <typename I>
FOLLY_INLINE_VARIABLE constexpr size_t to_ascii_size_max_decimal;

template <>
FOLLY_INLINE_VARIABLE constexpr size_t to_ascii_size_max_decimal<uint16_t> = 5;
template <>
FOLLY_INLINE_VARIABLE constexpr size_t to_ascii_size_max_decimal<uint32_t> = 10;
template <>
FOLLY_INLINE_VARIABLE constexpr size_t to_ascii_size_max_decimal<uint64_t> = 20;


namespace detail {

// [Windows] Moved most of the detail namespace into the cpp file to avoid having data fields on the dll boundary

template <uint64_t Base>
FOLLY_ALWAYS_INLINE size_t to_ascii_size_route(uint64_t v);


template <uint64_t Base, typename Alphabet>
FOLLY_ALWAYS_INLINE size_t
to_ascii_with_route(char* outb, char const* oute, uint64_t v);

template <uint64_t Base, typename Alphabet, size_t N>
FOLLY_ALWAYS_INLINE size_t to_ascii_with_route(char (&out)[N], uint64_t v);

}

//  to_ascii_size
//
//  Returns the number of digits in the base Base representation of a uint64_t.
//  Useful for preallocating buffers, etc.
//
//  async-signal-safe
template <uint64_t Base>
size_t to_ascii_size(uint64_t v) {
  return detail::to_ascii_size_route<Base>(v);
}

//  to_ascii_size_decimal
//
//  An alias to to_ascii_size<10>.
//
//  async-signal-safe
inline size_t to_ascii_size_decimal(uint64_t v) {
  return to_ascii_size<10>(v);
}

//  to_ascii_with
//
//  Copies the digits of v, in base Base, translated with Alphabet, into buffer
//  and returns the number of bytes written.
//
//  Does *not* append a null terminator. It is the caller's responsibility to
//  append a null terminator if one is required.
//
//  Assumes buffer points to at least to_ascii_size<Base>(v) bytes of writable
//  memory. It is the caller's responsibility to provide a writable buffer with
//  the required min size.
//
//  async-signal-safe
template <uint64_t Base, typename Alphabet>
size_t to_ascii_with(char* outb, char const* oute, uint64_t v) {
  return detail::to_ascii_with_route<Base, Alphabet>(outb, oute, v);
}
template <uint64_t Base, typename Alphabet, size_t N>
size_t to_ascii_with(char (&out)[N], uint64_t v) {
  return detail::to_ascii_with_route<Base, Alphabet>(out, v);
}

//  to_ascii_lower
//
//  Composes to_ascii_with with to_ascii_alphabet_lower.
//
//  async-signal-safe
template <uint64_t Base>
size_t to_ascii_lower(char* outb, char const* oute, uint64_t v) {
  return to_ascii_with<Base, to_ascii_alphabet_lower>(outb, oute, v);
}
template <uint64_t Base, size_t N>
size_t to_ascii_lower(char (&out)[N], uint64_t v) {
  return to_ascii_with<Base, to_ascii_alphabet_lower>(out, v);
}

//  to_ascii_upper
//
//  Composes to_ascii_with with to_ascii_alphabet_upper.
//
//  async-signal-safe
template <uint64_t Base>
size_t to_ascii_upper(char* outb, char const* oute, uint64_t v) {
  return to_ascii_with<Base, to_ascii_alphabet_upper>(outb, oute, v);
}
template <uint64_t Base, size_t N>
size_t to_ascii_upper(char (&out)[N], uint64_t v) {
  return to_ascii_with<Base, to_ascii_alphabet_upper>(out, v);
}

//  to_ascii_decimal
//
//  An alias to to_ascii<10, false>.
//
//  async-signals-afe
inline size_t to_ascii_decimal(char* outb, char const* oute, uint64_t v) {
  return to_ascii_lower<10>(outb, oute, v);
}
template <size_t N>
inline size_t to_ascii_decimal(char (&out)[N], uint64_t v) {
  return to_ascii_lower<10>(out, v);
}

} // namespace folly