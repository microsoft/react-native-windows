// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "object/unknownObject.h"
#include <future>
#include <thread>
#include <vector>
#include "comUtil/qiCast.h"
#include "eventWaitHandle/eventWaitHandle.h"
#include "motifCpp/testCheck.h"
#include "testAllocators.h"

//#define TEST_BAD_INHERITANCE1 // Uncomment to see compilation error
//#define TEST_BAD_INHERITANCE2 // Uncomment to confirm VEC, but observe a memory leak. We cannot safely destroy this
// class.

MSO_STRUCT_GUID(IBaseSample1, "16872411-FA64-436C-92F4-22FE6B536FC8")
struct DECLSPEC_NOVTABLE IBaseSample1 : public IUnknown {
  virtual int GetValue1() const = 0;
};

MSO_STRUCT_GUID(IBaseSample2, "978F3765-88B4-4610-A607-16FFD5537E9B")
struct DECLSPEC_NOVTABLE IBaseSample2 : public IUnknown {
  virtual int GetValue2() const = 0;
};

MSO_STRUCT_GUID(IBaseSample3, "DB0CE125-7AF8-47F5-A8F9-D176619ABA76")
struct DECLSPEC_NOVTABLE IBaseSample3 : public IUnknown {
  virtual int GetValue3() const = 0;
};

// Next four interfaces are interfaces for the case when we have only QueryInterface without AddRef/Release
// This is to address scenarios where we use ISimpleUnknown interface in Office code.
MSO_STRUCT_GUID(ISimpleUnknownSample1, "F770BE39-7ED8-4A8E-84FD-B5441CD7525E")
struct DECLSPEC_NOVTABLE ISimpleUnknownSample1 {
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(
      /* [in] */ REFIID riid,
      /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) = 0;
  virtual int GetValue1() const = 0;
};

MSO_STRUCT_GUID(ISimpleUnknownSample2, "47D5B036-BF11-4AF7-8D31-EB646E8D8CF2")
struct DECLSPEC_NOVTABLE ISimpleUnknownSample2 {
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(
      /* [in] */ REFIID riid,
      /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) = 0;
  virtual int GetValue2() const = 0;
};

MSO_STRUCT_GUID(ISimpleUnknownSample21, "ABB7D11B-D496-42CA-9529-DFB20226435E")
struct DECLSPEC_NOVTABLE ISimpleUnknownSample21 : public ISimpleUnknownSample2 {
  virtual int GetValue21() const = 0;
};

MSO_STRUCT_GUID(ISimpleUnknownSample3, "47BF0755-FA82-4D76-B609-520195A1F03E")
struct DECLSPEC_NOVTABLE ISimpleUnknownSample3 {
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(
      /* [in] */ REFIID riid,
      /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) = 0;
  virtual int GetValue3() const = 0;
};

// Unknown object with simple ref count that implements one IUnknown based interface.
class UnknownSample1 final : public Mso::UnknownObject<IBaseSample1> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

 protected:
  virtual ~UnknownSample1() noexcept {
    m_deleted = true;
  }

 private:
  UnknownSample1(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

class UnknownSample11 final : public Mso::UnknownObject<IBaseSample1> {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtorAndInitializeThis;
  friend MakePolicy; // To allow constructor to be private or protected.

  virtual int GetValue1() const noexcept override {
    return 1;
  }

 protected:
  virtual ~UnknownSample11() noexcept {
    *m_deleted = true;
  }

 private:
  UnknownSample11() noexcept {}

  bool InitializeThis(bool &deleted) noexcept {
    m_deleted = &deleted;
    return true;
  }

 private:
  bool *m_deleted = nullptr;
};

// Unknown object with simple ref count that implements two IUnknown based interfaces.
class UnknownSample2 final : public Mso::UnknownObject<IBaseSample1, IBaseSample2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

 protected:
  virtual ~UnknownSample2() noexcept {
    m_deleted = true;
  }

 private:
  UnknownSample2(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

class UnknownSample21 final : public Mso::UnknownObject<IBaseSample1, IBaseSample2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

 private:
  UnknownSample21() = default;
};

// Unknown object with simple ref count that implements two IUnknown based interfaces.
// Added QI support for the class itself.
MSO_CLASS_GUID(UnknownSample3, "5AF68CBD-A9D3-4B00-85F5-A0F8DA6E51DF")
class UnknownSample3 final
    : public Mso::UnknownObject<Mso::QueryCastDerived<UnknownSample3>, IBaseSample1, IBaseSample2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

 protected:
  virtual ~UnknownSample3() noexcept {
    m_deleted = true;
  }

 private:
  UnknownSample3(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Unknown object with support for weak references that implements one IUnknown based interface.
class UnknownSample4 final : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IBaseSample1> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

 protected:
  virtual ~UnknownSample4() noexcept {
    m_deleted = true;
  }

 private:
  UnknownSample4(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Unknown object with support for weak references that implements two IUnknown based interfaces.
class UnknownSample5 final : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IBaseSample1, IBaseSample2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

 protected:
  virtual ~UnknownSample5() noexcept {
    m_deleted = true;
  }

 private:
  UnknownSample5(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Unknown object with support for weak references that implements two IUnknown based interfaces.
// Added QI support for the class itself.
MSO_CLASS_GUID(UnknownSample6, "96075AC6-AE32-4ADF-AE8F-8B6B62790530")
class UnknownSample6 final : public Mso::UnknownObject<
                                 Mso::RefCountStrategy::WeakRef,
                                 Mso::QueryCastDerived<UnknownSample6>,
                                 IBaseSample1,
                                 IBaseSample2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

 private:
  UnknownSample6(bool &deleted) noexcept : m_deleted(deleted) {}

 protected:
  virtual ~UnknownSample6() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

// Types below are for a demo how to query for a base interface if querying for it is not implemented in a base class.
MSO_STRUCT_GUID(IDerivedBase1, "34C53457-1760-44BE-BC4B-39EC92E94DDC")
struct DECLSPEC_NOVTABLE IDerivedBase1 : IBaseSample1 {
  virtual int GetValue11() const = 0;
};

class IMockDerivedBase : public Mso::QueryCastList<IDerivedBase1> {
  virtual int GetValue1() const override {
    return 1;
  }

  virtual int GetValue11() const override {
    return 11;
  }
};

// To demo how to query for a base interface if querying for it is not implemented in a base class.
// Here we want to query for IBaseSample1, and IMockDerivedBase does not implement QueryCast for it.
// QueryCastChain only inherits from the first template parameter type and implements QueryCast for all provided types.
class UnknownSample7 final
    : public Mso::UnknownObject<Mso::QueryCastChain<IMockDerivedBase, IBaseSample1>, IBaseSample2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue2() const noexcept override {
    return 2;
  }

 protected:
  virtual ~UnknownSample7() noexcept {
    m_deleted = true;
  }

 private:
  UnknownSample7(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

class UnknownSample8 final : public Mso::UnknownObject<Mso::RefCountStrategy::NoRefCount, IBaseSample1> {
 public:
  UnknownSample8(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual ~UnknownSample8() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

/// Base class for UnknownObject that implements two interfaces and has non-default constructors.
class UnknownSample9Base : public Mso::QueryCastList<IBaseSample1, IBaseSample2> {
 public:
  virtual int GetValue1() const noexcept override {
    return m_param0 + m_param1 + m_param2;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

 protected:
  UnknownSample9Base() = default;
  UnknownSample9Base(int param0) noexcept : m_param0(param0) {}
  UnknownSample9Base(int param0, int param1) noexcept : m_param0(param0), m_param1(param1) {}
  UnknownSample9Base(int param0, int param1, int param2) noexcept
      : m_param0(param0), m_param1(param1), m_param2(param2) {}

 private:
  int m_param0 = 0;
  int m_param1 = 0;
  int m_param2 = 0;
};

/// Simple ref counted object inherited from UnknownSample9Base with non-default constructors.
class UnknownSample91 final : public Mso::UnknownObject<UnknownSample9Base, IBaseSample3> {
  using Super = UnknownObjectType;
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue3() const noexcept override {
    return 1;
  }

 protected:
  virtual ~UnknownSample91() noexcept {
    m_deleted = true;
  }

 private:
  UnknownSample91(bool &deleted) noexcept : m_deleted(deleted) {}

  UnknownSample91(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  UnknownSample91(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  UnknownSample91(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

/// Object with a weak reference inherited from UnknownSample9Base with non-default constructors.
class UnknownSample92 final
    : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, UnknownSample9Base, IBaseSample3> {
  using Super = UnknownObjectType;
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue3() const noexcept override {
    return 1;
  }

 protected:
  virtual ~UnknownSample92() noexcept {
    m_deleted = true;
  }

 private:
  UnknownSample92(bool &deleted) noexcept : m_deleted(deleted) {}

  UnknownSample92(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  UnknownSample92(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  UnknownSample92(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

/// Object without reference counting and inherited from UnknownSample9Base with non-default constructors.
class UnknownSample93 final
    : public Mso::UnknownObject<Mso::RefCountStrategy::NoRefCount, UnknownSample9Base, IBaseSample3> {
  using Super = UnknownObjectType;

 public:
  UnknownSample93(bool &deleted) noexcept : m_deleted(deleted) {}

  UnknownSample93(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  UnknownSample93(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  UnknownSample93(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

  virtual int GetValue3() const noexcept override {
    return 1;
  }

  virtual ~UnknownSample93() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

// Object with no reference and no query counting and inherited from UnknownSample9Base with non-default constructors.
class UnknownSample94 final
    : public Mso::UnknownObject<Mso::RefCountStrategy::NoRefCountNoQuery, UnknownSample9Base, IBaseSample3> {
  using Super = UnknownObjectType;

 public:
  UnknownSample94(bool &deleted) noexcept : m_deleted(deleted) {}

  UnknownSample94(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  UnknownSample94(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  UnknownSample94(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

  virtual int GetValue3() const noexcept override {
    return 1;
  }

  virtual ~UnknownSample94() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

struct AsyncDeleter2 {
  template <typename TObject>
  static void Delete(TObject *obj) noexcept {
    // Destroy object asynchronously
    obj->SetAsyncDestroy();
    try {
      // Ideally we want to show here how to use dispatch queues, but we cannot add DispatchQueue liblet dependency
      // here.
#pragma warning(suppress : 4834) // discarding return value of function with 'nodiscard' attribute
      std::async(std::launch::async, [obj]() noexcept { TObject::RefCountPolicy::template Delete(obj); });
    } catch (...) {
      VerifyElseCrashTag(false, 0x01003709 /* tag_bad2j */);
    }
  }
};

MSO_STRUCT_GUID(ITestAsyncDestroy2, "40d32222-f341-4d44-beae-cf6e2fe7fca9")
struct ITestAsyncDestroy2 {
  virtual void SetAsyncDestroy() noexcept = 0;
};

// To test custom deleter
class UnknownSample101 final
    : public Mso::UnknownObject<Mso::SimpleRefCountPolicy<AsyncDeleter2>, IBaseSample1, ITestAsyncDestroy2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual void SetAsyncDestroy() noexcept override {
    m_isAsyncDestroy = true;
  }

 protected:
  virtual ~UnknownSample101() noexcept override {
    m_deleted.Set();
  }

 private:
  UnknownSample101(Mso::ManualResetEvent const &deleted, bool &isAsyncDestroy) noexcept
      : m_deleted{deleted}, m_isAsyncDestroy{isAsyncDestroy} {}

 private:
  Mso::ManualResetEvent m_deleted;
  bool &m_isAsyncDestroy;
};

// To test custom deleter
class UnknownSample102 final
    : public Mso::UnknownObject<Mso::WeakRefCountPolicy<AsyncDeleter2>, IBaseSample1, ITestAsyncDestroy2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual void SetAsyncDestroy() noexcept override {
    m_isAsyncDestroy = true;
  }

 protected:
  virtual ~UnknownSample102() noexcept {
    m_deleted.Set();
  }

 private:
  UnknownSample102(Mso::ManualResetEvent const &deleted, bool &isAsyncDestroy) noexcept
      : m_deleted(deleted), m_isAsyncDestroy(isAsyncDestroy) {}

 private:
  Mso::ManualResetEvent m_deleted;
  bool &m_isAsyncDestroy;
};

// Object has a custom QueryInterface where it sets *ppvObject to nullptr in the beginning.
class UnknownSample111 final : public Mso::UnknownObject<IBaseSample1, IBaseSample2> {
  using Super = UnknownObjectType;
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  _Success_(return == S_OK) STDMETHOD(QueryInterface)(const GUID &riid, _Outptr_ void **ppvObject) noexcept override {
    *ppvObject = nullptr; // This is not needed but some code in Office does it.
    return Super::QueryInterface(riid, ppvObject);
  }

  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

 private:
  UnknownSample111() = default;
};

// Simple ref counted object inherited from IBaseSample1 with no refcounting and no query.
class UnknownSample12 final : public Mso::UnknownObject<Mso::RefCountStrategy::NoRefCountNoQuery, IBaseSample1> {
 public:
  UnknownSample12(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual ~UnknownSample12() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

// Agile object that implements a free threaded marshaller, and implements a simple ref counting.
class AgileSample1 final : public Mso::AgileUnknownObject<IBaseSample1, IBaseSample2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

 protected:
  virtual ~AgileSample1() noexcept {
    m_deleted = true;
  }

 private:
  AgileSample1(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Agile object that implements a free threaded marshaller, and supports weak references.
class AgileSample2 final : public Mso::AgileUnknownObject<Mso::RefCountStrategy::WeakRef, IBaseSample1> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

 protected:
  virtual ~AgileSample2() noexcept {
    m_deleted = true;
  }

 private:
  AgileSample2(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Agile object that implements a free threaded marshaller, and does not implement ref counting.
class AgileSample3 final : public Mso::AgileUnknownObject<Mso::RefCountStrategy::NoRefCount, IBaseSample1> {
 public:
  AgileSample3(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual ~AgileSample3() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

/// Simple ref counted agile object inherited from UnknownSample9Base with non-default constructors.
class AgileSample41 final : public Mso::AgileUnknownObject<UnknownSample9Base, IBaseSample3> {
  using Super = AgileUnknownObjectType;
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue3() const noexcept override {
    return 1;
  }

 protected:
  virtual ~AgileSample41() noexcept {
    m_deleted = true;
  }

 private:
  AgileSample41(bool &deleted) noexcept : m_deleted(deleted) {}

  AgileSample41(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  AgileSample41(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  AgileSample41(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

/// Agile object with a weak reference inherited from UnknownSample9Base with non-default constructors.
class AgileSample42 final
    : public Mso::AgileUnknownObject<Mso::RefCountStrategy::WeakRef, UnknownSample9Base, IBaseSample3> {
  using Super = AgileUnknownObjectType;
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue3() const noexcept override {
    return 1;
  }

 protected:
  virtual ~AgileSample42() noexcept {
    m_deleted = true;
  }

 private:
  AgileSample42(bool &deleted) noexcept : m_deleted(deleted) {}

  AgileSample42(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  AgileSample42(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  AgileSample42(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

/// Agile object without reference counting and inherited from UnknownSample9Base with non-default constructors.
class AgileSample43 final
    : public Mso::AgileUnknownObject<Mso::RefCountStrategy::NoRefCount, UnknownSample9Base, IBaseSample3> {
  using Super = AgileUnknownObjectType;

 public:
  AgileSample43(bool &deleted) noexcept : m_deleted(deleted) {}

  AgileSample43(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  AgileSample43(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  AgileSample43(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

  virtual int GetValue3() const noexcept override {
    return 1;
  }

  virtual ~AgileSample43() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

class SimpleUnknownSample1 final : public Mso::UnknownObject<Mso::RefCountStrategy::NoRefCount, ISimpleUnknownSample1> {
 public:
  SimpleUnknownSample1(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual ~SimpleUnknownSample1() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

class SimpleUnknownSample2 final
    : public Mso::UnknownObject<Mso::RefCountStrategy::NoRefCount, ISimpleUnknownSample1, ISimpleUnknownSample2> {
 public:
  SimpleUnknownSample2(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

  virtual ~SimpleUnknownSample2() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

class SimpleUnknownSample3 final : public Mso::UnknownObject<
                                       Mso::RefCountStrategy::NoRefCount,
                                       ISimpleUnknownSample1,
                                       Mso::QueryCastChain<ISimpleUnknownSample21, ISimpleUnknownSample2>> {
 public:
  SimpleUnknownSample3(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() const noexcept override {
    return 1;
  }

  virtual int GetValue2() const noexcept override {
    return 2;
  }

  virtual int GetValue21() const noexcept override {
    return 21;
  }

  virtual ~SimpleUnknownSample3() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

class SomeVirtualClass {
 public:
  virtual ~SomeVirtualClass() = default;

  int x;
  int y;
};

#ifdef TEST_BAD_INHERITANCE1
// !!! Mso::UnknownObject must be always the first one in the inheritance !!!
class BadUnknownObject1 final : public SomeVirtualClass, public Mso::UnknownObject<IBaseSample1> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(BadUnknownObject1);

  virtual int GetValue1() const override {
    return 1;
  }

 private:
  BadUnknownObject1() = default;
};
#endif

#ifdef TEST_BAD_INHERITANCE2
// !!! Mso::UnknownObject must be always the first one in the inheritance !!!
class BadUnknownObject2 final : public SomeVirtualClass,
                                public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IBaseSample1> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() const override {
    return 1;
  }

 private:
  BadUnknownObject1() = default;
};
#endif

class MyMemHeapUnknownSample1 final : public Mso::UnknownObject<StatefulAllocSimpleRefCount, IBaseSample1> {
 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

 protected:
  friend MakePolicy;

  MyMemHeapUnknownSample1(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual ~MyMemHeapUnknownSample1() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

class MyMemHeapUnknownSample2 final : public Mso::UnknownObject<StatefulAllocWeakRefCount, IBaseSample1> {
 public:
  virtual int GetValue1() const noexcept override {
    return 1;
  }

 protected:
  friend MakePolicy;

  MyMemHeapUnknownSample2(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual ~MyMemHeapUnknownSample2() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

TEST_CLASS (UnknownObjectTest) {
  TEST_METHOD(UnknownObject_SimpleRefCount_OneBaseInterface) {
    bool deleted = false;
    {
      Mso::CntPtr<UnknownSample1> unknown1 = Mso::Make<UnknownSample1>(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1->GetValue1());
      Debug(TestAssert::AreEqual(1u, unknown1->RefCount()));

      Mso::CntPtr<IBaseSample1> base1 = unknown1;
      TestAssert::AreEqual(1, base1->GetValue1());
      Debug(TestAssert::AreEqual(2u, unknown1->RefCount()));
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_SimpleRefCount_InitializeThis_OneBaseInterface) {
    bool deleted = false;
    {
      Mso::CntPtr<UnknownSample11> unknown1 = Mso::Make<UnknownSample11>(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1->GetValue1());
      Debug(TestAssert::AreEqual(1u, unknown1->RefCount()));

      Mso::CntPtr<IBaseSample1> base1 = unknown1;
      TestAssert::AreEqual(1, base1->GetValue1());
      Debug(TestAssert::AreEqual(2u, unknown1->RefCount()));
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_SimpleRefCount_TwoBaseInterfaces) {
    bool deleted = false;
    {
      Mso::CntPtr<UnknownSample2> unknown1 = Mso::Make<UnknownSample2>(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1->GetValue1());
      Debug(TestAssert::AreEqual(1u, unknown1->RefCount()));

      Mso::CntPtr<IBaseSample1> base1 = unknown1;
      TestAssert::AreEqual(1, base1->GetValue1());
      Debug(TestAssert::AreEqual(2u, unknown1->RefCount()));

      Mso::CntPtr<IBaseSample2> base2 = unknown1;
      TestAssert::AreEqual(2, base2->GetValue2());
      Debug(TestAssert::AreEqual(3u, unknown1->RefCount()));
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_SimpleRefCount_TwoBaseInterfacesAndDerived) {
    bool deleted = false;
    {
      Mso::CntPtr<UnknownSample3> unknown1 = Mso::Make<UnknownSample3>(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1->GetValue1());
      Debug(TestAssert::AreEqual(1u, unknown1->RefCount()));

      Mso::CntPtr<IBaseSample1> base1 = unknown1;
      TestAssert::AreEqual(1, base1->GetValue1());
      Debug(TestAssert::AreEqual(2u, unknown1->RefCount()));

      Mso::CntPtr<IBaseSample2> base2 = unknown1;
      TestAssert::AreEqual(2, base2->GetValue2());
      Debug(TestAssert::AreEqual(3u, unknown1->RefCount()));

      Mso::CntPtr<UnknownSample3> unknown2 = qi_cast<UnknownSample3>(base2.Get());
      TestAssert::AreEqual(2, unknown2->GetValue2());
      Debug(TestAssert::AreEqual(4u, unknown1->RefCount()));
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_WeakRefCount_ObjectWithWeakRefBase) {
    bool deleted = false;
    {
      Mso::CntPtr<UnknownSample4> unknown1 = Mso::Make<UnknownSample4>(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1->GetValue1());
      Debug(TestAssert::AreEqual(1u, unknown1->GetWeakRef().RefCount()));

      Mso::CntPtr<IBaseSample1> base1 = unknown1;
      Debug(TestAssert::AreEqual(2u, unknown1->GetWeakRef().RefCount()));

      Mso::ObjectWeakRef *obj1weakRef = query_cast<Mso::ObjectWeakRef *>(base1.Get());
      TestAssert::IsNotNull(obj1weakRef);
      Debug(TestAssert::AreEqual(2u, obj1weakRef->RefCount()));
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_WeakRefCount_OneInterface) {
    bool deleted = false;
    {
      Mso::WeakPtr<UnknownSample4> weakPtr;
      Mso::WeakPtr<IBaseSample1> weakPtr1;
      {
        Mso::CntPtr<UnknownSample4> unknown1 = Mso::Make<UnknownSample4>(/*ref*/ deleted);
        TestAssert::AreEqual(1, unknown1->GetValue1());

        Mso::CntPtr<IBaseSample1> base1 = unknown1;
        TestAssert::AreEqual(1, base1->GetValue1());

        weakPtr = unknown1;
        Mso::CntPtr<UnknownSample4> unknown11 = weakPtr.GetStrongPtr();
        TestAssert::IsNotNull(unknown11.Get());
        TestAssert::IsFalse(weakPtr.IsExpired());

        weakPtr1 = base1;
        Mso::CntPtr<IBaseSample1> base11 = weakPtr1.GetStrongPtr();
        TestAssert::IsNotNull(base11.Get());
        TestAssert::IsFalse(weakPtr1.IsExpired());
      }
      TestAssert::IsTrue(deleted);
      Mso::CntPtr<UnknownSample4> unknown12 = weakPtr.GetStrongPtr();
      TestAssert::IsNull(unknown12.Get());
      TestAssert::IsTrue(weakPtr1.IsExpired());
    }
  }

  TEST_METHOD(UnknownObject_WeakRefCount_TwoInterfaces) {
    bool deleted = false;
    {
      Mso::WeakPtr<UnknownSample5> weakPtr;
      Mso::WeakPtr<IBaseSample1> weakPtr1;
      Mso::WeakPtr<IBaseSample2> weakPtr2;
      {
        Mso::CntPtr<UnknownSample5> unknown1 = Mso::Make<UnknownSample5>(/*ref*/ deleted);
        TestAssert::AreEqual(1, unknown1->GetValue1());

        Mso::CntPtr<IBaseSample1> base1 = unknown1;
        TestAssert::AreEqual(1, base1->GetValue1());

        Mso::CntPtr<IBaseSample2> base2 = unknown1;
        TestAssert::AreEqual(2, base2->GetValue2());

        Mso::CntPtr<IBaseSample1> base11 = qi_cast<IBaseSample1>(base2.Get());
        TestAssert::IsTrue(base1.Get() == base11.Get(), L"Expected the same IBaseSample1 pointer");

        weakPtr = unknown1;
        Mso::CntPtr<UnknownSample5> unknown11 = weakPtr.GetStrongPtr();
        TestAssert::IsNotNull(unknown11.Get());
        TestAssert::IsFalse(weakPtr.IsExpired());

        weakPtr1 = base1;
        Mso::CntPtr<IBaseSample1> base12 = weakPtr1.GetStrongPtr();
        TestAssert::IsNotNull(base12.Get());
        TestAssert::IsFalse(weakPtr1.IsExpired());

        weakPtr2 = base2;
        Mso::CntPtr<IBaseSample2> base21 = weakPtr2.GetStrongPtr();
        TestAssert::IsNotNull(base21.Get());
        TestAssert::IsFalse(weakPtr2.IsExpired());
      }
      TestAssert::IsTrue(deleted);
      Mso::CntPtr<UnknownSample5> unknown12 = weakPtr.GetStrongPtr();
      TestAssert::IsNull(unknown12.Get());
      TestAssert::IsTrue(weakPtr1.IsExpired());
    }
  }

  TEST_METHOD(UnknownObject_WeakRefCount_TwoInterfacesAndDerived) {
    bool deleted = false;
    {
      Mso::WeakPtr<UnknownSample6> weakPtr;
      Mso::WeakPtr<IBaseSample1> weakPtr1;
      Mso::WeakPtr<IBaseSample2> weakPtr2;
      {
        Mso::CntPtr<UnknownSample6> unknown1 = Mso::Make<UnknownSample6>(/*ref*/ deleted);
        TestAssert::AreEqual(1, unknown1->GetValue1());

        Mso::CntPtr<IBaseSample1> base1 = unknown1;
        TestAssert::AreEqual(1, base1->GetValue1());

        Mso::CntPtr<IBaseSample2> base2 = unknown1;
        TestAssert::AreEqual(2, base2->GetValue2());

        Mso::CntPtr<UnknownSample6> unknown2 = qi_cast<UnknownSample6>(base1.Get());
        TestAssert::IsTrue(unknown1.Get() == unknown2.Get(), L"Expected the same UnknownSample6 instance");

        Mso::CntPtr<UnknownSample6> unknown3 = qi_cast<UnknownSample6>(base2.Get());
        TestAssert::IsTrue(unknown1.Get() == unknown3.Get(), L"Expected the same UnknownSample6 instance");

        weakPtr = unknown1;
        Mso::CntPtr<UnknownSample6> unknown11 = weakPtr.GetStrongPtr();
        TestAssert::IsNotNull(unknown11.Get());
        TestAssert::IsFalse(weakPtr.IsExpired());

        weakPtr1 = base1;
        Mso::CntPtr<IBaseSample1> base11 = weakPtr1.GetStrongPtr();
        TestAssert::IsNotNull(base11.Get());
        TestAssert::IsFalse(weakPtr1.IsExpired());

        weakPtr2 = base2;
        Mso::CntPtr<IBaseSample2> base21 = weakPtr2.GetStrongPtr();
        TestAssert::IsNotNull(base21.Get());
        TestAssert::IsFalse(weakPtr2.IsExpired());
      }
      TestAssert::IsTrue(deleted);
      Mso::CntPtr<UnknownSample6> unknown12 = weakPtr.GetStrongPtr();
      TestAssert::IsNull(unknown12.Get());
      TestAssert::IsTrue(weakPtr1.IsExpired());
    }
  }

  TEST_METHOD(UnknownObject_QueryForBaseInterface) {
    bool deleted = false;
    {
      Mso::CntPtr<UnknownSample7> unknown1 = Mso::Make<UnknownSample7>(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample2> base2 = qi_cast<IBaseSample2>(unknown1.Get());
      TestAssert::IsNotNull(base2.Get());
      TestAssert::AreEqual(2, base2->GetValue2());
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(base2.Get());
      TestAssert::IsNotNull(base1.Get());
      TestAssert::AreEqual(1, base1->GetValue1());
      Mso::CntPtr<IDerivedBase1> derivedBase1 = qi_cast<IDerivedBase1>(base2.Get());
      TestAssert::IsNotNull(derivedBase1.Get());
      TestAssert::AreEqual(11, derivedBase1->GetValue11());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_NoRefCount_Stack) {
    bool deleted = false;
    {
      UnknownSample8 unknown1(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample2> base2 = qi_cast<IBaseSample2>(&unknown1);
      TestAssert::IsNull(base2.Get());
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(&unknown1);
      TestAssert::IsNotNull(base1.Get());
      TestAssert::AreEqual(1, base1->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_NoRefCount_UniquePtr) {
    bool deleted = false;
    {
      std::unique_ptr<UnknownSample8> unknown1 = std::make_unique<UnknownSample8>(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample2> base2 = qi_cast<IBaseSample2>(unknown1.get());
      TestAssert::IsNull(base2.Get());
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(unknown1.get());
      TestAssert::IsNotNull(base1.Get());
      TestAssert::AreEqual(1, base1->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      Mso::CntPtr<UnknownSample91> unknown1 = Mso::Make<UnknownSample91>(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(unknown1.Get());
      TestAssert::AreEqual(0, base1->GetValue1());

      Mso::CntPtr<UnknownSample91> unknown2 = Mso::Make<UnknownSample91>(/*ref*/ deleted, 3);
      Mso::CntPtr<IBaseSample1> base2 = qi_cast<IBaseSample1>(unknown2.Get());
      TestAssert::AreEqual(3, base2->GetValue1());

      Mso::CntPtr<UnknownSample91> unknown3 = Mso::Make<UnknownSample91>(/*ref*/ deleted, 3, 5);
      Mso::CntPtr<IBaseSample1> base3 = qi_cast<IBaseSample1>(unknown3.Get());
      TestAssert::AreEqual(8, base3->GetValue1());

      Mso::CntPtr<UnknownSample91> unknown4 = Mso::Make<UnknownSample91>(/*ref*/ deleted, 3, 5, 11);
      Mso::CntPtr<IBaseSample1> base4 = qi_cast<IBaseSample1>(unknown4.Get());
      TestAssert::AreEqual(19, base4->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_WeakRef_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      Mso::CntPtr<UnknownSample92> unknown1 = Mso::Make<UnknownSample92>(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(unknown1.Get());
      TestAssert::AreEqual(0, base1->GetValue1());

      Mso::CntPtr<UnknownSample92> unknown2 = Mso::Make<UnknownSample92>(/*ref*/ deleted, 3);
      Mso::CntPtr<IBaseSample1> base2 = qi_cast<IBaseSample1>(unknown2.Get());
      TestAssert::AreEqual(3, base2->GetValue1());

      Mso::CntPtr<UnknownSample92> unknown3 = Mso::Make<UnknownSample92>(/*ref*/ deleted, 3, 5);
      Mso::CntPtr<IBaseSample1> base3 = qi_cast<IBaseSample1>(unknown3.Get());
      TestAssert::AreEqual(8, base3->GetValue1());

      Mso::CntPtr<UnknownSample92> unknown4 = Mso::Make<UnknownSample92>(/*ref*/ deleted, 3, 5, 11);
      Mso::CntPtr<IBaseSample1> base4 = qi_cast<IBaseSample1>(unknown4.Get());
      TestAssert::AreEqual(19, base4->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_NoRefCount_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      UnknownSample93 unknown1(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(&unknown1);
      TestAssert::AreEqual(0, base1->GetValue1());

      UnknownSample93 unknown2(/*ref*/ deleted, 3);
      Mso::CntPtr<IBaseSample1> base2 = qi_cast<IBaseSample1>(&unknown2);
      TestAssert::AreEqual(3, base2->GetValue1());

      UnknownSample93 unknown3(/*ref*/ deleted, 3, 5);
      Mso::CntPtr<IBaseSample1> base3 = qi_cast<IBaseSample1>(&unknown3);
      TestAssert::AreEqual(8, base3->GetValue1());

      UnknownSample93 unknown4(/*ref*/ deleted, 3, 5, 11);
      Mso::CntPtr<IBaseSample1> base4 = qi_cast<IBaseSample1>(&unknown4);
      TestAssert::AreEqual(19, base4->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_NoRefCountNoQuery_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      UnknownSample94 unknown1(/*ref*/ deleted);
      TestAssert::AreEqual(0, unknown1.GetValue1());

      UnknownSample94 unknown2(/*ref*/ deleted, 3);
      TestAssert::AreEqual(3, unknown2.GetValue1());

      UnknownSample94 unknown3(/*ref*/ deleted, 3, 5);
      TestAssert::AreEqual(8, unknown3.GetValue1());

      UnknownSample94 unknown4(/*ref*/ deleted, 3, 5, 11);
      TestAssert::AreEqual(19, unknown4.GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_DestroyThis) {
    Mso::ManualResetEvent deleted;
    bool isAsyncDestroy = false;

    {
      Mso::CntPtr<UnknownSample101> unknown1 = Mso::Make<UnknownSample101>(/*ref*/ deleted, /*ref*/ isAsyncDestroy);
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(unknown1.Get());
      TestAssert::AreEqual(1, base1->GetValue1());
    }

    // Wait for deletion to complete
    TestAssert::IsTrue(deleted.Wait());
    TestAssert::IsTrue(isAsyncDestroy);
  }

  TEST_METHOD(UnknownObject_WeakRef_DestroyThis) {
    Mso::ManualResetEvent deleted;
    bool isAsyncDestroy = false;

    {
      Mso::CntPtr<UnknownSample102> unknown1 = Mso::Make<UnknownSample102>(/*ref*/ deleted, /*ref*/ isAsyncDestroy);
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(unknown1.Get());
      TestAssert::AreEqual(1, base1->GetValue1());
    }

    // Wait for deletion to complete
    TestAssert::IsTrue(deleted.Wait());
    TestAssert::IsTrue(isAsyncDestroy);
  }

  TESTMETHOD_REQUIRES_SEH(UnknownObject_QI_NullPtr) {
    Mso::CntPtr<IBaseSample1> base1 = Mso::Make<UnknownSample21, IBaseSample1>();

    TestAssert::ExpectVEC([&]() noexcept {
      OACR_WARNING_SUPPRESS(
          INVALID_PARAM_VALUE_1,
          "Invalid parameter value. Our goal here is to see runtime check for the invalid value");
      (void)base1->QueryInterface(__uuidof(IBaseSample2), nullptr);
    });
  }

  TEST_METHOD(UnknownObject_NoRefCountNoQuery_Stack) {
    bool deleted = false;
    {
      UnknownSample12 unknown1(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample2> base2 = qi_cast<IBaseSample2>(&unknown1);
      TestAssert::IsNull(base2.Get());
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(&unknown1);
      TestAssert::IsNull(base1.Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_NoRefCountNoQuery_UniquePtr) {
    bool deleted = false;
    {
      std::unique_ptr<UnknownSample12> unknown1 = std::make_unique<UnknownSample12>(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample2> base2 = qi_cast<IBaseSample2>(unknown1.get());
      TestAssert::IsNull(base2.Get());
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(unknown1.get());
      TestAssert::IsNull(base1.Get());
    }
    TestAssert::IsTrue(deleted);
  }

#if defined(MSO_ENABLE_QICHECK) && defined(DEBUG) && !defined(__clang__)
  TESTMETHOD_REQUIRES_SEH(UnknownObject_QI_NotNullResult) {
    Mso::CntPtr<IBaseSample1> base1 = Mso::Make<UnknownSample21, IBaseSample1>();
    Mso::CntPtr<IBaseSample2> base2 = Mso::Make<UnknownSample21, IBaseSample2>();

    TestAssert::ExpectVEC(
        [&]() noexcept { base1->QueryInterface(__uuidof(IBaseSample2), reinterpret_cast<void **>(base2.GetRaw())); });
  }
#endif

  TEST_METHOD(AgileUnknownObject_SimpleRefCount) {
    bool deleted = false;
    {
      Mso::CntPtr<AgileSample1> unknown1 = Mso::Make<AgileSample1>(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1->GetValue1());
      Debug(TestAssert::AreEqual(1u, unknown1->RefCount()));

      Mso::CntPtr<IBaseSample1> base1 = unknown1;
      TestAssert::AreEqual(1, base1->GetValue1());
      Debug(TestAssert::AreEqual(2u, unknown1->RefCount()));

#ifndef __clang__
      Mso::CntPtr<IMarshal> marshal1 = qi_cast<IMarshal>(base1.Get());
      TestAssert::IsNotNull(marshal1.Get(), L"IMarshal must not be null.");
      Debug(TestAssert::AreEqual(3u, unknown1->RefCount()));

      Mso::CntPtr<IAgileObject> agile1 = qi_cast<IAgileObject>(base1.Get());
      TestAssert::IsNotNull(agile1.Get(), L"IAgileObject must not be null.");
      Debug(TestAssert::AreEqual(4u, unknown1->RefCount()));
#endif
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(AgileUnknownObject_WeakRefCount) {
    bool deleted = false;
    {
      Mso::WeakPtr<AgileSample2> weakAgile;
      Mso::WeakPtr<IBaseSample1> weakBase1;
#ifndef __clang__
      Mso::WeakPtr<IMarshal> weakMarshal;
      Mso::WeakPtr<IAgileObject> weakAgileObj;
#endif
      {
        Mso::CntPtr<AgileSample2> agile1 = Mso::Make<AgileSample2>(/*ref*/ deleted);
        TestAssert::AreEqual(1, agile1->GetValue1());
        Debug(TestAssert::AreEqual(1u, agile1->GetWeakRef().RefCount()));

        Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(agile1.Get());
        TestAssert::AreEqual(1, base1->GetValue1());
        Debug(TestAssert::AreEqual(2u, agile1->GetWeakRef().RefCount()));

#ifndef __clang__
        Mso::CntPtr<IMarshal> marshal1 = qi_cast<IMarshal>(base1.Get());
        TestAssert::IsNotNull(marshal1.Get(), L"IMarshal must not be null.");
        Debug(TestAssert::AreEqual(3u, agile1->GetWeakRef().RefCount()));

        Mso::CntPtr<IAgileObject> agileObj1 = qi_cast<IAgileObject>(marshal1.Get());
        TestAssert::IsNotNull(agileObj1.Get(), L"IAgileObject must not be null.");
        Debug(TestAssert::AreEqual(4u, agile1->GetWeakRef().RefCount()));
#endif

        weakAgile = agile1;
        Mso::CntPtr<AgileSample2> agile11 = weakAgile.GetStrongPtr();
        TestAssert::IsNotNull(agile11.Get());
        TestAssert::IsFalse(weakAgile.IsExpired());

        weakBase1 = base1;
        Mso::CntPtr<IBaseSample1> base11 = weakBase1.GetStrongPtr();
        TestAssert::IsNotNull(base11.Get());
        TestAssert::IsFalse(weakBase1.IsExpired());

#ifndef __clang__
        weakMarshal = marshal1;
        Mso::CntPtr<IMarshal> marshal11 = weakMarshal.GetStrongPtr();
        TestAssert::IsNotNull(marshal11.Get());
        TestAssert::IsFalse(weakMarshal.IsExpired());

        weakAgileObj = agileObj1;
        Mso::CntPtr<IAgileObject> agileObj11 = weakAgileObj.GetStrongPtr();
        TestAssert::IsNotNull(agileObj11.Get());
        TestAssert::IsFalse(weakAgileObj.IsExpired());
#endif
      }
      TestAssert::IsTrue(deleted);
      Mso::CntPtr<AgileSample2> agile12 = weakAgile.GetStrongPtr();
      TestAssert::IsNull(agile12.Get());
      TestAssert::IsTrue(weakAgile.IsExpired());
    }
  }

  TEST_METHOD(AgileUnknownObject_NoRefCount_Stack) {
    bool deleted = false;
    {
      AgileSample3 agile1(/*ref*/ deleted);

#ifndef __clang__
      Mso::CntPtr<IMarshal> marshal1 = qi_cast<IMarshal>(&agile1);
      TestAssert::IsNotNull(marshal1.Get());

      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(marshal1.Get());
      TestAssert::IsNotNull(base1.Get());

      Mso::CntPtr<IAgileObject> agileObj1 = qi_cast<IAgileObject>(base1.Get());
      TestAssert::IsNotNull(agileObj1.Get());
#endif

      Mso::CntPtr<IBaseSample1> base11 = qi_cast<IBaseSample1>(&agile1);
      TestAssert::IsNotNull(base11.Get());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(AgileUnknownObject_NoRefCount_UniquePtr) {
    bool deleted = false;
    {
      std::unique_ptr<AgileSample3> agile1 = std::make_unique<AgileSample3>(/*ref*/ deleted);

#ifndef __clang__
      Mso::CntPtr<IMarshal> marshal1 = qi_cast<IMarshal>(agile1.get());
      TestAssert::IsNotNull(marshal1.Get());

      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(marshal1.Get());
      TestAssert::IsNotNull(base1.Get());

      Mso::CntPtr<IAgileObject> agileObj1 = qi_cast<IAgileObject>(base1.Get());
      TestAssert::IsNotNull(agileObj1.Get());
#endif

      Mso::CntPtr<IBaseSample1> base11 = qi_cast<IBaseSample1>(agile1.get());
      TestAssert::IsNotNull(base11.Get());
    }
    TestAssert::IsTrue(deleted);
  }

// OM:2065596: Investigate why this test fails sth.exe for winrt nx64.
#if !defined(TESTWINRT) && !defined(__clang__)
  // Make sure that we initialize marshaller atomically
  TEST_METHOD(AgileUnknownObject_SimpleRefCount_ThreadSafety) {
    // Run this test several times
    for (int testIteration = 0; testIteration < 20; ++testIteration) {
      // Create an array of threads and ask for IMarshal in each thread,
      // store them in an array and see that we got the same result for all cases.
      const size_t threadCount = 20;
      std::vector<std::thread> threads;
      threads.reserve(20);

      std::vector<Mso::CntPtr<IMarshal>> results;
      results.resize(threadCount);

      bool deleted = false;
      Mso::CntPtr<AgileSample1> agile1 = Mso::Make<AgileSample1>(/*ref*/ deleted);

      for (size_t i = 0; i < threadCount; ++i) {
        threads.push_back(
            std::thread([&](size_t index) -> void { results[index] = qi_cast<IMarshal>(agile1.Get()); }, i));
      }

      // Wait until all threads finish
      for (auto &t : threads) {
        t.join();
      }

      // See that we have the same result
      Mso::CntPtr<IMarshal> expected = qi_cast<IMarshal>(agile1.Get());
      TestAssert::IsNotNull(expected.Get(), L"Expected not null IMarshal instance.");
      for (const auto &result : results) {
        TestAssert::IsTrue(expected.Get() == result.Get(), L"IMarshal instances are different");
      }
    }
  }
#endif

  TEST_METHOD(AgileUnknownObject_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      Mso::CntPtr<AgileSample41> agile1 = Mso::Make<AgileSample41>(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(agile1.Get());
      TestAssert::AreEqual(0, base1->GetValue1());

      Mso::CntPtr<AgileSample41> agile2 = Mso::Make<AgileSample41>(/*ref*/ deleted, 3);
      Mso::CntPtr<IBaseSample1> base2 = qi_cast<IBaseSample1>(agile2.Get());
      TestAssert::AreEqual(3, base2->GetValue1());

      Mso::CntPtr<AgileSample41> agile3 = Mso::Make<AgileSample41>(/*ref*/ deleted, 3, 5);
      Mso::CntPtr<IBaseSample1> base3 = qi_cast<IBaseSample1>(agile3.Get());
      TestAssert::AreEqual(8, base3->GetValue1());

      Mso::CntPtr<AgileSample41> agile4 = Mso::Make<AgileSample41>(/*ref*/ deleted, 3, 5, 11);
      Mso::CntPtr<IBaseSample1> base4 = qi_cast<IBaseSample1>(agile4.Get());
      TestAssert::AreEqual(19, base4->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(AgileUnknownObject_WeakRef_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      Mso::CntPtr<AgileSample42> agile1 = Mso::Make<AgileSample42>(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(agile1.Get());
      TestAssert::AreEqual(0, base1->GetValue1());

      Mso::CntPtr<AgileSample42> agile2 = Mso::Make<AgileSample42>(/*ref*/ deleted, 3);
      Mso::CntPtr<IBaseSample1> base2 = qi_cast<IBaseSample1>(agile2.Get());
      TestAssert::AreEqual(3, base2->GetValue1());

      Mso::CntPtr<AgileSample42> agile3 = Mso::Make<AgileSample42>(/*ref*/ deleted, 3, 5);
      Mso::CntPtr<IBaseSample1> base3 = qi_cast<IBaseSample1>(agile3.Get());
      TestAssert::AreEqual(8, base3->GetValue1());

      Mso::CntPtr<AgileSample42> agile4 = Mso::Make<AgileSample42>(/*ref*/ deleted, 3, 5, 11);
      Mso::CntPtr<IBaseSample1> base4 = qi_cast<IBaseSample1>(agile4.Get());
      TestAssert::AreEqual(19, base4->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(AgileUnknownObject_NoRefCount_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      AgileSample43 agile1(/*ref*/ deleted);
      Mso::CntPtr<IBaseSample1> base1 = qi_cast<IBaseSample1>(&agile1);
      TestAssert::AreEqual(0, base1->GetValue1());

      AgileSample43 agile2(/*ref*/ deleted, 3);
      Mso::CntPtr<IBaseSample1> base2 = qi_cast<IBaseSample1>(&agile2);
      TestAssert::AreEqual(3, base2->GetValue1());

      AgileSample43 agile3(/*ref*/ deleted, 3, 5);
      Mso::CntPtr<IBaseSample1> base3 = qi_cast<IBaseSample1>(&agile3);
      TestAssert::AreEqual(8, base3->GetValue1());

      AgileSample43 agile4(/*ref*/ deleted, 3, 5, 11);
      Mso::CntPtr<IBaseSample1> base4 = qi_cast<IBaseSample1>(&agile4);
      TestAssert::AreEqual(19, base4->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_query_cast_null) {
    // UnknownSample3 implements IBaseSample1, IBaseSample2, and not IBaseSample3
    UnknownSample3 *unknown1 = nullptr;
    IBaseSample1 *base1 = query_cast<IBaseSample1 *>(unknown1);
    TestAssert::IsNull(base1);
    IBaseSample2 *base2 = query_cast<IBaseSample2 *>(base1);
    TestAssert::IsNull(base2);
    IBaseSample3 *base3 = query_cast<IBaseSample3 *>(base1);
    TestAssert::IsNull(base3);
    UnknownSample3 *unknown2 = query_cast<UnknownSample3 *>(base2);
    TestAssert::IsNull(unknown2);
  }

  TEST_METHOD(UnknownObject_query_cast_const_null) {
    // UnknownSample3 implements IBaseSample1, IBaseSample2, and not IBaseSample3
    const UnknownSample3 *unknown1 = nullptr;
    const IBaseSample1 *base1 = query_cast<const IBaseSample1 *>(unknown1);
    TestAssert::IsNull(base1);
    const IBaseSample2 *base2 = query_cast<const IBaseSample2 *>(base1);
    TestAssert::IsNull(base2);
    const IBaseSample3 *base3 = query_cast<const IBaseSample3 *>(base1);
    TestAssert::IsNull(base3);
    const UnknownSample3 *unknown2 = query_cast<const UnknownSample3 *>(base2);
    TestAssert::IsNull(unknown2);
  }

  TEST_METHOD(UnknownObject_query_cast_ptr) {
    bool deleted = false;
    {
      // UnknownSample3 implements IBaseSample1, IBaseSample2, and not IBaseSample3
      Mso::CntPtr<UnknownSample3> unknown1 = Mso::Make<UnknownSample3>(/*ref*/ deleted);
      IBaseSample1 *base1 = query_cast<IBaseSample1 *>(unknown1.Get());
      TestAssert::AreEqual(1, base1->GetValue1());
      IBaseSample2 *base2 = query_cast<IBaseSample2 *>(base1);
      TestAssert::AreEqual(2, base2->GetValue2());
      IBaseSample3 *base3 = query_cast<IBaseSample3 *>(base1);
      TestAssert::IsNull(base3);
      UnknownSample3 *unknown2 = query_cast<UnknownSample3 *>(base2);
      OACR_USE_PTR(unknown2); // We do not want to make unknown2 const in this test.
      TestAssert::AreEqual(2, unknown2->GetValue2());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_query_cast_const_ptr) {
    bool deleted = false;
    {
      // UnknownSample3 implements IBaseSample1, IBaseSample2, and not IBaseSample3
      Mso::CntPtr<const UnknownSample3> unknown1 = Mso::Make<UnknownSample3>(/*ref*/ deleted);
      const IBaseSample1 *base1 = query_cast<const IBaseSample1 *>(unknown1.Get());
      TestAssert::AreEqual(1, base1->GetValue1());
      const IBaseSample2 *base2 = query_cast<const IBaseSample2 *>(base1);
      TestAssert::AreEqual(2, base2->GetValue2());
      const IBaseSample3 *base3 = query_cast<const IBaseSample3 *>(base1);
      TestAssert::IsNull(base3);
      const UnknownSample3 *unknown2 = query_cast<const UnknownSample3 *>(base2);
      TestAssert::AreEqual(2, unknown2->GetValue2());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_query_cast_const_ptr_from_ptr) {
    bool deleted = false;
    {
      // UnknownSample3 implements IBaseSample1, IBaseSample2, and not IBaseSample3
      Mso::CntPtr<UnknownSample3> unknown1 = Mso::Make<UnknownSample3>(/*ref*/ deleted);
      const IBaseSample1 *base1 = query_cast<const IBaseSample1 *>(unknown1.Get());
      TestAssert::AreEqual(1, base1->GetValue1());
      const IBaseSample2 *base2 = query_cast<const IBaseSample2 *>(const_cast<IBaseSample1 *>(base1));
      TestAssert::AreEqual(2, base2->GetValue2());
      const IBaseSample3 *base3 = query_cast<const IBaseSample3 *>(const_cast<IBaseSample1 *>(base1));
      TestAssert::IsNull(base3);
      const UnknownSample3 *unknown2 = query_cast<const UnknownSample3 *>(const_cast<IBaseSample2 *>(base2));
      TestAssert::AreEqual(2, unknown2->GetValue2());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_query_cast_if_set_nullptr) {
    // UnknownSample111 implements IBaseSample1, IBaseSample2, and not IBaseSample3
    Mso::CntPtr<IBaseSample1> base1 = Mso::Make<UnknownSample111, IBaseSample1>();
    IBaseSample2 *base2 = query_cast<IBaseSample2 *>(base1.Get());
    TestAssert::AreEqual(2, base2->GetValue2());
    const IBaseSample1 *base11 = query_cast<IBaseSample1 *>(base2);
    TestAssert::AreEqual(1, base11->GetValue1());
    IBaseSample3 *base3 = query_cast<IBaseSample3 *>(base1.Get());
    TestAssert::IsNull(base3);
  }

  TESTMETHOD_REQUIRES_SEH(UnknownObject_query_cast_ref) {
    bool deleted = false;
    {
      // UnknownSample3 implements IBaseSample1, IBaseSample2, and not IBaseSample3
      Mso::CntPtr<UnknownSample3> unknown1 = Mso::Make<UnknownSample3>(/*ref*/ deleted);
      IBaseSample1 &base1 = query_cast<IBaseSample1 &>(*unknown1);
      TestAssert::AreEqual(1, base1.GetValue1());
      IBaseSample2 &base2 = query_cast<IBaseSample2 &>(base1);

#if !__clang__ // OfficeMain:768379. TestAssert::ExpectCrash is not implemented in clang.
      TestAssert::AreEqual(2, base2.GetValue2());
      {
        TestAssert::ExpectVEC([&]() noexcept { query_cast<IBaseSample3 &>(base1); });
      }
#endif //!__clang__

      UnknownSample3 &unknown2 = query_cast<UnknownSample3 &>(base2);
      OACR_USE_PTR(&unknown2); // We do not want to make unknown2 const in this test.
      TestAssert::AreEqual(2, unknown2.GetValue2());
    }
    TestAssert::IsTrue(deleted);
  }

  TESTMETHOD_REQUIRES_SEH(UnknownObject_query_cast_const_ref) {
    bool deleted = false;
    {
      // UnknownSample3 implements IBaseSample1, IBaseSample2, and not IBaseSample3
      Mso::CntPtr<const UnknownSample3> unknown1 = Mso::Make<UnknownSample3>(/*ref*/ deleted);
      const IBaseSample1 &base1 = query_cast<const IBaseSample1 &>(*unknown1);
      TestAssert::AreEqual(1, base1.GetValue1());
      const IBaseSample2 &base2 = query_cast<const IBaseSample2 &>(base1);

#if !__clang__ // OfficeMain:768379. TestAssert::ExpectCrash is not implemented in clang.
      TestAssert::AreEqual(2, base2.GetValue2());
      {
        TestAssert::ExpectVEC([&]() noexcept { query_cast<const IBaseSample3 &>(base1); });
      }
#endif //!__clang__

      const UnknownSample3 &unknown2 = query_cast<const UnknownSample3 &>(base2);
      TestAssert::AreEqual(2, unknown2.GetValue2());
    }
    TestAssert::IsTrue(deleted);
  }

  TESTMETHOD_REQUIRES_SEH(UnknownObject_query_cast_const_ref_from_ref) {
    bool deleted = false;
    {
      // UnknownSample3 implements IBaseSample1, IBaseSample2, and not IBaseSample3
      Mso::CntPtr<const UnknownSample3> unknown1 = Mso::Make<UnknownSample3>(/*ref*/ deleted);
      const IBaseSample1 &base1 = query_cast<const IBaseSample1 &>(const_cast<UnknownSample3 &>(*unknown1));
      TestAssert::AreEqual(1, base1.GetValue1());
      const IBaseSample2 &base2 = query_cast<const IBaseSample2 &>(const_cast<IBaseSample1 &>(base1));

#if !__clang__ // OfficeMain:768379. TestAssert::ExpectCrash is not implemented in clang.
      TestAssert::AreEqual(2, base2.GetValue2());
      {
        TestAssert::ExpectVEC([&]() noexcept { query_cast<const IBaseSample3 &>(const_cast<IBaseSample1 &>(base1)); });
      }
#endif //!__clang__

      const UnknownSample3 &unknown2 = query_cast<const UnknownSample3 &>(const_cast<IBaseSample2 &>(base2));
      TestAssert::AreEqual(2, unknown2.GetValue2());
    }
    TestAssert::IsTrue(deleted);
  }

  TESTMETHOD_REQUIRES_SEH(UnknownObject_WeakRefCount_query_cast_ptr) {
    bool deleted = false;
    {
      // UnknownSample6 implements IBaseSample1, IBaseSample2, and not IBaseSample3
      Mso::CntPtr<UnknownSample6> unknown1 = Mso::Make<UnknownSample6>(/*ref*/ deleted);
      IBaseSample1 *base1 = query_cast<IBaseSample1 *>(unknown1.Get());
      TestAssert::AreEqual(1, base1->GetValue1());
      IBaseSample2 *base2 = query_cast<IBaseSample2 *>(base1);
      TestAssert::AreEqual(2, base2->GetValue2());
      IBaseSample3 *base3 = query_cast<IBaseSample3 *>(base1);
      TestAssert::IsNull(base3);
      UnknownSample6 *unknown2 = query_cast<UnknownSample6 *>(base2);
      OACR_USE_PTR(unknown2); // We do not want to make unknown2 const in this test.
      TestAssert::AreEqual(2, unknown2->GetValue2());
    }
    TestAssert::IsTrue(deleted);
  }

  TESTMETHOD_REQUIRES_SEH(UnknownObject_WeakRefCount_query_cast_ref) {
    bool deleted = false;
    {
      // UnknownSample6 implements IBaseSample1, IBaseSample2, and not IBaseSample3
      Mso::CntPtr<UnknownSample6> unknown1 = Mso::Make<UnknownSample6>(/*ref*/ deleted);
      IBaseSample1 &base1 = query_cast<IBaseSample1 &>(*unknown1);
      TestAssert::AreEqual(1, base1.GetValue1());
      IBaseSample2 &base2 = query_cast<IBaseSample2 &>(base1);

#if !__clang__ // OfficeMain:768379. TestAssert::ExpectCrash is not implemented in clang.
      TestAssert::AreEqual(2, base2.GetValue2());
      {
        TestAssert::ExpectVEC([&]() noexcept { query_cast<IBaseSample3 &>(base1); });
      }
#endif //!__clang__

      UnknownSample6 &unknown2 = query_cast<UnknownSample6 &>(base2);
      OACR_USE_PTR(&unknown2); // We do not want to make unknown2 const in this test.
      TestAssert::AreEqual(2, unknown2.GetValue2());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_SimpleUnknown_OneBaseInterface) {
    bool deleted = false;
    {
      SimpleUnknownSample1 unknown1(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1.GetValue1());

      const ISimpleUnknownSample1 *base1 = &unknown1;
      TestAssert::AreEqual(1, base1->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_SimpleUnknown_TwoBaseInterfaces) {
    bool deleted = false;
    {
      SimpleUnknownSample2 unknown1(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1.GetValue1());

      const ISimpleUnknownSample1 *base1 = &unknown1;
      TestAssert::AreEqual(1, base1->GetValue1());

      const ISimpleUnknownSample2 *base2 = simpleqi_cast<ISimpleUnknownSample2>(base1);
      TestAssert::AreEqual(2, base2->GetValue2());

      const ISimpleUnknownSample1 *base11 = simpleqi_cast<ISimpleUnknownSample1>(base2);
      TestAssert::AreEqual(1, base11->GetValue1());

      const ISimpleUnknownSample3 *base3 = simpleqi_cast<ISimpleUnknownSample3>(base2);
      TestAssert::IsNull(base3);
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_SimpleUnknown_TwoBaseInterfaces_Unique_ptr) {
    bool deleted = false;
    {
      std::unique_ptr<SimpleUnknownSample2> unknown1 = std::make_unique<SimpleUnknownSample2>(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1->GetValue1());

      const ISimpleUnknownSample1 *base1 = unknown1.get();
      TestAssert::AreEqual(1, base1->GetValue1());

      const ISimpleUnknownSample2 *base2 = simpleqi_cast<ISimpleUnknownSample2>(base1);
      TestAssert::AreEqual(2, base2->GetValue2());

      const ISimpleUnknownSample1 *base11 = simpleqi_cast<ISimpleUnknownSample1>(base2);
      TestAssert::AreEqual(1, base11->GetValue1());

      const ISimpleUnknownSample3 *base3 = simpleqi_cast<ISimpleUnknownSample3>(base2);
      TestAssert::IsNull(base3);
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(UnknownObject_SimpleUnknown_Derived) {
    bool deleted = false;
    {
      SimpleUnknownSample3 unknown1(/*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1.GetValue1());

      const ISimpleUnknownSample1 *base1 = &unknown1;
      TestAssert::AreEqual(1, base1->GetValue1());

      const ISimpleUnknownSample2 *base2 = simpleqi_cast<ISimpleUnknownSample2>(base1);
      TestAssert::AreEqual(2, base2->GetValue2());

      const ISimpleUnknownSample1 *base11 = simpleqi_cast<ISimpleUnknownSample1>(base2);
      TestAssert::AreEqual(1, base11->GetValue1());

      const ISimpleUnknownSample3 *base3 = simpleqi_cast<ISimpleUnknownSample3>(base2);
      TestAssert::IsNull(base3);

      const ISimpleUnknownSample21 *base21 = simpleqi_cast<ISimpleUnknownSample21>(base1);
      TestAssert::AreEqual(21, base21->GetValue21());
    }
    TestAssert::IsTrue(deleted);
  }

#if defined(DEBUG) && defined(TEST_BAD_INHERITANCE1)
  TESTMETHOD_REQUIRES_SEH(UnknownObject_BadInheritance1) {
    TestAssert::ExpectVEC([&]() noexcept {
      // This code must not compile, but if we remove the static assert from ObjectRefCount.h then we must have VEC
      // here. You will see a memory leak here because we cannot destroy object correctly.
      Mso::Make<BadUnknownObject1>();
    });
  }
#endif

#if defined(DEBUG) && defined(TEST_BAD_INHERITANCE2)
  TESTMETHOD_REQUIRES_SEH(UnknownObject_BadInheritance2) {
    TestAssert::ExpectVEC([&]() noexcept {
      // You will see a memory leak here because we cannot destroy object correctly.
      Mso::Make<BadUnknownObject2>();
    });
  }
#endif

  TEST_METHOD(UnknownObject_MyMemHeap_SimpleRefCount) {
    bool deleted = false;
    bool allocCalled = false;
    bool freeCalled = false;
    {
      MyMemHeap memHeap(/*ref*/ allocCalled, /*ref*/ freeCalled);
      Mso::CntPtr<IBaseSample1> unknown1 =
          Mso::MakeAlloc<MyMemHeapUnknownSample1, IBaseSample1>(&memHeap, /*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1->GetValue1());
    }
    TestAssert::IsTrue(deleted);
    TestAssert::IsTrue(allocCalled);
    TestAssert::IsTrue(freeCalled);
  }

  TEST_METHOD(UnknownObject_MyMemHeap_WeakRefCount) {
    bool deleted = false;
    bool allocCalled = false;
    bool freeCalled = false;
    {
      MyMemHeap memHeap(/*ref*/ allocCalled, /*ref*/ freeCalled);
      Mso::CntPtr<IBaseSample1> unknown1 =
          Mso::MakeAlloc<MyMemHeapUnknownSample2, IBaseSample1>(&memHeap, /*ref*/ deleted);
      TestAssert::AreEqual(1, unknown1->GetValue1());
    }
    TestAssert::IsTrue(deleted);
    TestAssert::IsTrue(allocCalled);
    TestAssert::IsTrue(freeCalled);
  }
};
