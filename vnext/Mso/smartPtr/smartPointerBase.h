// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_SMARTPTR_SMARTPOINTERBASE_H
#define MSO_SMARTPTR_SMARTPOINTERBASE_H

#include "compilerAdapters/cppMacros.h"
#include "crash/verifyElseCrash.h"
#include "debugAssertApi/debugAssertApi.h"
#include "typeTraits/typeTraits.h"

#pragma warning(push)
#pragma warning(disable : 4996) // wmemcpy
#include <utility>
#pragma warning(pop)

namespace Mso {

/**
  This base class manages the lifetime of various data types using a THelper
  struct for resource management. It can be used with scalar and pointer
  types and many strongly-typed classes have been defined later down.

  THelper class must define 1 method:

  1) static void Free(T pT) noexcept - this is called by the THolder class
          to free the type. This will never be called with an empty value.

  TEmptyTraits class can be overridden for special 'empty' behaviors.
*/
template <typename T, typename THelper, typename TEmptyTraits = Mso::EmptyTraits<T>>
class THolder {
  using _Myt = THolder<T, THelper, TEmptyTraits>;

 public:
  using TArrowType = typename Mso::RawTraits<T>::ArrowType;
  using TAddrType = typename Mso::RawTraits<T>::AddrType;
  using TRefType = typename std::conditional<
      std::is_same<std::remove_pointer_t<TArrowType>, void>::value,
      Mso::NilType,
      std::remove_pointer_t<TArrowType>>::type;

  // TODO: consider allowing this via extension of THelper?
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(THolder);

  /**
          Construction / Destruction
  */
  THolder() noexcept : m_pT(TEmptyTraits::EmptyVal()) {}
  /*explicit*/ THolder(T pT) noexcept : m_pT(pT) {}

  __forceinline THolder(_Inout_ THolder &&rFrom) noexcept : m_pT(rFrom.Detach()) {}
  template <typename T1>
  __forceinline THolder(_Inout_ THolder<T1, THelper, TEmptyTraits> &&rFrom) noexcept : m_pT(rFrom.Detach()) {}

  ~THolder() noexcept {
    EmptySafe();
  }

  /**
          explicit delete owned object
  */
  void Clear() noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    if (IsEmpty())
      return;

    // Using Detach ensures we don't double-free on recursion
    T t = Detach();
    THelper::Free(t);
  }

  // STL equivalent
  void clear() noexcept {
    this->Clear();
  } // equivalent to std::string::clear()
  void reset() noexcept {
    this->Clear();
  } // equivalent to std::unique_ptr::reset(), excpet for not supporting (yet) passing a new pointer in (like Attach
    // here).

  /**
          Empty check
  */
  bool IsEmpty() const noexcept {
    return TEmptyTraits::IsEmpty(m_pT);
  }

  explicit operator bool() const noexcept {
    return !IsEmpty();
  }

  /**
    Access the contained data
  */
  T Get() const noexcept {
    return m_pT;
  }

  // STL equivalent
  T get() const noexcept {
    return this->Get();
  }

  TRefType &operator[](ptrdiff_t iSubscript) noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    return m_pT[iSubscript];
  }

  TArrowType operator->() const noexcept {
    VerifyElseCrashTag(!IsEmpty(), 0x008c2697 /* tag_a9c0x */);
    typedef THolder<T, THelper, TEmptyTraits> &_self;
    return Mso::RawTraits<T>::GetArrowType(const_cast<_self>(*this).m_pT);
  }

  const THolder &operator=(T pT) noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    Clear();
    m_pT = pT;
    return *this;
  }

  THolder &operator=(_Inout_ THolder &&rFrom) noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    // REVIEW: Do I need to check pT != m_pT?
    TransferFrom(rFrom);
    return *this;
  }

  template <typename T1>
  THolder &operator=(_Inout_ THolder<T1, THelper, TEmptyTraits> &&rFrom) noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    // REVIEW: Do I need to check pT != m_pT?
    TransferFrom(rFrom);
    return *this;
  }

  void Swap(THolder &from) noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    T pT = m_pT;
    m_pT = from.m_pT;
    from.m_pT = pT;
  }

  // STL equivalent
  void swap(THolder &from) noexcept {
    Swap(from);
  }

  template <typename T1, typename THelper1, typename TEmptyTraits1>
  void TransferFrom(_Inout_ THolder<T1, THelper1, TEmptyTraits1> &from) noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    Attach(from.Detach());
  }

  /**
    take object ownership, m_pT must be empty
  */
  T Place(T pT) noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    AssertTag(IsEmpty(), 0x008c2698 /* tag_a9c0y */);
    m_pT = pT;
    return m_pT;
  }

  /**
    take object ownership, deletes previously owned object if any
  */
  T Attach(T pT) noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    Clear();
    return Place(pT);
  }

  /**
    release ownership without deleting object
  */
  T Detach() noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    T pT = m_pT;
    TEmptyTraits::Empty(m_pT);
    return pT;
  }

  // STL equivalent
  T release() noexcept {
    return this->Detach();
  } // equivalent to std::unique_ptr::release()

#ifndef MSO_THOLDER_NO_ADDR_OPERATOR
  /**
    & operator to retrieve object, THolder must be empty
  */
  TAddrType operator&() noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    AssertTag(IsEmpty(), 0x008c2699 /* tag_a9c0z */);
    return &m_pT;
  }
#endif // !MSO_THOLDER_NO_ADDR_OPERATOR

  /**
    Retrieves the address of the object. Asserts that THolder is empty.

    void GetSomething(T** ppT);

    Mso::THolder<T> pT;
    GetSomething(pT.GetAddressOf());

    ClearAndGetAddressOf() will ensure any existing object is cleared first.
    GetRaw() avoids the assert if you need the address of an existing object.
  */
  TAddrType GetAddressOf() noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    AssertSzTag(
        IsEmpty(),
        "Getting the address of an existing object? This usually leads to a leak.",
        0x008c269a /* tag_a9c00 */);
    return GetRaw();
  }

  TAddrType ClearAndGetAddressOf() noexcept {
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    Clear();
    return GetRaw();
  }

  TAddrType GetRaw() noexcept {
    // This is dangerous so you better know what you are doing
    static_assert(!Mso::RawTraits<T>::isReference, "Not allowed when T is a reference type");
    return &m_pT;
  }

  /**
    Sometimes the pointer to the actual THolder is needed
  */
  const THolder *GetThis() const noexcept {
    return this;
  }
  THolder *GetThis() noexcept {
    return this;
  }

 protected:
  T m_pT;

 private:
  // Handles cases where T is a reference type - must not be exposed to clients.
  void EmptySafe() noexcept {
    if (IsEmpty())
      return;

    T pT = m_pT;
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#pragma warning(suppress : 4996) // deprecated function
    TEmptyTraits::UnsafeEmpty(m_pT);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    THelper::Free(pT);
  }

  // TODO: consider allowing this via extension of THelper?
  template <typename T1>
  const THolder &operator=(const THolder<T1, THelper, TEmptyTraits> &from);

  // Can't mix helpers
  template <typename T1, typename THelper1, typename TEmptyTraits1>
  const THolder &operator=(const THolder<T1, THelper1, TEmptyTraits1> &from);
  template <typename T1, typename THelper1, typename TEmptyTraits1>
  THolder &operator=(_Inout_ THolder<T1, THelper1, TEmptyTraits1> &&rFrom);
  template <typename T1, typename THelper1, typename TEmptyTraits1>
  void Place(const THolder<T1, THelper1, TEmptyTraits1> &);
  template <typename T1, typename THelper1, typename TEmptyTraits1>
  void Attach(const THolder<T1, THelper1, TEmptyTraits1> &);
  template <typename T1, typename THelper1, typename TEmptyTraits1>
  void Transfer(const THolder<T1, THelper1, TEmptyTraits1> &);

  // Improper usage
  template <typename T1>
  void Place(const THolder<T1, THelper, TEmptyTraits> &); // use Transfer
  template <typename T1>
  void Attach(const THolder<T1, THelper, TEmptyTraits> &); // use Transfer
}; // class THolder

/**
  Operators for THolder
*/
template <
    typename T1,
    typename THelper1,
    typename TEmptyTraits1,
    typename T2,
    typename THelper2,
    typename TEmptyTraits2>
bool operator==(const THolder<T1, THelper1, TEmptyTraits1> &a, const THolder<T2, THelper2, TEmptyTraits2> &b) noexcept {
  return a.Get() == b.Get();
}

template <typename T1, typename THelper1, typename TEmptyTraits1>
bool operator==(const THolder<T1, THelper1, TEmptyTraits1> &a, decltype(__nullptr)) noexcept {
  return a.Get() == nullptr;
}

template <typename T1, typename THelper1, typename TEmptyTraits1>
bool operator==(decltype(__nullptr), const THolder<T1, THelper1, TEmptyTraits1> &a) noexcept {
  return a.Get() == nullptr;
}

template <
    typename T1,
    typename THelper1,
    typename TEmptyTraits1,
    typename T2,
    typename THelper2,
    typename TEmptyTraits2>
bool operator!=(const THolder<T1, THelper1, TEmptyTraits1> &a, const THolder<T2, THelper2, TEmptyTraits2> &b) noexcept {
  return a.Get() != b.Get();
}

template <typename T1, typename THelper1, typename TEmptyTraits1>
bool operator!=(const THolder<T1, THelper1, TEmptyTraits1> &a, decltype(__nullptr)) noexcept {
  return a.Get() != nullptr;
}

template <typename T1, typename THelper1, typename TEmptyTraits1>
bool operator!=(decltype(__nullptr), const THolder<T1, THelper1, TEmptyTraits1> &a) noexcept {
  return a.Get() != nullptr;
}

/**
        Macros to implement a few basic THolder methods in derived classes.
*/

/**
        Helper to define operator= in derived classes
*/
#define IMPLEMENT_THOLDER_OPERATOR_EQUALS(T)    \
  template <typename T1>                        \
  const T &operator=(_In_opt_ T1 pT) noexcept { \
    Super::operator=(pT);                       \
    return *this;                               \
  }

/**
        Helper to add RVALUE methods to derived THolder classes
*/
#define IMPLEMENT_THOLDER_RVALUE_REFS_(T, TBase)                       \
  T(_Inout_ T &&rFrom) noexcept : TBase(std::forward<TBase>(rFrom)) {} \
  T &operator=(_Inout_ T &&rFrom) noexcept {                           \
    this->TransferFrom(rFrom);                                         \
    return *this;                                                      \
  }
#define IMPLEMENT_THOLDER_RVALUE_REFS(T) IMPLEMENT_THOLDER_RVALUE_REFS_(T, Super)

/**
        Try to prevent mixing up constructors
*/
#define PREVENT_MISMATCH_THOLDER_CONSTRUCTORS(T) \
  template <typename T1, typename THelper>       \
  T(const THolder<T1, THelper> &ref)             \
  noexcept;

/**
        In some cases, additional data must be stored with the held object.
        THolderPair supports this. Note that no requirements are placed on the
        associated data although using a pointer type is strongly encouraged.
*/

/**
        THolderPairData stores a type and associated data
*/
template <typename T, typename TData>
struct THolderPairData {
 public:
  operator T() const noexcept {
    return pT;
  }
  T operator->() const noexcept {
    VerifyElseCrashTag(pT != nullptr, 0x008c288a /* tag_a9c8k */);
    return pT;
  }
  bool operator==(const THolderPairData &other) const {
    return (pT == other.pT && pData == other.pData);
  }
  bool operator!=(const THolderPairData &other) const {
    return !(*this == other);
  }

  T pT;
  TData pData;
};

/**
        EmptyTraits for the THolderPairData
*/
template <typename S, typename SData>
struct EmptyTraits<THolderPairData<S, SData>> {
  static THolderPairData<S, SData> EmptyVal() noexcept {
    THolderPairData<S, SData> sEmpty = {0};
    return sEmpty;
  }
  static bool IsEmpty(const THolderPairData<S, SData> &t) noexcept {
    return Mso::EmptyTraits<S>::IsEmpty(t.pT);
  }
  static void Empty(THolderPairData<S, SData> &t) noexcept {
    Mso::EmptyTraits<S>::Empty(t.pT);
    Mso::EmptyTraits<SData>::Empty(t.pData);
  }
  __declspec(deprecated) static void UnsafeEmpty(THolderPairData<S, SData> &t) noexcept {
    Empty(t);
  }
};

/**
        Smart pointer for holding a pair of data objects.
        typename T must be a pointer types
        Private THolder derivation avoids confusion to consumers.
*/
template <typename T, typename TData, typename THelper>
class THolderPair : protected THolder<THolderPairData<T, TData>, THelper> {
  using _Myt = THolderPair<T, TData, THelper>;

 public:
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(THolderPair);

  using Super = THolder<THolderPairData<T, TData>, THelper>;
  using TRefType = typename std::
      conditional<std::is_same<std::remove_pointer_t<T>, void>::value, Mso::NilType, std::remove_pointer_t<T>>::type;

  THolderPair() noexcept {}
  explicit THolderPair(T pT, _In_opt_ TData pData) noexcept {
    Attach(pT, pData);
  }
  THolderPair(_Inout_ THolderPair &&rFrom) noexcept : Super(std::forward<Super>(rFrom)) {}
  THolderPair &operator=(_Inout_ T &&rFrom) noexcept {
    TransferFrom(rFrom);
    return *this;
  }

  /**
          Promote basic methods from the base class
  */
  void Clear() noexcept {
    return Super::Clear();
  }
  void clear() noexcept {
    return Super::Clear();
  }
  bool IsEmpty() const noexcept {
    return Super::IsEmpty();
  }

  T Get() const noexcept {
    return Super::Get();
  }
  T get() const noexcept {
    return Super::get();
  }

  explicit operator bool() const noexcept {
    return Super::operator bool();
  }

  TRefType &operator[](ptrdiff_t iSubscript) noexcept {
    return Super::Get()[iSubscript];
  }

  T operator->() const noexcept {
    return Super::operator->();
  }

  void Swap(THolderPair &from) noexcept {
    return Super::Swap(from);
  }

  T Place(T pT, _In_opt_ TData pData) noexcept {
    THolderPairData<T, TData> pair = {pT, pData};
    return Super::Place(pair);
  }

  void Attach(T pT, _In_opt_ TData pData) noexcept {
    THolderPairData<T, TData> pair = {pT, pData};
    Super::Attach(pair);
  }

  T Detach(_Out_opt_ TData *pDataOut = nullptr) noexcept {
    if (pDataOut != nullptr)
      *pDataOut = this->m_pT.pData;
    return Super::Detach();
  }

  T *GetAddressOf(_In_opt_ TData pData) noexcept {
    AssertSzTag(
        IsEmpty(),
        "Getting the address of an existing object? This usually leads to a leak.",
        0x008c269c /* tag_a9c02 */);
    this->m_pT.pData = pData;
    return &this->m_pT.pT;
  }

  T *ClearAndGetAddressOf(_In_opt_ TData pData) noexcept {
    Clear();
    return GetAddressOf(pData);
  }

  /**
    GetData provides access to the data pair.
  */
  TData GetData() const noexcept {
    return this->m_pT.pData;
  }
};

/**
  Operators for THolderPair
*/
template <typename T1, typename TData1, typename THelper1, typename T2, typename TData2, typename THelper2>
bool operator==(const THolderPair<T1, TData1, THelper1> &a, const THolderPair<T2, TData2, THelper2> &b) noexcept {
  return a.Get() == b.Get();
}

template <typename T1, typename TData1, typename THelper1>
bool operator==(const THolderPair<T1, TData1, THelper1> &a, decltype(__nullptr)) noexcept {
  return a.Get() == nullptr;
}

template <typename T1, typename TData1, typename THelper1>
bool operator==(decltype(__nullptr), const THolderPair<T1, TData1, THelper1> &a) noexcept {
  return a.Get() == nullptr;
}

template <typename T1, typename TData1, typename THelper1, typename T2, typename TData2, typename THelper2>
bool operator!=(const THolderPair<T1, TData1, THelper1> &a, const THolderPair<T2, THelper2, THelper2> &b) noexcept {
  return a.Get() != b.Get();
}

template <typename T1, typename TData1, typename THelper1>
bool operator!=(const THolderPair<T1, TData1, THelper1> &a, decltype(__nullptr)) noexcept {
  return a.Get() != nullptr;
}

template <typename T1, typename TData1, typename THelper1>
bool operator!=(decltype(__nullptr), const THolderPair<T1, TData1, THelper1> &a) noexcept {
  return a.Get() != nullptr;
}

} // namespace Mso

#endif // MSO_SMARTPTR_SMARTPOINTERBASE_H
