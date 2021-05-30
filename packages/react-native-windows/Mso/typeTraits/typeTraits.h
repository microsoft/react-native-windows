// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
  Various C++ templates to help with compile-time type information.
*/
#pragma once
#ifndef MSO_TYPETRAITS_TYPETRAITS_H
#define MSO_TYPETRAITS_TYPETRAITS_H

#include <type_traits>
#include "compilerAdapters/declspecDefinitions.h"

namespace Mso {

/**
  Empty type used for terminating typelists and other places where
  a concept of 'no type' is needed.
  Note that 'no type' is different than 'void'.
*/
class NilType {};

/**
  Type2Type< T >
  Can be used to overload functions which otherwise would differ only by Return type.
  e.g.
  Since following is not allowed,
  Window* Create();
  Scrollbar* Create();

  Use:
  Window* Create( const Type2Type< Window >& );
  Scrollbar* Create( const Type2Type< Scrollbar >& );
*/
template <typename T>
struct Type2Type {
  typedef T OriginalType;
};

/**
  RawTraits can be used in templates that support pointer and non-pointer
  types to retrieve const and non-const versions.

  T = wchar_t, RawTraits< T >::ArrowType = wchar_t
  T = wchar_t, RawTraits< T >::AddrType = wchar_t*

  T = wchar_t&, RawTraits< T >::ArrowType = wchar_t*
  T = wchar_t&, RawTraits< T >::AddrType = wchar_t*

  T = wchar_t*, RawTraits< T >::ArrowType = wchar_t*
  T = wchar_t*, RawTraits< T >::AddrType = wchar_t**

  T = const wchar_t*, RawTraits< T >::ArrowType = const wchar_t*
  T = const wchar_t*, RawTraits< T >::AddrType = const wchar_t**
*/
template <typename T>
class RawTraits {
 public:
  using ArrowType = T;
  using AddrType = T *;

  static T &GetArrowType(T &t) noexcept {
    return t;
  }
  enum R { isPointer = false, isReference = false };
};

template <typename T>
class RawTraits<T &> {
 public:
  using ArrowType = T *;
  using AddrType = T *;

  static T *GetArrowType(T &t) noexcept {
    return &t;
  }
  enum R { isPointer = false, isReference = true };
};

template <typename T>
class RawTraits<T *> {
 public:
  using ArrowType = T *;
  using AddrType = T **;

  static ArrowType GetArrowType(T *t) noexcept {
    return t;
  }
  enum R { isPointer = true, isReference = false };
};

/**
  EmptyTraits provides methods to deal with whether variables are 'empty'.
*/

/**
  EmptyTraits to use with simple value types
*/
template <typename T>
struct EmptyTraits {
  static T EmptyVal() noexcept {
    return T();
  }
  static bool IsEmpty(const T &t) noexcept {
    return (t == EmptyVal());
  }
  static void Empty(T &t) noexcept {
    t = EmptyVal();
  }
  DECLSPEC_DEPRECATED static void UnsafeEmpty(T &t) noexcept {
    Empty(t);
  }
};

/**
  EmptyTraits to use with pointer types
*/
template <typename T>
struct EmptyTraits<T *> {
  static T *EmptyVal() noexcept {
    return nullptr;
  }
  static bool IsEmpty(std::add_const_t<T> *t) noexcept {
    return (t == EmptyVal());
  }
  static void Empty(T *&t) noexcept {
    t = EmptyVal();
  }
  DECLSPEC_DEPRECATED static void UnsafeEmpty(T *&t) noexcept {
    Empty(t);
  }
};

/**
  EmptyTraits to use with reference types
*/
template <typename T>
struct EmptyTraits<T &> {
  // EmptyVal is explicitly not present
  static bool IsEmpty(const T & /*t*/) noexcept {
    return false;
  }
  // Empty is explicitly not present
  DECLSPEC_DEPRECATED static void UnsafeEmpty(T & /*t*/) noexcept {} // Can't empty a reference
};

/**
  Custom EmptyTraits to use with non-zero defaults, TEmptyVal must be a constant
*/
template <typename T, T TEmptyVal>
struct CustomEmptyTraits {
  static T EmptyVal() noexcept {
    return TEmptyVal;
  }
  static bool IsEmpty(const T &t) noexcept {
    return (t == EmptyVal());
  }
  static void Empty(T &t) noexcept {
    t = EmptyVal();
  }
  DECLSPEC_DEPRECATED static void UnsafeEmpty(T &t) noexcept {
    Empty(t);
  }
};

/**
  Custom EmptyTraits to use with zero-init structures like STATSTG
*/
template <typename T>
struct PODEmptyTraits {
  static T EmptyVal() noexcept {
    T t = {0};
    return t;
  }
  static bool IsEmpty(const T &t) noexcept {
    T tEmpty = EmptyVal();
    return memcmp(&t, &tEmpty, sizeof(t)) == 0;
  }
  static void Empty(T &t) noexcept {
    t = EmptyVal();
  }
  DECLSPEC_DEPRECATED static void UnsafeEmpty(T &t) noexcept {
    Empty(t);
  }
};

/**
  Helper for use with template arguments to force callers to specify type explicitly.

  template <typename T> Bar(const typename Mso::DisableTypeDeduction<T>::type& value)
  [...]
  Bar(someInt);      // compile fails with C2783
  Bar<int>(someInt); // compiles successfully
*/
template <typename T>
struct DisableTypeDeduction {
  using type = T;
};

/**
  OverloadTag can be used be used to disambiguate two function overloads.
  OverloadTag instance passed to an overloaded function will give a higher priority to a function accepting
  OverloadTagP1 versus function that accepts OverloadTagP2.

  For example, we can use it to call a method if it is defined, or do something else if it is not defined.
  template <typename T> auto Foo(Mso::OverloadTagP1, T* x) -> decltype(x->Bar()) { return x->Bar(); }
  template <typename T> int Foo(Mso::OverloadTagP2, T* x) { return DefaultBar(x); }

  The right function overload will be chosen depending on whether type T has Bar() method or not:
  Foo(Mso::OverloadTag(), &x);
*/
using OverloadTagP1 = int;
struct OverloadTagP2 {
  OverloadTagP2(OverloadTagP1) noexcept {}
};
using OverloadTag = OverloadTagP1;

/**
  Identifies the largest type from a variadic set of types.
*/
template <typename... Ts>
struct LargestType;

template <typename T>
struct LargestType<T> {
  using type = T;
};

template <typename T1, typename T2, typename... Ts>
struct LargestType<T1, T2, Ts...> {
  using type = typename LargestType<typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type, Ts...>::type;
};

} // namespace Mso

#endif // MSO_TYPETRAITS_TYPETRAITS_H
