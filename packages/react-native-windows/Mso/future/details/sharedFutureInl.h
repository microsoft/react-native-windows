// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// We do not use pragma once because the file is empty if FUTURE_INLINE_DEFS is not defined
#ifdef MSO_FUTURE_INLINE_DEFS

#ifndef MSO_FUTURE_DETAILS_SHAREDFUTUREINL_H
#define MSO_FUTURE_DETAILS_SHAREDFUTUREINL_H

namespace Mso {

//=============================================================================
// SharedFuture<T> implementation
//=============================================================================

template <class T>
SharedFuture<T>::SharedFuture() noexcept {}

template <class T>
SharedFuture<T>::SharedFuture(std::nullptr_t) noexcept {}

template <class T>
SharedFuture<T>::SharedFuture(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept : m_state(std::move(state)) {}

template <class T>
SharedFuture<T>::SharedFuture(const SharedFuture &other) noexcept : m_state(other.m_state) {}

template <class T>
SharedFuture<T>::SharedFuture(SharedFuture &&other) noexcept : m_state(std::move(other.m_state)) {}

template <class T>
SharedFuture<T> &SharedFuture<T>::operator=(const SharedFuture &other) noexcept {
  m_state = other.m_state;
  return *this;
}

template <class T>
SharedFuture<T> &SharedFuture<T>::operator=(SharedFuture &&other) noexcept {
  m_state = std::move(other.m_state);
  return *this;
}

template <class T>
void SharedFuture<T>::Swap(SharedFuture &other) noexcept {
  m_state.Swap(other.m_state);
}

template <class T>
SharedFuture<T>::operator bool() const noexcept {
  return !m_state.IsEmpty();
}

template <class T>
template <class TExecutor, class TCallback>
auto SharedFuture<T>::Then(TCallback &&callback) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x012ca40b /* tag_blkql */);

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

  constexpr const auto &sharedTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::None,
      /*ResultType: */ T,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ Mso::Futures::CopyTaskInvoke<T>,
      /*CatchType:  */ Mso::Futures::CopyTaskCatch>::Traits;

  Mso::CntPtr<Mso::Futures::IFuture> shared = Mso::Futures::MakeFuture(sharedTraits, 0, nullptr);

  shared->AddContinuation(future.Get());
  m_state->AddContinuation(std::move(shared));

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

template <class T>
template <class TExecutor, class TCallback>
auto SharedFuture<T>::Then(TExecutor &&executor, TCallback &&callback) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x013d5111 /* tag_bpver */);

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

  constexpr const auto &sharedTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::None,
      /*ResultType: */ T,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ Mso::Futures::CopyTaskInvoke<T>,
      /*CatchType:  */ Mso::Futures::CopyTaskCatch>::Traits;

  Mso::CntPtr<Mso::Futures::IFuture> shared = Mso::Futures::MakeFuture(sharedTraits, 0, nullptr);

  shared->AddContinuation(future.Get());
  m_state->AddContinuation(std::move(shared));

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

template <class T>
template <class TExecutor, class TCallback>
Mso::Future<T> SharedFuture<T>::Catch(TCallback &&callback) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d9 /* tag_byf1z */);

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

  constexpr const auto &sharedTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::None,
      /*ResultType: */ T,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ Mso::Futures::CopyTaskInvoke<T>,
      /*CatchType:  */ Mso::Futures::CopyTaskCatch>::Traits;

  Mso::CntPtr<Mso::Futures::IFuture> shared = Mso::Futures::MakeFuture(sharedTraits, 0, nullptr);

  shared->AddContinuation(future.Get());
  m_state->AddContinuation(std::move(shared));

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

template <class T>
template <class TExecutor, class TCallback>
Mso::Future<T> SharedFuture<T>::Catch(TExecutor &&executor, TCallback &&callback) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056da /* tag_byf10 */);

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

  constexpr const auto &sharedTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::None,
      /*ResultType: */ T,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ Mso::Futures::CopyTaskInvoke<T>,
      /*CatchType:  */ Mso::Futures::CopyTaskCatch>::Traits;

  Mso::CntPtr<Mso::Futures::IFuture> shared = Mso::Futures::MakeFuture(sharedTraits, 0, nullptr);

  shared->AddContinuation(future.Get());
  m_state->AddContinuation(std::move(shared));

  return typename ExecutorTraits::template FutureType<ValueType>(std::move(future));
}

//=============================================================================
// SharedFuture<T> standalone functions
//=============================================================================

template <class T>
bool operator==(const SharedFuture<T> &left, const SharedFuture<T> &right) noexcept {
  return GetIFuture(left) == GetIFuture(right);
}

template <class T>
bool operator!=(const SharedFuture<T> &left, const SharedFuture<T> &right) noexcept {
  return GetIFuture(left) != GetIFuture(right);
}

template <class T>
bool operator==(const SharedFuture<T> &left, std::nullptr_t) noexcept {
  return GetIFuture(left) == nullptr;
}

template <class T>
bool operator!=(const SharedFuture<T> &left, std::nullptr_t) noexcept {
  return GetIFuture(left) != nullptr;
}

template <class T>
bool operator==(std::nullptr_t, const SharedFuture<T> &right) noexcept {
  return GetIFuture(right) == nullptr;
}

template <class T>
bool operator!=(std::nullptr_t, const SharedFuture<T> &right) noexcept {
  return GetIFuture(right) != nullptr;
}

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_SHAREDFUTUREINL_H
#endif // MSO_FUTURE_INLINE_DEFS
