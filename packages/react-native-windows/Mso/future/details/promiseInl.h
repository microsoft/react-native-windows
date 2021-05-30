// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// We do not use pragma once because the file is empty if FUTURE_INLINE_DEFS is not defined
#ifdef MSO_FUTURE_INLINE_DEFS

#ifndef MSO_FUTURE_DETAILS_PROMISEINL_H
#define MSO_FUTURE_DETAILS_PROMISEINL_H

namespace Mso {

//=============================================================================
// Promise<T> implementation
//=============================================================================

template <class T>
inline Promise<T>::Promise() noexcept {
  constexpr const auto &promiseTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:     */ Mso::Futures::FutureOptions::CancelIfUnfulfilled,
      /*ResultType:  */ T,
      /*TaskType:    */ void,
      /*PostType:    */ void,
      /*InvokeType:  */ void,
      /*AbandonType: */ void>::Traits;

  m_state = Mso::Futures::MakeFuture(promiseTraits, 0, nullptr);
}

template <class T>
inline Promise<T>::Promise(std::nullptr_t) noexcept {}

template <class T>
inline Promise<T>::Promise(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept : m_state(std::move(state)) {}

template <class T>
inline Promise<T>::Promise(const Promise &other) noexcept : m_state(other.m_state) {}

template <class T>
inline Promise<T>::Promise(Promise &&other) noexcept : m_state(std::move(other.m_state)) {}

template <class T>
inline Promise<T> &Promise<T>::operator=(const Promise<T> &other) noexcept {
  m_state = other.m_state;
  return *this;
}

template <class T>
inline Promise<T> &Promise<T>::operator=(Promise<T> &&other) noexcept {
  m_state = std::move(other.m_state);
  return *this;
}

template <class T>
inline void Promise<T>::Swap(Promise &other) noexcept {
  using std::swap;
  swap(m_state, other.m_state);
}

template <class T>
inline Promise<T>::operator bool() const noexcept {
  return !m_state.IsEmpty();
}

template <class T>
inline Future<T> Promise<T>::AsFuture() const noexcept {
  return Future<T>{Mso::CntPtr{m_state}};
}

template <class T>
inline void Promise<T>::SetValue(const T &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x012ca403 /* tag_blkqd */);
  m_state->SetValue<T>(value);
}

template <class T>
inline void Promise<T>::SetValue(T &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0130f549 /* tag_bmpvj */);
  m_state->SetValue<T>(std::move(value));
}

template <class T>
inline bool Promise<T>::TrySetValue(const T &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x012ca404 /* tag_blkqe */);
  return m_state->TrySetValue<T>(value);
}

template <class T>
inline bool Promise<T>::TrySetValue(T &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0130f54a /* tag_bmpvk */);
  return m_state->TrySetValue<T>(std::move(value));
}

template <class T>
inline void Promise<T>::SetValue(const Mso::Maybe<T> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056ca /* tag_byf1k */);
  if (!value.IsError()) {
    m_state->SetValue<T>(value.GetValue());
  } else {
    (void)m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ true);
  }
}

template <class T>
inline void Promise<T>::SetValue(Mso::Maybe<T> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056cb /* tag_byf1l */);
  if (!value.IsError()) {
    m_state->SetValue<T>(value.TakeValue());
  } else {
    (void)m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ true);
  }
}

template <class T>
inline bool Promise<T>::TrySetValue(const Mso::Maybe<T> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056cc /* tag_byf1m */);
  if (!value.IsError()) {
    return m_state->TrySetValue<T>(value.GetValue());
  } else {
    return m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ false);
  }
}

template <class T>
inline bool Promise<T>::TrySetValue(Mso::Maybe<T> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056cd /* tag_byf1n */);
  if (!value.IsError()) {
    return m_state->TrySetValue<T>(value.TakeValue());
  } else {
    return m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ false);
  }
}

template <class T>
template <class... TArgs>
inline void Promise<T>::EmplaceValue(TArgs &&... args) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x012ca405 /* tag_blkqf */);
  this->m_state->template SetValue<T>(std::forward<TArgs>(args)...);
}

template <class T>
template <class TArg, class... TArgs>
inline void Promise<T>::EmplaceValue(std::initializer_list<TArg> init, TArgs &&... args) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056ce /* tag_byf1o */);
  m_state->SetValue<T>(init, std::forward<TArgs>(args)...);
}

template <class T>
template <class... TArgs>
inline bool Promise<T>::TryEmplaceValue(TArgs &&... args) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x012ca406 /* tag_blkqg */);
  return m_state->TrySetValue<T>(std::forward<TArgs>(args)...);
}

template <class T>
template <class TArg, class... TArgs>
inline bool Promise<T>::TryEmplaceValue(std::initializer_list<TArg> init, TArgs &&... args) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056cf /* tag_byf1p */);
  return m_state->TrySetValue<T>(init, std::forward<TArgs>(args)...);
}

template <class T>
inline bool Promise<T>::TryCancel() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d0 /* tag_byf1q */);
  return m_state->TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true), /*crashIfFailed:*/ false);
}

template <class T>
inline void Promise<T>::SetError(const ErrorCode &errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d1 /* tag_byf1r */);
  (void)m_state->TrySetError(Mso::ErrorCode(errorCode), /*crashIfFailed:*/ true);
}

template <class T>
inline void Promise<T>::SetError(ErrorCode &&errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d2 /* tag_byf1s */);
  (void)m_state->TrySetError(std::move(errorCode), /*crashIfFailed:*/ true);
}

template <class T>
inline bool Promise<T>::TrySetError(const ErrorCode &errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d3 /* tag_byf1t */);
  return m_state->TrySetError(Mso::ErrorCode(errorCode), /*crashIfFailed:*/ false);
}

template <class T>
inline bool Promise<T>::TrySetError(ErrorCode &&errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d4 /* tag_byf1u */);
  return m_state->TrySetError(std::move(errorCode), /*crashIfFailed:*/ false);
}

template <class T>
inline void Promise<T>::SetMaybe(const Mso::Maybe<T> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d5 /* tag_byf1v */);
  if (!value.IsError()) {
    m_state->SetValue<T>(value.GetValue());
  } else {
    (void)m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ true);
  }
}

template <class T>
inline void Promise<T>::SetMaybe(Mso::Maybe<T> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d6 /* tag_byf1w */);
  if (!value.IsError()) {
    m_state->SetValue<T>(value.TakeValue());
  } else {
    (void)m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ true);
  }
}

template <class T>
inline bool Promise<T>::TrySetMaybe(const Mso::Maybe<T> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d7 /* tag_byf1x */);
  if (!value.IsError()) {
    return m_state->TrySetValue<T>(value.GetValue());
  } else {
    return m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ false);
  }
}

template <class T>
inline bool Promise<T>::TrySetMaybe(Mso::Maybe<T> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x016056d8 /* tag_byf1y */);
  if (!value.IsError()) {
    return m_state->TrySetValue<T>(value.TakeValue());
  } else {
    return m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ false);
  }
}

//=============================================================================
// Promise<Maybe<T>> specialization
//=============================================================================

template <class T>
Promise<Mso::Maybe<T>>::Promise() noexcept : Super() {}

template <class T>
Promise<Mso::Maybe<T>>::Promise(std::nullptr_t) noexcept : Super() {}

template <class T>
Promise<Mso::Maybe<T>>::Promise(const Mso::Promise<T> &other) noexcept : Super(other) {}

template <class T>
Promise<Mso::Maybe<T>>::Promise(Mso::Promise<T> &&other) noexcept : Super(std::move(other)) {}

template <class T>
Promise<Mso::Maybe<T>>::Promise(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept : Super(std::move(state)) {}

template <class T>
Promise<Mso::Maybe<T>>::Promise(const Promise<Mso::Maybe<T>> &other) noexcept : Super(other) {}

template <class T>
Promise<Mso::Maybe<T>>::Promise(Promise<Mso::Maybe<T>> &&other) noexcept : Super(std::move(other)) {}

template <class T>
Promise<Mso::Maybe<T>> &Promise<Mso::Maybe<T>>::operator=(const Promise<Mso::Maybe<T>> &other) noexcept {
  Super::operator=(other);
  return *this;
}

template <class T>
Promise<Mso::Maybe<T>> &Promise<Mso::Maybe<T>>::operator=(Promise<Mso::Maybe<T>> &&other) noexcept {
  Super::operator=(std::move(other));
  return *this;
}

//=============================================================================
// Promise<T> standalone functions
//=============================================================================

template <class T>
inline bool operator==(const Promise<T> &left, const Promise<T> &right) noexcept {
  return GetIFuture(left) == GetIFuture(right);
}

template <class T>
inline bool operator!=(const Promise<T> &left, const Promise<T> &right) noexcept {
  return GetIFuture(left) != GetIFuture(right);
}

template <class T>
inline bool operator==(const Promise<T> &left, std::nullptr_t) noexcept {
  return GetIFuture(left) == nullptr;
}

template <class T>
inline bool operator!=(const Promise<T> &left, std::nullptr_t) noexcept {
  return GetIFuture(left) != nullptr;
}

template <class T>
inline bool operator==(std::nullptr_t, const Promise<T> &right) noexcept {
  return GetIFuture(right) == nullptr;
}

template <class T>
inline bool operator!=(std::nullptr_t, const Promise<T> &right) noexcept {
  return GetIFuture(right) != nullptr;
}

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_PROMISEINL_H
#endif // MSO_FUTURE_INLINE_DEFS
