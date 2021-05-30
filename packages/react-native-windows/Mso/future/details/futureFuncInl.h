// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// We do not use pragma once because the file is empty if FUTURE_INLINE_DEFS is not defined
#ifdef MSO_FUTURE_INLINE_DEFS

#ifndef MSO_FUTURE_DETAILS_FUTUREFUNCINL_H
#define MSO_FUTURE_DETAILS_FUTUREFUNCINL_H

namespace Mso {

//=============================================================================
// Future<T> standalone methods
//=============================================================================

template <class T>
inline Mso::Futures::IFuture *GetIFuture(const T &future) noexcept {
  return future.m_state.Get();
}

template <class T>
inline auto MakeCompletedFuture(T &&value) noexcept {
  Promise<std::decay_t<T>> promise;
  promise.SetValue(std::forward<T>(value));
  return promise.AsFuture();
}

inline Future<void> MakeCompletedFuture() noexcept {
  Promise<void> promise;
  promise.SetValue();
  return promise.AsFuture();
}

template <class T, class... TArgs>
inline Future<T> MakeCompletedFutureEmplaced(TArgs &&... args) noexcept {
  Promise<T> promise;
  promise.EmplaceValue(std::forward<TArgs>(args)...);
  return promise.AsFuture();
}

template <class T, class U, class... TArgs>
inline Future<T> MakeCompletedFutureEmplaced(std::initializer_list<U> il, TArgs &&... args) noexcept {
  Promise<T> promise;
  promise.EmplaceValue(il, std::forward<TArgs>(args)...);
  return promise.AsFuture();
}

template <class T>
auto MakeSucceededFuture(T &&value) noexcept {
  Promise<std::decay_t<T>> promise;
  promise.SetValue(std::forward<T>(value));
  return promise.AsFuture();
}

inline Future<void> MakeSucceededFuture() noexcept {
  Promise<void> promise;
  promise.SetValue();
  return promise.AsFuture();
}

template <class T, class... TArgs>
inline Future<T> MakeSucceededFutureEmplaced(TArgs &&... args) noexcept {
  Promise<T> promise;
  promise.EmplaceValue(std::forward<TArgs>(args)...);
  return promise.AsFuture();
}

template <class T, class U, class... TArgs>
inline Future<T> MakeSucceededFutureEmplaced(std::initializer_list<U> il, TArgs &&... args) noexcept {
  Promise<T> promise;
  promise.EmplaceValue(il, std::forward<TArgs>(args)...);
  return promise.AsFuture();
}

template <class T>
inline Future<T> MakeFailedFuture(Mso::ErrorCode &&error) noexcept {
  Promise<T> promise;
  promise.SetError(std::move(error));
  return promise.AsFuture();
}

template <class T>
Future<T> MakeFailedFuture(const Mso::ErrorCode &error) noexcept {
  Promise<T> promise;
  promise.SetError(error);
  return promise.AsFuture();
}

template <class T>
auto MakeCompletedOptionalFuture(T &&value) noexcept {
  Promise<std::decay_t<T>> promise;
  promise.SetValue(std::forward<T>(value));
  return promise.AsFuture();
}

template <class TExecutor, class TCallback>
auto PostFuture(TCallback &&callback) noexcept {
  using Mso::Futures::GetExecutorType;
  using ExecutorType = decltype(GetExecutorType(std::declval<TExecutor>(), 0));
  using ExecutorTraits = Mso::Futures::ExecutorTraits<ExecutorType>;
  using CallbackType = std::decay_t<TCallback>;

  constexpr const auto argKind = ExecutorTraits::template GetPostArgKind<CallbackType, void>();
  using ResultTraits = typename Mso::Futures::GetResultTraits<void, ExecutorType, CallbackType, argKind>::Type;
  using ReturnType = typename ResultTraits::ReturnType;
  using ValueType = typename ResultTraits::ValueType;
  using TaskType = Mso::Futures::FutureTask<ExecutorType, CallbackType>;

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::DestroyTaskAfterInvoke,
      /*ResultType: */ ValueType,
      /*TaskType:   */ TaskType,
      /*PostType:   */ Mso::Futures::FutureTaskPost<ExecutorType>,
      /*InvokeType: */ Mso::Futures::FutureTaskInvoke<ExecutorType, CallbackType, void, ReturnType, argKind>,
      /*CatchType:  */ Mso::Futures::FutureTaskCatch<ExecutorType, CallbackType, void, ReturnType, argKind>>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  Mso::CntPtr<Mso::Futures::IFuture> future = Mso::Futures::MakeFuture(futureTraits, sizeof(TaskType), &taskBuffer);
  ::new (std::addressof(taskBuffer.As<TaskType>()->Executor)) ExecutorType();
  ::new (std::addressof(taskBuffer.As<TaskType>()->Callback)) CallbackType(std::forward<TCallback>(callback));

  future->Post();

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

template <class TExecutor, class TCallback>
auto PostFuture(TExecutor &&executor, TCallback &&callback) noexcept {
  using Mso::Futures::GetExecutorType;
  using ExecutorType = decltype(GetExecutorType(std::declval<TExecutor>(), 0));
  using ExecutorTraits = Mso::Futures::ExecutorTraits<ExecutorType>;
  using CallbackType = std::decay_t<TCallback>;

  constexpr const auto argKind = ExecutorTraits::template GetPostArgKind<CallbackType, void>();
  using ResultTraits = typename Mso::Futures::GetResultTraits<void, ExecutorType, CallbackType, argKind>::Type;
  using ReturnType = typename ResultTraits::ReturnType;
  using ValueType = typename ResultTraits::ValueType;
  using TaskType = Mso::Futures::FutureTask<ExecutorType, CallbackType>;

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::DestroyTaskAfterInvoke,
      /*ResultType: */ ValueType,
      /*TaskType:   */ TaskType,
      /*PostType:   */ Mso::Futures::FutureTaskPost<ExecutorType>,
      /*InvokeType: */ Mso::Futures::FutureTaskInvoke<ExecutorType, CallbackType, void, ReturnType, argKind>,
      /*CatchType:  */ Mso::Futures::FutureTaskCatch<ExecutorType, CallbackType, void, ReturnType, argKind>>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  Mso::CntPtr<Mso::Futures::IFuture> future = Mso::Futures::MakeFuture(futureTraits, sizeof(TaskType), &taskBuffer);
  ::new (std::addressof(taskBuffer.As<TaskType>()->Executor)) ExecutorType(std::forward<TExecutor>(executor));
  ::new (std::addressof(taskBuffer.As<TaskType>()->Callback)) CallbackType(std::forward<TCallback>(callback));

  future->Post();

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_FUTUREFUNCINL_H
#endif // MSO_FUTURE_INLINE_DEFS
