// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <type_traits>
#include <utility>

namespace Microsoft::Common::Utilities {

// A simple wrapper for reinterpret_casting from TOriginalTypePtr to
// TResultTypePtr that does a static_assert before performing the cast. The
// usage syntax for this function is the same as the syntax for doing a regular
// reinterpret_cast. For example:
//
//   uint32_t* i;
//   auto p = CheckedReinterpretCast<char32_t*>(i);
//
template <typename TResultTypePtr, typename TOriginalTypePtr>
inline TResultTypePtr CheckedReinterpretCast(TOriginalTypePtr p) noexcept {
  using TResultType = typename std::remove_pointer<TResultTypePtr>::type;
  using TOriginalType = typename std::remove_pointer<TOriginalTypePtr>::type;

  static_assert(
      std::is_pointer<TResultTypePtr>::value &&
          std::is_pointer<TOriginalTypePtr>::value &&
          std::is_integral<TResultType>::value &&
          std::is_integral<TOriginalType>::value &&
          sizeof(TResultType) == sizeof(TOriginalType),
      "CheckedReinterpretCast can only be used to cast from T1* to T2*, where "
      "T1 and T2 are integral types of the same size.");

  return reinterpret_cast<TResultTypePtr>(p);
}

// A compile time function that deduces the size of an array.
template <typename T, std::size_t N>
constexpr std::size_t ArraySize(T (&)[N]) noexcept {
  return N;
}

} // namespace Microsoft::Common::Utilities
