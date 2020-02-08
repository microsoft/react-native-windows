// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "motifCpp/testCheck.h"
#include "object/refCountedObject.h"
#include "testAllocators.h"

namespace Mso::UnitTests {

class ObjectWithWeakRefSample1 final
    : public Mso::RefCountedObjectNoVTable<Mso::RefCountStrategy::WeakRef, ObjectWithWeakRefSample1> {
 public:
  int Value() const noexcept {
    return m_value;
  }

 private:
  ObjectWithWeakRefSample1(int value, bool &deleted) noexcept : m_value(value), m_deleted(deleted) {}

  ~ObjectWithWeakRefSample1() noexcept {
    m_deleted = true;
  }

  friend MakePolicy; // To support private constructor
  friend RefCountPolicy; // To support private destructor

 private:
  int m_value;
  bool &m_deleted;
};

class ObjectWithWeakRefSample2Init final
    : public Mso::RefCountedObjectNoVTable<Mso::RefCountStrategy::WeakRef, ObjectWithWeakRefSample2Init> {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtorAndInitializeThis;

  int Value() const noexcept {
    return m_value;
  }

 private:
  void InitializeThis(int value, bool &deleted) noexcept {
    m_value = value;
    m_deleted = &deleted;
  }

  ObjectWithWeakRefSample2Init() noexcept = default;

  ~ObjectWithWeakRefSample2Init() noexcept {
    *m_deleted = true;
  }

  friend MakePolicy; // To support private constructor & InitializeThis
  friend RefCountPolicy; // To support private destructor

 private:
  int m_value;
  bool *m_deleted;
};

class ObjectWithWeakRefSample3CannotAllocate final
    : public Mso::RefCountedObjectNoVTable<BadAllocWeakRefCount, ObjectWithWeakRefSample3CannotAllocate> {};

class ObjectWithWeakRefSample4Throw final
    : public Mso::RefCountedObjectNoVTable<Mso::RefCountStrategy::WeakRef, ObjectWithWeakRefSample4Throw> {
 public:
  using MakePolicy = Mso::MakePolicy::ThrowCtor;

 private:
  ObjectWithWeakRefSample4Throw(bool &deleted) : m_deleted(deleted) {
    throw std::runtime_error("Test");
  }

  ~ObjectWithWeakRefSample4Throw() noexcept {
    m_deleted = true;
  }

  friend MakePolicy; // To support private constructor
  friend RefCountPolicy; // To support private destructor

 private:
  bool &m_deleted;
};

class ObjectWithWeakRefSample5InitThrow final
    : public Mso::RefCountedObjectNoVTable<Mso::RefCountStrategy::WeakRef, ObjectWithWeakRefSample5InitThrow> {
 public:
  using MakePolicy = Mso::MakePolicy::ThrowCtorAndInitializeThis;

 private:
  void InitializeThis(bool &deleted) {
    m_deleted = &deleted;
    throw std::runtime_error("Test");
  }

  ObjectWithWeakRefSample5InitThrow() noexcept = default;

  ~ObjectWithWeakRefSample5InitThrow() noexcept {
    *m_deleted = true;
  }

  friend MakePolicy; // To support private constructor & InitializeThis
  friend RefCountPolicy; // To support private destructor

 private:
  bool *m_deleted{nullptr};
};

//=============================================================================
// Classes below use stateful allocator
//=============================================================================

class ObjectWithWeakRefSample11 final
    : public Mso::RefCountedObjectNoVTable<StatefulAllocWeakRefCount, ObjectWithWeakRefSample11> {
 public:
  int Value() const noexcept {
    return m_value;
  }

 private:
  ObjectWithWeakRefSample11(int value, bool &deleted) noexcept : m_value(value), m_deleted(deleted) {}

  ~ObjectWithWeakRefSample11() noexcept {
    m_deleted = true;
  }

  friend MakePolicy; // To support private constructor
  friend RefCountPolicy; // To support private destructor

 private:
  int m_value;
  bool &m_deleted;
};

class ObjectWithWeakRefSample21Init final
    : public Mso::RefCountedObjectNoVTable<StatefulAllocWeakRefCount, ObjectWithWeakRefSample21Init> {
 public:
  using MakePolicy = Mso::MakePolicy::NoThrowCtorAndInitializeThis;

  int Value() const noexcept {
    return m_value;
  }

 private:
  void InitializeThis(int value, bool &deleted) noexcept {
    m_value = value;
    m_deleted = &deleted;
  }

  ObjectWithWeakRefSample21Init() noexcept = default;

  ~ObjectWithWeakRefSample21Init() noexcept {
    *m_deleted = true;
  }

  friend MakePolicy; // To support private constructor & InitializeThis
  friend RefCountPolicy; // To support private destructor

 private:
  int m_value;
  bool *m_deleted;
};

class ObjectWithWeakRefSample31CannotAllocate final
    : public Mso::RefCountedObjectNoVTable<BadStatefulAllocWeakRefCount, ObjectWithWeakRefSample31CannotAllocate> {};

class ObjectWithWeakRefSample41Throw final
    : public Mso::RefCountedObjectNoVTable<StatefulAllocWeakRefCount, ObjectWithWeakRefSample41Throw> {
 public:
  using MakePolicy = Mso::MakePolicy::ThrowCtor;

 private:
  ObjectWithWeakRefSample41Throw(bool &deleted) : m_deleted(deleted) {
    throw std::runtime_error("Test");
  }

  ~ObjectWithWeakRefSample41Throw() noexcept {
    m_deleted = true;
  }

  friend MakePolicy; // To support private constructor
  friend RefCountPolicy; // To support private destructor

 private:
  bool &m_deleted;
};

class ObjectWithWeakRefSample51InitThrow final
    : public Mso::RefCountedObjectNoVTable<StatefulAllocWeakRefCount, ObjectWithWeakRefSample51InitThrow> {
 public:
  using MakePolicy = Mso::MakePolicy::ThrowCtorAndInitializeThis;

 private:
  void InitializeThis(bool &deleted) {
    m_deleted = &deleted;
    throw std::runtime_error("Test");
  }

  ObjectWithWeakRefSample51InitThrow() noexcept = default;

  ~ObjectWithWeakRefSample51InitThrow() noexcept {
    *m_deleted = true;
  }

  friend MakePolicy; // To support private constructor & InitializeThis
  friend RefCountPolicy; // To support private destructor

 private:
  bool *m_deleted{nullptr};
};

TEST_CLASS (ObjectWithWeakRefTest) {
  TEST_METHOD(ObjectWithWeakRef_Make) {
    bool deleted = false;
    {
      Mso::CntPtr<ObjectWithWeakRefSample1> obj1 = Mso::Make<ObjectWithWeakRefSample1>(5, /*ref*/ deleted);
      {
        Debug(TestAssert::AreEqual(1u, obj1->GetWeakRef().RefCount()));
        TestAssert::AreEqual(5, obj1->Value());

        Mso::CntPtr<ObjectWithWeakRefSample1> obj2(obj1);
        Debug(TestAssert::AreEqual(2u, obj1->GetWeakRef().RefCount()));
        TestAssert::IsNotNull(obj2.Get());
        TestAssert::IsTrue(obj1.Get() == obj2.Get());
      }

      Debug(TestAssert::AreEqual(1u, obj1->GetWeakRef().RefCount()));
    }

    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(ObjectWithWeakRef_WeakRef) {
    // Ref count for ObjectWeakRef behaves the same way as for the object.
    bool deleted = false;
    {
      Mso::CntPtr<ObjectWithWeakRefSample1> obj1 = Mso::Make<ObjectWithWeakRefSample1>(5, /*ref*/ deleted);
      Mso::CntPtr<Mso::ObjectWeakRef> weakRef{&obj1->GetWeakRef()};
      {
        Debug(TestAssert::AreEqual(2u, weakRef->RefCount()));
        Debug(TestAssert::AreEqual(1u, weakRef->WeakRefCount()));

        Mso::CntPtr<ObjectWithWeakRefSample1> obj2(obj1);
        Debug(TestAssert::AreEqual(3u, weakRef->RefCount()));
      }

      Debug(TestAssert::AreEqual(2u, weakRef->RefCount()));
      TestAssert::IsFalse(deleted);
      TestAssert::IsFalse(weakRef->IsExpired());
    }

    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(ObjectWithWeakRef_WeakPtr) {
    bool deleted = false;
    Mso::WeakPtr<ObjectWithWeakRefSample1> weakPtr;
    {
      Mso::CntPtr<ObjectWithWeakRefSample1> obj1 = Mso::Make<ObjectWithWeakRefSample1>(5, /*ref*/ deleted);
      Mso::CntPtr<Mso::ObjectWeakRef> weakRef{&obj1->GetWeakRef()};
      weakPtr = obj1;
      {
        Debug(TestAssert::AreEqual(2u, weakRef->RefCount()));
        Debug(TestAssert::AreEqual(2u, weakRef->WeakRefCount()));

        Mso::CntPtr<ObjectWithWeakRefSample1> obj2(obj1);
        Debug(TestAssert::AreEqual(3u, weakRef->RefCount()));
      }

      Debug(TestAssert::AreEqual(2u, weakRef->RefCount()));
      TestAssert::IsFalse(deleted);
      TestAssert::IsFalse(weakPtr.IsExpired());
    }

    TestAssert::IsTrue(deleted);
    TestAssert::IsTrue(weakPtr.IsExpired());
  }

  TEST_METHOD(ObjectWithWeakRef_Make_InitializeThis) {
    bool deleted = false;
    {
      auto obj = Mso::Make<ObjectWithWeakRefSample2Init>(5, /*ref*/ deleted);
      Debug(TestAssert::AreEqual(1u, obj->GetWeakRef().RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    TestAssert::IsTrue(deleted);
  }

  TESTMETHOD_REQUIRES_SEH(ObjectWithWeakRef_Make_CannotAllocate) {
    Mso::CntPtr<ObjectWithWeakRefSample3CannotAllocate> obj;
    TestAssert::ExpectVEC([&]() noexcept { obj = Mso::Make<ObjectWithWeakRefSample3CannotAllocate>(); });

    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectWithWeakRef_Make_CtorThrows) {
    Mso::CntPtr<ObjectWithWeakRefSample4Throw> obj;
    bool deleted = false;
    TestAssert::ExpectException<std::runtime_error>(
        [&]() { obj = Mso::Make<ObjectWithWeakRefSample4Throw>(/*ref*/ deleted); });

    TestAssert::IsFalse(deleted); // Destructor is not called if constructor throws.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectWithWeakRef_Make_InitializeThisThrows) {
    Mso::CntPtr<ObjectWithWeakRefSample5InitThrow> obj;
    bool deleted = false;
    TestAssert::ExpectException<std::runtime_error>(
        [&]() { obj = Mso::Make<ObjectWithWeakRefSample5InitThrow>(/*ref*/ deleted); });

    TestAssert::IsTrue(deleted); // If InitializeThis throws then destructor must be called.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectWithWeakRef_MakeElseNull) {
    bool deleted = false;
    {
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      auto obj = Mso::MakeElseNull<ObjectWithWeakRefSample1>(5, /*ref*/ deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(ObjectWithWeakRef_MakeElseNull_InitializeThis) {
    bool deleted = false;
    {
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      auto obj = Mso::MakeElseNull<ObjectWithWeakRefSample2Init>(5, /*ref*/ deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(ObjectWithWeakRef_MakeElseNull_CannotAllocate) {
    OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
    auto obj = Mso::MakeElseNull<ObjectWithWeakRefSample3CannotAllocate>();
    TestAssert::IsNull(obj.Get());
  }

  TEST_METHOD(ObjectWithWeakRef_MakeElseNull_CtorThrows) {
    Mso::CntPtr<ObjectWithWeakRefSample4Throw> obj;
    bool deleted = false;
    TestAssert::ExpectException<std::runtime_error>([&]() {
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      obj = Mso::MakeElseNull<ObjectWithWeakRefSample4Throw>(/*ref*/ deleted);
    });
  }

  TEST_METHOD(ObjectWithWeakRef_MakeElseNull_InitializeThisThrows) {
    Mso::CntPtr<ObjectWithWeakRefSample5InitThrow> obj;
    bool deleted = false;
    TestAssert::ExpectException<std::runtime_error>([&]() {
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      obj = Mso::MakeElseNull<ObjectWithWeakRefSample5InitThrow>(/*ref*/ deleted);
    });

    TestAssert::IsTrue(deleted); // If InitializeThis throws then we must call destructor.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  struct AllocTestState {
    bool AllocCalled;
    bool FreeCalled;
    bool Deleted;
  };

  static void AssertAllocState(const AllocTestState &state, bool deleted = true) noexcept {
    TestAssert::IsTrue(state.AllocCalled, L"Allocate expected to be called");
    TestAssert::IsTrue(state.FreeCalled, L"Deallocate expected to be called");
    TestAssert::AreEqual(deleted, state.Deleted, L"Destructor to be called");
  }

  TEST_METHOD(ObjectWithWeakRef_MakeAlloc) {
    AllocTestState state = {};
    {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      auto obj = Mso::MakeAlloc<ObjectWithWeakRefSample11>(&memHeap, 5, /*ref*/ state.Deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    AssertAllocState(state);
  }

  TEST_METHOD(ObjectWithWeakRef_MakeAlloc_InitializeThis) {
    AllocTestState state = {};
    {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      auto obj = Mso::MakeAlloc<ObjectWithWeakRefSample21Init>(&memHeap, 5, /*ref*/ state.Deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    AssertAllocState(state);
  }

  TESTMETHOD_REQUIRES_SEH(ObjectWithWeakRef_MakeAlloc_CannotAllocate) {
    Mso::CntPtr<ObjectWithWeakRefSample31CannotAllocate> obj;
    TestAssert::ExpectVEC([&]() noexcept {
      AllocTestState state = {};
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      obj = Mso::MakeAlloc<ObjectWithWeakRefSample31CannotAllocate>(&memHeap);
    });

    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectWithWeakRef_MakeAlloc_CtorThrows) {
    Mso::CntPtr<ObjectWithWeakRefSample41Throw> obj;
    AllocTestState state = {};
    TestAssert::ExpectException<std::runtime_error>([&]() {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      obj = Mso::MakeAlloc<ObjectWithWeakRefSample41Throw>(&memHeap, /*ref*/ state.Deleted);
    });

    AssertAllocState(state, /*deleted:*/ false); // Destructor is not called if constructor throws.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectWithWeakRef_MakeAlloc_InitializeThisThrows) {
    Mso::CntPtr<ObjectWithWeakRefSample51InitThrow> obj;
    AllocTestState state = {};
    TestAssert::ExpectException<std::runtime_error>([&]() {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      obj = Mso::MakeAlloc<ObjectWithWeakRefSample51InitThrow>(&memHeap, /*ref*/ state.Deleted);
    });

    AssertAllocState(state); // If InitializeThis throws then destructor must be called.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectWithWeakRef_MakeAllocElseNull) {
    AllocTestState state = {};
    {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      auto obj = Mso::MakeAllocElseNull<ObjectWithWeakRefSample11>(&memHeap, 5, /*ref*/ state.Deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    AssertAllocState(state);
  }

  TEST_METHOD(ObjectWithWeakRef_MakeAllocElseNull_InitializeThis) {
    AllocTestState state = {};
    {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      auto obj = Mso::MakeAllocElseNull<ObjectWithWeakRefSample21Init>(&memHeap, 5, /*ref*/ state.Deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    AssertAllocState(state);
  }

  TEST_METHOD(ObjectWithWeakRef_MakeAllocElseNull_CannotAllocate) {
    AllocTestState state = {};
    MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
    OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
    auto obj = Mso::MakeAllocElseNull<ObjectWithWeakRefSample31CannotAllocate>(&memHeap);
    TestAssert::IsNull(obj.Get());
  }

  TEST_METHOD(ObjectWithWeakRef_MakeAllocElseNull_CtorThrows) {
    Mso::CntPtr<ObjectWithWeakRefSample41Throw> obj;
    AllocTestState state = {};
    TestAssert::ExpectException<std::runtime_error>([&]() {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      obj = Mso::MakeAllocElseNull<ObjectWithWeakRefSample41Throw>(&memHeap, /*ref*/ state.Deleted);
    });

    AssertAllocState(state, /*deleted:*/ false); // Destructor is not called if constructor throws.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectWithWeakRef_MakeAllocElseNull_InitializeThisThrows) {
    Mso::CntPtr<ObjectWithWeakRefSample51InitThrow> obj;
    AllocTestState state = {};
    TestAssert::ExpectException<std::runtime_error>([&]() {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      obj = Mso::MakeAllocElseNull<ObjectWithWeakRefSample51InitThrow>(&memHeap, /*ref*/ state.Deleted);
    });

    AssertAllocState(state); // If InitializeThis throws then we must call destructor.
    TestAssert::IsTrue(obj.IsEmpty());
  }
};

} // namespace Mso::UnitTests
