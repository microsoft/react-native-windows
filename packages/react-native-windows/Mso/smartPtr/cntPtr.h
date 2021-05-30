// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_SMARTPTR_CNTPTR_H
#define MSO_SMARTPTR_CNTPTR_H

#include <atomic>
#include <utility>
#include "compilerAdapters/declspecDefinitions.h"
#include "crash/verifyElseCrash.h"
#include "debugAssertApi/debugAssertApi.h"
#include "guid/msoGuid.h"
#include "platformAdapters/windowsFirst.h"
#include "typeTraits/sfinae.h"
#include "typeTraits/tags.h"

namespace Mso {

class ObjectWeakRef;

/**
  Simple ref-counting
*/
struct DECLSPEC_NOVTABLE IRefCounted {
  virtual HRESULT QueryInterface(REFIID /*riid*/, _COM_Outptr_ void **ppvObject) const noexcept {
    *ppvObject = nullptr;
    return E_NOINTERFACE;
  }

  virtual void AddRef() const noexcept = 0;
  virtual void Release() const noexcept = 0;
};

/**
  Provides weak ref-counting support at interface level: when there is a need to convert
  CntPtr<IFoo> to WeakPtr<IFoo>, deriving IFoo from IWeakRefCounted ensures that there
  is no run-time error tag_bad22 (under condition that implementations of IFoo are derived
  from RefCountedObject<RefCountStrategy::WeakRef, IFoo>).
*/
struct DECLSPEC_NOVTABLE IWeakRefCounted : IRefCounted {
  virtual ObjectWeakRef &GetWeakRef() const noexcept = 0;
};

} // namespace Mso

namespace Mso {

// Forward declarations
template <typename T>
struct CntPtr;
template <typename T>
struct CntPtrRef;

} // namespace Mso

namespace std {

//! Define swap in std namespace because it is a template function.
template <typename T>
void swap(Mso::CntPtr<T> &left, Mso::CntPtr<T> &right) noexcept;

} // namespace std

namespace Mso {

//! Ref-counted smart pointer
template <typename T>
struct CntPtr {
  CntPtr() = default;
  CntPtr(std::nullptr_t) noexcept;
  CntPtr(int) = delete; //!< Do not construct from NULL, use nullptr
  explicit CntPtr(_In_opt_ T *ptr) noexcept;
  explicit CntPtr(_In_opt_ T *ptr, AttachTagType) noexcept;
  template <typename TDerived, EnableIfIsBaseOf<T, TDerived> = 0>
  CntPtr(_In_opt_ TDerived *ptr) noexcept;
  template <typename TDerived, EnableIfIsBaseOf<T, TDerived> = 0>
  CntPtr(_In_opt_ TDerived *ptr, AttachTagType) noexcept;
  CntPtr(const CntPtr &other) noexcept;
  template <typename TDerived, EnableIfIsBaseOf<T, TDerived> = 0>
  CntPtr(const Mso::CntPtr<TDerived> &other) noexcept;
  CntPtr(CntPtr &&other) noexcept;
  template <typename TDerived, EnableIfIsBaseOf<T, TDerived> = 0>
  CntPtr(CntPtr<TDerived> &&other) noexcept;

  ~CntPtr() noexcept;

  CntPtr &operator=(std::nullptr_t) noexcept;
  CntPtr &operator=(_In_opt_ T *ptr) noexcept;
  template <typename TDerived, EnableIfIsBaseOf<T, TDerived> = 0>
  CntPtr &operator=(_In_opt_ TDerived *ptr) noexcept;
  CntPtr &operator=(const CntPtr &other) noexcept;
  template <typename TDerived, EnableIfIsBaseOf<T, TDerived> = 0>
  CntPtr &operator=(const CntPtr<TDerived> &other) noexcept;
  CntPtr &operator=(CntPtr &&other) noexcept;
  template <typename TDerived, EnableIfIsBaseOf<T, TDerived> = 0>
  CntPtr &operator=(CntPtr<TDerived> &&other) noexcept;

  explicit operator bool() const noexcept;

  bool IsEmpty() const noexcept;
  void Clear() noexcept;

  T *Get() const noexcept;

  T *Attach(T *ptr) noexcept; //! Take object ownership, deletes previously owned object if any
  T *Detach() noexcept; //!< Release ownership and return pointer without calling Release() object

  T &operator*() const noexcept;
  T *operator->() const noexcept;

  //! & operator to retrieve object.
  //! Returned CntPtrRef takes care about safe use of the pointer.
  CntPtrRef<T> operator&() noexcept;

  //! Clears the pointer first.
  T **ClearAndGetAddressOf() noexcept;

  //! Asserts if pointer is not null.
  T **GetAddressOf() const noexcept;

  //! Avoids the assert if you need the address of an existing object.
  T **GetRaw() const noexcept;

  template <typename T1, typename T2>
  friend bool operator==(CntPtr<T1> const &left, CntPtr<T2> const &right) noexcept;
  template <typename T>
  friend bool operator==(CntPtr<T> const &left, std::nullptr_t) noexcept;
  template <typename T>
  friend bool operator==(std::nullptr_t, CntPtr<T> const &right) noexcept;
  template <typename T1, typename T2>
  friend bool operator!=(CntPtr<T1> const &left, CntPtr<T2> const &right) noexcept;
  template <typename T>
  friend bool operator!=(CntPtr<T> const &left, std::nullptr_t) noexcept;
  template <typename T>
  friend bool operator!=(std::nullptr_t, CntPtr<T> const &right) noexcept;

  template <typename T>
  friend void std::swap(CntPtr<T> &left, CntPtr<T> &right) noexcept;

  template <typename TOther>
  friend struct CntPtr;

 private:
  T *m_ptr{nullptr};
};

//! Used for CntPtr operator& implementation.
//! It allows to avoid memory leaks when non-empty CntPtr is used as output parameter T**.
template <typename T>
struct CntPtrRef {
  // We only allow CntPtr to make instance of this type
  template <typename U>
  friend struct CntPtr;

  operator CntPtr<T> *() const noexcept;
  operator void *() const noexcept;
  operator T * *() const noexcept;
  operator void * *() const noexcept;
  operator const void * *() const noexcept;

  template <typename TBase, EnableIfIsBaseOf<TBase, T> = 0>
  explicit operator TBase * *() const noexcept;

  T *&operator*() noexcept;
  T **ClearAndGetAddressOf() noexcept;
  T **GetAddressOf() const noexcept;
  T **GetRaw() const noexcept;

 private:
  CntPtrRef(_In_ CntPtr<T> *pCntPtr) noexcept;

  template <typename T1, typename T2>
  friend bool operator==(const CntPtrRef<T1> &left, const CntPtrRef<T2> &right) noexcept;

  template <typename T1, typename T2>
  friend bool operator!=(const CntPtrRef<T1> &left, const CntPtrRef<T2> &right) noexcept;

 private:
  Mso::CntPtr<T> *m_pCntPtr;
};

//=============================================================================
// CntPtr inline implementation
//=============================================================================

template <typename T>
inline CntPtr<T>::CntPtr(std::nullptr_t) noexcept {}

template <typename T>
inline CntPtr<T>::CntPtr(_In_opt_ T *ptr) noexcept : m_ptr{ptr} {
  if (m_ptr) {
    const_cast<std::remove_const_t<T> *>(m_ptr)->AddRef();
  }
}

template <typename T>
inline CntPtr<T>::CntPtr(_In_opt_ T *ptr, AttachTagType) noexcept : m_ptr{ptr} {}

template <typename T>
template <typename TDerived, EnableIfIsBaseOf<T, TDerived>>
inline CntPtr<T>::CntPtr(_In_opt_ TDerived *ptr) noexcept : m_ptr{ptr} {
  if (m_ptr) {
    const_cast<std::remove_const_t<T> *>(m_ptr)->AddRef();
  }
}

template <typename T>
template <typename TDerived, EnableIfIsBaseOf<T, TDerived>>
inline CntPtr<T>::CntPtr(_In_opt_ TDerived *ptr, AttachTagType) noexcept : m_ptr{ptr} {}

template <typename T>
inline CntPtr<T>::CntPtr(const CntPtr &other) noexcept : m_ptr{other.m_ptr} {
  if (m_ptr) {
    const_cast<std::remove_const_t<T> *>(m_ptr)->AddRef();
  }
}

template <typename T>
template <typename TDerived, EnableIfIsBaseOf<T, TDerived>>
inline CntPtr<T>::CntPtr(const Mso::CntPtr<TDerived> &other) noexcept : m_ptr{other.m_ptr} {
  if (m_ptr) {
    const_cast<std::remove_const_t<T> *>(m_ptr)->AddRef();
  }
}

template <typename T>
inline CntPtr<T>::CntPtr(CntPtr &&other) noexcept : m_ptr{other.m_ptr} {
  other.m_ptr = nullptr;
}

template <typename T>
template <typename TDerived, EnableIfIsBaseOf<T, TDerived>>
inline CntPtr<T>::CntPtr(CntPtr<TDerived> &&other) noexcept : m_ptr{other.m_ptr} {
  other.m_ptr = nullptr;
}

template <typename T>
inline CntPtr<T>::~CntPtr() noexcept {
  if (m_ptr) {
    T *oldPtr = m_ptr;
    m_ptr = nullptr;
    const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
  }
}

template <typename T>
inline CntPtr<T> &CntPtr<T>::operator=(std::nullptr_t) noexcept {
  if (m_ptr) {
    T *oldPtr = m_ptr;
    m_ptr = nullptr;
    const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
  }

  return *this;
}

template <typename T>
inline CntPtr<T> &CntPtr<T>::operator=(_In_opt_ T *ptr) noexcept {
  if (m_ptr != ptr) {
    if (ptr) {
      const_cast<std::remove_const_t<T> *>(ptr)->AddRef();
    }

    T *oldPtr = m_ptr;
    m_ptr = ptr;

    if (oldPtr) {
      const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
    }
  }

  return *this;
}

template <typename T>
template <typename TDerived, EnableIfIsBaseOf<T, TDerived>>
inline CntPtr<T> &CntPtr<T>::operator=(_In_opt_ TDerived *ptr) noexcept {
  if (m_ptr != ptr) {
    if (ptr) {
      const_cast<std::remove_const_t<TDerived> *>(ptr)->AddRef();
    }

    T *oldPtr = m_ptr;
    m_ptr = ptr;

    if (oldPtr) {
      const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
    }
  }

  return *this;
}

template <typename T>
inline CntPtr<T> &CntPtr<T>::operator=(const CntPtr<T> &other) noexcept {
  T *ptr = other.m_ptr;
  if (m_ptr != ptr) {
    if (ptr) {
      const_cast<std::remove_const_t<T> *>(ptr)->AddRef();
    }

    T *oldPtr = m_ptr;
    m_ptr = ptr;

    if (oldPtr) {
      const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
    }
  }

  return *this;
}

template <typename T>
template <typename TDerived, EnableIfIsBaseOf<T, TDerived>>
inline CntPtr<T> &CntPtr<T>::operator=(const CntPtr<TDerived> &other) noexcept {
  T *ptr = other.m_ptr;
  if (m_ptr != ptr) {
    if (ptr) {
      const_cast<std::remove_const_t<T> *>(ptr)->AddRef();
    }

    T *oldPtr = m_ptr;
    m_ptr = ptr;

    if (oldPtr) {
      const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
    }
  }

  return *this;
}

template <typename T>
inline CntPtr<T> &CntPtr<T>::operator=(CntPtr<T> &&other) noexcept {
  if (m_ptr != other.m_ptr) {
    T *oldPtr = m_ptr;
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;

    if (oldPtr) {
      const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
    }
  }

  return *this;
}

template <typename T>
template <typename TDerived, EnableIfIsBaseOf<T, TDerived>>
inline CntPtr<T> &CntPtr<T>::operator=(CntPtr<TDerived> &&other) noexcept {
  if (m_ptr != other.m_ptr) {
    T *oldPtr = m_ptr;
    m_ptr = other.m_ptr;
    other.m_ptr = nullptr;

    if (oldPtr) {
      const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
    }
  }

  return *this;
}

template <typename T>
inline CntPtr<T>::operator bool() const noexcept {
  return m_ptr != nullptr;
}

template <typename T>
inline bool CntPtr<T>::IsEmpty() const noexcept {
  return m_ptr == nullptr;
}

template <typename T>
inline void CntPtr<T>::Clear() noexcept {
  if (m_ptr) {
    T *oldPtr = m_ptr;
    m_ptr = nullptr;
    const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
  }
}

template <typename T>
inline T *CntPtr<T>::Get() const noexcept {
  return const_cast<CntPtr *>(this)->m_ptr;
}

template <typename T>
inline T *CntPtr<T>::Attach(T *ptr) noexcept {
  if (m_ptr != ptr) {
    T *oldPtr = m_ptr;
    m_ptr = ptr;

    if (oldPtr) {
      const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
    }
  }

  return m_ptr;
}

template <typename T>
inline T *CntPtr<T>::Detach() noexcept {
  T *ptr = m_ptr;
  m_ptr = nullptr;
  return ptr;
}

template <typename T>
inline T &CntPtr<T>::operator*() const noexcept {
  return *const_cast<CntPtr *>(this)->m_ptr;
}

template <typename T>
inline T *CntPtr<T>::operator->() const noexcept {
  return const_cast<CntPtr *>(this)->m_ptr;
}

template <typename T>
inline CntPtrRef<T> CntPtr<T>::operator&() noexcept {
  return {this};
}

template <typename T>
inline T **CntPtr<T>::ClearAndGetAddressOf() noexcept {
  if (m_ptr) {
    T *oldPtr = m_ptr;
    m_ptr = nullptr;
    const_cast<std::remove_const_t<T> *>(oldPtr)->Release();
  }

  return &const_cast<CntPtr *>(this)->m_ptr;
}

template <typename T>
inline T **CntPtr<T>::GetAddressOf() const noexcept {
  AssertSz(m_ptr == nullptr, "CntrPtr value must be null");
  return &const_cast<CntPtr *>(this)->m_ptr;
}

template <typename T>
inline T **CntPtr<T>::GetRaw() const noexcept {
  // This is dangerous so you better know what you are doing
  return &const_cast<CntPtr *>(this)->m_ptr;
}

template <typename T1, typename T2>
inline bool operator==(CntPtr<T1> const &left, CntPtr<T2> const &right) noexcept {
  return left.m_ptr == right.m_ptr;
}

template <typename T>
inline bool operator==(CntPtr<T> const &left, std::nullptr_t) noexcept {
  return left.m_ptr == nullptr;
}

template <typename T>
inline bool operator==(std::nullptr_t, CntPtr<T> const &right) noexcept {
  return right.m_ptr == nullptr;
}

template <typename T1, typename T2>
inline bool operator!=(CntPtr<T1> const &left, CntPtr<T2> const &right) noexcept {
  return left.m_ptr != right.m_ptr;
}

template <typename T>
inline bool operator!=(CntPtr<T> const &left, std::nullptr_t) noexcept {
  return left.m_ptr != nullptr;
}

template <typename T>
inline bool operator!=(std::nullptr_t, CntPtr<T> const &right) noexcept {
  return right.m_ptr != nullptr;
}

} // namespace Mso

namespace std {

template <typename T>
inline void swap(Mso::CntPtr<T> &left, Mso::CntPtr<T> &right) noexcept {
  T *temp = left.m_ptr;
  left.m_ptr = right.m_ptr;
  right.m_ptr = temp;
}

} // namespace std

//=============================================================================
// CntPtrRef inline implementation
//=============================================================================

namespace Mso {

template <typename T>
inline CntPtrRef<T>::CntPtrRef(_In_ CntPtr<T> *pCntPtr) noexcept : m_pCntPtr{pCntPtr} {}

template <typename T>
inline CntPtrRef<T>::operator CntPtr<T> *() const noexcept {
  return const_cast<CntPtrRef *>(this)->m_pCntPtr;
}

template <typename T>
inline CntPtrRef<T>::operator void *() const noexcept {
  return const_cast<CntPtrRef *>(this)->m_pCntPtr;
}

template <typename T>
inline CntPtrRef<T>::operator T * *() const noexcept {
  return const_cast<CntPtrRef *>(this)->m_pCntPtr->GetAddressOf();
}

template <typename T>
inline CntPtrRef<T>::operator void * *() const noexcept {
  return reinterpret_cast<void **>(const_cast<CntPtrRef *>(this)->m_pCntPtr->GetAddressOf());
}

template <typename T>
inline CntPtrRef<T>::operator const void * *() const noexcept {
  return (const void **)(const_cast<CntPtrRef *>(this)->m_pCntPtr->GetAddressOf());
}

template <typename T>
template <typename TBase, EnableIfIsBaseOf<TBase, T>>
inline CntPtrRef<T>::operator TBase * *() const noexcept {
  return (TBase **)const_cast<CntPtrRef *>(this)->m_pCntPtr->GetAddressOf();
}

template <typename T>
inline T *&CntPtrRef<T>::operator*() noexcept {
  return *(m_pCntPtr->GetAddressOf());
}

template <typename T>
inline T **CntPtrRef<T>::ClearAndGetAddressOf() noexcept {
  return m_pCntPtr->ClearAndGetAddressOf();
}

template <typename T>
inline T **CntPtrRef<T>::GetAddressOf() const noexcept {
  return m_pCntPtr->GetAddressOf();
}

template <typename T>
inline T **CntPtrRef<T>::GetRaw() const noexcept {
  return m_pCntPtr->GetRaw();
}

template <typename T1, typename T2>
inline bool operator==(const CntPtrRef<T1> &left, const CntPtrRef<T2> &right) noexcept {
  return left.m_pCntPtr == right.m_pCntPtr;
}

template <typename T1, typename T2>
inline bool operator!=(const CntPtrRef<T1> &left, const CntPtrRef<T2> &right) noexcept {
  return left.m_pCntPtr != right.m_pCntPtr;
}

} // namespace Mso

#endif // MSO_SMARTPTR_CNTPTR_H
