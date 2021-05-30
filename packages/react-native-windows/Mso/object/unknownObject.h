// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_OBJECT_UNKNOWNOBJECT_H
#define MSO_OBJECT_UNKNOWNOBJECT_H

#include "comUtil/IUnknownShim.h"
#include "compilerAdapters/compilerWarnings.h"
#include "object/objectRefCount.h"
#include "object/objectWithWeakRef.h"
#include "object/queryCast.h"
#include "object/weakPtr.h"

BEGIN_DISABLE_WARNING_INCONSISTENT_MISSING_OVERRIDE()

/**
  UnknownObject is a class template that implements the IUnknown interface (AddRef, Release, and QueryInterface).

  To implement just Mso::IRefCounted (similar to IUnknown but without QueryInterface), use Mso::RefCountedObject.

  UnknownObject provides variants that allow for various combinations of weak references,
  multiple inheritance, or just implementing a simple COM interface that derives from IUnknown.

  Use Mso::Make to instantiate classes that derive from UnknownObject.


  EXAMPLES
  --------

  1)  A simple class that implements a COM interface that derives from IUnknown.

    AddRef and Release are implemented using a simple ref-count strategy that does not support weak references.
    QueryInterface (QI) is automatically implemented to return the ISomeComInterface pointer when Foo is QI'd for it,
    as long as ISomeComeInterface is tagged with one of the MSO guid macros (MSO_STRUCT_GUID/MSO_CLASS_GUID).

      MSO_STRUCT_GUID(ISomeComInterface, "01234567-ABCD-0123-ABCDEF012345")
      struct ISomeComInterface : public IUnknown
      {
        virtual void DoSomething() = 0;
      };

      class Foo : public Mso::UnknownObject<ISomeComInterface>
      {
      public:
        void DoSomething() override { ... }
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  2)  A class that implements multiple COM interfaces.

    QueryInterface is automatically implemented to support each interface that is tagged with one of the MSO guid macros
    (MSO_STRUCT_GUID/MSO_CLASS_GUID).

      MSO_STRUCT_GUID(IBar, "01234567-ABCD-0123-ABCDEF012345")
      struct IBar : public IUnknown
      {
        virtual void DoBarStuff() = 0;
      };

      MSO_STRUCT_GUID(IBaz, "76543210-ABCD-0123-012345ABCDEF")
      struct IBaz : public IUnknown
      {
        virtual void DoBazStuff() = 0;
      };

      MSO_STRUCT_GUID(IQux, "45670123-ABCD-0123-543210FEDCBA")
      struct IQux : public IUnknown
      {
        virtual void DoQuxStuff() = 0;
      };

      class Foo : public Mso::UnknownObject<IBar, IBaz, IQux>
      {
      public:
        void DoBarStuff() override { ... }
        void DoBazStuff() override { ... }
        void DoQuxStuff() override { ... }
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();
      Mso::CntPtr<IBar> spBar = Mso::Make<Foo, IBar>();      // Create a CntPtr<IBar> directly.


  3)  A class that inherits from one or more base classes.

    QueryInterface is automatically implemented to support each base type that is tagged with one of the MSO guid macros
    (MSO_STRUCT_GUID/MSO_CLASS_GUID). Base classes that derive from QueryCastList can be used to implement other COM
    interfaces.

      MSO_STRUCT_GUID(IBar, "01234567-ABCD-0123-ABCDEF012345")
      struct IBar : public IUnknown
      {
        virtual void DoBarStuff() noexcept = 0;
      };

      class BarMixin : public Mso::QueryCastList<IBar>
      {
        virtual void DoBarStuff() noexcept override { ... }
      };

      MSO_STRUCT_GUID(SomeBaseType, "76543210-ABCD-0123-012345ABCDEF")
      struct SomeBaseType : public IUnknown
      {
        virtual void DoSomething() = 0;
      };

      class Foo : public Mso::UnknownObject<BarMixin, SomeBaseType>
      {
      public:
        void DoSomething() override { ... }
        // Foo::QueryInterface will succeed when queried for IBar, returning a BarMixin*
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  4)  A class that implements a COM interface and has support for weak references:

      class Foo : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, ISomeComInterface>
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


  5) A class that implements a COM interface with weak references and a custom deleter:

      struct FooDeleter
      {
        template <typename T>
        static void Delete(T* obj) noexcept
        {
          ...
        }
      };

      using FooRefCountPolicy = Mso::WeakRefCountPolicy<FooDeleter>;

      class Foo : public Mso::UnknownObject<FooRefCountPolicy, ISomeComInterface>
      {
        ...
      };

      void Foo::RegisterForEventRequiringWeakRef(SomeEventSource& event)
      {
        Mso::WeakPtr<Foo> wrThis(this);
        event.Register([wrThis]
        {
          auto spThis = wrThis.GetStrongPtr();
          if (spThis)
            spThis->DoSomethingInResponseToEvent();
        });
      }

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  6)  A class that implements a COM interface, with a private constructor.

    Use 'friend MakePolicy' to make your constructor private.  This helps ensure that
    people only use Mso::Make to instantiate your class.

      class Foo : public Mso::UnknownObject<ISomeComInterface>
      {
        friend MakePolicy;

      private:
        Foo(const Bar& bar) { }    // Private constructor

        ...
      };

      const Bar& bar = ...;
      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>(bar);


  7)  A class that uses the 'InitializeThis' pattern, which allows you to separate object construction
    from other initialization.

    If InitializeThis returns false, Mso::Make will crash the app.

    Mso::Make does not bubble up exceptions, even from throwing constructors, due to the large code-size
    cost of exception handling.

      class Foo : public Mso::UnknownObject<ISomeComInterface>
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


  8)  A class that implements a COM interface without actual ref counting.

    This can be useful if the object's lifetime is managed via some other method, but the object
    still needs to be used with ComPtr, CntPtr or other code that expects AddRef/Release.
    This can happen in several scenarios:

      - Singletons that want to avoid any AddRef/Release because there is one long-lived instance.
      - Stack-based allocations for unit tests.
      - Support for an ISimpleUnknown interface that has only a QueryInterface method, and no
        AddRef/Release methods.

      class Foo : public Mso::UnknownObject<Mso::RefCountStrategy::NoRefCount, ISomeComInterface>
      {
        ...
      };


  9)  A class that implements a COM interface but with an empty (E_FAIL) implementation of
    QueryInterface (QI).

    This can be useful in targeted cases where you know that QI is not necessary and you don't
    want to pay the code-bloat cost of an unused QI implementation, but you also don't want
    to switch from IUnknown to Mso::IRefCounted.

      class Foo : public Mso::UnknownObject<Mso::RefCountStrategy::SimpleNoQuery, ISomeComInterface>
      {
        ...
      };


  10) A class that implements a COM interface but with empty implementations of the IUnknown
    methods (AddRef, Release, QueryInterface).

      class Foo : public Mso::UnknownObject<Mso::RefCountStrategy::NoRefCountNoQuery, ISomeComInterface>
      {
        ...
      };


  11) A class that implements IRefCounted with a custom stateless allocator.

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

      class Foo : public Mso::UnknownObject<Mso::SimpleRefCountPolicy<Mso::DefaultRefCountedDeleter, FooAllocator>,
  ISomeComInterface>
      {
        ...
      };

      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>();


  12) A class that implements a COM interface with a custom stateful allocator.

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

      class Foo : public Mso::UnknownObject<Mso::SimpleRefCountPolicy<Mso::DefaultRefCountedDeleter, FooAllocator>,
  ISomeComInterface>
      {
        ...
      };

      ICustomHeap& heap = ...;
      Mso::CntPtr<Foo> spFoo = Mso::Make<Foo>(&heap);
*/

namespace Mso::Details {

template <typename T>
struct QueryInterfaceHelper {
  _Success_(return == S_OK) static HRESULT
      QueryInterface(T *obj, const GUID &riid, _Outptr_ void **ppvObject) noexcept {
    VerifyElseCrashSzTag(ppvObject != nullptr, "ppvObject must not be null.", 0x01003717 /* tag_bad2x */);

#if defined(MSO_ENABLE_QICHECK) && defined(DEBUG) && !defined(__clang__)
    // Windows gives un-initialized pointers when querying for IMarshal and IAgileObjectthese interfaces. Ignore them.
    if (riid != __uuidof(IMarshal) && riid != __uuidof(IAgileObject)) {
      VerifyElseCrashSzTag(
          *ppvObject == nullptr, "*ppvObject must be null to avoid memory leaks.", 0x01003718 /* tag_bad2y */);
    }
#endif

    // QueryCastBridge is used to QI for an interface without AddRef
    const GUID &intfGuid =
        (riid == __uuidof(QueryCastBridge)) ? reinterpret_cast<QueryCastBridge *>(ppvObject)->ObjectId : riid;

    if (intfGuid == __uuidof(Mso::IUnknown)) {
      *ppvObject = obj->template StaticCastElseNull<Mso::IUnknown *>();
    } else {
      *ppvObject = obj->QueryCast(intfGuid);
    }

    if (!*ppvObject) {
      return E_NOINTERFACE;
    }

    if (&riid == &intfGuid) {
      obj->AddRef();
    }

    return S_OK;
  }
};

} // namespace Mso::Details

namespace Mso {
template <typename TBaseType0, typename... TBaseTypes>
class DECLSPEC_NOVTABLE UnknownObject : public Mso::QueryCastList<TBaseType0, TBaseTypes...> {
  using Super = Mso::QueryCastList<TBaseType0, TBaseTypes...>;

 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::SimpleRefCountPolicy<Mso::DefaultRefCountedDeleter, Mso::MakeAllocator>;
  friend RefCountPolicy;

  using UnknownObjectType = UnknownObject; // To use in derived class as "using Super = UnknownObjectType"
  using TypeToDelete = UnknownObject; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_SIMPLEREFCOUNT(UnknownObject);

  _Success_(return == S_OK) STDMETHOD(QueryInterface)(const GUID &riid, _Outptr_ void **ppvObject) noexcept override {
    return ::Mso::Details::QueryInterfaceHelper<UnknownObject>::QueryInterface(this, riid, ppvObject);
  }

  STDMETHOD_(ULONG, AddRef)() noexcept override {
    if (++m_refCount == 1) {
      Debug(VerifyElseCrashSzTag(false, "Ref count must not bounce from zero", 0x0110559f /* tag_befw5 */));
    }

    return 1;
  }

  STDMETHOD_(ULONG, Release)() noexcept override {
    const uint32_t refCount = --m_refCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) >= 0, "Ref count must not be negative.", 0x011055a0 /* tag_befw6 */));
    if (refCount == 0) {
      RefCountPolicy::Delete(this);
    }

    return 1;
  }

 protected:
  template <typename... TArgs>
  UnknownObject(TArgs &&... args) noexcept : Super(std::forward<TArgs>(args)...) {}

  virtual ~UnknownObject() noexcept = default;

 private:
  mutable std::atomic<uint32_t> m_refCount{1};
};

template <typename TDeleter, typename TAllocator, typename TBaseType0, typename... TBaseTypes>
class DECLSPEC_NOVTABLE UnknownObject<Mso::SimpleRefCountPolicy<TDeleter, TAllocator>, TBaseType0, TBaseTypes...>
    : public Mso::QueryCastList<TBaseType0, TBaseTypes...> {
  using Super = Mso::QueryCastList<TBaseType0, TBaseTypes...>;

 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::SimpleRefCountPolicy<TDeleter, TAllocator>;
  friend RefCountPolicy;

  using UnknownObjectType = UnknownObject; // To use in derived class as "using Super = UnknownObjectType"
  using TypeToDelete = UnknownObject; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_SIMPLEREFCOUNT(UnknownObject);

  _Success_(return == S_OK) STDMETHOD(QueryInterface)(const GUID &riid, _Outptr_ void **ppvObject) override {
    return ::Mso::Details::QueryInterfaceHelper<UnknownObject>::QueryInterface(this, riid, ppvObject);
  }

  STDMETHOD_(ULONG, AddRef)() override {
    if (++m_refCount == 1) {
      Debug(VerifyElseCrashSzTag(false, "Ref count must not bounce from zero", 0x011055a1 /* tag_befw7 */));
    }

    return 1;
  }

  STDMETHOD_(ULONG, Release)() override {
    const uint32_t refCount = --m_refCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) >= 0, "Ref count must not be negative.", 0x011055a2 /* tag_befw8 */));
    if (refCount == 0) {
      TDeleter::Delete(this);
    }

    return 1;
  }

 protected:
  template <typename... TArgs>
  UnknownObject(TArgs &&... args) noexcept : Super(std::forward<TArgs>(args)...) {}

  virtual ~UnknownObject() noexcept = default;

 private:
  mutable std::atomic<uint32_t> m_refCount{1};
};

template <typename TBaseType0, typename... TBaseTypes>
class DECLSPEC_NOVTABLE UnknownObject<Mso::RefCountStrategy::SimpleNoQuery, TBaseType0, TBaseTypes...>
    : public TBaseType0, public TBaseTypes... {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::SimpleRefCountPolicy<Mso::DefaultRefCountedDeleter, Mso::MakeAllocator>;
  friend RefCountPolicy;

  using UnknownObjectType = UnknownObject; // To use in derived class as "using Super = UnknownObjectType"
  using TypeToDelete = UnknownObject; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_SIMPLEREFCOUNT(UnknownObject);

  _Success_(return == S_OK)
      STDMETHOD(QueryInterface)(const GUID & /*riid*/, _Outptr_ void ** /*ppvObject*/) noexcept override {
    return E_FAIL;
  }

  STDMETHOD_(ULONG, AddRef)() noexcept override {
    if (++m_refCount == 1) {
      Debug(VerifyElseCrashSzTag(false, "Ref count must not bounce from zero", 0x011055a3 /* tag_befw9 */));
    }

    return 1;
  }

  STDMETHOD_(ULONG, Release)() noexcept override {
    const uint32_t refCount = --m_refCount;
    Debug(VerifyElseCrashSzTag(
        static_cast<int32_t>(refCount) >= 0, "Ref count must not be negative.", 0x011055c0 /* tag_befxa */));
    if (refCount == 0) {
      RefCountPolicy::Delete(this);
    }

    return 1;
  }

 protected:
  template <typename... TArgs>
  UnknownObject(TArgs &&... args) noexcept : TBaseType0(std::forward<TArgs>(args)...) {}

  virtual ~UnknownObject() noexcept = default;

 private:
  mutable std::atomic<uint32_t> m_refCount{1};
};

template <typename TDeleter, typename TAllocator, typename TBaseType0, typename... TBaseTypes>
class UnknownObject<Mso::WeakRefCountPolicy<TDeleter, TAllocator>, TBaseType0, TBaseTypes...>
    : public Mso::QueryCastList<TBaseType0, TBaseTypes...> {
  using Super = Mso::QueryCastList<TBaseType0, TBaseTypes...>;

 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtor;
  using RefCountPolicy = Mso::WeakRefCountPolicy<TDeleter, TAllocator>;
  friend RefCountPolicy;

  using UnknownObjectType = UnknownObject; // To use in derived class as "using Super = UnknownObjectType"
  using TypeToDelete = UnknownObject; // To verify that TypeToDelete is the first in the inheritance chain.

  MSO_OBJECT_WEAKREFCOUNT(UnknownObject);

  void *QueryCast(const GUID &riid) noexcept {
    if (riid == __uuidof(ObjectWeakRef)) {
      return &GetWeakRef();
    }

    return Super::QueryCast(riid);
  }

  _Success_(return == S_OK) STDMETHOD(QueryInterface)(const GUID &riid, _Outptr_ void **ppvObject) noexcept override {
    return ::Mso::Details::QueryInterfaceHelper<UnknownObject>::QueryInterface(this, riid, ppvObject);
  }

  STDMETHOD_(ULONG, AddRef)() noexcept override {
    GetWeakRef().AddRef();
    return 1;
  }

  STDMETHOD_(ULONG, Release)() noexcept override {
    GetWeakRef().Release();
    return 1;
  }

 protected:
  template <typename... TArgs>
  UnknownObject(TArgs &&... args) noexcept : Super(std::forward<TArgs>(args)...) {}

  virtual ~UnknownObject() noexcept = default;
};

template <typename... TBaseTypes>
class DECLSPEC_NOVTABLE UnknownObject<Mso::RefCountStrategy::NoRefCount, TBaseTypes...>
    : public Mso::QueryCastList<TBaseTypes...> {
  using Super = Mso::QueryCastList<TBaseTypes...>;

 public:
  using UnknownObjectType = UnknownObject; // To use in derived class as "using Super = UnknownObjectType"

  MSO_OBJECT_NOREFCOUNT(UnknownObject);

  template <typename... TArgs>
  UnknownObject(TArgs &&... args) noexcept : Super(std::forward<TArgs>(args)...) {}

  _Success_(return == S_OK) STDMETHOD(QueryInterface)(const GUID &riid, _Outptr_ void **ppvObject) noexcept override {
    return ::Mso::Details::QueryInterfaceHelper<UnknownObject>::QueryInterface(this, riid, ppvObject);
  }

  // Do not use override to support ISimpleUnknown interfaces
  STDMETHOD_(ULONG, AddRef)() noexcept {
    return 1;
  }

  // Do not use override to support ISimpleUnknown interfaces
  STDMETHOD_(ULONG, Release)() noexcept {
    return 1;
  }

  virtual ~UnknownObject() noexcept = default;
};

template <typename... TBaseTypes>
class DECLSPEC_NOVTABLE UnknownObject<Mso::RefCountStrategy::NoRefCountNoQuery, TBaseTypes...>
    : public Mso::QueryCastList<TBaseTypes...> {
  using Super = Mso::QueryCastList<TBaseTypes...>;

 public:
  using UnknownObjectType = UnknownObject; // To use in derived class as "using Super = UnknownObjectType"

  MSO_OBJECT_NOREFCOUNT(UnknownObject);

  template <typename... TArgs>
  UnknownObject(TArgs &&... args) noexcept : Super(std::forward<TArgs>(args)...) {}

  _Success_(return == S_OK)
      STDMETHOD(QueryInterface)(const GUID & /*riid*/, _Outptr_ void ** /*ppvObject*/) noexcept override {
    return E_FAIL;
  }

  // Do not use override to support ISimpleUnknown interfaces
  STDMETHOD_(ULONG, AddRef)() noexcept {
    return 1;
  }

  // Do not use override to support ISimpleUnknown interfaces
  STDMETHOD_(ULONG, Release)() noexcept {
    return 1;
  }

  virtual ~UnknownObject() noexcept = default;
};

/**
        A base class that supports a free threaded marshaler.
*/
template <typename... TBaseTypes>
class AgileUnknownObject : public UnknownObject<TBaseTypes...> {
  using Super = UnknownObject<TBaseTypes...>;

 public:
  MSO_NO_COPY_CTOR_AND_ASSIGNMENT(AgileUnknownObject);

  template <typename... TArgs>
  AgileUnknownObject(TArgs &&... args) noexcept : Super(std::forward<TArgs>(args)...) {}

  _Success_(return == S_OK) STDMETHOD(QueryInterface)(const GUID &riid, _Outptr_ void **ppvObject) noexcept override {
    HRESULT hr = Super::QueryInterface(riid, ppvObject);
    if (hr == S_OK) {
      return hr;
    }

#if !defined(__clang__) && !defined(__GNUC__)
    // Free threaded marshaler currently supported only for Windows and VC++
    if (riid == __uuidof(IMarshal) || riid == __uuidof(IAgileObject)) {
      if (m_ftm.IsEmpty()) {
        Mso::CntPtr<Mso::IUnknown> ftm;
        VerifySucceededElseCrashTag(
            CoCreateFreeThreadedMarshaler(this->template StaticCastElseNull<Mso::IUnknown *>(), &ftm),
            0x01003719 /* tag_bad2z */);
        // Only assign ftm to m_ftm if its value is nullptr. Otherwise we will delete the new ftm and use the existing
        // m_ftm.
        if (InterlockedCompareExchangePointer((volatile PVOID *)m_ftm.GetRaw(), ftm.Get(), nullptr) == nullptr) {
          // Success: the object is now owned by the m_ftm. Make sure that ftm does not delete it.
          ftm.Detach();
        }
      }

      return m_ftm->QueryInterface(riid, ppvObject);
    }
#endif

    return E_NOINTERFACE;
  }

 protected:
  using AgileUnknownObjectType = AgileUnknownObject;

 private:
  Mso::CntPtr<Mso::IUnknown> m_ftm;
};

} // namespace Mso

END_DISABLE_WARNING_INCONSISTENT_MISSING_OVERRIDE()

#endif // MSO_OBJECT_UNKNOWNOBJECT_H
