// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "object/refCountedObject.h"
#include <future>
#include "eventWaitHandle/eventWaitHandle.h"
#include "motifCpp/testCheck.h"

//#define TEST_BAD_INHERITANCE1 // Uncomment to see compilation error
//#define TEST_BAD_INHERITANCE2 // Uncomment to confirm VEC, but observe a memory leak. We cannot safely destroy this
// class.

struct DECLSPEC_NOVTABLE IRefBaseSample1 : public Mso::IRefCounted {
  virtual int GetValue1() = 0;
};

struct DECLSPEC_NOVTABLE IRefBaseSample2 : public Mso::IRefCounted {
  virtual int GetValue2() = 0;
};

// Simple ref counted object that implements one IRefCounted based interface
class RefCountSample1 final : public Mso::RefCountedObject<IRefBaseSample1> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() noexcept override {
    return 1;
  }

 protected:
  virtual ~RefCountSample1() noexcept {
    m_deleted = true;
  }

 private:
  RefCountSample1(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Simple ref counted object that implements two IRefCounted based interfaces
class RefCountSample2 final : public Mso::RefCountedObject<IRefBaseSample1, IRefBaseSample2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() noexcept override {
    return 1;
  }

  virtual int GetValue2() noexcept override {
    return 2;
  }

 protected:
  virtual ~RefCountSample2() noexcept {
    m_deleted = true;
  }

 private:
  RefCountSample2(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Ref counted object that supports weak reference and implements one IRefCounted based interface.
class RefCountSample3 final : public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, IRefBaseSample1> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() noexcept override {
    return 1;
  }

 protected:
  virtual ~RefCountSample3() noexcept {
    m_deleted = true;
  }

 private:
  RefCountSample3(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Ref counted object that supports weak reference and implements two IRefCounted based interfaces.
class RefCountSample4 final
    : public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, IRefBaseSample1, IRefBaseSample2> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() noexcept override {
    return 1;
  }

  virtual int GetValue2() noexcept override {
    return 2;
  }

 protected:
  virtual ~RefCountSample4() noexcept {
    m_deleted = true;
  }

 private:
  RefCountSample4(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Object that does not have ref count and implements one IRefCounted based interface.
class RefCountSample5 final : public Mso::RefCountedObject<Mso::RefCountStrategy::NoRefCount, IRefBaseSample1> {
 public:
  RefCountSample5(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() noexcept override {
    return 1;
  }

  virtual ~RefCountSample5() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

// Object that does not have ref count and implements two IRefCounted based interfaces.
class RefCountSample6 final
    : public Mso::RefCountedObject<Mso::RefCountStrategy::NoRefCount, IRefBaseSample1, IRefBaseSample2> {
 public:
  RefCountSample6(bool &deleted) noexcept : m_deleted(deleted) {}

  virtual int GetValue1() noexcept override {
    return 1;
  }

  virtual int GetValue2() noexcept override {
    return 2;
  }

  virtual ~RefCountSample6() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

// Simple ref counted object without V-table
class RefCountSample7 final : public Mso::RefCountedObjectNoVTable<RefCountSample7> {
  friend MakePolicy; // To allow constructor to be private or protected.
  friend RefCountPolicy; // Allow it to call our destructor

 public:
  int GetValue1() noexcept {
    OACR_USE_PTR(this);
    return 1;
  }

 protected:
  ~RefCountSample7() noexcept {
    m_deleted = true;
  }

 private:
  RefCountSample7(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

// Object that supports weak ref and has no V-table
class RefCountSample8 final : public Mso::RefCountedObjectNoVTable<Mso::RefCountStrategy::WeakRef, RefCountSample8> {
  friend MakePolicy; // To allow constructor to be private or protected.
  friend RefCountPolicy; // Allow it to call our destructor

 public:
  int GetValue1() noexcept {
    OACR_USE_PTR(this);
    return 1;
  }

 protected:
  ~RefCountSample8() noexcept {
    m_deleted = true;
  }

 private:
  RefCountSample8(bool &deleted) noexcept : m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

/// Base class for RefCountedObject that implements one interface and has non-default constructors.
class RefCountSample9Base : public IRefBaseSample1 {
 public:
  virtual int GetValue1() noexcept override {
    return m_param0 + m_param1 + m_param2;
  }

 protected:
  RefCountSample9Base() = default;
  RefCountSample9Base(int param0) noexcept : m_param0(param0) {}
  RefCountSample9Base(int param0, int param1) noexcept : m_param0(param0), m_param1(param1) {}
  RefCountSample9Base(int param0, int param1, int param2) noexcept
      : m_param0(param0), m_param1(param1), m_param2(param2) {}

 private:
  int m_param0 = 0;
  int m_param1 = 0;
  int m_param2 = 0;
};

/// Simple ref counted object inherited from RefCountSample9Base with non-default constructors.
class RefCountSample91 final : public Mso::RefCountedObject<RefCountSample9Base, IRefBaseSample2> {
  using Super = RefCountedObjectType;
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue2() noexcept override {
    return 1;
  }

 protected:
  virtual ~RefCountSample91() noexcept {
    m_deleted = true;
  }

 private:
  RefCountSample91(bool &deleted) noexcept : m_deleted(deleted) {}

  RefCountSample91(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  RefCountSample91(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  RefCountSample91(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

/// Object with a weak reference inherited from RefCountSample9Base with non-default constructors.
class RefCountSample92 final
    : public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, RefCountSample9Base, IRefBaseSample2> {
  using Super = RefCountedObjectType;
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue2() noexcept override {
    return 1;
  }

 protected:
  virtual ~RefCountSample92() noexcept {
    m_deleted = true;
  }

 private:
  RefCountSample92(bool &deleted) noexcept : m_deleted(deleted) {}

  RefCountSample92(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  RefCountSample92(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  RefCountSample92(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

 private:
  bool &m_deleted;
};

/// Object without reference counting and inherited from RefCountSample9Base with non-default constructors.
class RefCountSample93 final
    : public Mso::RefCountedObject<Mso::RefCountStrategy::NoRefCount, RefCountSample9Base, IRefBaseSample2> {
  using Super = RefCountedObjectType;

 public:
  RefCountSample93(bool &deleted) noexcept : m_deleted(deleted) {}

  RefCountSample93(bool &deleted, int param0) noexcept : Super(param0), m_deleted(deleted) {}

  RefCountSample93(bool &deleted, int param0, int param1) noexcept : Super(param0, param1), m_deleted(deleted) {}

  RefCountSample93(bool &deleted, int param0, int param1, int param2) noexcept
      : Super(param0, param1, param2), m_deleted(deleted) {}

  virtual int GetValue2() noexcept override {
    return 1;
  }

  virtual ~RefCountSample93() noexcept {
    m_deleted = true;
  }

 private:
  bool &m_deleted;
};

struct AsyncDeleter {
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
      VerifyElseCrashTag(false, 0x01003707 /* tag_bad2h */);
    }
  }
};

MSO_STRUCT_GUID(ITestAsyncDestroy, "18cd4582-2a30-4cdb-94c7-cf8d310101b8")
struct ITestAsyncDestroy {
  virtual void SetAsyncDestroy() noexcept = 0;
};

// To test custom DestroyThis method
class RefCountSample101 final
    : public Mso::RefCountedObject<Mso::SimpleRefCountPolicy<AsyncDeleter>, IRefBaseSample1, ITestAsyncDestroy> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() noexcept override {
    return 1;
  }

  virtual void SetAsyncDestroy() noexcept override {
    m_isAsyncDestroy = true;
  }

 protected:
  virtual ~RefCountSample101() noexcept {
    m_deleted.Set();
  }

 private:
  RefCountSample101(Mso::ManualResetEvent const &deleted, bool &isAsyncDestroy) noexcept
      : m_deleted{deleted}, m_isAsyncDestroy{isAsyncDestroy} {}

 private:
  Mso::ManualResetEvent m_deleted;
  bool &m_isAsyncDestroy;
};

// To test custom DestroyThis method
class RefCountSample102 final
    : public Mso::RefCountedObject<Mso::WeakRefCountPolicy<AsyncDeleter>, IRefBaseSample1, ITestAsyncDestroy> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() noexcept override {
    return 1;
  }

  virtual void SetAsyncDestroy() noexcept override {
    m_isAsyncDestroy = true;
  }

 protected:
  virtual ~RefCountSample102() noexcept {
    m_deleted.Set();
  }

 private:
  RefCountSample102(Mso::ManualResetEvent const &deleted, bool &isAsyncDestroy) noexcept
      : m_deleted(deleted), m_isAsyncDestroy(isAsyncDestroy) {}

 private:
  Mso::ManualResetEvent m_deleted;
  bool &m_isAsyncDestroy;
};

// To test custom DestroyThis method
class RefCountSample103 final
    : public Mso::RefCountedObjectNoVTable<Mso::SimpleRefCountPolicy<AsyncDeleter>, RefCountSample103> {
  friend MakePolicy; // To allow constructor to be private or protected.
  friend RefCountPolicy; // Allow it to call our destructor.

 public:
  int GetValue1() noexcept {
    OACR_USE_PTR(this);
    return 1;
  }

  void SetAsyncDestroy() noexcept {
    m_isAsyncDestroy = true;
  }

 protected:
  ~RefCountSample103() noexcept {
    m_deleted.Set();
  }

 private:
  RefCountSample103(Mso::ManualResetEvent const &deleted, bool &isAsyncDestroy) noexcept
      : m_deleted(deleted), m_isAsyncDestroy(isAsyncDestroy) {}

 private:
  Mso::ManualResetEvent m_deleted;
  bool &m_isAsyncDestroy;
};

// To test custom DestroyThis method
class RefCountSample104 final
    : public Mso::RefCountedObjectNoVTable<Mso::WeakRefCountPolicy<AsyncDeleter>, RefCountSample104> {
  friend MakePolicy; // To allow constructor to be private or protected.
  friend RefCountPolicy; // Allow it to call our destructor.

 public:
  int GetValue1() noexcept {
    OACR_USE_PTR(this);
    return 1;
  }

  void SetAsyncDestroy() noexcept {
    m_isAsyncDestroy = true;
  }

 protected:
  ~RefCountSample104() noexcept {
    m_deleted.Set();
  }

 private:
  RefCountSample104(Mso::ManualResetEvent const &deleted, bool &isAsyncDestroy) noexcept
      : m_deleted(deleted), m_isAsyncDestroy(isAsyncDestroy) {}

 private:
  Mso::ManualResetEvent m_deleted;
  bool &m_isAsyncDestroy;
};

class SomeVirtualClass {
 public:
  virtual ~SomeVirtualClass() = default;

  int x;
  int y;
};

#ifdef TEST_BAD_INHERITANCE1
// !!! Mso::RefCountedObject must be always the first one in the inheritance !!!
class BadRefCountedObject1 final : public SomeVirtualClass, public Mso::RefCountedObject<IRefBaseSample1> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  DECLARE_COPYCONSTR_AND_ASSIGNMENT(BadRefCountedObject1);

  virtual int GetValue1() override {
    return 1;
  }

 private:
  BadRefCountedObject1() = default;
};
#endif

#ifdef TEST_BAD_INHERITANCE2
// !!! Mso::RefCountedObject must be always the first one in the inheritance !!!
class BadRefCountedObject2 final : public SomeVirtualClass,
                                   public Mso::RefCountedObject<Mso::RefCountStrategy::WeakRef, IRefBaseSample1> {
  friend MakePolicy; // To allow constructor to be private or protected.

 public:
  virtual int GetValue1() override {
    return 1;
  }

 private:
  BadRefCountedObject2() = default;
};
#endif

TEST_CLASS (RefCountedObjectTest) {
  TEST_METHOD(RefCountedObject_SimpleRefCount) {
    bool deleted = false;
    {
      Mso::CntPtr<RefCountSample1> refCounted = Mso::Make<RefCountSample1>(/*ref*/ deleted);
      TestAssert::AreEqual(1, refCounted->GetValue1());
      Debug(TestAssert::AreEqual(1u, refCounted->RefCount()));

      Mso::CntPtr<IRefBaseSample1> base1 = refCounted;
      TestAssert::AreEqual(1, base1->GetValue1());
      Debug(TestAssert::AreEqual(2u, refCounted->RefCount()));
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(RefCountedObject_SimpleRefCount2) {
    bool deleted = false;
    {
      Mso::CntPtr<RefCountSample2> refCounted = Mso::Make<RefCountSample2>(/*ref*/ deleted);
      TestAssert::AreEqual(1, refCounted->GetValue1());
      Debug(TestAssert::AreEqual(1u, refCounted->RefCount()));

      Mso::CntPtr<IRefBaseSample1> base1 = refCounted;
      TestAssert::AreEqual(1, base1->GetValue1());
      Debug(TestAssert::AreEqual(2u, refCounted->RefCount()));

      Mso::CntPtr<IRefBaseSample2> base2 = refCounted;
      TestAssert::AreEqual(2, base2->GetValue2());
      Debug(TestAssert::AreEqual(3u, refCounted->RefCount()));
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(RefCountedObject_WeakRefCount) {
    bool deleted = false;
    {
      Mso::WeakPtr<RefCountSample3> weakPtr1;
      Mso::WeakPtr<IRefBaseSample1> weakPtrBase1;
      {
        Mso::CntPtr<RefCountSample3> refCounted = Mso::Make<RefCountSample3>(/*ref*/ deleted);
        Mso::CntPtr<IRefBaseSample1> base1 = refCounted;
        TestAssert::AreEqual(1, base1->GetValue1());

        weakPtr1 = refCounted;
        Mso::CntPtr<RefCountSample3> refCounted1 = weakPtr1.GetStrongPtr();
        TestAssert::IsNotNull(refCounted1.Get());
        TestAssert::IsFalse(weakPtr1.IsExpired());

        weakPtrBase1 = refCounted;
        Mso::CntPtr<IRefBaseSample1> base11 = weakPtrBase1.GetStrongPtr();
        TestAssert::IsNotNull(base11.Get());
        TestAssert::IsFalse(weakPtrBase1.IsExpired());
      }

      TestAssert::IsTrue(deleted);

      Mso::CntPtr<RefCountSample3> refCounted11 = weakPtr1.GetStrongPtr();
      TestAssert::IsNull(refCounted11.Get());
      TestAssert::IsTrue(weakPtr1.IsExpired());

      Mso::CntPtr<IRefBaseSample1> base111 = weakPtrBase1.GetStrongPtr();
      TestAssert::IsNull(base111.Get());
      TestAssert::IsTrue(weakPtrBase1.IsExpired());
    }
  }

  TEST_METHOD(RefCountedObject_WeakRefCount2) {
    bool deleted = false;
    {
      Mso::WeakPtr<RefCountSample4> weakPtr1;
      Mso::WeakPtr<IRefBaseSample1> weakPtrBase1;
      Mso::WeakPtr<IRefBaseSample2> weakPtrBase2;
      {
        Mso::CntPtr<RefCountSample4> refCounted = Mso::Make<RefCountSample4>(/*ref*/ deleted);
        Mso::CntPtr<IRefBaseSample1> base1 = refCounted;
        TestAssert::AreEqual(1, base1->GetValue1());
        Mso::CntPtr<IRefBaseSample2> base2 = refCounted;
        TestAssert::AreEqual(2, base2->GetValue2());

        weakPtr1 = refCounted;
        Mso::CntPtr<RefCountSample4> refCounted1 = weakPtr1.GetStrongPtr();
        TestAssert::IsNotNull(refCounted1.Get());
        TestAssert::IsFalse(weakPtr1.IsExpired());

        weakPtrBase1 = refCounted;
        Mso::CntPtr<IRefBaseSample1> base11 = weakPtrBase1.GetStrongPtr();
        TestAssert::IsNotNull(base11.Get());
        TestAssert::IsFalse(weakPtrBase1.IsExpired());

        weakPtrBase2 = refCounted;
        Mso::CntPtr<IRefBaseSample2> base21 = weakPtrBase2.GetStrongPtr();
        TestAssert::IsNotNull(base21.Get());
        TestAssert::IsFalse(weakPtrBase2.IsExpired());
      }

      TestAssert::IsTrue(deleted);

      Mso::CntPtr<RefCountSample4> refCounted11 = weakPtr1.GetStrongPtr();
      TestAssert::IsNull(refCounted11.Get());
      TestAssert::IsTrue(weakPtr1.IsExpired());

      Mso::CntPtr<IRefBaseSample1> base111 = weakPtrBase1.GetStrongPtr();
      TestAssert::IsNull(base111.Get());
      TestAssert::IsTrue(weakPtrBase1.IsExpired());

      Mso::CntPtr<IRefBaseSample2> base211 = weakPtrBase2.GetStrongPtr();
      TestAssert::IsNull(base211.Get());
      TestAssert::IsTrue(weakPtrBase2.IsExpired());
    }
  }

  TEST_METHOD(RefCountedObject_NoRefCount) {
    bool deleted = false;
    {
      RefCountSample5 obj(/*ref*/ deleted); // Stack allocation
      Mso::CntPtr<RefCountSample5> refCounted(&obj);
      TestAssert::AreEqual(1, refCounted->GetValue1());

      Mso::CntPtr<IRefBaseSample1> base1 = refCounted;
      TestAssert::AreEqual(1, base1->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(RefCountedObject_NoRefCount2) {
    bool deleted = false;
    {
      RefCountSample6 obj(/*ref*/ deleted); // Stack allocation
      Mso::CntPtr<RefCountSample6> refCounted(&obj);
      TestAssert::AreEqual(1, refCounted->GetValue1());

      Mso::CntPtr<IRefBaseSample1> base1 = refCounted;
      TestAssert::AreEqual(1, base1->GetValue1());

      Mso::CntPtr<IRefBaseSample2> base2 = refCounted;
      TestAssert::AreEqual(2, base2->GetValue2());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(RefCountedObject_DestroyThis) {
    Mso::ManualResetEvent deleted;
    bool isAsyncDestroy = false;

    {
      Mso::CntPtr<RefCountSample101> obj1 = Mso::Make<RefCountSample101>(/*ref*/ deleted, /*ref*/ isAsyncDestroy);
      Mso::CntPtr<IRefBaseSample1> base1{obj1};
      TestAssert::AreEqual(1, base1->GetValue1());
    }

    // Wait for deletion to complete
    TestAssert::IsTrue(deleted.Wait());
    TestAssert::IsTrue(isAsyncDestroy);
  }

  TEST_METHOD(RefCountedObject_WeakRef_DestroyThis) {
    Mso::ManualResetEvent deleted;
    bool isAsyncDestroy = false;

    {
      Mso::CntPtr<RefCountSample102> obj1{Mso::Make<RefCountSample102>(/*ref*/ deleted, /*ref*/ isAsyncDestroy)};
      Mso::CntPtr<IRefBaseSample1> base1{obj1};
      TestAssert::AreEqual(1, base1->GetValue1());
    }

    // Wait for deletion to complete
    TestAssert::IsTrue(deleted.Wait());
    TestAssert::IsTrue(isAsyncDestroy);
  }

  TEST_METHOD(RefCountedObject_NoVTable_DestroyThis) {
    Mso::ManualResetEvent deleted;
    bool isAsyncDestroy = false;

    {
      Mso::CntPtr<RefCountSample103> obj1 = Mso::Make<RefCountSample103>(/*ref*/ deleted, /*ref*/ isAsyncDestroy);
      TestAssert::AreEqual(1, obj1->GetValue1());
    }

    // Wait for deletion to complete
    TestAssert::IsTrue(deleted.Wait());
    TestAssert::IsTrue(isAsyncDestroy);
  }

  TEST_METHOD(RefCountedObject_NoVTable_WeakRef_DestroyThis) {
    Mso::ManualResetEvent deleted;
    bool isAsyncDestroy = false;

    {
      Mso::CntPtr<RefCountSample104> obj1 = Mso::Make<RefCountSample104>(/*ref*/ deleted, /*ref*/ isAsyncDestroy);
      TestAssert::AreEqual(1, obj1->GetValue1());
    }

    // Wait for deletion to complete
    TestAssert::IsTrue(deleted.Wait());
    TestAssert::IsTrue(isAsyncDestroy);
  }

  TEST_METHOD(RefCountedObject_NoVTable_SimpleRefCount) {
    bool deleted = false;
    {
      Mso::CntPtr<RefCountSample7> refCounted1 = Mso::Make<RefCountSample7>(/*ref*/ deleted);
      TestAssert::AreEqual(1, refCounted1->GetValue1());
      Debug(TestAssert::AreEqual(1u, refCounted1->RefCount()));
      Mso::CntPtr<RefCountSample7> refCounted2(refCounted1);
      UNREFERENCED_OACR(refCounted2);
      Debug(TestAssert::AreEqual(2u, refCounted1->RefCount()));
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(RefCountedObject_NoVTable_WeakRefCount) {
    bool deleted = false;
    {
      Mso::WeakPtr<RefCountSample8> weakPtr1;
      {
        Mso::CntPtr<RefCountSample8> refCounted = Mso::Make<RefCountSample8>(/*ref*/ deleted);
        TestAssert::AreEqual(1, refCounted->GetValue1());

        weakPtr1 = refCounted;
        Mso::CntPtr<RefCountSample8> refCounted1 = weakPtr1.GetStrongPtr();
        TestAssert::IsNotNull(refCounted1.Get());
        TestAssert::IsFalse(weakPtr1.IsExpired());
      }

      TestAssert::IsTrue(deleted);

      Mso::CntPtr<RefCountSample8> refCounted11 = weakPtr1.GetStrongPtr();
      TestAssert::IsNull(refCounted11.Get());
      TestAssert::IsTrue(weakPtr1.IsExpired());
    }
  }

  TEST_METHOD(RefCountedObject_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      Mso::CntPtr<RefCountSample91> refCounted1{Mso::Make<RefCountSample91>(/*ref*/ deleted)};
      Mso::CntPtr<IRefBaseSample1> base1{refCounted1};
      TestAssert::AreEqual(0, base1->GetValue1());

      Mso::CntPtr<RefCountSample91> refCounted2{Mso::Make<RefCountSample91>(/*ref*/ deleted, 3)};
      Mso::CntPtr<IRefBaseSample1> base2{refCounted2};
      TestAssert::AreEqual(3, base2->GetValue1());

      Mso::CntPtr<RefCountSample91> refCounted3{Mso::Make<RefCountSample91>(/*ref*/ deleted, 3, 5)};
      Mso::CntPtr<IRefBaseSample1> base3{refCounted3};
      TestAssert::AreEqual(8, base3->GetValue1());

      Mso::CntPtr<RefCountSample91> refCounted4{Mso::Make<RefCountSample91>(/*ref*/ deleted, 3, 5, 11)};
      Mso::CntPtr<IRefBaseSample1> base4{refCounted4};
      TestAssert::AreEqual(19, base4->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(RefCountedObject_WeakRef_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      Mso::CntPtr<RefCountSample92> refCounted1{Mso::Make<RefCountSample92>(/*ref*/ deleted)};
      Mso::CntPtr<IRefBaseSample1> base1{refCounted1};
      TestAssert::AreEqual(0, base1->GetValue1());

      Mso::CntPtr<RefCountSample92> refCounted2{Mso::Make<RefCountSample92>(/*ref*/ deleted, 3)};
      Mso::CntPtr<IRefBaseSample1> base2{refCounted2};
      TestAssert::AreEqual(3, base2->GetValue1());

      Mso::CntPtr<RefCountSample92> refCounted3{Mso::Make<RefCountSample92>(/*ref*/ deleted, 3, 5)};
      Mso::CntPtr<IRefBaseSample1> base3{refCounted3};
      TestAssert::AreEqual(8, base3->GetValue1());

      Mso::CntPtr<RefCountSample92> refCounted4{Mso::Make<RefCountSample92>(/*ref*/ deleted, 3, 5, 11)};
      Mso::CntPtr<IRefBaseSample1> base4{refCounted4};
      TestAssert::AreEqual(19, base4->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(RefCountedObject_NoRefCount_NonDefaultBaseConstructor) {
    bool deleted = false;
    {
      RefCountSample93 refCounted1{/*ref*/ deleted};
      Mso::CntPtr<IRefBaseSample1> base1{&refCounted1};
      TestAssert::AreEqual(0, base1->GetValue1());

      RefCountSample93 refCounted2{/*ref*/ deleted, 3};
      Mso::CntPtr<IRefBaseSample1> base2{&refCounted2};
      TestAssert::AreEqual(3, base2->GetValue1());

      RefCountSample93 refCounted3(/*ref*/ deleted, 3, 5);
      Mso::CntPtr<IRefBaseSample1> base3{&refCounted3};
      TestAssert::AreEqual(8, base3->GetValue1());

      RefCountSample93 refCounted4(/*ref*/ deleted, 3, 5, 11);
      Mso::CntPtr<IRefBaseSample1> base4{&refCounted4};
      TestAssert::AreEqual(19, base4->GetValue1());
    }
    TestAssert::IsTrue(deleted);
  }

#if defined(DEBUG) && defined(TEST_BAD_INHERITANCE1)
  TESTMETHOD_REQUIRES_SEH(RefCountedObject_BadInheritance1) {
    TestAssert::ExpectVEC([&]() noexcept {
      // This code must not compile, but if we remove the static assert from ObjectRefCount.h then we must have VEC
      // here. You will see a memory leak here because we cannot destroy object correctly.
      Mso::Make<BadRefCountedObject1>();
    });
  }
#endif

#if defined(DEBUG) && defined(TEST_BAD_INHERITANCE2)
  TESTMETHOD_REQUIRES_SEH(RefCountedObject_BadInheritance2) {
    TestAssert::ExpectVEC([&]() noexcept {
      // You will see a memory leak here because we cannot destroy object correctly.
      Mso::Make<BadRefCountedObject2>();
    });
  }
#endif
};
