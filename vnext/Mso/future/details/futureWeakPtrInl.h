// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// We do not use pragma once because the file is empty if FUTURE_INLINE_DEFS is not defined
#ifdef MSO_FUTURE_INLINE_DEFS

#ifndef MSO_FUTURE_DETAILS_FUTUREWEAKPTRINL_H
#define MSO_FUTURE_DETAILS_FUTUREWEAKPTRINL_H

namespace Mso {

template <class T>
WeakPtr<Future<T>>::WeakPtr() noexcept : Super() {}

template <class T>
WeakPtr<Future<T>>::WeakPtr(std::nullptr_t) noexcept : Super() {}

template <class T>
WeakPtr<Future<T>>::WeakPtr(const Future<T> &future) noexcept : Super(GetIFuture(future)) {}

template <class T>
WeakPtr<Future<T>>::WeakPtr(const WeakPtr &from) noexcept : Super(from.m_ptr) {}

template <class T>
WeakPtr<Future<T>>::WeakPtr(WeakPtr &&from) noexcept : Super(from.m_ptr, /*shouldAddWeakRef*/ false) {
  from.m_ptr = nullptr;
}

template <class T>
WeakPtr<Future<T>> &WeakPtr<Future<T>>::operator=(std::nullptr_t) noexcept {
  Reset();
  return *this;
}

template <class T>
WeakPtr<Future<T>> &WeakPtr<Future<T>>::operator=(const Mso::Future<T> &future) noexcept {
  Assign(GetIFuture(future));
  return *this;
}

template <class T>
WeakPtr<Future<T>> &WeakPtr<Future<T>>::operator=(const WeakPtr &from) noexcept {
  Assign(from.m_ptr);
  return *this;
}

template <class T>
WeakPtr<Future<T>> &WeakPtr<Future<T>>::operator=(WeakPtr &&from) noexcept {
  Attach(std::move(from));
  return *this;
}

template <class T>
Mso::Future<T> WeakPtr<Future<T>>::GetFuture() const noexcept {
  Mso::Future<T> future;
  if (IncrementRefCountIfNotZero(m_ptr)) {
    future = Mso::Future<T>{Mso::CntPtr{static_cast<Mso::Futures::IFuture *>(m_ptr), AttachTag}};
  }

  return future;
}

template <class T>
void WeakPtr<Future<T>>::Swap(WeakPtr &other) noexcept {
  using std::swap;
  swap(m_ptr, other.m_ptr);
}

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_FUTUREWEAKPTRINL_H
#endif // MSO_FUTURE_INLINE_DEFS
