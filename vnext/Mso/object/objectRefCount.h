// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_OBJECT_OBJECTREFCOUNT_H
#define MSO_OBJECT_OBJECTREFCOUNT_H

#include <cstdint>
#include <type_traits>
#include "compilerAdapters/cppMacrosDebug.h"
#include "object/make.h"

//
// Simple ref counting manages object lifetime using a ref counter.
// It does not provide any additional features such as WeakPtr, and uses minimum memory per instance: only 4 bytes.
// The ObjectRefCount<TDerived> template does not have a v-table. It calls the TDerived type destructor.
//

#define MSO_OBJECT_SIMPLEREFCOUNT(TObject)                                                         \
 public:                                                                                           \
  bool IsUniqueRef() const noexcept {                                                              \
    return m_refCount.load(std::memory_order_acquire) == 1;                                        \
  }                                                                                                \
  Debug(uint32_t RefCount() const noexcept { return m_refCount.load(std::memory_order_acquire); }) \
                                                                                                   \
      template <typename UseMsoMakeInsteadOfOperatorNew>                                           \
      void *operator new(size_t, UseMsoMakeInsteadOfOperatorNew * = nullptr);                      \
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(TObject)

#define MSO_OBJECT_NOREFCOUNT(TObject) \
 public:                               \
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(TObject)

namespace Mso {

template <typename T, typename TAllocator = MakeAllocator>
struct SimpleRefCountMemoryGuard {
  ~SimpleRefCountMemoryGuard() noexcept {
    if (ObjMemory) {
      TAllocator::Deallocate(ObjMemory);
    } else if (Obj) {
      Obj->Release();
    }
  }

 public: // We use public fields to reduce number of generated methods.
  // VC++ bug: Make sure that the order of the fields is the same for all memory guards. Otherwise, VC++ generates
  // incorrect code for ship builds.
  void *ObjMemory;
  T *Obj;
};

template <typename TDeleter, typename TAllocator = MakeAllocator>
struct SimpleRefCountPolicy {
  using Deleter = TDeleter;
  using Allocator = TAllocator;

  template <typename T>
  using MemoryGuard = SimpleRefCountMemoryGuard<T, TAllocator>;

  template <typename T>
  static void AllocateMemory(_Out_ MemoryGuard<T> &memoryGuard) noexcept {
    memoryGuard.ObjMemory = TAllocator::Allocate(sizeof(T));
  }

  template <typename T, typename TAllocArg>
  static void AllocateMemory(_Out_ MemoryGuard<T> &memoryGuard, TAllocArg &&allocArg) noexcept {
    memoryGuard.ObjMemory = TAllocator::Allocate(sizeof(T), std::forward<TAllocArg>(allocArg));
  }

  template <typename T>
  static void Delete(T *obj) noexcept {
    obj->~T();
    TAllocator::Deallocate(obj);
  }

#if DEBUG
  template <typename T>
  static void ValidateObject(MemoryGuard<T> &memoryGuard) noexcept {
    static_assert(
        offsetof(typename T::TypeToDelete, m_refCount) == offsetof(T, m_refCount),
        "Ref counted object must be the first type in T inheritance list.");

    VerifyElseCrashSzTag(memoryGuard.Obj, "Object was not created", 0x01105590 /* tag_befwq */);
    VerifyElseCrashSzTag(
        reinterpret_cast<void *>(memoryGuard.Obj) == static_cast<typename T::TypeToDelete *>(memoryGuard.Obj),
        "Ref counted object must be the first type in T inheritance list.",
        0x01105591 /* tag_befwr */);
  }
#endif
};

struct DefaultRefCountedDeleter {
  template <typename TObject>
  static void Delete(TObject *obj) noexcept {
    TObject::RefCountPolicy::template Delete(obj);
  }
};

/**
  Supported Object ref count strategies. They are used to select base class for ref counting and to choose Make
  algorithm. The struct can be changed to a namespace if in future we need many strategies in different files.
*/
namespace RefCountStrategy {
using Simple = SimpleRefCountPolicy<DefaultRefCountedDeleter, MakeAllocator>;
struct SimpleNoQuery;
struct NoRefCount;
struct NoRefCountNoQuery;
}; // namespace RefCountStrategy

/**
  std::shared_ptr<T> is large - it's 2 pointers.
  This is handy for cases where memory management (one pointer) is separated from the actual
  data (second pointer). But in cases where we have std::shared_ptr<T> and construct it using
  std::make_shared<T>() it does not make much sense - we waste one pointer.
  An example would be well-defined final types - like std::shared_ptr<std::vector<T>>.
  To avoid wasting memory, created 2 new concepts that mirror STL - RefCountedPtr<> & Make_RefCounted<>()
  Helper class RefCountedWrapper is used to facilitate change

  NOTE: there are potentially 2 types of objects we need:
  1) final & no vtable. Useful for case of std::shared_ptr<std::vector<T>>.
  2) with virtual destructor. Useful for type eraser, to make sure object is deleted properly.

  Also we can do various optimizations / specializations, like
  - ref-counted without interlocked operations (where it's safe)
  - ref-counting using short (2 bytes), vs. int (useful if to pack better, like case of
  RefCountedWrapper<std::pair<short, T>> in CallbackStoreCombined
  That said, this comes at the cost of complexity - more types, harder to navigate and easier to make mistakes.
  Should be done through optional template args.
*/
class RefCountedWrapperBase {
 public:
  using RefCountPolicy = SimpleRefCountPolicy<DefaultRefCountedDeleter, MakeAllocator>;
  friend RefCountPolicy;

  using TypeToDelete = RefCountedWrapperBase; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_SIMPLEREFCOUNT(RefCountedWrapperBase);

  void AddRef() const noexcept {
    uint32_t refCount = ++m_refCount;
    (void)(refCount);
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) > 1, "Ref count must not bounce from zero", 0x01105592 /* tag_befws */));
  }

  void Release() const noexcept {
    uint32_t refCount = --m_refCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) >= 0, "Ref count must not be negative.", 0x01105593 /* tag_befwt */));
    if (refCount == 0) {
      RefCountPolicy::Delete(const_cast<RefCountedWrapperBase *>(this));
    }
  }

 protected:
  RefCountedWrapperBase() noexcept = default;
  virtual ~RefCountedWrapperBase() noexcept = default; // for type erasure

 private:
  mutable std::atomic<uint32_t> m_refCount{1};
};

template <typename T>
class RefCountedWrapper : public RefCountedWrapperBase, public T {
 public:
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(RefCountedWrapper);

  using MakePolicy = Mso::MakePolicy::ThrowCtor;

  template <typename... U>
  RefCountedWrapper(U &&... args) noexcept : T(std::forward<U>(args)...) {}
};

/**
  Special case for base object that does not know about its type.
  Useful for generic containers, where container does not care about the type,
  but has external user that can differentiate between instances of these objects
  (for example by storing property ID as a first field, i.e.
  RefCountedWrapper<std::pair<PropertyId, SomeObject>>)
*/
template <>
class RefCountedWrapper<void> : public RefCountedWrapperBase {
 public:
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(RefCountedWrapper);
  RefCountedWrapper() noexcept = default;
};

/**
  The actual type / Make() function everybody would use
*/
template <typename T>
using RefCountedPtr = Mso::CntPtr<RefCountedWrapper<T>>;

template <typename T, typename... U>
RefCountedPtr<T> Make_RefCounted(U &&... args) noexcept {
  return Mso::Make<RefCountedWrapper<T>>(std::forward<U>(args)...);
}

} // namespace Mso

#endif // MSO_OBJECT_OBJECTREFCOUNT_H
