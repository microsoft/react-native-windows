// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// We do not use pragma once because the file is empty if FUTURE_INLINE_DEFS is not defined
#ifdef MSO_FUTURE_INLINE_DEFS

#ifndef MSO_FUTURE_DETAILS_PROMISEGROUPINL_H
#define MSO_FUTURE_DETAILS_PROMISEGROUPINL_H

namespace Mso {

//=============================================================================
// PromiseGroup<T> implementation
//=============================================================================

template <class T>
inline PromiseGroup<T>::PromiseGroup() noexcept {
  constexpr const auto &promiseGroupTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:     */ Mso::Futures::FutureOptions::IsShared | Mso::Futures::FutureOptions::CancelIfUnfulfilled,
      /*ResultType:  */ T,
      /*TaskType:    */ void,
      /*PostType:    */ void,
      /*InvokeType:  */ void,
      /*AbandonType: */ void>::Traits;

  m_state = Mso::Futures::MakeFuture(promiseGroupTraits, 0, nullptr);
}

template <class T>
inline PromiseGroup<T>::PromiseGroup(std::nullptr_t) noexcept {}

template <class T>
inline PromiseGroup<T>::PromiseGroup(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept : m_state(std::move(state)) {}

template <class T>
inline PromiseGroup<T>::PromiseGroup(const PromiseGroup &other) noexcept : m_state(other.m_state) {}

template <class T>
inline PromiseGroup<T>::PromiseGroup(PromiseGroup &&other) noexcept : m_state(std::move(other.m_state)) {}

template <class T>
inline PromiseGroup<T> &PromiseGroup<T>::operator=(const PromiseGroup<T> &other) noexcept {
  m_state = other.m_state;
  return *this;
}

template <class T>
inline PromiseGroup<T> &PromiseGroup<T>::operator=(PromiseGroup<T> &&other) noexcept {
  m_state = std::move(other.m_state);
  return *this;
}

template <class T>
inline void PromiseGroup<T>::Swap(PromiseGroup &other) noexcept {
  using std::swap;
  swap(m_state, other.m_state);
}

template <class T>
inline PromiseGroup<T>::operator bool() const noexcept {
  return !m_state.IsEmpty();
}

template <class T>
inline Future<T> PromiseGroup<T>::AddFuture() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d406 /* tag_bunqg */);
  constexpr const auto &promiseTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::None,
      /*ResultType: */ T,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ Mso::Futures::CopyTaskInvoke<T>,
      /*CatchType:  */ Mso::Futures::CopyTaskCatch>::Traits;

  Mso::CntPtr<Mso::Futures::IFuture> promise = Mso::Futures::MakeFuture(promiseTraits, 0, nullptr);
  m_state->AddContinuation(Mso::CntPtr{promise});
  return Mso::Future<T>(std::move(promise));
}

template <class T>
inline void PromiseGroup<T>::SetValue(const T &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d407 /* tag_bunqh */);
  m_state->SetValue<T>(value);
}

template <class T>
inline void PromiseGroup<T>::SetValue(T &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d408 /* tag_bunqi */);
  m_state->SetValue<T>(std::move(value));
}

template <class T>
inline bool PromiseGroup<T>::TrySetValue(const T &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d409 /* tag_bunqj */);
  return m_state->TrySetValue<T>(value);
}

template <class T>
inline bool PromiseGroup<T>::TrySetValue(T &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d40a /* tag_bunqk */);
  return m_state->TrySetValue<T>(std::move(value));
}

template <class T>
template <class... TArgs>
inline void PromiseGroup<T>::EmplaceValue(TArgs &&... args) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d40b /* tag_bunql */);
  m_state->SetValue<T>(std::forward<TArgs>(args)...);
}

template <class T>
template <class TArg, class... TArgs>
inline void PromiseGroup<T>::EmplaceValue(std::initializer_list<TArg> init, TArgs &&... args) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d40c /* tag_bunqm */);
  m_state->SetValue<T>(init, std::forward<TArgs>(args)...);
}

template <class T>
template <class... TArgs>
inline bool PromiseGroup<T>::TryEmplaceValue(TArgs &&... args) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d40d /* tag_bunqn */);
  return m_state->TrySetValue<T>(std::forward<TArgs>(args)...);
}

template <class T>
template <class TArg, class... TArgs>
inline bool PromiseGroup<T>::TryEmplaceValue(std::initializer_list<TArg> init, TArgs &&... args) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d40e /* tag_bunqo */);
  return m_state->TrySetValue<T>(init, std::forward<TArgs>(args)...);
}

template <class T>
inline void PromiseGroup<T>::SetError(const ErrorCode &errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0150d40f /* tag_bunqp */);
  m_state->TrySetError(Mso::ErrorCode(errorCode), /*crashIfFailed:*/ true);
}

template <class T>
inline void PromiseGroup<T>::SetError(ErrorCode &&errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056c3 /* tag_byf1d */);
  m_state->TrySetError(std::move(errorCode), /*crashIfFailed:*/ true);
}

template <class T>
inline bool PromiseGroup<T>::TrySetError(const ErrorCode &errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056c4 /* tag_byf1e */);
  return m_state->TrySetError(Mso::ErrorCode(errorCode), /*crashIfFailed:*/ false);
}

template <class T>
inline bool PromiseGroup<T>::TrySetError(ErrorCode &&errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056c5 /* tag_byf1f */);
  return m_state->TrySetError(std::move(errorCode), /*crashIfFailed:*/ false);
}

template <class T>
inline void PromiseGroup<T>::SetMaybe(const Mso::Maybe<T> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056c6 /* tag_byf1g */);
  if (!value.IsError()) {
    m_state->SetValue<T>(value.GetValue());
  } else {
    m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ true);
  }
}

template <class T>
inline void PromiseGroup<T>::SetMaybe(Mso::Maybe<T> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056c7 /* tag_byf1h */);
  if (!value.IsError()) {
    m_state->SetValue<T>(value.TakeValue());
  } else {
    m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ true);
  }
}

template <class T>
inline bool PromiseGroup<T>::TrySetMaybe(const Mso::Maybe<T> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056c8 /* tag_byf1i */);
  if (!value.IsError()) {
    return m_state->TrySetValue<T>(value.GetValue());
  } else {
    return m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ false);
  }
}

template <class T>
inline bool PromiseGroup<T>::TrySetMaybe(Mso::Maybe<T> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056c9 /* tag_byf1j */);
  if (!value.IsError()) {
    return m_state->TrySetValue<T>(value.TakeValue());
  } else {
    return m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ false);
  }
}

//=============================================================================
// PromiseGroup<T> standalone functions
//=============================================================================

template <class T>
inline bool operator==(const PromiseGroup<T> &left, const PromiseGroup<T> &right) noexcept {
  return GetIFuture(left) == GetIFuture(right);
}

template <class T>
inline bool operator!=(const PromiseGroup<T> &left, const PromiseGroup<T> &right) noexcept {
  return GetIFuture(left) != GetIFuture(right);
}

template <class T>
inline bool operator==(const PromiseGroup<T> &left, std::nullptr_t) noexcept {
  return GetIFuture(left) == nullptr;
}

template <class T>
inline bool operator!=(const PromiseGroup<T> &left, std::nullptr_t) noexcept {
  return GetIFuture(left) != nullptr;
}

template <class T>
inline bool operator==(std::nullptr_t, const PromiseGroup<T> &right) noexcept {
  return GetIFuture(right) == nullptr;
}

template <class T>
inline bool operator!=(std::nullptr_t, const PromiseGroup<T> &right) noexcept {
  return GetIFuture(right) != nullptr;
}

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_PROMISEGROUPINL_H
#endif // MSO_FUTURE_INLINE_DEFS
