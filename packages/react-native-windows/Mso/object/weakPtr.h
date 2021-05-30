// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_OBJECT_WEAKPTR_H
#define MSO_OBJECT_WEAKPTR_H

#include "object/objectWithWeakRef.h"
#include "object/queryCast.h"

namespace Mso {

template <class T>
class WeakPtr;

namespace Details {

class WeakPtrBase {
 public:
  void Reset() noexcept {
    CheckedReleaseWeakRef();
    m_ptr = nullptr;
    m_weakRef = nullptr;
  }

  bool IsEmpty() const noexcept {
    return m_ptr == nullptr;
  }

  bool IsExpired() const noexcept {
    return !m_ptr || m_weakRef->IsExpired();
  }

  template <class T>
  static Mso::ObjectWeakRef *GetWeakRef(_In_opt_ T *from) noexcept {
    return GetWeakRefCore(from, 0, 0);
  }

 protected:
  enum { AttachTag = 0 };

  WeakPtrBase() noexcept : m_ptr(nullptr), m_weakRef(nullptr) {}

  WeakPtrBase(_In_opt_ void *ptr, _In_opt_ Mso::ObjectWeakRef *weakRef) noexcept
      : m_ptr(ptr), m_weakRef(ptr ? weakRef : nullptr) {
    CheckedAddWeakRef();
  }

  WeakPtrBase(_In_opt_ void *ptr, _In_opt_ Mso::ObjectWeakRef *weakRef, int /*attachTag*/) noexcept
      : m_ptr(ptr), m_weakRef(weakRef) {}

  ~WeakPtrBase() noexcept {
    CheckedReleaseWeakRef();
  }

  void Assign(_In_opt_ void *ptr, _In_opt_ ObjectWeakRef *weakRef) noexcept {
    CheckedReleaseWeakRef();
    m_ptr = ptr;
    m_weakRef = weakRef;
    CheckedAddWeakRef();
  }

  void Assign(_In_opt_ void *ptr, _In_opt_ ObjectWeakRef *weakRef, int /*attachTag*/) noexcept {
    CheckedReleaseWeakRef();
    m_ptr = ptr;
    m_weakRef = weakRef;
  }

  bool IncrementRefCountIfNotZero() const noexcept {
    return m_weakRef && m_weakRef->IncrementRefCountIfNotZero();
  }

 private:
  void CheckedAddWeakRef() const noexcept {
    if (m_weakRef)
      m_weakRef->AddWeakRef();
  }

  void CheckedReleaseWeakRef() const noexcept {
    if (m_weakRef)
      m_weakRef->ReleaseWeakRef();
  }

  // Called when T has GetWeakRef method.
  template <class T>
  static auto GetWeakRefCore(_In_opt_ T *from, int, int) noexcept -> decltype(&from->GetWeakRef()) {
    if (from) {
      Mso::ObjectWeakRef *result = &from->GetWeakRef();
      VerifyElseCrashSzTag(result, "GetWeakRef() returned null", 0x0100371a /* tag_bad20 */);
      return result;
    }

    return nullptr;
  }

  // Called when T has QueryInterface method.
  template <class T>
  static auto GetWeakRefCore(_In_opt_ T *from, int, ...) noexcept
      -> decltype(from->QueryInterface(__uuidof(Mso::ObjectWeakRef), nullptr), (Mso::ObjectWeakRef *)nullptr) {
    if (from) {
      Mso::ObjectWeakRef *result = query_cast<Mso::ObjectWeakRef *>(from);
      VerifyElseCrashSzTag(result, "query_cast<Mso::ObjectWeakRef*>() returned null", 0x0100371b /* tag_bad21 */);
      return result;
    }

    return nullptr;
  }

  // Called when T does not have GetWeakRef method.
  template <class T>
  static Mso::ObjectWeakRef *GetWeakRefCore(_In_opt_ T * /*from*/, ...) noexcept {
    VerifyElseCrashSzTag(false, "Cannot get ObjectWeakRef for an object.", 0x0100371c /* tag_bad22 */);
    return nullptr;
  }

  friend const void *GetUnsafePtr(const WeakPtrBase &weakPtrBase) noexcept {
    return weakPtrBase.m_ptr;
  }

 private:
  void *m_ptr;
  Mso::ObjectWeakRef *m_weakRef;

  template <class T>
  friend class Mso::WeakPtr;
};

} // namespace Details

/**
  A weak pointer based on the ObjectWeakRef.
  It has two pointers: a pointer to the ObjectWeakRef and a pointer to the object.
  Having two pointers allow us to address the following scenarios:
  - We do not want to do any casting when retrieving a strong pointer.
  - Multiple objects share the same ObjectWeakRef.
*/
template <class T>
class WeakPtr final : public Details::WeakPtrBase {
  using Super = Details::WeakPtrBase;

 public:
  WeakPtr() noexcept : Super() {}
  WeakPtr(std::nullptr_t) noexcept : Super() {}

  WeakPtr(_In_opt_ T *from, _In_opt_ Mso::ObjectWeakRef *weakRef = nullptr) noexcept
      : Super(from, weakRef ? weakRef : GetWeakRefCore(from, 0, 0)) {}

  template <class TOther, class = typename std::enable_if<std::is_convertible<TOther *, T *>::value>::type>
  WeakPtr(_In_opt_ TOther *from, _In_opt_ Mso::ObjectWeakRef *weakRef = nullptr) noexcept
      : Super(static_cast<T *>(from), weakRef ? weakRef : GetWeakRefCore(from, 0, 0)) {}

  WeakPtr(const Mso::CntPtr<T> &from, _In_opt_ Mso::ObjectWeakRef *weakRef = nullptr) noexcept
      : Super(from.Get(), weakRef ? weakRef : GetWeakRefCore(from.Get(), 0, 0)) {}

  template <class TOther, class = typename std::enable_if<std::is_convertible<TOther *, T *>::value>::type>
  WeakPtr(const Mso::CntPtr<TOther> &from, _In_opt_ Mso::ObjectWeakRef *weakRef = nullptr) noexcept
      : Super(static_cast<T *>(from.Get()), weakRef ? weakRef : GetWeakRefCore(from.Get(), 0, 0)) {}

  WeakPtr(const WeakPtr &from) noexcept : Super(from.m_ptr, from.m_weakRef) {}

  template <class TOther, class = typename std::enable_if<std::is_convertible<TOther *, T *>::value>::type>
  WeakPtr(const WeakPtr<TOther> &from) noexcept
      : Super(static_cast<T *>(static_cast<TOther *>(from.m_ptr)), from.m_weakRef) {}

  WeakPtr(WeakPtr &&from) noexcept : Super(from.m_ptr, from.m_weakRef, AttachTag) {
    from.m_ptr = nullptr;
    from.m_weakRef = nullptr;
  }

  template <class TOther, class = typename std::enable_if<std::is_convertible<TOther *, T *>::value>::type>
  WeakPtr(WeakPtr<TOther> &&from) noexcept
      : Super(static_cast<T *>(static_cast<TOther *>(from.m_ptr)), from.m_weakRef, AttachTag) {
    from.m_ptr = nullptr;
    from.m_weakRef = nullptr;
  }

  WeakPtr &operator=(std::nullptr_t) noexcept {
    Reset();
    return *this;
  }

  WeakPtr &operator=(_In_opt_ T *from) noexcept {
    if (m_ptr != from) {
      Assign(from, GetWeakRefCore(from, 0, 0));
    }

    return *this;
  }

  template <class TOther, class = typename std::enable_if<std::is_convertible<TOther *, T *>::value>::type>
  WeakPtr &operator=(_In_opt_ TOther *from) noexcept {
    T *ptr = static_cast<T *>(from);
    if (m_ptr != ptr) {
      Assign(ptr, GetWeakRefCore(from, 0, 0));
    }

    return *this;
  }

  WeakPtr &operator=(const Mso::CntPtr<T> &from) noexcept {
    T *ptr = from.Get();
    if (m_ptr != ptr) {
      Assign(ptr, GetWeakRefCore(ptr, 0, 0));
    }

    return *this;
  }

  template <class TOther, class = typename std::enable_if<std::is_convertible<TOther *, T *>::value>::type>
  WeakPtr &operator=(const Mso::CntPtr<TOther> &from) noexcept {
    TOther *otherPtr = from.Get();
    T *ptr = static_cast<T *>(otherPtr);
    if (m_ptr != ptr) {
      Assign(ptr, GetWeakRefCore(otherPtr, 0, 0));
    }

    return *this;
  }

  WeakPtr &operator=(const WeakPtr &from) noexcept {
    if (m_ptr != from.m_ptr) {
      Assign(from.m_ptr, from.m_weakRef);
    }

    return *this;
  }

  template <class TOther, class = typename std::enable_if<std::is_convertible<TOther *, T *>::value>::type>
  WeakPtr &operator=(const WeakPtr<TOther> &from) noexcept {
    T *ptr = static_cast<T *>(static_cast<TOther *>(from.m_ptr));
    if (m_ptr != ptr) {
      Assign(ptr, from.m_weakRef);
    }

    return *this;
  }

  WeakPtr &operator=(WeakPtr &&from) noexcept {
    if (m_ptr != from.m_ptr) {
      Assign(from.m_ptr, from.m_weakRef, AttachTag);
      from.m_ptr = nullptr;
      from.m_weakRef = nullptr;
    }

    return *this;
  }

  template <class TOther, class = typename std::enable_if<std::is_convertible<TOther *, T *>::value>::type>
  WeakPtr &operator=(WeakPtr<TOther> &&from) noexcept {
    T *ptr = static_cast<T *>(static_cast<TOther *>(from.m_ptr));
    if (m_ptr != ptr) {
      Assign(ptr, from.m_weakRef, AttachTag);
      from.m_ptr = nullptr;
      from.m_weakRef = nullptr;
    }

    return *this;
  }

  Mso::CntPtr<T> GetStrongPtr() const noexcept {
    Mso::CntPtr<T> ptr;
    if (IncrementRefCountIfNotZero()) {
      *ptr.GetRaw() = static_cast<T *>(m_ptr);
    }

    return ptr;
  }

  void Swap(WeakPtr &other) noexcept {
    using std::swap;
    std::swap(m_ptr, other.m_ptr);
    std::swap(m_weakRef, other.m_weakRef);
  }
};

template <typename T1, typename T2>
bool operator==(const WeakPtr<T1> &left, const WeakPtr<T2> &right) noexcept {
  return GetUnsafePtr(left) == GetUnsafePtr(right);
}

template <typename T>
bool operator==(const WeakPtr<T> &left, std::nullptr_t) noexcept {
  return left.IsEmpty();
}

template <typename T>
bool operator==(std::nullptr_t, const WeakPtr<T> &right) noexcept {
  return right.IsEmpty();
}

template <typename T1, typename T2>
bool operator!=(const WeakPtr<T1> &left, const WeakPtr<T2> &right) noexcept {
  return GetUnsafePtr(left) != GetUnsafePtr(right);
}

template <typename T>
bool operator!=(const WeakPtr<T> &left, std::nullptr_t) noexcept {
  return !left.IsEmpty();
}

template <typename T>
bool operator!=(std::nullptr_t, const WeakPtr<T> &right) noexcept {
  return !right.IsEmpty();
}

} // namespace Mso

#endif // MSO_OBJECT_WEAKPTR_H
