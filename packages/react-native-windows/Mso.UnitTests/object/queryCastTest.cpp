// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "object/queryCast.h"
#include <string>

#define DEFINE_STRING_GUID(var, guid) \
  MSO_STRUCT_GUID(_test_##var, guid)  \
  struct _test_##var {};              \
  GUID var = __uuidof(_test_##var);

MSO_STRUCT_GUID(IQueryCastBase1, "EAE07273-AEFC-4E94-8087-E72F5D028BFC")
struct DECLSPEC_NOVTABLE IQueryCastBase1 {
  virtual int GetValue1() = 0;
};

MSO_STRUCT_GUID(IQueryCastBase2, "D62D6F01-0A54-40C0-9D23-A1C95EAF234D")
struct DECLSPEC_NOVTABLE IQueryCastBase2 {
  virtual int GetValue2() = 0;
};

MSO_STRUCT_GUID(IQueryCastBase3, "54A5CAC7-4788-499E-BAD1-1BBEF60A30F9")
struct DECLSPEC_NOVTABLE IQueryCastBase3 {
  virtual int GetValue3() = 0;
};

MSO_STRUCT_GUID(IQueryCastDerived1, "26747765-CB3C-42BC-8CBE-810F19779169")
struct DECLSPEC_NOVTABLE IQueryCastDerived1 : public IQueryCastBase1 {
  virtual int GetValue11() = 0;
};

class QueryCastTraitsSample1 : public IQueryCastBase1 {
 public:
  virtual int GetValue1() override {
    return 1;
  }
};

struct QueryCastTraitsBase2 {
  void *QueryCast(const GUID &riid) noexcept {
    if (riid == __uuidof(IQueryCastBase1)) {
      return this;
    }

    return nullptr;
  }
};

class QueryCastTraitsSample2 : public QueryCastTraitsBase2 {};

MSO_STRUCT_GUID(QueryCastTraitsBase3, "5AF7D77B-7620-403D-9684-98AC591CF1A6")
struct QueryCastTraitsBase3 {
  void *QueryCast(const GUID &riid) noexcept {
    if (riid == __uuidof(IQueryCastBase1)) {
      return this;
    }

    return nullptr;
  }
};

class QueryCastTraitsSample3 : public QueryCastTraitsBase3 {};

class QueryCastChainSample1 : public Mso::QueryCastChain<IQueryCastDerived1, IQueryCastBase1> {
 public:
  virtual int GetValue1() override {
    return 1;
  }
  virtual int GetValue11() override {
    return 11;
  }
};

class QueryCastChainSample2
    : public Mso::QueryCastList<Mso::QueryCastChain<IQueryCastDerived1, IQueryCastBase1>, IQueryCastBase2> {
 public:
  virtual int GetValue1() override {
    return 1;
  }
  virtual int GetValue11() override {
    return 11;
  }
  virtual int GetValue2() override {
    return 2;
  }
};

MSO_CLASS_GUID(QueryCastDerivedSample1, "1B7B2202-8B80-4E08-B20B-85235696BE02")
class QueryCastDerivedSample1
    : public Mso::QueryCastList<IQueryCastBase1, Mso::QueryCastDerived<QueryCastDerivedSample1>> {
 public:
  virtual int GetValue1() override {
    return 1;
  }
};

DEFINE_STRING_GUID(MyTestGuid1, "65ADDF73-B4EE-4E36-AB61-FEEA1F01A169");

struct DECLSPEC_NOVTABLE IQueryCastNoGuid1 {
  virtual int GetValue1() = 0;
};

class QueryCastGuidSample1
    : public Mso::
          QueryCastList<Mso::QueryCastGuid<IQueryCastNoGuid1, &MyTestGuid1>, Mso::QueryCastGuid<IQueryCastBase2>> {
 public:
  virtual int GetValue1() override {
    return 1;
  }
  virtual int GetValue2() override {
    return 2;
  }
};

class QueryCastHiddenSample1 : public Mso::QueryCastHidden<IQueryCastBase1> {
 public:
  virtual int GetValue1() override {
    return 1;
  }
};

class QueryCastListSample1 : public Mso::QueryCastList<IQueryCastBase1> {
 public:
  virtual int GetValue1() override {
    return 1;
  }
};

class QueryCastListSample2 : public Mso::QueryCastList<IQueryCastBase1, IQueryCastBase2> {
 public:
  virtual int GetValue1() override {
    return 12;
  }
  virtual int GetValue2() override {
    return 22;
  }
};

class QueryCastListSample3 : public Mso::QueryCastList<IQueryCastBase1, IQueryCastBase2, IQueryCastBase3> {
 public:
  virtual int GetValue1() override {
    return 13;
  }
  virtual int GetValue2() override {
    return 23;
  }
  virtual int GetValue3() override {
    return 33;
  }
};

class QueryCastListDerivedSample1 : public Mso::QueryCastList<QueryCastListSample2, IQueryCastBase3> {
 public:
  virtual int GetValue3() override {
    return 3;
  }
};

struct StaticCastSample1 : public Mso::QueryCastList<QueryCastListSample2, QueryCastListSample3> {};

//
// Four interfaces to test StaticCastElseNull method: two of them inherit from IUnknown and other two don't
//

struct IUnkStaticBase1 : IUnknown {
  virtual int GetValue1() = 0;
};

struct IUnkStaticBase2 : IUnknown {
  virtual int GetValue2() = 0;
};

struct IStaticBase3 {
  virtual int GetValue3() = 0;
};

struct IStaticBase4 {
  virtual int GetValue4() = 0;
};

// To see if StaticCastElseNull<IUnknown*> works for one interface inherited from IUnknown
struct StaticSample1 : Mso::QueryCastList<IUnkStaticBase1> {
  virtual int GetValue1() override {
    return 1;
  }
  STDMETHOD(QueryInterface)(const GUID & /*riid*/, void ** /*ppvObject*/) noexcept override {
    return E_NOINTERFACE;
  }
  STDMETHOD_(ULONG, AddRef)() noexcept override {
    return 1;
  }
  STDMETHOD_(ULONG, Release)() noexcept override {
    return 1;
  }
};

// To see if StaticCastElseNull<IUnknown*> works for two interfaces inherited from IUnknown
struct StaticSample2 : Mso::QueryCastList<IUnkStaticBase1, IUnkStaticBase2> {
  virtual int GetValue1() override {
    return 1;
  }
  virtual int GetValue2() override {
    return 2;
  }
  STDMETHOD(QueryInterface)(const GUID & /*riid*/, void ** /*ppvObject*/) noexcept override {
    return E_NOINTERFACE;
  }
  STDMETHOD_(ULONG, AddRef)() noexcept override {
    return 1;
  }
  STDMETHOD_(ULONG, Release)() noexcept override {
    return 1;
  }
};

// To see if StaticCastElseNull<IUnknown*> returns nullptr for one interface not inherited from IUnknown
struct StaticSample3 : Mso::QueryCastList<IStaticBase3> {
  virtual int GetValue3() override {
    return 3;
  }
};

// To see if StaticCastElseNull<IUnknown*> returns nullptr for two interfaces not inherited from IUnknown
struct StaticSample4 : Mso::QueryCastList<IStaticBase3, IStaticBase4> {
  virtual int GetValue3() override {
    return 3;
  }
  virtual int GetValue4() override {
    return 4;
  }
};

// To see if StaticCastElseNull<IUnknown*> works for two interfaces inherited from IUnknown preceded by
// an interface not inherited from IUnknown.
struct StaticSample5 : Mso::QueryCastList<IStaticBase3, IUnkStaticBase1, IUnkStaticBase2> {
  virtual int GetValue1() override {
    return 1;
  }
  virtual int GetValue2() override {
    return 2;
  }
  virtual int GetValue3() override {
    return 3;
  }
  STDMETHOD(QueryInterface)(const GUID & /*riid*/, void ** /*ppvObject*/) noexcept override {
    return E_NOINTERFACE;
  }
  STDMETHOD_(ULONG, AddRef)() noexcept override {
    return 1;
  }
  STDMETHOD_(ULONG, Release)() noexcept override {
    return 1;
  }
};

// To see if StaticCastElseNull<IUnknown*> works for two interfaces inherited from IUnknown preceded by
// two interfaces not inherited from IUnknown.
struct StaticSample6 : Mso::QueryCastList<IStaticBase3, IStaticBase4, IUnkStaticBase1, IUnkStaticBase2> {
  virtual int GetValue1() override {
    return 1;
  }
  virtual int GetValue2() override {
    return 2;
  }
  virtual int GetValue3() override {
    return 3;
  }
  virtual int GetValue4() override {
    return 4;
  }
  STDMETHOD(QueryInterface)(const GUID & /*riid*/, void ** /*ppvObject*/) noexcept override {
    return E_NOINTERFACE;
  }
  STDMETHOD_(ULONG, AddRef)() noexcept override {
    return 1;
  }
  STDMETHOD_(ULONG, Release)() noexcept override {
    return 1;
  }
};

// To test nested hierarchy: IUnknown is implemented by nested QueryCastList.
struct StaticSample7
    : Mso::QueryCastList<IStaticBase3, Mso::QueryCastList<IStaticBase4, IUnkStaticBase1, IUnkStaticBase2>> {
  virtual int GetValue1() override {
    return 1;
  }
  virtual int GetValue2() override {
    return 2;
  }
  virtual int GetValue3() override {
    return 3;
  }
  virtual int GetValue4() override {
    return 4;
  }
  STDMETHOD(QueryInterface)(const GUID & /*riid*/, void ** /*ppvObject*/) noexcept override {
    return E_NOINTERFACE;
  }
  STDMETHOD_(ULONG, AddRef)() noexcept override {
    return 1;
  }
  STDMETHOD_(ULONG, Release)() noexcept override {
    return 1;
  }
};

// To test nested hierarchy: IUnknown is implemented by nested QueryCastList.
struct StaticSample8 : Mso::QueryCastList<
                           Mso::QueryCastList<IStaticBase3, IStaticBase4>,
                           Mso::QueryCastList<IUnkStaticBase1, IUnkStaticBase2>> {
  virtual int GetValue1() override {
    return 1;
  }
  virtual int GetValue2() override {
    return 2;
  }
  virtual int GetValue3() override {
    return 3;
  }
  virtual int GetValue4() override {
    return 4;
  }
  STDMETHOD(QueryInterface)(const GUID & /*riid*/, void ** /*ppvObject*/) noexcept override {
    return E_NOINTERFACE;
  }
  STDMETHOD_(ULONG, AddRef)() noexcept override {
    return 1;
  }
  STDMETHOD_(ULONG, Release)() noexcept override {
    return 1;
  }
};

// To test nested hierarchy: IUnknown is not implemented.
struct StaticSample9 : Mso::QueryCastList<Mso::QueryCastList<IStaticBase3, IStaticBase4>> {
  virtual int GetValue3() override {
    return 3;
  }
  virtual int GetValue4() override {
    return 4;
  }
};

class QueryCastBase1WithArgs : public IQueryCastBase1 {
 public:
  int m_int;
  std::string m_string;

  QueryCastBase1WithArgs(int i, const std::string &str) : m_int(i), m_string(str) {}
  virtual int GetValue1() override {
    return 1;
  }
};

// Wrapper so that we can call protected constructors
template <typename T>
struct StructWithBase : public T {
  template <typename... Args>
  StructWithBase(Args &&... args) : T(std::forward<Args>(args)...) {}
};

// A helper method to call QueryCastTraits::QueryCast.
template <typename TTarget, typename TSource>
inline TTarget *TraitsQueryCast(const TSource &source, const GUID &riid = __uuidof(TTarget)) noexcept {
  if (source != nullptr) {
    return static_cast<TTarget *>(
        Mso::QueryCastHelper::QueryCast<typename std::remove_pointer<TSource>::type>(source, riid));
  }

  return nullptr;
}

TestClassComponent(ObjectQueryCastTest, Mso.ObjectQueryCast)
    TEST_CLASS (ObjectQueryCastTest){// Use GUID by default.
                                     TEST_METHOD(QueryCastTraits_Guid){QueryCastTraitsSample1 obj;

auto base1 = TraitsQueryCast<IQueryCastBase1, IQueryCastBase1 *>(&obj);
TestAssert::IsNotNull(base1);
TestAssert::IsTrue((void *)&obj == (void *)base1);

auto base2 = TraitsQueryCast<IQueryCastBase1, IQueryCastBase1 *>(&obj, __uuidof(IQueryCastBase2));
TestAssert::IsNull(base2);
}

// Use QueryCast method if it is present.
TEST_METHOD(QueryCastTraits_QueryCast) {
  QueryCastTraitsSample2 obj;

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNotNull(base1);

  auto base2 = TraitsQueryCast<IQueryCastBase2>(&obj);
  TestAssert::IsNull(base2);
}

// QueryCast has higher priority over the assigned GUID.
TEST_METHOD(QueryCastTraits_QueryCastOverridesGuid) {
  QueryCastTraitsSample3 obj;

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNotNull(base1);

  auto base2 = TraitsQueryCast<QueryCastTraitsBase3>(&obj);
  TestAssert::IsNull(base2);
}

// Test use of QueryCastChain to query for a base interface.
TEST_METHOD(QueryCastChain_Test) {
  QueryCastChainSample1 obj;
  TestAssert::AreEqual(sizeof(uintptr_t), sizeof(obj), L"There must be only one v-table.");

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNotNull(base1);

  auto derived1 = TraitsQueryCast<IQueryCastDerived1>(&obj);
  TestAssert::IsNotNull(derived1);
}

// Test use of QueryCastChain to query for a base interface.
TEST_METHOD(QueryCastChain_InList) {
  QueryCastChainSample2 obj;
  TestAssert::AreEqual(sizeof(uintptr_t) * 2, sizeof(obj), L"There must be only two v-tables.");

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNotNull(base1);

  auto derived1 = TraitsQueryCast<IQueryCastDerived1>(&obj);
  TestAssert::IsNotNull(derived1);

  auto base2 = TraitsQueryCast<IQueryCastBase2>(&obj);
  TestAssert::IsNotNull(base2);
}

// Test use of QueryCastDerived to query the derived type by its GUID.
TEST_METHOD(QueryCastDerived_Test) {
  QueryCastDerivedSample1 obj;
  TestAssert::AreEqual(sizeof(uintptr_t), sizeof(obj), L"There must be only one v-table.");

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNotNull(base1);

  auto derived = TraitsQueryCast<QueryCastDerivedSample1>(&obj);
  TestAssert::IsNotNull(derived);
}

// Test use of QueryCastGuid to query for a base interface.
TEST_METHOD(QueryCastGuid_Test) {
  QueryCastGuidSample1 obj;
  TestAssert::AreEqual(sizeof(uintptr_t) * 2, sizeof(obj), L"There must be only two v-tables.");

  auto base1 = TraitsQueryCast<IQueryCastNoGuid1>(&obj, MyTestGuid1);
  TestAssert::IsNotNull(base1);

  auto base2 = TraitsQueryCast<IQueryCastBase2>(&obj);
  TestAssert::IsNotNull(base2);
}

// QueryCastHidden hides the base interface from the query cast.
TEST_METHOD(QueryCastHidden_Test) {
  QueryCastHiddenSample1 obj;

  IQueryCastBase1 *intf = &obj;
  TestAssert::IsNotNull(intf);

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNull(base1);
}

// Test use of QueryCastList with one base type.
TEST_METHOD(QueryCastList_OneBaseType) {
  QueryCastListSample1 obj;

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNotNull(base1);
}

// Test use of QueryCastList with two base types.
TEST_METHOD(QueryCastList_TwoBaseTypes) {
  QueryCastListSample2 obj;

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNotNull(base1);

  auto base2 = TraitsQueryCast<IQueryCastBase2>(&obj);
  TestAssert::IsNotNull(base2);
}

// Test use of QueryCastList with three base types.
TEST_METHOD(QueryCastList_ThreeBaseTypes) {
  QueryCastListSample3 obj;

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNotNull(base1);

  auto base2 = TraitsQueryCast<IQueryCastBase2>(&obj);
  TestAssert::IsNotNull(base2);

  auto base3 = TraitsQueryCast<IQueryCastBase3>(&obj);
  TestAssert::IsNotNull(base3);
}

// Test use of QueryCastList with a type that already inherits from the QueryCastList.
TEST_METHOD(QueryCastList_DerivedQueryCastList) {
  QueryCastListDerivedSample1 obj;

  auto base1 = TraitsQueryCast<IQueryCastBase1>(&obj);
  TestAssert::IsNotNull(base1);

  auto base2 = TraitsQueryCast<IQueryCastBase2>(&obj);
  TestAssert::IsNotNull(base2);

  auto base3 = TraitsQueryCast<IQueryCastBase3>(&obj);
  TestAssert::IsNotNull(base3);
}

TEST_METHOD(QueryCastList_StaticCast) {
  StaticCastSample1 obj;

  IQueryCastBase1 *base1 = obj.StaticCastElseNull<IQueryCastBase1 *>();
  TestAssert::IsNotNull(base1);
  TestAssert::AreEqual(12, base1->GetValue1());

  IQueryCastBase2 *base2 = obj.StaticCastElseNull<IQueryCastBase2 *>();
  TestAssert::IsNotNull(base2);
  TestAssert::AreEqual(22, base2->GetValue2());

  IQueryCastBase3 *base3 = obj.StaticCastElseNull<IQueryCastBase3 *>();
  TestAssert::IsNotNull(base3);
  TestAssert::AreEqual(33, base3->GetValue3());
}

TEST_METHOD(QueryCastList_StaticCast2) {
  StaticSample1 sample1;
  StaticSample2 sample2;
  StaticSample3 sample3;
  StaticSample4 sample4;
  StaticSample5 sample5;
  StaticSample6 sample6;
  StaticSample7 sample7;
  StaticSample8 sample8;
  StaticSample9 sample9;
  TestAssert::IsNotNull(sample1.StaticCastElseNull<IUnknown *>(), L"sample1");
  TestAssert::IsNotNull(sample2.StaticCastElseNull<IUnknown *>(), L"sample2");
  TestAssert::IsNull(sample3.StaticCastElseNull<IUnknown *>(), L"sample3");
  TestAssert::IsNull(sample4.StaticCastElseNull<IUnknown *>(), L"sample4");
  TestAssert::IsNotNull(sample5.StaticCastElseNull<IUnknown *>(), L"sample5");
  TestAssert::IsNotNull(sample6.StaticCastElseNull<IUnknown *>(), L"sample6");
  TestAssert::IsNotNull(sample7.StaticCastElseNull<IUnknown *>(), L"sample7");
  TestAssert::IsNotNull(sample8.StaticCastElseNull<IUnknown *>(), L"sample8");
  TestAssert::IsNull(sample9.StaticCastElseNull<IUnknown *>(), L"sample9");
}

TEST_METHOD(QueryCastChain_ForwardCtorArgs) {
  StructWithBase<Mso::QueryCastChain<QueryCastBase1WithArgs, IQueryCastBase1>> testStruct(5, "asdf");
  TestAssert::AreEqual(5, testStruct.m_int);
  TestAssert::AreEqual("asdf", testStruct.m_string.c_str());
}

TEST_METHOD(QueryCastGuid_ForwardCtorArgs) {
  StructWithBase<Mso::QueryCastGuid<QueryCastBase1WithArgs, &MyTestGuid1>> testStruct(5, "asdf");
  TestAssert::AreEqual(5, testStruct.m_int);
  TestAssert::AreEqual("asdf", testStruct.m_string.c_str());
}

TEST_METHOD(QueryCastHidden_ForwardCtorArgs) {
  StructWithBase<Mso::QueryCastHidden<QueryCastBase1WithArgs>> testStruct(5, "asdf");
  TestAssert::AreEqual(5, testStruct.m_int);
  TestAssert::AreEqual("asdf", testStruct.m_string.c_str());
}
}
;
