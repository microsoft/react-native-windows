// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_ERRORCODE_MAYBE_H
#define MSO_ERRORCODE_MAYBE_H

#include "crash/verifyElseCrash.h"
#include "errorCode/errorCode.h"
#include "memoryApi/memoryApi.h"

#pragma pack(push, _CRT_PACKING)

namespace Mso {

/**
  In-place construction

  The struct InPlaceTag is an empty structure type used as a unique type to
  disambiguate constructor and function overloading. Specifically, Maybe<T> has
  a constructor with InPlaceTag as the first argument followed by an argument
  pack; this indicates that T should be constructed in-place (as if by a call to
  placement new expression) with the forwarded argument pack as parameters.
*/
struct InPlaceTag {};

enum class MaybeKind : uint32_t {
  Value,
  Error,
};

/**
  Maybe carries either ErrorCode or value. An application can access value by calling GetValue/TakeValue which will
  VEC if Maybe contains an error. An application can also use GetValueElseThrow or TakeValueElseThrow which will
  generate ErrorCodeException if Maybe contains an error. The difference between verbs Get and Take is that the Get will
  return a const reference, while Take will move the value.
*/
template <class T>
class Maybe {
 public:
  Maybe() noexcept;
  Maybe(const Maybe &other) noexcept;
  Maybe(Maybe &&other) noexcept;

  _Allow_implicit_ctor_ Maybe(const T &value) noexcept;
  _Allow_implicit_ctor_ Maybe(T &&value) noexcept;

  template <class... TArgs>
  explicit Maybe(InPlaceTag, TArgs &&... args) noexcept;

  template <
      class TItem,
      class... TArgs,
      class TDummy =
          typename std::enable_if<std::is_constructible<T, std::initializer_list<TItem> &, TArgs &&...>::value>::type>
  explicit Maybe(InPlaceTag, std::initializer_list<TItem> il, TArgs &&... args) noexcept;

  _Allow_implicit_ctor_ Maybe(const ErrorCode &error) noexcept;
  _Allow_implicit_ctor_ Maybe(ErrorCode &&error) noexcept;

  ~Maybe() noexcept;

  Maybe &operator=(const Maybe &other) noexcept;
  Maybe &operator=(Maybe &&other) noexcept;

  bool IsValue() const noexcept;
  bool IsError() const noexcept;

  const T &GetValue() const noexcept;
  const T &GetValueElseThrow() const;
  T TakeValue() noexcept;
  T TakeValueElseThrow();

  const ErrorCode &GetError() const noexcept;
  ErrorCode TakeError() noexcept;
  void ThrowOnError() const;

 private:
  MaybeKind m_kind{MaybeKind::Value};

#pragma warning(suppress : 4624) // union's destructor is implicitly defined as deleted.
  union {
    ErrorCode m_error;
    T m_value;
  };
};

template <>
class Maybe<void> {
 public:
  Maybe() noexcept;
  Maybe(const Maybe &other) noexcept;
  Maybe(Maybe &&other) noexcept;

  _Allow_implicit_ctor_ Maybe(const ErrorCode &error) noexcept;
  _Allow_implicit_ctor_ Maybe(ErrorCode &&error) noexcept;

  ~Maybe() noexcept;

  Maybe &operator=(const Maybe &other) noexcept;
  Maybe &operator=(Maybe &&other) noexcept;

  bool IsValue() const noexcept;
  bool IsError() const noexcept;

  const ErrorCode &GetError() const noexcept;
  ErrorCode TakeError() noexcept;
  void ThrowOnError() const;

 private:
  MaybeKind m_kind{MaybeKind::Value};

#pragma warning(suppress : 4624) // union's destructor is implicitly defined as deleted.
  union {
    ErrorCode m_error;
    uint8_t m_dummy; // not used. It is only needed to keep the m_error in a union.
  };
};

//============================================================================================
// Maybe<T> implementation
//============================================================================================

template <class T>
Maybe<T>::Maybe() noexcept {
  // The reinterpret_cast "engineering workaround" helps for cases when T has an overloaded operator &.
  ::new (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value))) T();
}

template <class T>
Maybe<T>::Maybe(const Maybe &other) noexcept : m_kind(other.m_kind) {
  switch (other.m_kind) {
    case MaybeKind::Value:
      ::new (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value))) T(other.m_value);
      break;
    case MaybeKind::Error:
      ::new (&m_error) Mso::ErrorCode(other.m_error);
      break;
  }
}

template <class T>
Maybe<T>::Maybe(Maybe &&other) noexcept : m_kind(other.m_kind) {
  switch (other.m_kind) {
    case MaybeKind::Value:
      ::new (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value))) T(std::move(other.m_value));
      break;
    case MaybeKind::Error:
      ::new (&m_error) Mso::ErrorCode(std::move(other.m_error));
      break;
  }
}

template <class T>
Maybe<T>::Maybe(const T &value) noexcept {
  ::new (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value))) T(value);
}

template <class T>
Maybe<T>::Maybe(T &&value) noexcept {
  ::new (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value))) T(std::move(value));
}

template <class T>
template <class... TArgs>
Maybe<T>::Maybe(InPlaceTag, TArgs &&... args) noexcept {
  ::new (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value))) T(std::forward<TArgs>(args)...);
}

template <class T>
template <class TItem, class... TArgs, class TDummy>
Maybe<T>::Maybe(InPlaceTag, std::initializer_list<TItem> il, TArgs &&... args) noexcept {
  ::new (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value))) T(il, std::forward<TArgs>(args)...);
}

template <class T>
Maybe<T>::Maybe(const ErrorCode &error) noexcept : m_kind(MaybeKind::Error) {
  ::new (&m_error) Mso::ErrorCode(error);
}

template <class T>
Maybe<T>::Maybe(ErrorCode &&error) noexcept : m_kind(MaybeKind::Error) {
  ::new (&m_error) Mso::ErrorCode(std::move(error));
}

template <class T>
Maybe<T>::~Maybe() noexcept
#pragma warning(suppress : 4583) //  destructor is not implicitly called
{
  switch (m_kind) {
    case MaybeKind::Value:
      (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value)))->~T();
      break;
    case MaybeKind::Error:
      (&m_error)->~ErrorCode();
      break;
  }
}

template <class T>
Maybe<T> &Maybe<T>::operator=(const Maybe &other) noexcept
#pragma warning(suppress : 4583) //  destructor is not implicitly called for some simple types
{
  if (this != &other) {
    switch (m_kind) {
      case MaybeKind::Value:
        (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value)))->~T();
        break;
      case MaybeKind::Error:
        (&m_error)->~ErrorCode();
        break;
    }

    m_kind = other.m_kind;

    switch (m_kind) {
      case MaybeKind::Value:
        ::new (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value))) T(other.m_value);
        break;
      case MaybeKind::Error:
        ::new (&m_error) Mso::ErrorCode(other.m_error);
        break;
    }
  }

  return *this;
}

template <class T>
Maybe<T> &Maybe<T>::operator=(Maybe &&other) noexcept
#pragma warning(suppress : 4583) //  destructor is not implicitly called for some simple types
{
  if (this != &other) {
    switch (m_kind) {
      case MaybeKind::Value:
        (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value)))->~T();
        break;
      case MaybeKind::Error:
        (&m_error)->~ErrorCode();
        break;
    }

    m_kind = other.m_kind;

    switch (m_kind) {
      case MaybeKind::Value:
        ::new (reinterpret_cast<T *>(&reinterpret_cast<char &>(m_value))) T(std::move(other.m_value));
        break;
      case MaybeKind::Error:
        ::new (&m_error) Mso::ErrorCode(std::move(other.m_error));
        break;
    }
  }

  return *this;
}

template <class T>
bool Maybe<T>::IsValue() const noexcept {
  return m_kind == MaybeKind::Value;
}

template <class T>
bool Maybe<T>::IsError() const noexcept {
  return m_kind == MaybeKind::Error;
}

template <class T>
const T &Maybe<T>::GetValue() const noexcept {
  VerifyElseCrashSzTag(m_kind == MaybeKind::Value, "Not a value", 0x0130f540 /* tag_bmpva */);
  return m_value;
}

template <class T>
const T &Maybe<T>::GetValueElseThrow() const {
  if (m_kind == MaybeKind::Error) {
    m_error.Throw();
  }

  return m_value;
}

template <class T>
T Maybe<T>::TakeValue() noexcept {
  VerifyElseCrashSzTag(m_kind == MaybeKind::Value, "Not a value", 0x0130f541 /* tag_bmpvb */);
  return std::move(m_value);
}

template <class T>
T Maybe<T>::TakeValueElseThrow() {
  if (m_kind == MaybeKind::Error) {
    m_error.Throw();
  }

  return std::move(m_value);
}

template <class T>
const ErrorCode &Maybe<T>::GetError() const noexcept {
  VerifyElseCrashSzTag(m_kind == MaybeKind::Error, "Not an error", 0x0130f542 /* tag_bmpvc */);
  return m_error;
}

template <class T>
ErrorCode Maybe<T>::TakeError() noexcept {
  VerifyElseCrashSzTag(m_kind == MaybeKind::Error, "Not an error", 0x0130f543 /* tag_bmpvd */);
  return std::move(m_error);
}

template <class T>
void Maybe<T>::ThrowOnError() const {
  if (m_kind == MaybeKind::Error) {
    m_error.HandleAndThrow();
  }
}

//============================================================================================
// Maybe<void> implementation
//============================================================================================

inline Maybe<void>::Maybe() noexcept {}

inline Maybe<void>::Maybe(const Maybe &other) noexcept : m_kind(other.m_kind) {
  if (m_kind == MaybeKind::Error) {
    ::new (&m_error) Mso::ErrorCode(other.m_error);
  }
}

inline Maybe<void>::Maybe(Maybe &&other) noexcept : m_kind(other.m_kind) {
  if (m_kind == MaybeKind::Error) {
    ::new (&m_error) Mso::ErrorCode(std::move(other.m_error));
  }
}

inline Maybe<void>::Maybe(const ErrorCode &error) noexcept : m_kind(MaybeKind::Error) {
  ::new (&m_error) Mso::ErrorCode(error);
}

inline Maybe<void>::Maybe(ErrorCode &&error) noexcept : m_kind(MaybeKind::Error) {
  ::new (&m_error) Mso::ErrorCode(std::move(error));
}

inline Maybe<void>::~Maybe() noexcept
#pragma warning(suppress : 4583) //  destructor is not implicitly called
{
  if (m_kind == MaybeKind::Error) {
    (&m_error)->~ErrorCode();
  }
}

inline Maybe<void> &Maybe<void>::operator=(const Maybe &other) noexcept {
  if (this != &other) {
    if (m_kind == MaybeKind::Error) {
      (&m_error)->~ErrorCode();
    }

    m_kind = other.m_kind;

    if (m_kind == MaybeKind::Error) {
      ::new (&m_error) Mso::ErrorCode(other.m_error);
    }
  }

  return *this;
}

inline Maybe<void> &Maybe<void>::operator=(Maybe &&other) noexcept {
  if (this != &other) {
    if (m_kind == MaybeKind::Error) {
      (&m_error)->~ErrorCode();
    }

    m_kind = other.m_kind;

    if (m_kind == MaybeKind::Error) {
      ::new (&m_error) Mso::ErrorCode(std::move(other.m_error));
    }
  }

  return *this;
}

inline bool Maybe<void>::IsValue() const noexcept {
  return m_kind == MaybeKind::Value;
}

inline bool Maybe<void>::IsError() const noexcept {
  return m_kind == MaybeKind::Error;
}

inline const ErrorCode &Maybe<void>::GetError() const noexcept {
  VerifyElseCrashSzTag(m_kind == MaybeKind::Error, "Not an error", 0x0130f544 /* tag_bmpve */);
  return m_error;
}

inline ErrorCode Maybe<void>::TakeError() noexcept {
  VerifyElseCrashSzTag(m_kind == MaybeKind::Error, "Not an error", 0x0130f545 /* tag_bmpvf */);
  return std::move(m_error);
}

inline void Maybe<void>::ThrowOnError() const {
  if (m_kind == MaybeKind::Error) {
    m_error.HandleAndThrow();
  }
}

} // namespace Mso

#pragma pack(pop)

#endif // MSO_ERRORCODE_MAYBE_H
