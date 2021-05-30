// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_OBJECT_REFCOUNTEDOBJECT_H
#define MSO_OBJECT_REFCOUNTEDOBJECT_H

#include "compilerAdapters/compilerWarnings.h"
#include "object/make.h"
#include "object/objectRefCount.h"
#include "object/objectWithWeakRef.h"
#include "object/weakPtr.h"

BEGIN_DISABLE_WARNING_INCONSISTENT_MISSING_OVERRIDE()

namespace Mso {

/**
  RefCountedObject is a class template to implement the IRefCounted interface.

  Because RefCountedObject overrides AddRef and Release, at least one of your base types should
  declare virtual AddRef/Release methods.  Usually you'd specify at least "Mso::IRefCounted" or
  some interface that derives from IRefCounted.

  If you want to avoid the overhead of a v-table, you can use RefCountedObjectNoVTable, which
  defines non-virtual AddRef/Release methods.  These are still compatible with templated
  smart pointers like CntPtr and ComPtr because those smart pointers don't actually use
  virtual method dispatch to call AddRef/Release.

  To implement IUnknown, use Mso::UnknownObject.

  RefCountedObject provides variants that allow for various combinations of weak references,
  multiple inheritance, or just implementing IRefCounted.

  Use Mso::Make to instantiate classes that derive from RefCountedObject.


  EXAMPLES
  --------

  1)  A simple class that implements IRefCounted:

      class Foo : public Mso::RefCountedObject<Mso::IRefCounted>
      {
        ...
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  2)  A class that implements some interface that derives from IRefCounted:

      struct IBar : public Mso::IRefCounted { ... };

      class Foo : public Mso::RefCountedObject<IBar>
      {
        ...
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();
      Mso::CntPtr<IBar> spBar = Mso::Make<Foo, IBar>();      // Create a CntPtr<IBar> directly.


  3)  A class that inherits from multiple base types and implements IRefCounted:

      class Foo : public Mso::RefCountedObject<SomeBaseType, SomeOtherBaseType, Mso::IRefCounted>
      {
        ...
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  4)  A class that implements IRefCounted and has support for weak references:

      class Foo : public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, Mso::IRefCounted>
      {
        ...
      };

      void Foo::RegisterForEventRequiringWeakRef(SomeEventSource& event)
      {
        // Use WeakPtr to capture a weak reference.
        Mso::WeakPtr<Foo> wrThis(this);
        event.Register([wrThis]
        {
          // Use GetStrongPtr to resolve the weak reference.
          auto spThis = wrThis.GetStrongPtr();
          if (spThis)
            spThis->DoSomethingInResponseToEvent();
        });
      }

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  5)  A class that implements IRefCounted with weak references and a custom deleter:

      struct FooDeleter
      {
        template <typename T>
        static void Delete(T* obj) noexcept
        {
          ...
        }
      };

      using FooRefCountPolicy = Mso::WeakRefCountPolicy<FooDeleter>;

      class Foo : public Mso::RefCountedObject<FooRefCountPolicy, Mso::IRefCounted>
      {
        ...
      };

      void Foo::RegisterForEventRequiringWeakRef(SomeEventSource& event)
      {
        // Use WeakPtr to capture a weak reference.
        Mso::WeakPtr<Foo> wrThis(this);
        event.Register([wrThis]
        {
          // Use GetStrongPtr to resolve the weak reference.
          auto spThis = wrThis.GetStrongPtr();
          if (spThis)
            spThis->DoSomethingInResponseToEvent();
        });
      }

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  6)  A class that implements some IRefCounted-derived interface, with a private constructor.

    Use 'friend MakePolicy' to make your constructor private.  This helps ensure that
    people only use Mso::Make to instantiate your class.

      struct IBar : public Mso::IRefCounted { ... };

      class Foo : public Mso::RefCountedObject<IBar>
      {
        friend MakePolicy;

      private:
        Foo(const Bar& bar) noexcept { ... }  // Private constructor

        ...
      };

      const Bar& bar = ...;
      Mso::CntPtr<Foo> spFoo = Mso::Make(bar);


  7)  A class that uses the 'InitializeThis' pattern, which allows you to separate object construction
    from other initialization.

    If InitializeThis returns false, Mso::Make will crash the app.

      struct IBar : public Mso::IRefCounted { ... };

      class Foo : public Mso::RefCountedObject<IBar>
      {
      public:
        using MakePolicy = Mso::MakePolicy::NoThrowCtorAndInitializeThis;

        Foo() noexcept { ... }

        bool InitializeThis(const Baz& baz, const Qux& qux)
        {
          ...

          // Return 'true' to indicate the object was successfully initialized.  'false' will crash the app.
          return true;
        }
      };

      const Baz& baz = ...;
      const Qux& qux = ...;
      Mso::CntPtr<Foo> spFoo = Mso::Make(baz, qux);


  8)  A class that uses ref counting without a v-table.

    Non-virtual AddRef and Release methods are implemented.  These are still compatible with most
    template-based smart-pointers (like CntPtr and ComPtr) since they call 'AddRef' and 'Release'
    by name and not necessarily through virtual-method dispatch.

    Note that RefCountedObjectNoVTable uses the "Curiously Recursive Template Pattern" which requires
    that the derived type be passed in as a template parameter.

      class Foo : public Mso::RefCountedObjectNoVTable<Foo>
      {
        ...
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  9)  A class that implements IRefCounted without actual ref counting.

    This can be useful if the object's lifetime is managed via some other method, but the object
    still needs to be used with CntPtr or other code that expects AddRef/Release.
    This often happens in two scenarios:

    - Singletons that want to avoid any AddRef/Release because there is one long-lived instance.
    - Stack-based allocations for unit tests.

      class Foo : public Mso::RefCountedObject<Mso::RefCountStrategy::NoRefCount, Mso::IRefCounted>
      {
        ...
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  10) A class that implements IRefCounted with a custom stateless allocator.

      struct FooAllocator
      {
        static void* Allocate(size_t size) noexcept
        {
          ...
        }

        static void Deallocate(void* ptr) noexcept
        {
          ...
        }
      };

      struct IBar : public Mso::IRefCounted { ... };

      class Foo : public Mso::RefCountedObject<Mso::SimpleRefCountPolicy<Mso::DefaultRefCountedDeleter, FooAllocator>,
  IBar>
      {
        ...
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  11) A class that implements IRefCounted with a custom stateful allocator.

      struct ICustomHeap
      {
        virtual void* Alloc(size_t size) noexcept = 0;
        virtual void Free(void* ptr) noexcept = 0;
      };

      struct FooAllocator
      {
        static void* Allocate(size_t size, ICustomHeap* heap) noexcept
        {
          ...
        }

        static void Deallocate(void* ptr) noexcept
        {
          ...
        }
      };

      struct IBar : public Mso::IRefCounted { ... };

      class Foo : public Mso::RefCountedObject<Mso::SimpleRefCountPolicy<Mso::DefaultRefCountedDeleter, FooAllocator>,
  IBar>
      {
        ...
      };

      ICustomHeap& heap = ...;
      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>(&heap);


  12) A class that has a throwing ctor and implements IRefCounted:

      struct Foo : public Mso::RefCountedObject<Mso::IRefCounted>
      {
        using MakePolicy = Mso::MakePolicy::ThrowCtor;

        Foo(); // can throw
        ...
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>(); // can throw

*/

template <typename TBaseType0, typename... TBaseTypes>
class RefCountedObject : public TBaseType0, public TBaseTypes... {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::SimpleRefCountPolicy<DefaultRefCountedDeleter, MakeAllocator>;
  friend RefCountPolicy;

  using RefCountedObjectType = RefCountedObject; // To use in derived class as "using Super = RefCountedObjectType"
  using TypeToDelete = RefCountedObject; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_SIMPLEREFCOUNT(RefCountedObject);

  void AddRef() const noexcept override {
    if (++m_refCount == 1) {
      Debug(VerifyElseCrashSzTag(false, "Ref count must not bounce from zero", 0x01105597 /* tag_befwx */));
    }
  }

  void Release() const noexcept override {
    const uint32_t refCount = --m_refCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) >= 0, "Ref count must not be negative.", 0x01105598 /* tag_befwy */));
    if (refCount == 0) {
      RefCountPolicy::Delete(const_cast<RefCountedObject *>(this));
    }
  }

 protected:
  template <typename... TArgs>
  RefCountedObject(TArgs &&... args) noexcept : TBaseType0(std::forward<TArgs>(args)...) {}

  virtual ~RefCountedObject() = default;

 private:
  mutable std::atomic<uint32_t> m_refCount{1};
};

template <typename TDeleter, typename TAllocator, typename TBaseType0, typename... TBaseTypes>
class RefCountedObject<Mso::SimpleRefCountPolicy<TDeleter, TAllocator>, TBaseType0, TBaseTypes...>
    : public TBaseType0, public TBaseTypes... {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::SimpleRefCountPolicy<TDeleter, TAllocator>;
  friend RefCountPolicy;

  using RefCountedObjectType = RefCountedObject; // To use in derived class as "using Super = RefCountedObjectType"
  using TypeToDelete = RefCountedObject; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_SIMPLEREFCOUNT(RefCountedObject);

  void AddRef() const noexcept override {
    if (++m_refCount == 1) {
      Debug(VerifyElseCrashSzTag(false, "Ref count must not bounce from zero", 0x01105599 /* tag_befwz */));
    }
  }

  void Release() const noexcept override {
    const uint32_t refCount = --m_refCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) >= 0, "Ref count must not be negative.", 0x0110559a /* tag_befw0 */));
    if (refCount == 0) {
      TDeleter::Delete(const_cast<RefCountedObject *>(this));
    }
  }

 protected:
  template <typename... TArgs>
  RefCountedObject(TArgs &&... args) noexcept : TBaseType0(std::forward<TArgs>(args)...) {}

  virtual ~RefCountedObject() = default;

 private:
  mutable std::atomic<uint32_t> m_refCount{1};
};

template <typename TDeleter, typename TAllocator, typename TBaseType0, typename... TBaseTypes>
class RefCountedObject<Mso::WeakRefCountPolicy<TDeleter, TAllocator>, TBaseType0, TBaseTypes...>
    : public TBaseType0, public TBaseTypes... {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::WeakRefCountPolicy<TDeleter, TAllocator>;
  friend RefCountPolicy;

  using RefCountedObjectType = RefCountedObject; // To use in derived class as "using Super = RefCountedObjectType"
  using TypeToDelete = RefCountedObject; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_WEAKREFCOUNT(RefCountedObject);

  void AddRef() const noexcept override {
    GetWeakRef().AddRef();
  }

  void Release() const noexcept override {
    GetWeakRef().Release();
  }

 protected:
  template <typename... TArgs>
  RefCountedObject(TArgs &&... args) noexcept : TBaseType0(std::forward<TArgs>(args)...) {}

  virtual ~RefCountedObject() = default;
};

template <typename TBaseType0, typename... TBaseTypes>
class RefCountedObject<Mso::RefCountStrategy::NoRefCount, TBaseType0, TBaseTypes...> : public TBaseType0,
                                                                                       public TBaseTypes... {
 public:
  using RefCountedObjectType = RefCountedObject; // To use in derived class as "using Super = RefCountedObjectType"

  MSO_OBJECT_NOREFCOUNT(RefCountedObject);

  template <typename... TArgs>
  RefCountedObject(TArgs &&... args) noexcept : TBaseType0(std::forward<TArgs>(args)...) {}

  void AddRef() const noexcept override {}
  void Release() const noexcept override {}
  virtual ~RefCountedObject() = default;
};

template <typename... Ts>
class RefCountedObjectNoVTable;

/**
        A base class for ref counted objects that do not have v-table and need a simple ref count.
*/
template <typename TDerived>
class RefCountedObjectNoVTable<TDerived> {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::SimpleRefCountPolicy<DefaultRefCountedDeleter, MakeAllocator>;
  friend RefCountPolicy;

  using RefCountedObjectNoVTableType =
      RefCountedObjectNoVTable; // To use in derived class as "using Super = RefCountedObjectNoVTableType"
  using TypeToDelete = TDerived; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_SIMPLEREFCOUNT(RefCountedObjectNoVTable);

  void AddRef() const noexcept {
    if (++m_refCount == 1) {
      Debug(VerifyElseCrashSzTag(false, "Ref count must not bounce from zero", 0x0110559b /* tag_befw1 */));
    }
  }

  void Release() const noexcept {
    const uint32_t refCount = --m_refCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) >= 0, "Ref count must not be negative.", 0x0110559c /* tag_befw2 */));
    if (refCount == 0) {
      RefCountPolicy::Delete(const_cast<TDerived *>(static_cast<const TDerived *>(this)));
    }
  }

 protected:
  RefCountedObjectNoVTable() = default;

 private:
  mutable std::atomic<uint32_t> m_refCount{1};
};

/**
        A base class for RefCounted objects that do not have v-table and need a simple ref count.
*/
template <typename TDeleter, typename TAllocator, typename TDerived>
class RefCountedObjectNoVTable<Mso::SimpleRefCountPolicy<TDeleter, TAllocator>, TDerived> {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::SimpleRefCountPolicy<TDeleter, TAllocator>;
  friend RefCountPolicy;

  using RefCountedObjectNoVTableType =
      RefCountedObjectNoVTable; // To use in derived class as "using Super = RefCountedObjectNoVTableType"
  using TypeToDelete = TDerived; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_SIMPLEREFCOUNT(RefCountedObjectNoVTable);

  void AddRef() const noexcept {
    if (++m_refCount == 1) {
      Debug(VerifyElseCrashSzTag(false, "Ref count must not bounce from zero", 0x0110559d /* tag_befw3 */));
    }
  }

  void Release() const noexcept {
    const uint32_t refCount = --m_refCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) >= 0, "Ref count must not be negative.", 0x0110559e /* tag_befw4 */));
    if (refCount == 0) {
      TDeleter::Delete(const_cast<TDerived *>(static_cast<const TDerived *>(this)));
    }
  }

 protected:
  RefCountedObjectNoVTable() = default;

 private:
  mutable std::atomic<uint32_t> m_refCount{1};
};

/**
        A base class for RefCounted objects that do not have v-table and need support for weak ref count.
*/
template <typename TDeleter, typename TAllocator, typename TDerived>
class RefCountedObjectNoVTable<Mso::WeakRefCountPolicy<TDeleter, TAllocator>, TDerived> {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::WeakRefCountPolicy<TDeleter, TAllocator>;
  friend RefCountPolicy;

  using RefCountedObjectNoVTableType =
      RefCountedObjectNoVTable; // To use in derived class as "using Super = RefCountedObjectNoVTableType"
  using TypeToDelete = TDerived; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_WEAKREFCOUNT(RefCountedObjectNoVTable);

  void AddRef() const noexcept {
    GetWeakRef().AddRef();
  }

  void Release() const noexcept {
    GetWeakRef().Release();
  }

 protected:
  RefCountedObjectNoVTable() = default;
};

} // namespace Mso

END_DISABLE_WARNING_INCONSISTENT_MISSING_OVERRIDE()

#endif // MSO_OBJECT_REFCOUNTEDOBJECT_H
