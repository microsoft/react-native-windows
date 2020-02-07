// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "object/refCountedObject.h"
#include "testAllocators.h"

class ObjectRefCountSample1 final : public Mso::RefCountedObjectNoVTable<ObjectRefCountSample1> {
 public:
  int Value() const noexcept {
    return m_value;
  }

 private:
  ObjectRefCountSample1(int value, bool &deleted) noexcept : m_value(value), m_deleted(deleted) {}

  ~ObjectRefCountSample1() noexcept {
    m_deleted = true;
  }

  friend MakePolicy; // To support private constructor
  friend RefCountPolicy; // To support private destructor

 private:
  int m_value;
  bool &m_deleted;
};

class ObjectRefCountSample2Init final : public Mso::RefCountedObjectNoVTable<ObjectRefCountSample2Init> {
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

  ObjectRefCountSample2Init() noexcept = default;

  ~ObjectRefCountSample2Init() noexcept {
    *m_deleted = true;
  }

  friend MakePolicy; // To support private constructor & InitializeThis
  friend RefCountPolicy; // To support private destructor

 private:
  int m_value;
  bool *m_deleted;
};

class ObjectRefCountSample3CannotAllocate final
    : public Mso::RefCountedObjectNoVTable<BadAllocSimpleRefCount, ObjectRefCountSample3CannotAllocate> {};

class ObjectRefCountSample4Throw final : public Mso::RefCountedObjectNoVTable<ObjectRefCountSample4Throw> {
 public:
  using MakePolicy = Mso::MakePolicy::ThrowCtor;

 private:
  ObjectRefCountSample4Throw(bool &deleted) : m_deleted(deleted) {
    throw std::runtime_error("Test");
  }

  ~ObjectRefCountSample4Throw() noexcept {
    m_deleted = true;
  }

  friend MakePolicy; // To support private constructor
  friend RefCountPolicy; // To support private destructor

 private:
  bool &m_deleted;
};

class ObjectRefCountSample5InitThrow final : public Mso::RefCountedObjectNoVTable<ObjectRefCountSample5InitThrow> {
 public:
  using MakePolicy = Mso::MakePolicy::ThrowCtorAndInitializeThis;

 private:
  void InitializeThis(bool &deleted) {
    m_deleted = &deleted;
    throw std::runtime_error("Test");
  }

  ObjectRefCountSample5InitThrow() noexcept = default;

  ~ObjectRefCountSample5InitThrow() noexcept {
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

class ObjectRefCountSample11 final
    : public Mso::RefCountedObjectNoVTable<StatefulAllocSimpleRefCount, ObjectRefCountSample11> {
 public:
  int Value() const noexcept {
    return m_value;
  }

 private:
  ObjectRefCountSample11(int value, bool &deleted) noexcept : m_value(value), m_deleted(deleted) {}

  ~ObjectRefCountSample11() noexcept {
    m_deleted = true;
  }

  friend MakePolicy; // To support private constructor
  friend RefCountPolicy; // To support private destructor

 private:
  int m_value;
  bool &m_deleted;
};

class ObjectRefCountSample21Init final
    : public Mso::RefCountedObjectNoVTable<StatefulAllocSimpleRefCount, ObjectRefCountSample21Init> {
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

  ObjectRefCountSample21Init() noexcept = default;

  ~ObjectRefCountSample21Init() noexcept {
    *m_deleted = true;
  }

  friend MakePolicy; // To support private constructor & InitializeThis
  friend RefCountPolicy; // To support private destructor

 private:
  int m_value;
  bool *m_deleted;
};

class ObjectRefCountSample31CannotAllocate final
    : public Mso::RefCountedObjectNoVTable<BadStatefulAllocSimpleRefCount, ObjectRefCountSample31CannotAllocate> {};

class ObjectRefCountSample41Throw final
    : public Mso::RefCountedObjectNoVTable<StatefulAllocSimpleRefCount, ObjectRefCountSample41Throw> {
 public:
  using MakePolicy = Mso::MakePolicy::ThrowCtor;

 private:
  ObjectRefCountSample41Throw(bool &deleted) : m_deleted(deleted) {
    throw std::runtime_error("Test");
  }

  ~ObjectRefCountSample41Throw() noexcept {
    m_deleted = true;
  }

  friend MakePolicy; // To support private constructor
  friend RefCountPolicy; // To support private destructor

 private:
  bool &m_deleted;
};

class ObjectRefCountSample51InitThrow final
    : public Mso::RefCountedObjectNoVTable<StatefulAllocSimpleRefCount, ObjectRefCountSample51InitThrow> {
 public:
  using MakePolicy = Mso::MakePolicy::ThrowCtorAndInitializeThis;

 private:
  void InitializeThis(bool &deleted) {
    m_deleted = &deleted;
    throw std::runtime_error("Test");
  }

  ObjectRefCountSample51InitThrow() noexcept = default;

  ~ObjectRefCountSample51InitThrow() noexcept {
    *m_deleted = true;
  }

  friend MakePolicy; // To support private constructor & InitializeThis
  friend RefCountPolicy; // To support private destructor

 private:
  bool *m_deleted{nullptr};
};

TEST_CLASS (ObjectRefCountTest) {
  TEST_METHOD(ObjectRefCount_Make) {
    TestAssert::AreEqual(sizeof(uint32_t), sizeof(Mso::RefCountedObjectNoVTable<ObjectRefCountSample1>));

    bool deleted = false;
    {
      Mso::CntPtr<ObjectRefCountSample1> obj1 = Mso::Make<ObjectRefCountSample1>(5, /*ref*/ deleted);
      {
        Debug(TestAssert::AreEqual(1u, obj1->RefCount()));
        TestAssert::AreEqual(5, obj1->Value());

        Mso::CntPtr<ObjectRefCountSample1> obj2(obj1);
        Debug(TestAssert::AreEqual(2u, obj1->RefCount()));
        TestAssert::IsNotNull(obj2.Get());
        TestAssert::IsTrue(obj1.Get() == obj2.Get());
      }
      Debug(TestAssert::AreEqual(1u, obj1->RefCount()));
    }

    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(ObjectRefCount_Make_InitializeThis) {
    bool deleted = false;
    {
      auto obj = Mso::Make<ObjectRefCountSample2Init>(5, /*ref*/ deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    TestAssert::IsTrue(deleted);
  }

  TESTMETHOD_REQUIRES_SEH(ObjectRefCount_Make_CannotAllocate) {
    Mso::CntPtr<ObjectRefCountSample3CannotAllocate> obj;
    TestAssert::ExpectVEC([&]() noexcept { obj = Mso::Make<ObjectRefCountSample3CannotAllocate>(); });

    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectRefCount_Make_CtorThrows) {
    Mso::CntPtr<ObjectRefCountSample4Throw> obj;
    bool deleted = false;
    TestAssert::ExpectException<std::runtime_error>(
        [&]() { obj = Mso::Make<ObjectRefCountSample4Throw>(/*ref*/ deleted); });

    TestAssert::IsFalse(deleted); // Destructor is not called if constructor throws.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectRefCount_Make_InitializeThisThrows) {
    Mso::CntPtr<ObjectRefCountSample5InitThrow> obj;
    bool deleted = false;
    TestAssert::ExpectException<std::runtime_error>(
        [&]() { obj = Mso::Make<ObjectRefCountSample5InitThrow>(/*ref*/ deleted); });

    TestAssert::IsTrue(deleted); // If InitializeThis throws then destructor must be called.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectRefCount_MakeElseNull) {
    bool deleted = false;
    {
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      auto obj = Mso::MakeElseNull<ObjectRefCountSample1>(5, /*ref*/ deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(ObjectRefCount_MakeElseNull_InitializeThis) {
    bool deleted = false;
    {
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      auto obj = Mso::MakeElseNull<ObjectRefCountSample2Init>(5, /*ref*/ deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    TestAssert::IsTrue(deleted);
  }

  TEST_METHOD(ObjectRefCount_MakeElseNull_CannotAllocate) {
    OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
    auto obj = Mso::MakeElseNull<ObjectRefCountSample3CannotAllocate>();
    TestAssert::IsNull(obj.Get());
  }

  TEST_METHOD(ObjectRefCount_MakeElseNull_CtorThrows) {
    Mso::CntPtr<ObjectRefCountSample4Throw> obj;
    bool deleted = false;
    TestAssert::ExpectException<std::runtime_error>([&]() {
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      obj = Mso::MakeElseNull<ObjectRefCountSample4Throw>(/*ref*/ deleted);
    });

    TestAssert::IsFalse(deleted); // Destructor is not called if constructor throws.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectRefCount_MakeElseNull_InitializeThisThrows) {
    Mso::CntPtr<ObjectRefCountSample5InitThrow> obj;
    bool deleted = false;
    TestAssert::ExpectException<std::runtime_error>([&]() {
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      obj = Mso::MakeElseNull<ObjectRefCountSample5InitThrow>(/*ref*/ deleted);
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

  TEST_METHOD(ObjectRefCount_MakeAlloc) {
    AllocTestState state = {};
    {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      auto obj = Mso::MakeAlloc<ObjectRefCountSample11>(&memHeap, 5, /*ref*/ state.Deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    AssertAllocState(state);
  }

  TEST_METHOD(ObjectRefCount_MakeAlloc_InitializeThis) {
    AllocTestState state = {};
    {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      auto obj = Mso::MakeAlloc<ObjectRefCountSample21Init>(&memHeap, 5, /*ref*/ state.Deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    AssertAllocState(state);
  }

  TESTMETHOD_REQUIRES_SEH(ObjectRefCount_MakeAlloc_CannotAllocate) {
    Mso::CntPtr<ObjectRefCountSample31CannotAllocate> obj;
    TestAssert::ExpectVEC([&]() noexcept {
      AllocTestState state = {};
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      obj = Mso::MakeAlloc<ObjectRefCountSample31CannotAllocate>(&memHeap);
    });

    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectRefCount_MakeAlloc_CtorThrows) {
    Mso::CntPtr<ObjectRefCountSample41Throw> obj;
    AllocTestState state = {};
    TestAssert::ExpectException<std::runtime_error>([&]() {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      obj = Mso::MakeAlloc<ObjectRefCountSample41Throw>(&memHeap, /*ref*/ state.Deleted);
    });

    AssertAllocState(state, /*deleted:*/ false); // Destructor is not called if constructor throws.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectRefCount_MakeAlloc_InitializeThisThrows) {
    Mso::CntPtr<ObjectRefCountSample51InitThrow> obj;
    AllocTestState state = {};
    TestAssert::ExpectException<std::runtime_error>([&]() {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      obj = Mso::MakeAlloc<ObjectRefCountSample51InitThrow>(&memHeap, /*ref*/ state.Deleted);
    });

    AssertAllocState(state); // If InitializeThis throws then destructor must be called.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectRefCount_MakeAllocElseNull) {
    AllocTestState state = {};
    {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      auto obj = Mso::MakeAllocElseNull<ObjectRefCountSample11>(&memHeap, 5, /*ref*/ state.Deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    AssertAllocState(state);
  }

  TEST_METHOD(ObjectRefCount_MakeAllocElseNull_InitializeThis) {
    AllocTestState state = {};
    {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      auto obj = Mso::MakeAllocElseNull<ObjectRefCountSample21Init>(&memHeap, 5, /*ref*/ state.Deleted);
      Debug(TestAssert::AreEqual(1u, obj->RefCount()));
      TestAssert::AreEqual(5, obj->Value());
    }

    AssertAllocState(state);
  }

  TEST_METHOD(ObjectRefCount_MakeAllocElseNull_CannotAllocate) {
    AllocTestState state = {};
    MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
    OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
    auto obj = Mso::MakeAllocElseNull<ObjectRefCountSample31CannotAllocate>(&memHeap);
    TestAssert::IsNull(obj.Get());
  }

  TEST_METHOD(ObjectRefCount_MakeAllocElseNull_CtorThrows) {
    Mso::CntPtr<ObjectRefCountSample41Throw> obj;
    AllocTestState state = {};
    TestAssert::ExpectException<std::runtime_error>([&]() {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      obj = Mso::MakeAllocElseNull<ObjectRefCountSample41Throw>(&memHeap, /*ref*/ state.Deleted);
    });

    AssertAllocState(state, /*deleted:*/ false); // Destructor is not called if constructor throws.
    TestAssert::IsTrue(obj.IsEmpty());
  }

  TEST_METHOD(ObjectRefCount_MakeAllocElseNull_InitializeThisThrows) {
    Mso::CntPtr<ObjectRefCountSample51InitThrow> obj;
    AllocTestState state = {};
    TestAssert::ExpectException<std::runtime_error>([&]() {
      MyMemHeap memHeap(/*ref*/ state.AllocCalled, /*ref*/ state.FreeCalled);
      OACR_WARNING_SUPPRESS(DEPRECATED_FUNCTION, "Exemption for deprecated function when CG is off.");
      obj = Mso::MakeAllocElseNull<ObjectRefCountSample51InitThrow>(&memHeap, /*ref*/ state.Deleted);
    });

    AssertAllocState(state); // If InitializeThis throws then we must call destructor.
    TestAssert::IsTrue(obj.IsEmpty());
  }
};
