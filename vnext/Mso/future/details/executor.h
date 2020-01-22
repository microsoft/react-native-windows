// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_DETAILS_EXECUTOR_H
#define MSO_FUTURE_DETAILS_EXECUTOR_H

#include "dispatchQueue/dispatchQueue.h"
#include "functional/functor.h"
#include "future/futureForwardDecl.h"
#include "maybeInvoker.h"

namespace Mso::Futures {

//! DefaultTag is used to disambiguate between two overload functions.
//! Function that receives integer parameter has higher priority than a
//! function that receives DefaultTag because it requires an extra conversion step.
using DefaultTag = void *;

//! Argument kinds accepted by callbacks
enum class CallbackArgKind {
  Value,
  ValueRef,
  Maybe,
  MaybeRef,
  Void,
  MaybeVoid,
  MaybeVoidRef,
  ErrorCode,
  LegacyFuture,
  Invalid,
};

//! Types for each callback argument kind.
using CallbackValueArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::Value>;
using CallbackValueRefArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::ValueRef>;
using CallbackMaybeArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::Maybe>;
using CallbackMaybeRefArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::MaybeRef>;
using CallbackVoidArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::Void>;
using CallbackMaybeVoidArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::MaybeVoid>;
using CallbackMaybeVoidRefArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::MaybeVoidRef>;
using CallbackErrorCodeArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::ErrorCode>;
using CallbackLegacyFutureArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::LegacyFuture>;
using CallbackInvalidArgType = std::integral_constant<CallbackArgKind, CallbackArgKind::Invalid>;

//! Default validator for an argument type passed to a PostFuture() callback.
template <class T>
struct DefaultPostArgValidator {
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, int) -> decltype(callback(), CallbackVoidArgType());
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, void *) -> CallbackInvalidArgType;

  template <class TCallback>
  static auto CheckArgs() -> decltype(CheckArgs0(std::declval<TCallback>(), 0));

  template <class TCallback>
  constexpr static CallbackArgKind GetArgKind() noexcept {
    static_assert(
        decltype(CheckArgs<TCallback>())::value != CallbackArgKind::Invalid, "Callback must not have parameters.");
    return decltype(CheckArgs<TCallback>())::value;
  }
};

//! Default validator for an argument type passed to a .Then() callback.
template <class T>
struct DefaultThenArgValidator {
  // Since value is implicitly convertible to Maybe, we check for Maybe argument first with a higher priority.
  // Otherwise, callback accepting Maybe would report that it accepts just a value.
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, int, int, int, int)
      -> decltype(callback(std::declval<Mso::Maybe<T> &>()), CallbackMaybeRefArgType());
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, int, int, int, void *)
      -> decltype(callback(std::declval<Mso::Maybe<T> &&>()), CallbackMaybeArgType());
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, int, int, void *, void *)
      -> decltype(callback(std::declval<T &>()), CallbackValueRefArgType());
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, int, void *, void *, void *)
      -> decltype(callback(std::declval<T &&>()), CallbackValueArgType());
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, void *, void *, void *, void *) -> CallbackInvalidArgType;

  template <class TCallback>
  static auto CheckArgs() -> decltype(CheckArgs0(std::declval<TCallback>(), 0, 0, 0, 0));

  template <class TCallback>
  constexpr static const CallbackArgKind GetArgKind() noexcept {
    static_assert(
        decltype(CheckArgs<TCallback>())::value != CallbackArgKind::Invalid,
        "Callback valid parameters: T&&, const T&, T, Maybe<T>&&, or Maybe<T>&.");
    return decltype(CheckArgs<TCallback>())::value;
  }
};

//! Default validator for a void argument type passed to a .Then() callback.
template <>
struct DefaultThenArgValidator<void> {
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, int, int, int) -> decltype(callback(), CallbackVoidArgType());
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, int, int, void *)
      -> decltype(callback(std::declval<Mso::Maybe<void> &>()), CallbackMaybeVoidRefArgType());
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, int, void *, void *)
      -> decltype(callback(std::declval<Mso::Maybe<void> &&>()), CallbackMaybeVoidArgType());
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, void *, void *, void *) -> CallbackInvalidArgType;

  template <class TCallback>
  static auto CheckArgs() -> decltype(CheckArgs0(std::declval<TCallback>(), 0, 0, 0));

  template <class TCallback>
  constexpr static CallbackArgKind GetArgKind() noexcept {
    static_assert(
        decltype(CheckArgs<TCallback>())::value != CallbackArgKind::Invalid,
        "Callback valid parameters: no parameter, Mso::Maybe<void>&&, or Maybe<void>&.");
    return decltype(CheckArgs<TCallback>())::value;
  }
};

//! Default validator for an argument type passed to a .Catch() callback.
template <class T>
struct DefaultCatchArgValidator {
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, int)
      -> decltype(callback(std::declval<ErrorCode &&>()), CallbackErrorCodeArgType());
  template <class TCallback>
  static auto CheckArgs0(TCallback &&callback, void *) -> CallbackInvalidArgType;

  template <class TCallback>
  static auto CheckArgs() -> decltype(CheckArgs0(std::declval<TCallback>(), 0));

  template <class TCallback>
  constexpr static CallbackArgKind GetArgKind() noexcept {
    static_assert(
        decltype(CheckArgs<TCallback>())::value != CallbackArgKind::Invalid,
        "Callback must accept 'ErrorCode&&' parameter.");
    return decltype(CheckArgs<TCallback>())::value;
  }
};

//! Traits for executors.
template <class TExecutor>
struct ExecutorTraits {
  template <class TExec, class T>
  static auto GetFutureType(int) -> typename TExec::template FutureType<T>;
  template <class TExec, class T>
  static auto GetFutureType(void *) -> Mso::Future<T>;

  template <class TExec, class T>
  static auto GetPostArgValidator(int) -> typename TExec::template PostArgValidator<T>;
  template <class TExec, class T>
  static auto GetPostArgValidator(void *) -> DefaultPostArgValidator<T>;

  template <class TExec, class T>
  static auto GetThenArgValidator(int) -> typename TExec::template ThenArgValidator<T>;
  template <class TExec, class T>
  static auto GetThenArgValidator(void *) -> DefaultThenArgValidator<T>;

  template <class TExec, class T>
  static auto GetCatchArgValidator(int) -> typename TExec::template CatchArgValidator<T>;
  template <class TExec, class T>
  static auto GetCatchArgValidator(void *) -> DefaultCatchArgValidator<T>;

 public:
  template <class T>
  using FutureType = decltype(GetFutureType<TExecutor, T>(0));
  template <class T>
  using PostArgValidator = decltype(GetPostArgValidator<TExecutor, T>(0));
  template <class T>
  using ThenArgValidator = decltype(GetThenArgValidator<TExecutor, T>(0));
  template <class T>
  using CatchArgValidator = decltype(GetCatchArgValidator<TExecutor, T>(0));

  template <class TCallback, class T>
  constexpr static CallbackArgKind GetPostArgKind() noexcept {
    return PostArgValidator<T>::template GetArgKind<TCallback>();
  }

  template <class TCallback, class T>
  constexpr static CallbackArgKind GetThenArgKind() noexcept {
    return ThenArgValidator<T>::template GetArgKind<TCallback>();
  }

  template <class TCallback, class T>
  constexpr static CallbackArgKind GetCatchArgKind() noexcept {
    return CatchArgValidator<T>::template GetArgKind<TCallback>();
  }
};

} // namespace Mso::Futures

namespace Mso::Executors {
namespace Internal {

struct ExecutorInvoker {
  template <class TCallback, class... TArgs>
  auto Invoke(TCallback &&callback, TArgs &&... args) noexcept -> decltype(callback(std::forward<TArgs>(args)...)) {
    UNREFERENCED_OACR(this);
    static_assert(noexcept(callback(std::forward<TArgs>(args)...)), "Callback must not throw.");
    return callback(std::forward<TArgs>(args)...);
  }
};

template <class TBaseExecutor>
struct ThrowingExecutor : TBaseExecutor {
  using TBaseExecutor::TBaseExecutor;

  template <class TCallback, class... TArgs>
  auto Invoke(TCallback &&callback, TArgs &&... args) noexcept {
    using TResult = decltype(callback(std::forward<TArgs>(args)...));
    constexpr const bool isNoExcept = noexcept(callback(std::forward<TArgs>(args)...));
    return Mso::Futures::MaybeInvoker<TResult, isNoExcept>::Invoke(
        std::forward<TCallback>(callback), std::forward<TArgs>(args)...);
  }

 private:
  TBaseExecutor m_base;
};

} // namespace Internal

struct Executor : Internal::ExecutorInvoker {
  using Throwing = Internal::ThrowingExecutor<Executor>;

  Executor() noexcept = default;
  LIBLET_PUBLICAPI Executor(DispatchQueue const &queue) noexcept;
  LIBLET_PUBLICAPI Executor(DispatchQueue &&queue) noexcept;

  LIBLET_PUBLICAPI void Post(DispatchTask &&task) noexcept;

 protected:
  DispatchQueue m_queue{nullptr};
};

struct Concurrent : Internal::ExecutorInvoker {
  using Throwing = Internal::ThrowingExecutor<Concurrent>;

  LIBLET_PUBLICAPI void Post(DispatchTask &&task) noexcept;
};

struct Inline : Internal::ExecutorInvoker {
  using Throwing = Internal::ThrowingExecutor<Inline>;

  LIBLET_PUBLICAPI void Post(DispatchTask &&Task) noexcept;
};

} // namespace Mso::Executors

namespace Mso::Futures {

//! Default declarations of GetExecutorType return the type itself as an executor.
//! For custom executor types provide definitions for these three functions below, and change last parameter type to int
//! to override the default functions.
//! Custom GetExecutorType should return the executor type instead of T.
template <class T>
T GetExecutorType(_In_ T *executor, DefaultTag);
template <class T>
T GetExecutorType(_In_ T &executor, DefaultTag);
template <class T>
T GetExecutorType(const T &executor, DefaultTag);

//! GetExecutorType overloads for Mso::CntPtr/TCntRef. They are good for any type, and must not be
//! repeated for other types or executors.
template <class T>
auto GetExecutorType(Mso::CntPtr<T> &executor, int) -> decltype(GetExecutorType((T *)nullptr, 0));
template <class T>
auto GetExecutorType(const Mso::CntPtr<T> &executor, int) -> decltype(GetExecutorType((T *)nullptr, 0));

} // namespace Mso::Futures

namespace Mso {

//! The GetExecutorType function definitions below are used to associate Mso::Executor with DispatchQueue type.
//! They must not be implemented. We use decltype to get the function's result type.
//! The function resolution uses ADL (argument dependent lookup), so they must be in the same
//! namespace as the type T.
//! Since the Mso::CntPtr is defined in Mso:: namespace, we do not have them here.
//! They are defined above in Mso:: namespace are good for any Mso::CntPtr<T> type.
template <typename T, std::enable_if_t<std::is_convertible_v<T *, Mso::DispatchQueue *>, int> = 1>
Mso::Executors::Executor GetExecutorType(_In_ T &queue, int) noexcept;

template <typename T, std::enable_if_t<std::is_convertible_v<T *, Mso::DispatchQueue *>, int> = 1>
Mso::Executors::Executor GetExecutorType(const T &queue, int) noexcept;

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_EXECUTOR_H
