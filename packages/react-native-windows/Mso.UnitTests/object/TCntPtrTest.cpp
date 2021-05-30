// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <atomic>
#include <functional>
#include "comUtil/qiCast.h"
#include "object/unknownObject.h"

MSO_STRUCT_GUID(IUnkSimple, "9FEAB33F-E5D0-4A52-9216-6BA8BA9990A4")
struct IUnkSimple : public IUnknown {
  virtual void DoSomething() = 0;
};

MSO_STRUCT_GUID(IUnkSample, "8A2560F5-E28D-4342-8716-1BBD3A4603B3")
struct IUnkSample : public IUnknown {
  virtual void DoAnything() = 0;
};

TestClassComponent(CntPtrTest, Mso.CntPtr) TEST_CLASS (CntPtrTest) {
  struct ISimple : public Mso::IRefCounted {
    virtual void DoSomething() = 0;
  };

  OACR_WARNING_DISABLE(
      AVOID_STD_FUNCTION,
      "Mso::Functor cannot be used in this test because it may use the components being tested here")
  typedef std::function<void(bool inc)> RefCountChangedCallback;
  class SimpleClass : public ISimple {
   public:
    SimpleClass(RefCountChangedCallback &&onRefCountChanged)
        : m_refCount(0), m_onRefCountChanged(std::move(onRefCountChanged)) {}

    virtual void AddRef() const noexcept override {
      OACR_ASSUME_NOTHROW_BEGIN

      m_onRefCountChanged(/*incremented*/ true);

      OACR_ASSUME_NOTHROW_END

      ++m_refCount;
    }

    virtual void Release() const noexcept override {
      OACR_ASSUME_NOTHROW_BEGIN

      m_onRefCountChanged(/*incremented*/ false);

      OACR_ASSUME_NOTHROW_END

      if (--m_refCount == 0) {
        delete this;
      }
    }

    virtual void DoSomething() noexcept override {}

    void ClassDoSomething() noexcept {
      OACR_USE_PTR(this); // simulates access to 'this' for OACR build
    }

   private:
    mutable std::atomic<uint32_t> m_refCount;
    RefCountChangedCallback m_onRefCountChanged;
  };

  class UnkSimpleClass : public IUnkSimple {
   public:
    UnkSimpleClass(RefCountChangedCallback &&onRefCountChanged)
        : m_refCount(0), m_onRefCountChanged(std::move(onRefCountChanged)) {}

    _Success_(return == S_OK)
        STDMETHOD(QueryInterface)(const GUID & /*riid*/, _Outptr_ void ** /*ppvObject*/) noexcept override {
      return E_NOINTERFACE;
    }

    STDMETHOD_(ULONG, AddRef)() noexcept override {
      OACR_ASSUME_NOTHROW_BEGIN
      m_onRefCountChanged(/*incremented*/ true);
      OACR_ASSUME_NOTHROW_END
      ++m_refCount;
      return 1;
    }

    STDMETHOD_(ULONG, Release)() noexcept override {
      OACR_ASSUME_NOTHROW_BEGIN
      m_onRefCountChanged(/*incremented*/ false);
      OACR_ASSUME_NOTHROW_END
      if (--m_refCount == 0) {
        delete this;
      }

      return 1;
    }

    virtual void DoSomething() noexcept override {}

    void ClassDoSomething() noexcept {
      OACR_USE_PTR(this); // simulates access to 'this' for OACR build
    }

   private:
    mutable std::atomic<uint32_t> m_refCount;
    RefCountChangedCallback m_onRefCountChanged;
  };

  class AggregatedObject : public Mso::UnknownObject<IUnkSimple, IUnkSample> {
   public:
    virtual void DoSomething() noexcept override {}

    virtual void DoAnything() noexcept override {}
  };

  template <typename TAction>
  static void ValidateRefCount(uint32_t expectedIncRefCountCallCount, TAction action) {
    uint32_t actualIncRefCountCallCount = 0;
    uint32_t actualDecRefCountCallCount = 0;
    auto callback = [&actualIncRefCountCallCount, &actualDecRefCountCallCount](bool incremented) noexcept {
      if (incremented) {
        ++actualIncRefCountCallCount;
      } else {
        ++actualDecRefCountCallCount;
      }
    };

    action(RefCountChangedCallback(callback));

    TestAssert::AreEqual(actualIncRefCountCallCount, actualDecRefCountCallCount, L"IncCount != DecCount.");
    TestAssert::AreEqual(expectedIncRefCountCallCount, actualIncRefCountCallCount, L"Unexpected IncCount.");
  }

  TEST_METHOD(CntPtr_EmptyCtor) {
    Mso::CntPtr<SimpleClass> spObj;

    TestAssert::IsNull(spObj.Get(), L"Expected null");
  }

  TEST_METHOD(CntPtr_NullCtor) {
    Mso::CntPtr<SimpleClass> spObj{nullptr};

    TestAssert::IsNull(spObj.Get(), L"Expected null");
  }

  TEST_METHOD(CntPtr_DeprecatedNullCtor) {
    // TODO: Remove when we stop using NULL
    Mso::CntPtr<SimpleClass> spObj;

    TestAssert::IsNull(spObj.Get(), L"Expected null");
  }

  TEST_METHOD(CntPtr_Create) {
    ValidateRefCount(1, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj(ptr);

      TestAssert::AreEqual(ptr, spObj.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_CreateInterface) {
    ValidateRefCount(1, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<ISimple> spIntf(ptr);

      TestAssert::AreEqual(ptr, spIntf.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_CopyConstructor) {
    ValidateRefCount(2, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj(ptr);
      Mso::CntPtr<SimpleClass> spSameObj(spObj);

      TestAssert::AreEqual(ptr, spObj.Get(), L"Expected ptr");
      TestAssert::AreEqual(ptr, spSameObj.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_CopyConstructorInterface) {
    ValidateRefCount(2, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj(ptr);
      Mso::CntPtr<ISimple> spIntf(spObj);

      TestAssert::AreEqual(ptr, spObj.Get(), L"Expected ptr");
      TestAssert::AreEqual(ptr, spIntf.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_MoveConstructor) {
    ValidateRefCount(1, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj(ptr);
      Mso::CntPtr<SimpleClass> spSameObj(std::move(spObj));

      TestAssert::IsNull(spObj.Get(), L"Expected null");
      TestAssert::AreEqual(ptr, spSameObj.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_MoveConstructorInterface) {
    ValidateRefCount(1, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj(ptr);
      Mso::CntPtr<ISimple> spIntf(std::move(spObj));

      TestAssert::IsNull(spObj.Get(), L"Expected null");
      TestAssert::AreEqual(ptr, spIntf.Get(), L"Expected ptr");
    });
  }

  // Factory method to get benefits from using the move constructor
  static Mso::CntPtr<SimpleClass> CreateSimpleClass(RefCountChangedCallback && onRefCountChanged) {
    Mso::CntPtr<SimpleClass> spObj{new SimpleClass(std::move(onRefCountChanged))};
    spObj->ClassDoSomething();
    return spObj; // std::move() not needed because the same type allows the named return value optimization.
  }

  TEST_METHOD(CntPtr_CallCreateSimpleClass) {
    ValidateRefCount(1, [](RefCountChangedCallback &&onRefCountChanged) {
      Mso::CntPtr<ISimple> spObj{CreateSimpleClass(std::move(onRefCountChanged))};

      TestAssert::IsNotNull(spObj.Get(), L"Expected not a null value");
    });
  }

  // Factory method to get benefits from using the move constructor
  static Mso::CntPtr<ISimple> CreateISimple(RefCountChangedCallback && onRefCountChanged) {
    Mso::CntPtr<SimpleClass> spObj{new SimpleClass(std::move(onRefCountChanged))};
    spObj->ClassDoSomething();
    return std::move(spObj); // We should use std::move() here to avoid use of copy constructor.
                             // Named value return optimization will not work because we have different types.
  }

  TEST_METHOD(CntPtr_CallCreateISimple) {
    ValidateRefCount(1, [](RefCountChangedCallback &&onRefCountChanged) {
      Mso::CntPtr<ISimple> spIntf{CreateISimple(std::move(onRefCountChanged))};

      TestAssert::IsNotNull(spIntf.Get(), L"Expected not a null value");
    });
  }

  TEST_METHOD(CntPtr_CopyAssignment) {
    ValidateRefCount(3, [](RefCountChangedCallback &&onRefCountChanged) {
      Mso::CntPtr<SimpleClass> spObj1{new SimpleClass(RefCountChangedCallback(onRefCountChanged))};
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj2(ptr);
      spObj1 = spObj2;

      TestAssert::AreEqual(ptr, spObj1.Get(), L"Expected ptr");
      TestAssert::AreEqual(ptr, spObj2.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_CopyAssignmentInterface) {
    ValidateRefCount(3, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(RefCountChangedCallback(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj(ptr);
      Mso::CntPtr<ISimple> spIntf{new SimpleClass(std::move(onRefCountChanged))};
      spIntf = spObj;

      TestAssert::AreEqual(ptr, spObj.Get(), L"Expected ptr");
      TestAssert::AreEqual(ptr, spIntf.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_CopyAssignmentSameObject) {
    // See what happens when we assign CntPtr to itself.
    ValidateRefCount(1, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj{ptr};

      OACR_WARNING_SUPPRESS(IDENTITY_ASSIGNMENT, "We want to test our code that nothing bad happens in this case");
      spObj = spObj;

      TestAssert::AreEqual(ptr, spObj.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_CopyAssignmentConst) {
    // Test that CntPtr can accept a const variable and AddRef/Release methods are not const.
    ValidateRefCount(3, [](RefCountChangedCallback &&onRefCountChanged) {
      Mso::CntPtr<const UnkSimpleClass> spObj1(new UnkSimpleClass(RefCountChangedCallback(onRefCountChanged)));
      const UnkSimpleClass *ptr = new UnkSimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<const UnkSimpleClass> spObj2(ptr);
      spObj1 = spObj2;

      TestAssert::AreEqual(ptr, spObj1.Get(), L"Expected ptr");
      TestAssert::AreEqual(ptr, spObj2.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_MoveAssignment) {
    ValidateRefCount(2, [](RefCountChangedCallback &&onRefCountChanged) {
      Mso::CntPtr<SimpleClass> spObj1{new SimpleClass(RefCountChangedCallback(onRefCountChanged))};
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj2(ptr);
      spObj1 = std::move(spObj2);

      TestAssert::AreEqual(ptr, spObj1.Get(), L"Expected ptr");
      TestAssert::IsNull(spObj2.Get(), L"Expected null");
    });
  }

  TEST_METHOD(CntPtr_MoveAssignmentInterface) {
    ValidateRefCount(2, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(RefCountChangedCallback(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj(ptr);
      Mso::CntPtr<ISimple> spIntf{new SimpleClass(std::move(onRefCountChanged))};
      spIntf = std::move(spObj);

      TestAssert::IsNull(spObj.Get(), L"Expected null");
      TestAssert::AreEqual(ptr, spIntf.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_MoveAssignmentSameObject) {
    // Our copy assignment does not check if we use the same object. This test is to see that nothing bad happens.
    ValidateRefCount(1, [](RefCountChangedCallback &&onRefCountChanged) {
      SimpleClass *ptr = new SimpleClass(std::move(onRefCountChanged));
      Mso::CntPtr<SimpleClass> spObj(ptr);
      spObj = std::move(spObj);

      TestAssert::AreEqual(ptr, spObj.Get(), L"Expected ptr");
    });
  }

  TEST_METHOD(CntPtr_NullAssignment) {
    ValidateRefCount(1, [](RefCountChangedCallback &&onRefCountChanged) {
      Mso::CntPtr<SimpleClass> spObj{new SimpleClass(RefCountChangedCallback(onRefCountChanged))};
      spObj = nullptr;

      TestAssert::IsNull(spObj.Get(), L"Expected null");
    });
  }

  TEST_METHOD(CntPtr_IsEqualObject_BothISimpleClass_AreEqual) {
    Mso::CntPtr<IUnkSimple> spObj = Mso::Make<AggregatedObject>();
    Mso::CntPtr<IUnkSimple> spObjTwo = spObj;

    TestAssert::IsTrue(Mso::ComUtil::AreEqualObjects(spObj, spObjTwo));
  }

  TEST_METHOD(CntPtr_IsEqualObject_DifferentInterfaceTypes_AreEqual) {
    Mso::CntPtr<IUnkSimple> spObj = Mso::Make<AggregatedObject>();
    Mso::CntPtr<IUnkSample> spSample;
    TestAssert::HrSucceeded(Mso::ComUtil::HrQueryFrom(spSample, spObj));

    TestAssert::IsTrue(Mso::ComUtil::AreEqualObjects(spObj, spSample));
  }

  TEST_METHOD(CntPtr_IsEqualObject_DifferentObject_AreNotEqual) {
    Mso::CntPtr<IUnkSimple> spObj = Mso::Make<AggregatedObject>();
    Mso::CntPtr<IUnkSimple> spObjTwo = Mso::Make<AggregatedObject>();

    TestAssert::IsFalse(Mso::ComUtil::AreEqualObjects(spObj, spObjTwo));
  }
};
