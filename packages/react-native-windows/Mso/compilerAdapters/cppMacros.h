// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_COMPILERADAPTERS_CPPMACROS_H
#define MSO_COMPILERADAPTERS_CPPMACROS_H

/**
  Various low level macros that are globally useful
*/

/**
  Broadly used macros
*/
#ifndef FImplies
#define FImplies(a, b) (!(a) || (b))
#endif

#ifndef FBiImplies
#define FBiImplies(a, b) (!(a) == !(b))
#endif

#ifndef RgC
#define RgC(ary) (ary), _countof(ary)
#endif

/**
  Statement - used for control flow macros like Check, IfFailGo
  Make the macro act like a statement.
*/
#ifndef __GNUC__
#define Statement(x)                                                   \
  __pragma(warning(push)) __pragma(warning(disable : 4127 25037)) do { \
    x;                                                                 \
  }                                                                    \
  while (0)                                                            \
  __pragma(warning(pop))
#else
#define Statement(x) \
  do {               \
    x;               \
  } while (0)
// error: there are no arguments to '__noop' that depend on a template parameter, so a declaration of '__noop' must be
// available [-fpermissive]
#define __noop()
#endif

// Prevent the compiler from automatically providing implementations of various
// class features. Use the macro in your class public: section.
#define MSO_NO_COPY_CTOR(type) type(const type &) = delete
#define MSO_NO_COPY_ASSIGNMENT(type) type &operator=(const type &) = delete
#define MSO_NO_DEFAULT_CTOR(type) type() = delete
#define MSO_NO_COPY_CTOR_AND_ASSIGNMENT(type) \
  MSO_NO_COPY_CTOR(type);                     \
  MSO_NO_COPY_ASSIGNMENT(type)

#if defined(__cplusplus)
#include <type_traits>

/**
  When using an enum class to define a set of bitflags, normal bitflag
  enum operations, such as |, ^, and &, don't work without lots of casts.
  Use this macro to define |, ^, and & for your enum class, where |, ^ and ~
  will return an enum class type, and & will evaluate to true or false.
  The implementation causes error C3281 (global operator cannot have
  managed type in signature) for managed code.
*/
#define ENUM_CLASS_FLAGS_OPERATORS(TEnum)                                                                             \
  constexpr TEnum operator~(TEnum a) noexcept {                                                                       \
    return static_cast<TEnum>(~static_cast<std::underlying_type<TEnum>::type>(a));                                    \
  }                                                                                                                   \
  constexpr TEnum operator|(TEnum a, TEnum b) noexcept {                                                              \
    return static_cast<TEnum>(                                                                                        \
        static_cast<std::underlying_type<TEnum>::type>(a) | static_cast<std::underlying_type<TEnum>::type>(b));       \
  }                                                                                                                   \
  constexpr bool operator&(TEnum a, TEnum b) noexcept {                                                               \
    return !!(static_cast<std::underlying_type<TEnum>::type>(a) & static_cast<std::underlying_type<TEnum>::type>(b)); \
  }                                                                                                                   \
  constexpr TEnum operator^(TEnum a, TEnum b) noexcept {                                                              \
    return static_cast<TEnum>(                                                                                        \
        static_cast<std::underlying_type<TEnum>::type>(a) ^ static_cast<std::underlying_type<TEnum>::type>(b));       \
  }                                                                                                                   \
  inline TEnum &operator|=(TEnum &a, TEnum b) noexcept {                                                              \
    return reinterpret_cast<TEnum &>(                                                                                 \
        reinterpret_cast<std::underlying_type<TEnum>::type &>(a) |=                                                   \
        static_cast<std::underlying_type<TEnum>::type>(b));                                                           \
  }                                                                                                                   \
  inline TEnum &operator&=(TEnum &a, TEnum b) noexcept {                                                              \
    return reinterpret_cast<TEnum &>(                                                                                 \
        reinterpret_cast<std::underlying_type<TEnum>::type &>(a) &=                                                   \
        static_cast<std::underlying_type<TEnum>::type>(b));                                                           \
  }                                                                                                                   \
  inline TEnum &operator^=(TEnum &a, TEnum b) noexcept {                                                              \
    return reinterpret_cast<TEnum &>(                                                                                 \
        reinterpret_cast<std::underlying_type<TEnum>::type &>(a) ^=                                                   \
        static_cast<std::underlying_type<TEnum>::type>(b));                                                           \
  }

#else // !__cplusplus

#define ENUM_CLASS_FLAGS_OPERATORS(TEnum)

#endif // !__cplusplus

/**
  Macros to help share enums between C++ and CX.
  Enums must derive from int (the default)
  Flag enums must derive from unsigned int
*/
#if defined(_WINRT_COMP) && !defined(__clang__)
#define EXPOSE_WINRT_ENUM public
#define EXPOSE_WINRT_FLAGS_ENUM [::Platform::Metadata::Flags] EXPOSE_WINRT_ENUM
#else
#define EXPOSE_WINRT_ENUM
#define EXPOSE_WINRT_FLAGS_ENUM
#endif

/**
  Macros to force placement of symbols into a particular segment.
  For example on iOS ensure large globals aren't placed in the text
  segment because there is a size limit enforced by Apple.
 */
#if MS_TARGET_APPLE
#define DATA_SEGMENT_CONST __attribute__((section("__DATA,__const")))
#else
#define DATA_SEGMENT_CONST
#endif

#if __GNUC__
#define __forceinline __attribute__((always_inline, unused))
// Stub __declspec in GCC
#define __declspec(s)
#endif

#endif // MSO_COMPILERADAPTERS_CPPMACROS_H
