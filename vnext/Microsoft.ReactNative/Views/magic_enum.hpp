//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.8.1
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2022 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef NEARGYE_MAGIC_ENUM_HPP
#define NEARGYE_MAGIC_ENUM_HPP

#define MAGIC_ENUM_VERSION_MAJOR 0
#define MAGIC_ENUM_VERSION_MINOR 8
#define MAGIC_ENUM_VERSION_PATCH 1

#include <array>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <limits>
#include <type_traits>
#include <utility>
#include <variant>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunknown-warning-option"
#  pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wmaybe-uninitialized" // May be used uninitialized 'return {};'.
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 26495) // Variable 'static_string<N>::chars_' is uninitialized.
#  pragma warning(disable : 28020) // Arithmetic overflow: Using operator '-' on a 4 byte value and then casting the result to a 8 byte value.
#  pragma warning(disable : 26451) // The expression '0<=_Param_(1)&&_Param_(1)<=1-1' is not true at this call.
#  pragma warning(disable : 4514) // Unreferenced inline function has been removed.
#endif

// Checks magic_enum compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1910
#  undef  MAGIC_ENUM_SUPPORTED
#  define MAGIC_ENUM_SUPPORTED 1
#endif

// Checks magic_enum compiler aliases compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  undef  MAGIC_ENUM_SUPPORTED_ALIASES
#  define MAGIC_ENUM_SUPPORTED_ALIASES 1
#endif

// Enum value must be greater or equals than MAGIC_ENUM_RANGE_MIN. By default MAGIC_ENUM_RANGE_MIN = -128.
// If need another min range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MIN.
#if !defined(MAGIC_ENUM_RANGE_MIN)
#  define MAGIC_ENUM_RANGE_MIN -128
#endif

// Enum value must be less or equals than MAGIC_ENUM_RANGE_MAX. By default MAGIC_ENUM_RANGE_MAX = 128.
// If need another max range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MAX.
#if !defined(MAGIC_ENUM_RANGE_MAX)
#  define MAGIC_ENUM_RANGE_MAX 128
#endif

namespace magic_enum {

using std::optional;
using std::string_view;
using std::string;

namespace customize {
// Enum value must be in range [MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX]. By default MAGIC_ENUM_RANGE_MIN = -128, MAGIC_ENUM_RANGE_MAX = 128.
// If need another range for all enum types by default, redefine the macro MAGIC_ENUM_RANGE_MIN and MAGIC_ENUM_RANGE_MAX.
// If need another range for specific enum type, add specialization enum_range for necessary enum type.
template <typename E>
struct enum_range {
  static_assert(std::is_enum_v<E>, "magic_enum::customize::enum_range requires enum type.");
  static constexpr int min = MAGIC_ENUM_RANGE_MIN;
  static constexpr int max = MAGIC_ENUM_RANGE_MAX;
  static_assert(max > min, "magic_enum::customize::enum_range requires max > min.");
};

static_assert(MAGIC_ENUM_RANGE_MAX > MAGIC_ENUM_RANGE_MIN, "MAGIC_ENUM_RANGE_MAX must be greater than MAGIC_ENUM_RANGE_MIN.");
static_assert((MAGIC_ENUM_RANGE_MAX - MAGIC_ENUM_RANGE_MIN) < (std::numeric_limits<std::uint16_t>::max)(), "MAGIC_ENUM_RANGE must be less than UINT16_MAX.");

namespace detail {
enum class default_customize_tag {};
enum class invalid_customize_tag {};
} // namespace magic_enum::customize::detail

using customize_t = std::variant<string_view, detail::default_customize_tag, detail::invalid_customize_tag>;

// Default customize.
inline constexpr auto default_tag = detail::default_customize_tag{};
// Invalid customize.
inline constexpr auto invalid_tag = detail::invalid_customize_tag{};

// If need custom names for enum, add specialization enum_name for necessary enum type.
template <typename E>
constexpr customize_t enum_name(E) noexcept {
  return default_tag;
}

} // namespace magic_enum::customize

namespace detail {

template <auto V, typename = std::enable_if_t<std::is_enum_v<std::decay_t<decltype(V)>>>>
using enum_constant = std::integral_constant<std::decay_t<decltype(V)>, V>;

template <typename... T>
inline constexpr bool always_false_v = false;

template <typename T>
struct supported
#if defined(MAGIC_ENUM_SUPPORTED) && MAGIC_ENUM_SUPPORTED || defined(MAGIC_ENUM_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <typename T, typename = void>
struct has_is_flags : std::false_type {};

template <typename T>
struct has_is_flags<T, std::void_t<decltype(customize::enum_range<T>::is_flags)>> : std::bool_constant<std::is_same_v<bool, std::decay_t<decltype(customize::enum_range<T>::is_flags)>>> {};

template <typename T, typename = void>
struct range_min : std::integral_constant<int, MAGIC_ENUM_RANGE_MIN> {};

template <typename T>
struct range_min<T, std::void_t<decltype(customize::enum_range<T>::min)>> : std::integral_constant<decltype(customize::enum_range<T>::min), customize::enum_range<T>::min> {};

template <typename T, typename = void>
struct range_max : std::integral_constant<int, MAGIC_ENUM_RANGE_MAX> {};

template <typename T>
struct range_max<T, std::void_t<decltype(customize::enum_range<T>::max)>> : std::integral_constant<decltype(customize::enum_range<T>::max), customize::enum_range<T>::max> {};

template <std::uint16_t N>
class static_string {
 public:
  constexpr explicit static_string(string_view str) noexcept : static_string{str, std::make_integer_sequence<std::uint16_t, N>{}} {
    assert(str.size() == N);
  }

  constexpr const char* data() const noexcept { return chars_; }

  constexpr std::uint16_t size() const noexcept { return N; }

  constexpr operator string_view() const noexcept { return {data(), size()}; }

 private:
  template <std::uint16_t... I>
  constexpr static_string(string_view str, std::integer_sequence<std::uint16_t, I...>) noexcept : chars_{str[I]..., '\0'} {}

  char chars_[static_cast<std::size_t>(N) + 1];
};

template <>
class static_string<0> {
 public:
  constexpr explicit static_string() = default;

  constexpr explicit static_string(string_view) noexcept {}

  constexpr const char* data() const noexcept { return nullptr; }

  constexpr std::uint16_t size() const noexcept { return 0; }

  constexpr operator string_view() const noexcept { return {}; }
};

constexpr string_view pretty_name(string_view name) noexcept {
  for (std::size_t i = name.size(); i > 0; --i) {
    if (!((name[i - 1] >= '0' && name[i - 1] <= '9') ||
          (name[i - 1] >= 'a' && name[i - 1] <= 'z') ||
          (name[i - 1] >= 'A' && name[i - 1] <= 'Z') ||
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
          (name[i - 1] & 0x80) ||
#endif
          (name[i - 1] == '_'))) {
      name.remove_prefix(i);
      break;
    }
  }

  if (name.size() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
                          (name.front() >= 'A' && name.front() <= 'Z') ||
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
                          (name.front() & 0x80) ||
#endif
                          (name.front() == '_'))) {
    return name;
  }

  return {}; // Invalid name.
}

class case_insensitive {
  static constexpr char to_lower(char c) noexcept {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
  }

 public:
  template <typename L, typename R>
  constexpr auto operator()([[maybe_unused]] L lhs, [[maybe_unused]] R rhs) const noexcept -> std::enable_if_t<std::is_same_v<std::decay_t<L>, char> && std::is_same_v<std::decay_t<R>, char>, bool> {
#if defined(MAGIC_ENUM_ENABLE_NONASCII)
    static_assert(always_false_v<L, R>, "magic_enum::case_insensitive not supported Non-ASCII feature.");
    return false;
#else
    return to_lower(lhs) == to_lower(rhs);
#endif
  }
};

template <typename T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N], std::index_sequence<I...>) noexcept {
  return {{a[I]...}};
}

template <typename L, typename R>
constexpr bool cmp_less(L lhs, R rhs) noexcept {
  static_assert(std::is_integral_v<L> && std::is_integral_v<R>, "magic_enum::detail::cmp_less requires integral type.");

  if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
    // If same signedness (both signed or both unsigned).
    return lhs < rhs;
  } else if constexpr (std::is_same_v<L, bool>) { // bool special case
      return static_cast<R>(lhs) < rhs;
  } else if constexpr (std::is_same_v<R, bool>) { // bool special case
      return lhs < static_cast<L>(rhs);
  } else if constexpr (std::is_signed_v<R>) {
    // If 'right' is negative, then result is 'false', otherwise cast & compare.
    return rhs > 0 && lhs < static_cast<std::make_unsigned_t<R>>(rhs);
  } else {
    // If 'left' is negative, then result is 'true', otherwise cast & compare.
    return lhs < 0 || static_cast<std::make_unsigned_t<L>>(lhs) < rhs;
  }
}

template <typename T>
inline constexpr bool is_enum_v = std::is_enum_v<T> && std::is_same_v<T, std::decay_t<T>>;

template <typename E>
constexpr auto n() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::n requires enum type.");

  if constexpr (supported<E>::value) {
#if defined(__clang__) || defined(__GNUC__)
    constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
    constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
    constexpr auto name = string_view{};
#endif
    return static_string<name.size()>{name};
  } else {
    return static_string<0>{}; // Unsupported compiler or Invalid customize.
  }
}

template <typename E, E V>
constexpr auto n() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::n requires enum type.");

  if constexpr (supported<E>::value) {
#if defined(__clang__) || defined(__GNUC__)
    constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
    constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
    constexpr auto name = string_view{};
#endif
    return static_string<name.size()>{name};
  } else {
    return static_string<0>{}; // Unsupported compiler or Invalid customize.
  }
}
template <typename E, E V>
constexpr auto enum_name() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::n requires enum type.");

  [[maybe_unused]] constexpr auto custom = customize::enum_name<E>(V);
  static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t>, "magic_enum::customize requires customize_t type.");
  if constexpr (std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t> && custom.index() == 0) {
    constexpr auto name = std::get<string_view>(custom);
    static_assert(!name.empty(), "magic_enum::customize requires not empty string.");
    return static_string<name.size()>{name};
  } else {
    return n<E, V>();
  }
}

template <typename E, auto V>
constexpr bool is_valid() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::is_valid requires enum type.");

#if defined(__clang__) && __clang_major__ >= 16
  // https://reviews.llvm.org/D130058, https://reviews.llvm.org/D131307
  constexpr E v = __builtin_bit_cast(E, V);
  [[maybe_unused]] constexpr auto custom = customize::enum_name<E>(v);
  static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t>, "magic_enum::customize requires customize_t type.");
  if constexpr (std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t> && custom.index() == 0) {
    constexpr auto name = std::get<string_view>(custom);
    static_assert(!name.empty(), "magic_enum::customize requires not empty string.");
    return name.size() != 0;
  } else {
    return n<E, v>().size() != 0;
  }
#else
  return enum_name<E, static_cast<E>(V)>().size() != 0;
#endif
}

template <typename E, int O, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr U ualue(std::size_t i) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::ualue requires enum type.");

  if constexpr (std::is_same_v<U, bool>) { // bool special case
    static_assert(O == 0, "magic_enum::detail::ualue requires valid offset.");

    return static_cast<U>(i);
  } else if constexpr (IsFlags) {
    return static_cast<U>(U{1} << static_cast<U>(static_cast<int>(i) + O));
  } else {
    return static_cast<U>(static_cast<int>(i) + O);
  }
}

template <typename E, int O, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr E value(std::size_t i) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::value requires enum type.");

  return static_cast<E>(ualue<E, O, IsFlags>(i));
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_min() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::reflected_min requires enum type.");

  if constexpr (IsFlags) {
    return 0;
  } else {
    constexpr auto lhs = range_min<E>::value;
    constexpr auto rhs = (std::numeric_limits<U>::min)();

    if constexpr (cmp_less(rhs, lhs)) {
      return lhs;
    } else {
      return rhs;
    }
  }
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_max() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::reflected_max requires enum type.");

  if constexpr (IsFlags) {
    return std::numeric_limits<U>::digits - 1;
  } else {
    constexpr auto lhs = range_max<E>::value;
    constexpr auto rhs = (std::numeric_limits<U>::max)();

    if constexpr (cmp_less(lhs, rhs)) {
      return lhs;
    } else {
      return rhs;
    }
  }
}

template <typename E, bool IsFlags>
inline constexpr auto reflected_min_v = reflected_min<E, IsFlags>();

template <typename E, bool IsFlags>
inline constexpr auto reflected_max_v = reflected_max<E, IsFlags>();

template <std::size_t N>
constexpr std::size_t values_count(const bool (&valid)[N]) noexcept {
  auto count = std::size_t{0};
  for (std::size_t i = 0; i < N; ++i) {
    if (valid[i]) {
      ++count;
    }
  }

  return count;
}

template <typename E, bool IsFlags, int Min, std::size_t... I>
constexpr auto values(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::values requires enum type.");
  constexpr bool valid[sizeof...(I)] = {is_valid<E, ualue<E, Min, IsFlags>(I)>()...};
  constexpr std::size_t count = values_count(valid);

  if constexpr (count > 0) {
    E values[count] = {};
    for (std::size_t i = 0, v = 0; v < count; ++i) {
      if (valid[i]) {
        values[v++] = value<E, Min, IsFlags>(i);
      }
    }

    return to_array(values, std::make_index_sequence<count>{});
  } else {
    return std::array<E, 0>{};
  }
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr auto values() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::values requires enum type.");
  constexpr auto min = reflected_min_v<E, IsFlags>;
  constexpr auto max = reflected_max_v<E, IsFlags>;
  constexpr auto range_size = max - min + 1;
  static_assert(range_size > 0, "magic_enum::enum_range requires valid size.");
  static_assert(range_size < (std::numeric_limits<std::uint16_t>::max)(), "magic_enum::enum_range requires valid size.");

  return values<E, IsFlags, reflected_min_v<E, IsFlags>>(std::make_index_sequence<range_size>{});
}

template <typename E, typename U = std::underlying_type_t<E>>
constexpr bool is_flags_enum() noexcept {
  static_assert(is_enum_v<E>, "magic_enum::detail::is_flags_enum requires enum type.");

  if constexpr (has_is_flags<E>::value) {
    return customize::enum_range<E>::is_flags;
  } else if constexpr (std::is_same_v<U, bool>) { // bool special case
    return false;
  } else {
#if defined(MAGIC_ENUM_NO_CHECK_FLAGS)
    return false;
#else
    constexpr auto flags_values = values<E, true>();
    constexpr auto default_values = values<E, false>();
    if (flags_values.size() == 0 || default_values.size() > flags_values.size()) {
      return false;
    }
    for (std::size_t i = 0; i < default_values.size(); ++i) {
      const auto v = static_cast<U>(default_values[i]);
      if (v != 0 && (v & (v - 1)) != 0) {
        return false;
      }
    }
    return flags_values.size() > 0;
#endif
  }
}

template <typename E>
inline constexpr bool is_flags_v = is_flags_enum<E>();

template <typename E>
inline constexpr std::array values_v = values<E, is_flags_v<E>>();
template <typename E>
inline constexpr auto count_v = values_v<E>.size();

template <bool, typename R>
struct enable_if_enum {};

template <typename R>
struct enable_if_enum<true, R> {
  using type = R;
  static_assert(supported<R>::value, "magic_enum unsupported compiler (https://github.com/Neargye/magic_enum#compiler-compatibility).");
};

template <typename T, typename R, typename BinaryPredicate = std::equal_to<>>
using enable_if_t = typename enable_if_enum<std::is_enum_v<std::decay_t<T>> && std::is_invocable_r_v<bool, BinaryPredicate, char, char>, R>::type;

template <typename T, typename Enable = std::enable_if_t<std::is_enum_v<std::decay_t<T>>>>
using enum_concept = T;

template <typename T, bool = std::is_enum_v<std::decay_t<T>>>
struct underlying_type {};

template <typename T>
struct underlying_type<T, true> : std::underlying_type<std::decay_t<T>> {};
} // namespace magic_enum::detail

template <typename T>
using Enum = detail::enum_concept<T>;

// If T is a complete enumeration type, provides a member typedef type that names the underlying type of T.
// Otherwise, if T is not an enumeration type, there is no member type. Otherwise (T is an incomplete enumeration type), the program is ill-formed.
template <typename T>
struct underlying_type : detail::underlying_type<T> {};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

template <auto V>
using enum_constant = detail::enum_constant<V>;

// Returns number of enum values.
template <typename E>
[[nodiscard]] constexpr auto enum_count() noexcept -> detail::enable_if_t<E, std::size_t> {
  return detail::count_v<std::decay_t<E>>;
}
} // namespace magic_enum

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_MAGIC_ENUM_HPP
