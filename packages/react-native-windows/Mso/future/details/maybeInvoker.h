// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_DETAILS_MAYBEINVOKER_H
#define MSO_FUTURE_DETAILS_MAYBEINVOKER_H

#include "errorCode/exceptionErrorProvider.h"

namespace Mso::Futures {

// The MaybeInvoker does the template magic to execute callbacks depending on whether it returns void or not.
// The result type will be Maybe<T>. Any exceptions will be converted to ErrorCode by ExceptionErrorProvider.
// In case if callback returns Mso::Maybe<T>, the result still will be Mso::Maybe<T>, not Mso::Maybe<Mso::Maybe<T>>
template <class TResult, bool isNoExcept>
struct MaybeInvoker {
  template <class TCallback, class... TArgs>
  static auto Invoke(TCallback &&callback, TArgs &&... args) noexcept {
    OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Here we only execute lambdas that have no noexcept");
    try {
      return Mso::Maybe<TResult>(callback(std::forward<TArgs>(args)...));
    } catch (...) {
      return Mso::ExceptionErrorProvider().MakeMaybe<TResult>(std::current_exception());
    }
  }
};

// A specialization for a callback that has noexcept.
template <class TResult>
struct MaybeInvoker<TResult, /*isNoExcept:*/ true> {
  template <class TCallback, class... TArgs>
  static auto Invoke(TCallback &&callback, TArgs &&... args) noexcept {
    return Mso::Maybe<TResult>(callback(std::forward<TArgs>(args)...));
  }
};

// A specialization for a throwing callback which returns void.
template <>
struct MaybeInvoker<void, /*isNoExcept:*/ false> {
  template <class TCallback, class... TArgs>
  static auto Invoke(TCallback &&callback, TArgs &&... args) noexcept {
    OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Here we only execute lambdas that have no noexcept");
    try {
      callback(std::forward<TArgs>(args)...);
      return Mso::Maybe<void>();
    } catch (...) {
      return Mso::ExceptionErrorProvider().MakeMaybe<void>(std::current_exception());
    }
  }
};

// A specialization for a non-throwing callback which returns void.
template <>
struct MaybeInvoker<void, /*isNoExcept:*/ true> {
  template <class TCallback, class... TArgs>
  static auto Invoke(TCallback &&callback, TArgs &&... args) noexcept {
    callback(std::forward<TArgs>(args)...);
    return Mso::Maybe<void>();
  }
};

// A specialization for a throwing callback which returns Mso::Maybe<T>.
template <class T>
struct MaybeInvoker<Mso::Maybe<T>, /*isNoExcept:*/ false> {
  template <class TCallback, class... TArgs>
  static auto Invoke(TCallback &&callback, TArgs &&... args) noexcept {
    OACR_WARNING_SUPPRESS(UNNECESSARY_TRY_CATCH, "Here we only execute lambdas that have no noexcept");
    try {
      return callback(std::forward<TArgs>(args)...);
    } catch (...) {
      return Mso::ExceptionErrorProvider().MakeMaybe<T>(std::current_exception());
    }
  }
};

// A specialization for a non-throwing callback which returns Mso::Maybe<T>.
template <class T>
struct MaybeInvoker<Mso::Maybe<T>, /*isNoExcept:*/ true> {
  template <class TCallback, class... TArgs>
  static auto Invoke(TCallback &&callback, TArgs &&... args) noexcept {
    return callback(std::forward<TArgs>(args)...);
  }
};

} // namespace Mso::Futures

#endif // MSO_FUTURE_DETAILS_MAYBEINVOKER_H
