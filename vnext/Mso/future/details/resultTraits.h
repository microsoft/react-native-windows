// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_DETAILS_RESULTTRAITS_H
#define MSO_FUTURE_DETAILS_RESULTTRAITS_H

#include <utility>
#include "errorCode/maybe.h"
#include "executor.h"
#include "future/futureForwardDecl.h"

namespace Mso::Futures {

template <class TFutureValue, class TResult>
struct ResultTraits {
  using ReturnType = TResult;
  using ValueType = TResult;
};

template <class TFutureValue>
struct ResultTraits<TFutureValue, Mso::ErrorCode> {
  using ReturnType = Mso::Maybe<TFutureValue>;
  using ValueType = TFutureValue;
};

template <class TFutureValue, class TResult>
struct ResultTraits<TFutureValue, Mso::Maybe<TResult>> {
  using ReturnType = Mso::Maybe<TResult>;
  using ValueType = TResult;
};

template <class TFutureValue>
struct ResultTraits<TFutureValue, void> {
  using ReturnType = void;
  using ValueType = void;
};

template <class TFutureValue, class TResult>
struct ResultTraits<TFutureValue, Mso::Future<TResult>> {
  using ReturnType = Mso::Future<TResult>;
  using ValueType = typename Mso::Future<TResult>::ResultType;
};

template <class TFutureValue, class TResult>
struct ResultTraits<TFutureValue, Mso::Maybe<Mso::Future<TResult>>> {
  using ReturnType = Mso::Maybe<Mso::Future<TResult>>;
  using ValueType = typename Mso::Future<TResult>::ResultType;
};

template <class TFutureValue, class TExecutor, class TCallback, CallbackArgKind argKind>
struct GetResultTraits;

template <class TFutureValue, class TExecutor, class TCallback>
struct GetResultTraits<TFutureValue, TExecutor, TCallback, CallbackArgKind::Value> {
  using Type = ResultTraits<
      TFutureValue,
      decltype(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<TFutureValue &&>()))>;
  static_assert(
      noexcept(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<TFutureValue &&>())),
      "Executor's Invoke method must not throw");
};

template <class TFutureValue, class TExecutor, class TCallback>
struct GetResultTraits<TFutureValue, TExecutor, TCallback, CallbackArgKind::ValueRef> {
  using Type = ResultTraits<
      TFutureValue,
      decltype(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<TFutureValue &>()))>;
  static_assert(
      noexcept(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<TFutureValue &>())),
      "Executor's Invoke method must not throw");
};

template <class TFutureValue, class TExecutor, class TCallback>
struct GetResultTraits<TFutureValue, TExecutor, TCallback, CallbackArgKind::Maybe> {
  using Type = ResultTraits<
      TFutureValue,
      decltype(
          std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::Maybe<TFutureValue> &&>()))>;
  static_assert(
      noexcept(
          std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::Maybe<TFutureValue> &&>())),
      "Executor's Invoke method must not throw");
};

template <class TFutureValue, class TExecutor, class TCallback>
struct GetResultTraits<TFutureValue, TExecutor, TCallback, CallbackArgKind::MaybeRef> {
  using Type = ResultTraits<
      TFutureValue,
      decltype(
          std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::Maybe<TFutureValue> &>()))>;
  static_assert(
      noexcept(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::Maybe<TFutureValue> &>())),
      "Executor's Invoke method must not throw");
};

template <class TFutureValue, class TExecutor, class TCallback>
struct GetResultTraits<TFutureValue, TExecutor, TCallback, CallbackArgKind::Void> {
  using Type = ResultTraits<TFutureValue, decltype(std::declval<TExecutor>().Invoke(std::declval<TCallback>()))>;
  static_assert(
      noexcept(std::declval<TExecutor>().Invoke(std::declval<TCallback>())),
      "Executor's Invoke method must not throw");
};

template <class TFutureValue, class TExecutor, class TCallback>
struct GetResultTraits<TFutureValue, TExecutor, TCallback, CallbackArgKind::MaybeVoid> {
  using Type = ResultTraits<
      TFutureValue,
      decltype(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::Maybe<void> &&>()))>;
  static_assert(
      noexcept(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::Maybe<void> &&>())),
      "Executor's Invoke method must not throw");
};

template <class TFutureValue, class TExecutor, class TCallback>
struct GetResultTraits<TFutureValue, TExecutor, TCallback, CallbackArgKind::MaybeVoidRef> {
  using Type = ResultTraits<
      TFutureValue,
      decltype(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::Maybe<void> &>()))>;
  static_assert(
      noexcept(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::Maybe<void> &>())),
      "Executor's Invoke method must not throw");
};

template <class TFutureValue, class TExecutor, class TCallback>
struct GetResultTraits<TFutureValue, TExecutor, TCallback, CallbackArgKind::ErrorCode> {
  using Type = ResultTraits<
      TFutureValue,
      decltype(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::ErrorCode &&>()))>;
  static_assert(
      noexcept(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<Mso::ErrorCode &&>())),
      "Executor's Invoke method must not throw");
};

template <class T, class TExecutor, class TCallback>
struct GetSharedResultTraits {
  using Type =
      ResultTraits<T, decltype(std::declval<TExecutor>().Invoke(std::declval<TCallback>(), std::declval<const T &>()))>;
};

template <class TExecutor, class TCallback>
struct GetSharedResultTraits<void, TExecutor, TCallback> {
  using Type = ResultTraits<void, decltype(std::declval<TExecutor>().Invoke(std::declval<TCallback>()))>;
};

template <class T>
struct AsConst {
  using Type = const T;
};

template <>
struct AsConst<void> {
  using Type = void;
};

} // namespace Mso::Futures

#endif // MSO_FUTURE_DETAILS_RESULTTRAITS_H
