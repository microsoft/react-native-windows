// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_OBJECT_OBJECTWITHWEAKREF_H
#define MSO_OBJECT_OBJECTWITHWEAKREF_H

#include "guid/msoGuid.h"
#include "object/objectRefCount.h"

//
// Classes in this file implement ref counting that supports weak pointers, and also provides a foundation for
// more advanced object lifetime management such as sharing the same ref count for multiple object, and object deletion
// in a different thread. (See ObjectSwarm.h)
//
// Design of these classes is inspired by the std::shared_ptr implementation. The main differences are that we:
// - Expose the reference counting control block as a public API - ObjectWeakRef class.
// - Add a base class ObjectWithWeakRef to support CntPtr.
//
// Classes must be inherited from the ObjectWithWeakRef and assigned an instance of ObjectWeakRef right after
// the construction. ObjectWithWeakRef delegates all ref counting to ObjectWeakRef.
//
// ObjectWeakRef is an ObjectRefCount that maintains a m_strongRefCount variable to control the lifetime of classes
// inherited from the ObjectWithWeakRef.
//
// ObjectWithWeakRef base class has a CntPtr to the ObjectWeakRef that contributes +1 to the m_weakRefCount
// while object is alive. It provides a guarantee that the ObjectWeakRef will always be destroyed after
// the controlled object.
//
// When a new instance is constructed both m_weakRefCount and m_refCount have value 1:
// - m_weakRefCount has 1 because the new ObjectWithWeakRef has a strong reference to it.
// - m_refCount has 1 because we return a CntPtr to the newly constructed object.
//
// To allocate memory we use ObjectWeakRefContainer. It is inherited from the ObjectWeakRef and has storage
// for the constructed object. So, we have only one memory allocation instead of two.
//
// It is very similar to how the std::make_shared works.
// The main difference here is that the base class for the ObjectWeakRefContainer is a template which
// gives ability to have different base classes. They just required to have a virtual method DestroyOwnedObject() to
// signal when object must be destroyed. The base class must also have a virtual destructor and be responsible for its
// destruction.
//
//     -------------------------------------------------------------------------------------------
//     |                                   ObjectWeakRefContainer                                |
//     |-----------------------------------------------------------------------------------------|
//     |                ObjectWeakRef             | m_weakRef |          m_objectStorage         |
//     |------------------------------------------| 4/8 bytes | T derived from ObjectWithWeakRef |
//     |   v-table  | m_weakRefCount | m_refCount |           |                                  |
//     |  4/8 bytes |     4 bytes    |   4 bytes  |           |                                  |
//     -------------------------------------------------------------------------------------------
//       ^                                              |
//       |                                              |
//       ------------------------------------------------
//
// To create a new instance of a class inherited from ObjectWithWeakRef use the Make() or MakeAndInitialize() functions
// defined in the ObjectRefCount.h. In this file we provide the MakeTraits class that provides the implementation
// for the Make() function.
//
// Please note that the ObjectWithWeakRef does not have a v-table. This is because the ObjectWeakRefContainer always
// calls destructor of the most derived type that it created and stored in the m_objectStorage.
//
// In future we can also support classes that are not inherited from the ObjectWithWeakRef. In that case we would need
// to implement a SharedPtr which has two pointers: one to the object and another to the ObjectWeakRef.
// The only reason we have the ObjectWithWeakRef base class is to be able to have a single pointer to the object using
// CntPtr and get a pointer to the ObjectWeakRef using the base ObjectWithWeakRef class.
//

#define MSO_OBJECT_WEAKREFCOUNT(TObject)                                        \
 public:                                                                        \
  Mso::ObjectWeakRef &GetWeakRef() const noexcept {                             \
    return *Mso::Details::GetWeakRef(this);                                     \
  }                                                                             \
  bool IsUniqueRef() const noexcept {                                           \
    return GetWeakRef().IsUniqueRef();                                          \
  }                                                                             \
  Debug(uint32_t RefCount() const noexcept { return GetWeakRef().RefCount(); }) \
                                                                                \
      template <typename UseMsoMakeInsteadOfOperatorNew>                        \
      void *operator new(size_t, UseMsoMakeInsteadOfOperatorNew * = nullptr);   \
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(TObject)

namespace Mso {

/**
  Reference counting block to support weak pointers.
*/
MSO_CLASS_GUID(ObjectWeakRef, "3063C26C-DB10-4BCC-AF5C-340E4D7AA0F6")
class ObjectWeakRef {
 public:
  using RefCountPolicy = Mso::SimpleRefCountPolicy<DefaultRefCountedDeleter, MakeAllocator>;
  friend RefCountPolicy;

  using TypeToDelete = ObjectWeakRef; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_SIMPLEREFCOUNT(ObjectWeakRef);

  ObjectWeakRef() = default;

  void AddRef() const noexcept {
    if (++m_refCount == 1) {
      Debug(VerifyElseCrashSzTag(false, "Ref count must not bounce from zero", 0x01105594 /* tag_befwu */));
    }
  }

  void Release() const noexcept {
    const uint32_t refCount = --m_refCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) >= 0, "Ref count must not be negative.", 0x008c2685 /* tag_a9c0f */));
    if (refCount == 0) {
      const_cast<ObjectWeakRef *>(this)->DestroyObject();
    }
  }

  void AddWeakRef() const noexcept {
    if (++m_weakRefCount == 1) {
      Debug(VerifyElseCrashSzTag(false, "Weak ref count must not bounce from zero", 0x01105595 /* tag_befwv */));
    }
  }

  void ReleaseWeakRef() const noexcept {
    const uint32_t weakRefCount = --m_weakRefCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(weakRefCount) >= 0, "Weak ref count must not be negative.", 0x008c2686 /* tag_a9c0g */));
    if (weakRefCount == 0) {
      DestroyContainer();
    }
  }

  Debug(uint32_t WeakRefCount() const noexcept { return m_weakRefCount.load(std::memory_order_acquire); })

      Mso::ObjectWeakRef &GetWeakRef() const noexcept {
    return *const_cast<ObjectWeakRef *>(this);
  }

  virtual void *QueryCastWeakRef(const GUID & /*riid*/) noexcept {
    return nullptr;
  }

  bool IsExpired() const noexcept {
    return (m_refCount.load(std::memory_order_acquire) == 0);
  }

  bool IncrementRefCountIfNotZero() noexcept {
    uint32_t count = m_refCount.load(std::memory_order_acquire);
    for (;;) {
      if (count == 0) {
        return false;
      }

      if (m_refCount.compare_exchange_weak(/*ref*/ count, count + 1)) {
        return true;
      }
    }
  }

  virtual void DestroyObject() noexcept {}

 protected:
  virtual void DestroyContainer() const noexcept = 0;
  virtual ~ObjectWeakRef() = default;

 private:
  mutable std::atomic<uint32_t> m_refCount{1};
  mutable std::atomic<uint32_t> m_weakRefCount{1}; // Controls ObjectWeakRef lifetime.
};

namespace Details {

// Weak reference is always stored in object negative space.
inline Mso::ObjectWeakRef *GetWeakRef(const void *obj) noexcept {
  return *(static_cast<Mso::ObjectWeakRef **>(const_cast<void *>(obj)) - 1);
}

inline void SetWeakRef(const void *obj, Mso::ObjectWeakRef *weakRef) noexcept {
  *(static_cast<Mso::ObjectWeakRef **>(const_cast<void *>(obj)) - 1) = weakRef;
}

} // namespace Details

/**
  Allocates RefCountBlock and the target object in the same chunk of memory.
  TWeakRef is expected to have a virtual destructor and virtual DestroyOwnedObject method.
*/
template <typename TObject, typename TWeakRef>
class ObjectWeakRefContainer : public TWeakRef {
 public:
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(ObjectWeakRefContainer);

  ObjectWeakRefContainer() = default;

  TObject *Get() noexcept {
    return reinterpret_cast<TObject *>(&m_objectStorage);
  }

 protected:
  virtual void DestroyObject() noexcept override {
    TWeakRef::DestroyObject();

    TObject *obj = Get();
    if (Mso::Details::GetWeakRef(obj)) // Check if object was correctly initialized.
    {
      // Call the TObject destructor last: this call may destroy the ObjectWeakRefContainer because it may hold the last
      // reference.
      TObject::RefCountPolicy::Deleter::Delete(static_cast<typename TObject::TypeToDelete *>(obj));
    }
  }

  virtual void DestroyContainer() const noexcept override {
    this->~ObjectWeakRefContainer();
    TObject::RefCountPolicy::Allocator::Deallocate(const_cast<ObjectWeakRefContainer *>(this));
  }

 private:
  ObjectWeakRef *m_weakRefPlaceholder; // reserves negative space before m_objectStorage
  typename std::aligned_storage<sizeof(TObject), std::alignment_of<TObject>::value>::type m_objectStorage;
};

template <typename T, typename TContainer>
struct WeakRefCountMemoryGuard {
  using Type = T;

  ~WeakRefCountMemoryGuard() noexcept {
    if (ObjMemory) {
      // Constructor failed, Make sure that destructor is not called.
      ObjectWeakRef *weakRef = Mso::Details::GetWeakRef(ObjMemory);
      Mso::Details::SetWeakRef(ObjMemory, nullptr); // This should prevent destructor call.
      Container->Release(); // Release the strong reference which we add to object when we create it.
      weakRef->ReleaseWeakRef(); // WeakRef is released when object is destroyed. This call may destroy container and
                                 // must be the last.
    } else if (Obj) {
      // Initialization failed. Use normal object destruction sequence.
      Obj->Release();
    }
  }

 public: // We use a public fields to reduce number of generated methods.
  // VC++ bug: Make sure that the order of the fields is the same for all memory guards. Otherwise, VC++ generates
  // incorrect code for ship builds.
  void *ObjMemory;
  T *Obj;
  TContainer *Container;
};

template <typename TDeleter, typename TAllocator = MakeAllocator>
struct WeakRefCountPolicy {
  using Deleter = TDeleter;
  using Allocator = TAllocator;

  template <typename T, typename TContainer = ObjectWeakRefContainer<T, ObjectWeakRef>>
  using MemoryGuard = WeakRefCountMemoryGuard<T, TContainer>;

  template <typename T, typename TContainer>
  static void AllocateMemory(_Out_ MemoryGuard<T, TContainer> &memoryGuard) noexcept {
    void *containerMemory = TAllocator::Allocate(sizeof(TContainer));
    if (containerMemory) {
      memoryGuard.Container = ::new (containerMemory) TContainer();
      memoryGuard.ObjMemory = memoryGuard.Container->Get();
      Mso::Details::SetWeakRef(memoryGuard.ObjMemory, memoryGuard.Container);
    }
  }

  template <typename T, typename TContainer, typename TAllocArg>
  static void AllocateMemory(_Out_ MemoryGuard<T, TContainer> &memoryGuard, TAllocArg &&allocArg) noexcept {
    void *containerMemory = TAllocator::Allocate(sizeof(TContainer), std::forward<TAllocArg>(allocArg));
    if (containerMemory) {
      memoryGuard.Container = ::new (containerMemory) TContainer();
      memoryGuard.ObjMemory = memoryGuard.Container->Get();
      Mso::Details::SetWeakRef(memoryGuard.ObjMemory, memoryGuard.Container);
    }
  }

  template <typename T>
  static void Delete(T *obj) noexcept {
    obj->~T();
    ObjectWeakRef *weakRef = Mso::Details::GetWeakRef(obj);
    Mso::Details::SetWeakRef(obj, nullptr);
    weakRef->ReleaseWeakRef(); // This call may destroy container and must be the last.
  }

#if DEBUG
  template <typename TMemoryGuard>
  static void ValidateObject(TMemoryGuard &memoryGuard) noexcept {
    VerifyElseCrashSzTag(memoryGuard.Obj, "Object was not created", 0x01105596 /* tag_befww */);
    VerifyElseCrashSzTag(
        reinterpret_cast<void *>(memoryGuard.Obj) ==
            static_cast<typename TMemoryGuard::Type::TypeToDelete *>(memoryGuard.Obj),
        "Ref counted object must be the first type in T inheritance list.",
        0x008c2687 /* tag_a9c0h */);
    VerifyElseCrashSzTag(
        reinterpret_cast<void *>(memoryGuard.Container) == &memoryGuard.Obj->GetWeakRef(),
        "Bad WeakRef",
        0x008c2688 /* tag_a9c0i */);
  }
#endif
};

/**
  Extend the supported Object ref count strategies with WeakRef.
*/
namespace RefCountStrategy {
using WeakRef = WeakRefCountPolicy<DefaultRefCountedDeleter, MakeAllocator>;
}

namespace Details {

/**
  Mso::MakeWeakRefObject creates a new instance of class T in container TContainer and returns a CntPtr to the
  instance of type TResult. TResult is either the original type T (default), or one of its interfaces. Returning an
  interface type can help to avoid creation of unnecessary CntPtr template instances.

  Method MakeWeakRefObject is noexcept depending on the Make policy noexcept value.
*/
template <
    typename T,
    typename TResult = T,
    typename TContainer = ObjectWeakRefContainer<T, ObjectWeakRef>,
    typename... TArgs>
inline Mso::CntPtr<TResult> MakeWeakRefObject(TArgs &&... args) noexcept(T::MakePolicy::IsNoExcept) {
  typename T::RefCountPolicy::template MemoryGuard<T, TContainer> memoryGuard = {};
  T::RefCountPolicy::AllocateMemory(memoryGuard);
  VerifyAllocElseCrashTag(memoryGuard.ObjMemory, 0x0111774a /* tag_bex3k */);

  T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
  Debug(T::RefCountPolicy::ValidateObject(memoryGuard));

  TResult *result = memoryGuard.Obj;
  memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
  return Mso::CntPtr<TResult>(result, /*fDoAddRef*/ false);
}

/**
  Mso::MakeAllocWeakRefObject is an Mso::MakeWeakRefObject method for types T that use allocators accepting an
  argument. The allocator argument allows to implement stateful allocators.

  MakeAllocWeakRefObject creates a new instance of class T in container TContainer and returns a CntPtr to the
  instance of type TResult. TResult is either the original type T (default), or one of its interfaces. Returning an
  interface type can help to avoid creation of unnecessary CntPtr template instantiations.

  Method MakeAllocWeakRefObject is noexcept depending on the Make policy noexcept value.
*/
template <
    typename T,
    typename TResult = T,
    typename TContainer = ObjectWeakRefContainer<T, ObjectWeakRef>,
    typename TAllocArg,
    typename... TArgs>
inline Mso::CntPtr<TResult> MakeAllocWeakRefObject(TAllocArg &&allocArg, TArgs &&... args) noexcept(
    T::MakePolicy::IsNoExcept) {
  typename T::RefCountPolicy::template MemoryGuard<T, TContainer> memoryGuard = {};
  T::RefCountPolicy::AllocateMemory(memoryGuard, std::forward<TAllocArg>(allocArg));
  VerifyAllocElseCrashTag(memoryGuard.ObjMemory, 0x0111774b /* tag_bex3l */);

  T::MakePolicy::template Make<T>(memoryGuard, std::forward<TArgs>(args)...);
  Debug(T::RefCountPolicy::ValidateObject(memoryGuard));

  TResult *result = memoryGuard.Obj;
  memoryGuard.Obj = nullptr; // To prevent memoryGuard from destroying the object.
  return Mso::CntPtr<TResult>(result, /*fDoAddRef*/ false);
}

} // namespace Details
} // namespace Mso

#endif // MSO_OBJECT_OBJECTWITHWEAKREF_H
