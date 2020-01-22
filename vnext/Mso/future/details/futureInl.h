// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// We do not use pragma once because the file is empty if FUTURE_INLINE_DEFS is not defined
#ifdef MSO_FUTURE_INLINE_DEFS

#ifndef MSO_FUTURE_DETAILS_FUTUREINL_H
#define MSO_FUTURE_DETAILS_FUTUREINL_H

namespace Mso {

//=============================================================================
// Future<T> implementation
//=============================================================================

template <class T>
Future<T>::Future() noexcept {}

template <class T>
Future<T>::Future(std::nullptr_t) noexcept {}

template <class T>
Future<T>::Future(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept : m_state(std::move(state)) {}

template <class T>
Future<T>::Future(const Future &other) noexcept : m_state(other.m_state) {}

template <class T>
Future<T>::Future(Future &&other) noexcept : m_state(std::move(other.m_state)) {}

template <class T>
Future<T> &Future<T>::operator=(const Future &other) noexcept {
  m_state = other.m_state;
  return *this;
}

template <class T>
Future<T> &Future<T>::operator=(Future &&other) noexcept {
  m_state = std::move(other.m_state);
  return *this;
}

template <class T>
void Future<T>::Swap(Future &other) noexcept {
  using std::swap;
  swap(m_state, other.m_state);
}

template <class T>
Future<T>::operator bool() const noexcept {
  return !m_state.IsEmpty();
}

template <class T>
template <class TExecutor, class TCallback>
auto Future<T>::Then(TCallback &&callback) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x012ca3dc /* tag_blkp2 */);

  using Mso::Futures::GetExecutorType;
  using ExecutorType = decltype(GetExecutorType(static_cast<TExecutor *>(nullptr), 0));
  using ExecutorTraits = Mso::Futures::ExecutorTraits<ExecutorType>;
  using CallbackType = std::decay_t<TCallback>;

  constexpr const auto argKind = ExecutorTraits::template GetThenArgKind<CallbackType, T>();
  using ResultTraits = typename Mso::Futures::GetResultTraits<T, ExecutorType, CallbackType, argKind>::Type;
  using ReturnType = typename ResultTraits::ReturnType;
  using ValueType = typename ResultTraits::ValueType;
  using TaskType = Mso::Futures::FutureTask<ExecutorType, CallbackType>;

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::DestroyTaskAfterInvoke,
      /*ResultType: */ ValueType,
      /*TaskType:   */ TaskType,
      /*PostType:   */ Mso::Futures::FutureTaskPost<ExecutorType>,
      /*InvokeType: */ Mso::Futures::FutureTaskInvoke<ExecutorType, CallbackType, T, ReturnType, argKind>,
      /*CatchType:  */ Mso::Futures::FutureTaskCatch<ExecutorType, CallbackType, T, ReturnType, argKind>>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  Mso::CntPtr<Mso::Futures::IFuture> future = Mso::Futures::MakeFuture(futureTraits, sizeof(TaskType), &taskBuffer);
  ::new (std::addressof(taskBuffer.As<TaskType>()->Executor)) ExecutorType();
  ::new (std::addressof(taskBuffer.As<TaskType>()->Callback)) CallbackType(std::forward<TCallback>(callback));

  m_state->AddContinuation(Mso::CntPtr{future});

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

template <class T>
template <class TExecutor, class TCallback>
auto Future<T>::Then(TExecutor &&executor, TCallback &&callback) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x013d5106 /* tag_bpveg */);

  using Mso::Futures::GetExecutorType;
  using ExecutorType = decltype(GetExecutorType(std::forward<TExecutor>(executor), 0));
  using ExecutorTraits = Mso::Futures::ExecutorTraits<ExecutorType>;
  using CallbackType = std::decay_t<TCallback>;

  constexpr const auto argKind = ExecutorTraits::template GetThenArgKind<CallbackType, T>();
  using ResultTraits = typename Mso::Futures::GetResultTraits<T, ExecutorType, CallbackType, argKind>::Type;
  using ReturnType = typename ResultTraits::ReturnType;
  using ValueType = typename ResultTraits::ValueType;
  using TaskType = Mso::Futures::FutureTask<ExecutorType, CallbackType>;

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::DestroyTaskAfterInvoke,
      /*ResultType: */ ValueType,
      /*TaskType:   */ TaskType,
      /*PostType:   */ Mso::Futures::FutureTaskPost<ExecutorType>,
      /*InvokeType: */ Mso::Futures::FutureTaskInvoke<ExecutorType, CallbackType, T, ReturnType, argKind>,
      /*CatchType:  */ Mso::Futures::FutureTaskCatch<ExecutorType, CallbackType, T, ReturnType, argKind>>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  Mso::CntPtr<Mso::Futures::IFuture> future = Mso::Futures::MakeFuture(futureTraits, sizeof(TaskType), &taskBuffer);
  ::new (std::addressof(taskBuffer.As<TaskType>()->Executor)) ExecutorType(std::forward<TExecutor>(executor));
  ::new (std::addressof(taskBuffer.As<TaskType>()->Callback)) CallbackType(std::forward<TCallback>(callback));

  m_state->AddContinuation(Mso::CntPtr{future});

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

template <class T>
template <class TExecutor, class TCallback>
Mso::Future<T> Future<T>::Catch(TCallback &&callback) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605692 /* tag_byf0s */);

  using Mso::Futures::GetExecutorType;
  using ExecutorType = decltype(GetExecutorType(static_cast<TExecutor *>(nullptr), 0));
  using ExecutorTraits = Mso::Futures::ExecutorTraits<ExecutorType>;
  using CallbackType = std::decay_t<TCallback>;

  constexpr const auto argKind = ExecutorTraits::template GetCatchArgKind<CallbackType, T>();
  using ResultTraits = typename Mso::Futures::GetResultTraits<T, ExecutorType, CallbackType, argKind>::Type;
  using ReturnType = typename ResultTraits::ReturnType;
  using ValueType = typename ResultTraits::ValueType;
  static_assert(
      std::is_same<T, ValueType>::value, "Catch callback must result with the same value type as the T in Future<T>.");
  using TaskType = Mso::Futures::FutureTask<ExecutorType, CallbackType>;

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::DestroyTaskAfterInvoke,
      /*ResultType: */ ValueType,
      /*TaskType:   */ TaskType,
      /*PostType:   */ Mso::Futures::FutureTaskPost<ExecutorType>,
      /*InvokeType: */ Mso::Futures::MoveTaskInvoke<T>,
      /*CatchType:  */ Mso::Futures::FutureTaskCatch<ExecutorType, CallbackType, T, ReturnType, argKind>>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  Mso::CntPtr<Mso::Futures::IFuture> future = Mso::Futures::MakeFuture(futureTraits, sizeof(TaskType), &taskBuffer);
  ::new (std::addressof(taskBuffer.As<TaskType>()->Executor)) ExecutorType();
  ::new (std::addressof(taskBuffer.As<TaskType>()->Callback)) CallbackType(std::forward<TCallback>(callback));

  m_state->AddContinuation(Mso::CntPtr{future});

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

template <class T>
template <class TExecutor, class TCallback>
Mso::Future<T> Future<T>::Catch(TExecutor &&executor, TCallback &&callback) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605693 /* tag_byf0t */);

  using Mso::Futures::GetExecutorType;
  using ExecutorType = decltype(GetExecutorType(std::forward<TExecutor>(executor), 0));
  using ExecutorTraits = Mso::Futures::ExecutorTraits<ExecutorType>;
  using CallbackType = std::decay_t<TCallback>;

  constexpr const auto argKind = ExecutorTraits::template GetCatchArgKind<CallbackType, T>();
  using ResultTraits = typename Mso::Futures::GetResultTraits<T, ExecutorType, CallbackType, argKind>::Type;
  using ReturnType = typename ResultTraits::ReturnType;
  using ValueType = typename ResultTraits::ValueType;
  static_assert(
      std::is_same<T, ValueType>::value, "Catch callback must result with the same value type as the T in Future<T>.");
  using TaskType = Mso::Futures::FutureTask<ExecutorType, CallbackType>;

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::DestroyTaskAfterInvoke,
      /*ResultType: */ ValueType,
      /*TaskType:   */ TaskType,
      /*PostType:   */ Mso::Futures::FutureTaskPost<ExecutorType>,
      /*InvokeType: */ Mso::Futures::MoveTaskInvoke<T>,
      /*CatchType:  */ Mso::Futures::FutureTaskCatch<ExecutorType, CallbackType, T, ReturnType, argKind>>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  Mso::CntPtr<Mso::Futures::IFuture> future = Mso::Futures::MakeFuture(futureTraits, sizeof(TaskType), &taskBuffer);
  ::new (std::addressof(taskBuffer.As<TaskType>()->Executor)) ExecutorType(std::forward<TExecutor>(executor));
  ::new (std::addressof(taskBuffer.As<TaskType>()->Callback)) CallbackType(std::forward<TCallback>(callback));

  m_state->AddContinuation(Mso::CntPtr{future});

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

template <class T>
SharedFuture<T> Future<T>::Share() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x012ca3df /* tag_blkp5 */);

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::IsShared | Mso::Futures::FutureOptions::UseParentValue,
      /*ResultType: */ void,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ void,
      /*CatchType:  */ void>::Traits;

  Mso::CntPtr<Mso::Futures::IFuture> state = Mso::Futures::MakeFuture(futureTraits, 0, nullptr);
  m_state->AddContinuation(state.Get());
  return Mso::SharedFuture<T>(std::move(state));
}

//=============================================================================
// Future<Maybe<T>> specialization
//=============================================================================

template <class T>
Future<Mso::Maybe<T>>::Future() noexcept : Super() {}

template <class T>
Future<Mso::Maybe<T>>::Future(std::nullptr_t) noexcept : Super() {}

template <class T>
Future<Mso::Maybe<T>>::Future(const Mso::Future<T> &other) noexcept : Super(other) {}

template <class T>
Future<Mso::Maybe<T>>::Future(Mso::Future<T> &&other) noexcept : Super(std::move(other)) {}

template <class T>
Future<Mso::Maybe<T>>::Future(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept : Super(std::move(state)) {}

template <class T>
Future<Mso::Maybe<T>>::Future(const Future<Mso::Maybe<T>> &other) noexcept : Super(other) {}

template <class T>
Future<Mso::Maybe<T>>::Future(Future<Mso::Maybe<T>> &&other) noexcept : Super(std::move(other)) {}

template <class T>
Future<Mso::Maybe<T>> &Future<Mso::Maybe<T>>::operator=(const Future<Mso::Maybe<T>> &other) noexcept {
  Super::operator=(other);
  return *this;
}

template <class T>
Future<Mso::Maybe<T>> &Future<Mso::Maybe<T>>::operator=(Future<Mso::Maybe<T>> &&other) noexcept {
  Super::operator=(std::move(other));
  return *this;
}

//=============================================================================
// Future<T> standalone functions
//=============================================================================

template <class T>
bool operator==(const Future<T> &left, const Future<T> &right) noexcept {
  return GetIFuture(left) == GetIFuture(right);
}

template <class T>
bool operator!=(const Future<T> &left, const Future<T> &right) noexcept {
  return GetIFuture(left) != GetIFuture(right);
}

template <class T>
bool operator==(const Future<T> &left, std::nullptr_t) noexcept {
  return GetIFuture(left) == nullptr;
}

template <class T>
bool operator!=(const Future<T> &left, std::nullptr_t) noexcept {
  return GetIFuture(left) != nullptr;
}

template <class T>
bool operator==(std::nullptr_t, const Future<T> &right) noexcept {
  return GetIFuture(right) == nullptr;
}

template <class T>
bool operator!=(std::nullptr_t, const Future<T> &right) noexcept {
  return GetIFuture(right) != nullptr;
}

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_FUTUREINL_H
#endif // MSO_FUTURE_INLINE_DEFS
