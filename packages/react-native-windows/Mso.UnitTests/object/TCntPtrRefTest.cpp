// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <combaseapi.h>
#include <atomic>
#include <functional>
#include "motifCpp/testCheck.h"
#include "object/refCountedObject.h"
#include "object/unknownObject.h"

MSO_STRUCT_GUID(ISimpleUnknown, "C70B7853-1EDB-4AB8-B2D2-F9951A99A847")
struct DECLSPEC_NOVTABLE ISimpleUnknown : public IUnknown {
  virtual void DoSomething() = 0;
};

TestClassComponent(CntPtrRefTest, Mso.CntPtrRef);
TEST_CLASS (CntPtrRefTest) {
  struct ISimple : public Mso::IRefCounted {
    virtual void DoSomething() = 0;
  };

  class SimpleTestRef : public Mso::RefCountedObject<ISimple> {
    void DoSomething() override {}
  };

  TEST_METHOD(CntPtr_CntPtrRef_GetRaw) {
    Mso::CntPtr<ISimple> spObj;
    TestAssert::AreEqual(spObj.GetRaw(), (&spObj).GetRaw());
  }

  TEST_METHOD(CntPtr_CntPtrRef_StarOperator) {
    Mso::CntPtr<ISimple> spObj;
    TestAssert::AreEqual(spObj.Get(), *(&spObj));
  }

  TEST_METHOD(CntPtr_CntPtrRef_GetAddressOf) {
    Mso::CntPtr<ISimple> spObj;
    TestAssert::AreEqual(spObj.GetAddressOf(), (&spObj).GetAddressOf());
  }

  TEST_METHOD(CntPtr_CntPtrRef_ClearAndGetAddressOf) {
    Mso::CntPtr<ISimple> spObj = Mso::Make<SimpleTestRef>();
    TestAssert::AreEqual(spObj.GetRaw(), (&spObj).ClearAndGetAddressOf());
    TestAssert::IsNull(spObj.Get());
  }

  TEST_METHOD(CntPtr_CntPtrRef_IID_PPV_ARGS_Helper) {
    Mso::CntPtr<IUnknown> spObj;
    void **ppObj = IID_PPV_ARGS_Helper<IUnknown>(&spObj);
    void **ppDesired = &spObj;
    TestAssert::AreEqual(ppDesired, ppObj);
  }

  TEST_METHOD(CntPtr_CntPtrRef_EqualOperator) {
    Mso::CntPtr<IUnknown> spObj1;
    Mso::CntPtr<IUnknown> spObj2;
    TestAssert::IsTrue(&spObj1 == &spObj1);
    TestAssert::IsTrue(&spObj2 == &spObj2);
    TestAssert::IsFalse(&spObj1 == &spObj2);
    TestAssert::IsFalse(&spObj2 == &spObj1);
  }

  TEST_METHOD(CntPtr_CntPtrRef_NotEqualOperator) {
    Mso::CntPtr<IUnknown> spObj1;
    Mso::CntPtr<IUnknown> spObj2;
    TestAssert::IsTrue(&spObj1 != &spObj2);
    TestAssert::IsTrue(&spObj2 != &spObj1);
    TestAssert::IsFalse(&spObj2 != &spObj2);
    TestAssert::IsFalse(&spObj1 != &spObj1);
  }

  static void TestFunc1(_In_ _Notnull_ ISimple * pObj, _Out_ ISimple * *ppObj) {
    *ppObj = pObj;
  }

  static void TestFunc2(_In_ _Notnull_ ISimple * pObj, _Out_ Mso::CntPtr<ISimple> * spObj) {
    *spObj = Mso::CntPtr<ISimple>{pObj, Mso::AttachTag};
  }

  static void TestFunc3(_In_ _Notnull_ ISimple * pObj, _Out_ void **ppT) {
    *ppT = pObj;
  }

  static void TestFunc4(_In_ _Notnull_ const ISimple *pObj, _Out_ const void **ppT) {
    *ppT = pObj;
  }

  template <typename T, typename U>
  static void TestCntPtrRef(U && func) {
    T spObjOutput;
    Mso::CntPtr<ISimple> spObjInput = Mso::Make<SimpleTestRef>();
    ISimple *pObjInput = spObjInput.Detach();

    func(pObjInput, &spObjOutput);

    TestAssert::AreEqual(pObjInput, spObjOutput.Get());
  }

  TEST_METHOD(CntPtr_CntPtrRef_NonConst_ppT) {
    TestCntPtrRef<Mso::CntPtr<ISimple>>(TestFunc1);
  }

  TEST_METHOD(CntPtr_CntPtrRef_NonConst_pCntPtr) {
    TestCntPtrRef<Mso::CntPtr<ISimple>>(TestFunc2);
  }

  TEST_METHOD(CntPtr_CntPtrRef_NonConst_ppVoid) {
    TestCntPtrRef<Mso::CntPtr<ISimple>>(TestFunc3);
  }

  TEST_METHOD(CntPtr_CntPtrRef_NonConst_ppConstVoid) {
    TestCntPtrRef<Mso::CntPtr<ISimple>>(TestFunc4);
  }

  TEST_METHOD(CntPtr_CntPtrRef_Const_CntPtr) {
    const Mso::CntPtr<ISimple> spObj;
    TestAssert::AreEqual(std::addressof(spObj), &spObj);
  }

  template <typename T, typename U>
  static void TestCntPtrRef_Reinterpret_Cast() {
    using TNonConst = typename std::remove_const<T>::type;
    T spObj;
    U ppObjDesired = reinterpret_cast<U>(const_cast<TNonConst &>(spObj).GetAddressOf());
    U ppObj = static_cast<U>(&spObj);
    TestAssert::AreEqual(ppObjDesired, ppObj);
  }

  TEST_METHOD(CntPtr_CntPtrRef_NonConst_Reinterpret_Cast_ppVoid) {
    TestCntPtrRef_Reinterpret_Cast<Mso::CntPtr<ISimple>, void **>();
  }

  class SimpleTestRefUnknown : public Mso::UnknownObject<ISimpleUnknown> {
    void DoSomething() override {}
  };

  TEST_METHOD(CntPtr_CntPtrRef_NonConst_Reinterpret_Cast_ppIUnknown) {
    TestCntPtrRef_Reinterpret_Cast<Mso::CntPtr<SimpleTestRefUnknown>, IUnknown **>();
  }

  template <typename T>
  static void TestCntPtrRef_Cast_pVoid() {
    T spObj;
    void *pVoid = &spObj;
    Mso::CntPtr<ISimple> *pCntPtr = &spObj;
    TestAssert::AreEqual(pVoid, pCntPtr);
  }

  TEST_METHOD(CntPtr_CntPtrRef_NonConst_Cast_pVoid) {
    TestCntPtrRef_Cast_pVoid<Mso::CntPtr<ISimple>>();
  }
};
