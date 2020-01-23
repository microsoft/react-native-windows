// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
This file contains the basic set of allocation APIs that anyone can use:
- Non-throwing Mso::Memory allocation APIs
- Allocation flags
- Mso::MemoryPtr smart pointer for Mso::Memory APIs

See MsoMemory.h for information about operator new.
*/
#pragma once
#ifndef MSO_MEMORYAPI_MEMORYAPI_H
#define MSO_MEMORYAPI_MEMORYAPI_H

#ifdef __cplusplus

#include <memory>
#include "compilerAdapters/cppMacrosDebug.h"
#include "compilerAdapters/functionDecorations.h"
#include "crash/verifyElseCrash.h"
#include "debugAssertApi/debugAssertApi.h"
#include "memoryApi/memoryLeakScope.h"
#include "oacr/oacr.h"
#include "smartPtr/smartPointerBase.h"

namespace Mso {
namespace Memory {
namespace Details {
template <typename T>
struct Emplacer {
  template <typename... Args>
  static void Place(_Inout_updates_bytes_all_(sizeof(T)) void *mem, Args &&... args) noexcept {
    new (mem) T(std::forward<Args>(args)...);
  }
};

template <typename T, size_t N>
struct Emplacer<T[N]> {
  template <typename... Args>
  static void Place(void *mem, Args &&... args) {
    new (mem) T[N]{std::forward<Args>(args)...};
  }
};

template <typename T, typename Enable = void>
struct Destructor {
  static void Destruct(T &) noexcept { /* noop */
  }
};

template <typename T>
struct Destructor<T, typename std::enable_if<std::is_destructible<T>::value && !std::is_pod<T>::value>::type> {
  static void Destruct(T &obj) noexcept {
    UNREFERENCED_PARAMETER(obj);
    obj.~T();
  }
};
} // namespace Details

/**
Mso::Memory::AllocFlags
*/
namespace AllocFlags {
enum Enum : unsigned int {
  // check this memory during shutdown leak detection
  ShutdownLeak = 0x0001,

  // ignore this memory during leak detection
  IgnoreLeak = 0x0002,

  // track this memory using memory marking / idle time leak detection
  MarkingLeak = 0x0004,
};
};

/**
Return a new allocation of the requested size (cb)
Returns nullptr on failure
*/
LIBLET_PUBLICAPI_EX("win", "android")
_Ret_maybenull_ _Post_writable_byte_size_(cb) void *AllocateEx(size_t cb, uint32_t allocFlags) noexcept;

/**
Return a new allocation of the requested size (cb)
Returns nullptr on failure
*/
inline _Ret_maybenull_ _Post_writable_byte_size_(cb) void *Allocate(size_t cb) noexcept {
  return Mso::Memory::AllocateEx(cb, 0);
}

/**
Reallocate an existing allocation to a new size
Returns nullptr on failure
TODO: Do we need ReallocateEx? Only if allocFlags grows.
*/
LIBLET_PUBLICAPI_EX("win", "android") _Ret_maybenull_ void *Reallocate(_Inout_ void **ppv, size_t cb) noexcept;

template <typename T>
_Ret_maybenull_ T *Reallocate(T *pT, size_t cb) noexcept {
  return reinterpret_cast<T *>(Reallocate(reinterpret_cast<void **>(&pT), cb));
}

/**
Return the allocation size of a given pointer
*/
LIBLET_PUBLICAPI_EX("win", "android") size_t AllocationSize(_In_opt_ const void *pv) noexcept;

/**
Release a previously allocated block of memory
*/
LIBLET_PUBLICAPI_EX("win", "android") void Free(_Pre_maybenull_ _Post_invalid_ void *pv) noexcept;

/**
Disambiguator used to ensure a throwing new
new (Mso::Memory::throwNew) Zoo();
*/
OACR_WARNING_SUPPRESS(SPECIFY_SELECTANY, "Not needed for marker type")
static const struct throwNew_t { throwNew_t() noexcept = default; } throwNew;

/**
Disambiguator used to ensure a crashing new
new (Mso::Memory::failFast) Zoo();
*/
OACR_WARNING_SUPPRESS(SPECIFY_SELECTANY, "Not needed for marker type")
static const struct failFast_t { failFast_t() noexcept = default; } failFast;

/**
Construct a object of type `T` stored at `mem`.

Arguments are forwarded to constructor of `T`.
*/
template <typename T, typename... Args>
static void Place(__inout_bcount(sizeof(T)) void *mem, Args &&... args) {
  Details::Emplacer<T>::Place(mem, std::forward<Args>(args)...);
}

/**
Manually destruct an object of type `T`.
*/
template <typename T, typename = typename std::enable_if<!std::is_array<T>::value>::type>
static void Destruct(T &obj) noexcept {
  Details::Destructor<T>::Destruct(obj);
}

/**
Manually destruct a `T[N]`.

Destructs contents of the array in reverse order.
*/
template <typename T, size_t N>
static void Destruct(T (&obj)[N]) noexcept {
  size_t i = N;
  while (i--)
    Destruct(obj[i]);
}
} // namespace Memory
} // namespace Mso

namespace Mso {
namespace Memory {
namespace FailFast {
/**
Return a new allocation of the requested size (cb)
Never returns nullptr
*/
_Ret_maybenull_ _Post_writable_byte_size_(cb) inline void *AllocateEx(size_t cb, uint32_t allocFlags) noexcept {
  auto pv = Mso::Memory::AllocateEx(cb, allocFlags);
  if (pv == nullptr)
    CrashWithRecoveryOnOOM();
  return pv;
}

_Ret_maybenull_ _Post_writable_byte_size_(cb) inline void *Allocate(size_t cb) noexcept {
  return AllocateEx(cb, 0);
}
} // namespace FailFast
} // namespace Memory
} // namespace Mso

namespace Mso {
namespace Memory {
namespace NoThrow {

/**
        non-throwing operator new using memory-marking leak detection
        new (Mso::Memory::NoThrow::MarkingLeak) Zoo();
*/
OACR_WARNING_SUPPRESS(SPECIFY_SELECTANY, "Not needed for marker type")
static const struct MarkingLeak_t { MarkingLeak_t() noexcept = default; } MarkingLeak;

} // namespace NoThrow
} // namespace Memory
} // namespace Mso

namespace Mso {
namespace Memory {
namespace Throw {
#pragma warning(push)
#pragma warning(disable : 4100)
/**
Mso::Memory::Throw::New<T>(args)

Allocates object T by passing args to its constructor.
*/
template <typename T, typename... TArgs>
OACR_WARNING_SUPPRESS(NULL_ON_NON_POINTER, "false positive")
_Ret_notnull_ T *New(TArgs &&... t) {
  Debug(Mso::Memory::AutoShutdownLeakScope scope);
  T *pT = new (std::nothrow) T(std::forward<TArgs>(t)...);
  if (pT == nullptr)
    throw std::bad_alloc();
  return pT;
}
#pragma warning(pop)
} // namespace Throw
} // namespace Memory
} // namespace Mso

namespace Mso {
/**
Frees memory allocated by Mso::Memory
*/
template <typename T>
struct MemoryPtrHelper {
  static void Free(T *pT) noexcept {
    Mso::Memory::Free(pT);
  }
};

/**
Mso::MemoryPtr

Smart pointer for memory allocated by Mso::Memory.
Suitable for raw memory or vanilla structs.
Does not run copy constructors or destructors.

The equals operator is purposely left out. Use the c'tor or Attach().

Mso::MemoryPtr<BYTE> pFoo; // equivalent to BYTE* pFoo;
*/
template <typename T, uint32_t allocFlags = 0>
class MemoryPtr : public Mso::THolder<T *, MemoryPtrHelper<T>> {
  typedef Mso::THolder<T *, MemoryPtrHelper<T>> Super;

 public:
  MemoryPtr() noexcept {}
  explicit MemoryPtr(_In_opt_ T *pT) noexcept {
    this->Attach(pT);
  } // Takes ownership
  IMPLEMENT_THOLDER_RVALUE_REFS_(MemoryPtr, Super);

  /**
  AllocBytes

  Allocate space for the given number of bytes.
  On success, any previously held data is replaced.
  Returns false on OOM / overflow.

  AllocElem-variants are strongly preferred to avoid integer overflows.

  Mso::MemoryPtr<BYTE> pbFoo;
  if (pbFoo.AllocBytes(cbData)) { ... }
  */
  bool AllocBytes(size_t cb) noexcept {
    T *pT = static_cast<T *>(Mso::Memory::AllocateEx(cb, allocFlags));
    if (pT != nullptr)
      this->Attach(pT);
    return (pT != nullptr);
  }

  /**
  AllocElem

  Allocate space for cElements + cExtra instances.
  On success, any previously held data is replaced.
  Optional out parameters to receive the number of elements/bytes allocated
  Returns false on OOM / overflow.

  Mso::MemoryPtr<wchar_t> wzFoo;
  size_t cchFoo = 0;
  if (wzFoo.AllocElemCb(cch, 1, &cchFoo)) { ... }
  */
  bool AllocElem(
      size_t cElements,
      size_t cExtra = 0,
      _Out_opt_ size_t *pcElements = nullptr,
      _Out_opt_ size_t *pcbAlloc = nullptr) noexcept {
    // TODO: size_t cbAlloc = (msl::utilities::SafeInt<size_t>(cElements) +cExtra) * sizeof(T);
    size_t cbAlloc = (cElements + cExtra) * sizeof(T);
    bool fRet = AllocBytes(cbAlloc);
    if (pcElements != nullptr)
      *pcElements = (fRet ? cbAlloc / sizeof(T) : 0);
    if (pcbAlloc != nullptr)
      *pcbAlloc = (fRet ? cbAlloc : 0);
    return fRet;
  }

  /**
  AllocOne

  Allocate space for 1 instance.
  On success, any previously held data is replaced.
  Returns false on OOM / overflow.

  Mso::MemoryPtr<Foo> pFoo;
  if (pFoo.AllocOne()) { ... }
  */
  bool AllocOne() noexcept {
    return AllocElem(1);
  }

  // TODO: Does it make sense for CallocElemCb? See if there is need.

  /**
  ReallocBytes

  Reallocate the exact number of bytes.
  Returns false on OOM / overflow.

  ReallocElem is strongly preferred.
  */
  bool ReallocBytes(size_t cb) noexcept {
    const T *pT = static_cast<const T *>(Mso::Memory::Reallocate(reinterpret_cast<void **>(&this->m_pT), cb));
    return (pT != nullptr);
  }

  /**
  ReallocElem

  Reallocate space for cElements + cExtra instances.
  Optional out parameters to receive the number of elements/bytes allocated
  Returns false on OOM / overflow.
  */
  bool ReallocElem(
      size_t cElements,
      size_t cExtra = 0,
      _Out_opt_ size_t *pcElements = nullptr,
      _Out_opt_ size_t *pcbAlloc = nullptr) noexcept {
    // TODO: size_t cbAlloc = (msl::utilities::SafeInt<size_t>(cElements) +cExtra) * sizeof(T);
    size_t cbAlloc = (cElements + cExtra) * sizeof(T);
    bool fRet = ReallocBytes(cbAlloc);
    if (pcElements != nullptr)
      *pcElements = (fRet ? cbAlloc / sizeof(T) : 0);
    if (pcbAlloc != nullptr)
      *pcbAlloc = (fRet ? cbAlloc : 0);
    return fRet;
  }

  /**
  CloneBytes

  Allocates a new buffer and copies the data into it.
  On success, any previously held data is replaced.
  Returns false on OOM / overflow.

  CloneElem-variants are strongly preferred to avoid integer overflow.
  FUTURE: I could add cbExtra parameter if it makes sense.

  Mso::MemoryPtr<BYTE> pbFoo;
  if (pbFoo.CloneBytes(pbSrc, cbSrc)) { ... }
  */
  bool CloneBytes(_In_opt_bytecount_(cb) const T *pT, size_t cb) noexcept {
    if (pT != nullptr) {
      Mso::MemoryPtr<T, allocFlags> pNew;
      if (pNew.AllocBytes(cb)) {
        memcpy_s(pNew.Get(), cb, pT, cb);
        this->Swap(pNew);
        return true;
      }
    }

    return false;
  }

  /**
  CloneElem

  Allocate space and copies cElements instances from the source.
  On success, any previously held data is replaced.
  Returns false on OOM / overflow.

  Mso::MemoryPtr<FooData> rgFooData;
  if (rgFooData.CloneElem(rgFooDataSrc, cFooDataSrc) { ... }
  */
  bool CloneElem(_In_opt_count_(cElements) const T *pT, size_t cElements) noexcept {
    // TODO: size_t cbAlloc = msl::utilities::SafeInt<size_t>(cElements) * sizeof(T);
    size_t cbAlloc = (cElements) * sizeof(T);
    return CloneBytes(pT, cbAlloc);
  }

  /**
  CloneOne

  Allocates space and copies 1 instance from the source.
  Any previously held data is replaced on success.

  Mso::MemoryPtr<Foo> pFoo;
  if (pFoo.AllocOne(pFooSrc)) { ... }
  */
  bool CloneOne(_In_opt_count_(1) const T *pT) noexcept {
    return CloneElem(pT, 1);
  }

#ifdef MSO_THOLDER_EXPLICIT_GET_ONLY
  T &operator*() const noexcept {
    return *this->Get();
  }
#endif

 private:
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(MemoryPtr);
};

} // namespace Mso

#endif // __cplusplus

#endif // MSO_MEMORYAPI_MEMORYAPI_H
