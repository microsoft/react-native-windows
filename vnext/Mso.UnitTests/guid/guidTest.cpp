// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "guid/msoGuid.h"
#include "motifCpp/testCheck.h"

MSO_STRUCT_GUID(IBaseInterface, "38E23DC7-92B1-4B21-B5FE-6EA786817915")
struct DECLSPEC_NOVTABLE IBaseInterface {
  virtual void BaseMethod() = 0;
};

MSO_STRUCT_GUID(IDerivedInterface, "9E11BC42-1416-45B1-9712-AA4AAD87F78B")
struct DECLSPEC_NOVTABLE IDerivedInterface : public IBaseInterface {
  virtual void DerivedMethod() = 0;
};

MSO_CLASS_GUID(MyClass, "866E3389-0194-4416-94CE-6BE8A5185387")
class MyClass {
  virtual void DerivedMethod() = 0;
};

MSO_STRUCT_GUID(AlreadyDefinedType1, "BAA524C8-F5D1-4CE7-AE24-1FF5EAB306B2")
struct AlreadyDefinedType1 {
  virtual void Method() = 0;
};

MSO_CLASS_GUID(AlreadyDefinedType2, "8C76ECFE-0715-4DA1-9746-2FD4DE39118F")
class AlreadyDefinedType2 {
  virtual void Method() = 0;
};

class TypeWithoutGuid {};

// Associate GUIDs in a namespace
namespace MyAppTest {
namespace MyDetails {

MSO_STRUCT_GUID(IBaseInterface2, "86DE7178-00E6-4A40-8B2C-BA2FBFFB4D06")
struct DECLSPEC_NOVTABLE IBaseInterface2 {
  virtual void BaseMethod() = 0;
};

MSO_STRUCT_GUID(IDerivedInterface2, "C91CAD11-98AB-4218-952F-87509F8F2116")
struct DECLSPEC_NOVTABLE IDerivedInterface2 : public IBaseInterface2 {
  virtual void DerivedMethod() = 0;
};

MSO_CLASS_GUID(MyClass2, "06A68045-D24C-4816-84DC-C18888EC46A9")
class MyClass2 {
  virtual void DerivedMethod() = 0;
};

MSO_STRUCT_GUID(AlreadyDefinedType21, "6B2050DF-6C60-48ED-809D-A88B2476E5A2")
struct AlreadyDefinedType21 {
  virtual void Method() = 0;
};

MSO_CLASS_GUID(AlreadyDefinedType22, "78CEA0B9-2AC5-4FD0-8EE8-C02836BE800D")
class AlreadyDefinedType22 {
  virtual void Method() = 0;
};

class TypeWithoutGuid2 {};

} // namespace MyDetails
} // namespace MyAppTest

// Test that we can pass __uuidof() as a template parameter
template <const IID &clsid>
struct MyGuidHolder {
  const IID &GetGuid() const noexcept {
    UNREFERENCED_PARAMETER(this);
    return clsid;
  }
};

MSO_DEFINE_GUID_TOKEN(MyProductId, "67A42775-79F7-469D-8A28-7348ED9F8D71")

namespace MyAppTest {
namespace MyDetails {

MSO_DEFINE_GUID_TOKEN(MyProductId2, "AABFD353-B463-41A2-B04C-9A6AB7541D20")

}
} // namespace MyAppTest

MSO_DEFINE_GUID_TOKEN(FailingToParseId, "A39D5FC8-0641-4EEE-8C97-DDEF114D487D")

TestClassComponent(GuidTest, Mso.Guid)
    TEST_CLASS (GuidTest){static std::string GuidToString(const GUID &guid){char str[37];
sprintf_s(
    str,
    _countof(str),
    "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
    guid.Data1,
    guid.Data2,
    guid.Data3,
    guid.Data4[0],
    guid.Data4[1],
    guid.Data4[2],
    guid.Data4[3],
    guid.Data4[4],
    guid.Data4[5],
    guid.Data4[6],
    guid.Data4[7]);
return str;
}

TEST_METHOD(TestTypeGuids) {
  TestAssert::AreEqual(std::string("38E23DC7-92B1-4B21-B5FE-6EA786817915"), GuidToString(__uuidof(IBaseInterface)));
  TestAssert::AreEqual(std::string("9E11BC42-1416-45B1-9712-AA4AAD87F78B"), GuidToString(__uuidof(IDerivedInterface)));
  TestAssert::AreEqual(std::string("866E3389-0194-4416-94CE-6BE8A5185387"), GuidToString(__uuidof(MyClass)));
  TestAssert::AreEqual(
      std::string("BAA524C8-F5D1-4CE7-AE24-1FF5EAB306B2"), GuidToString(__uuidof(AlreadyDefinedType1)));
  TestAssert::AreEqual(
      std::string("8C76ECFE-0715-4DA1-9746-2FD4DE39118F"), GuidToString(__uuidof(AlreadyDefinedType2)));
}

TEST_METHOD(TestTypeGuids_InNamespace) {
  TestAssert::AreEqual(
      std::string("86DE7178-00E6-4A40-8B2C-BA2FBFFB4D06"),
      GuidToString(__uuidof(MyAppTest::MyDetails::IBaseInterface2)));
  TestAssert::AreEqual(
      std::string("C91CAD11-98AB-4218-952F-87509F8F2116"),
      GuidToString(__uuidof(MyAppTest::MyDetails::IDerivedInterface2)));
  TestAssert::AreEqual(
      std::string("06A68045-D24C-4816-84DC-C18888EC46A9"), GuidToString(__uuidof(MyAppTest::MyDetails::MyClass2)));
  TestAssert::AreEqual(
      std::string("6B2050DF-6C60-48ED-809D-A88B2476E5A2"),
      GuidToString(__uuidof(MyAppTest::MyDetails::AlreadyDefinedType21)));
  TestAssert::AreEqual(
      std::string("78CEA0B9-2AC5-4FD0-8EE8-C02836BE800D"),
      GuidToString(__uuidof(MyAppTest::MyDetails::AlreadyDefinedType22)));
}

TEST_METHOD(TestTypeGuids_uuidof_AsTemplateParameter) {
  auto h1 = MyGuidHolder<__uuidof(IBaseInterface)>();
  TestAssert::AreEqual(std::string("38E23DC7-92B1-4B21-B5FE-6EA786817915"), GuidToString(h1.GetGuid()));
  auto h2 = MyGuidHolder<__uuidof(MyAppTest::MyDetails::IBaseInterface2)>();
  TestAssert::AreEqual(std::string("86DE7178-00E6-4A40-8B2C-BA2FBFFB4D06"), GuidToString(h2.GetGuid()));
}

TEST_METHOD(TestTypeGuids_TypeHasGuid) {
  TestAssert::IsTrue(Mso::TypeHasGuid<IBaseInterface>::Value);
  TestAssert::IsTrue(Mso::TypeHasGuid<IDerivedInterface>::Value);
  TestAssert::IsTrue(Mso::TypeHasGuid<MyClass>::Value);
  TestAssert::IsTrue(Mso::TypeHasGuid<AlreadyDefinedType1>::Value);
  TestAssert::IsTrue(Mso::TypeHasGuid<AlreadyDefinedType2>::Value);
  TestAssert::IsFalse(Mso::TypeHasGuid<TypeWithoutGuid>::Value);

  TestAssert::IsTrue(Mso::TypeHasGuid<MyAppTest::MyDetails::IBaseInterface2>::Value);
  TestAssert::IsTrue(Mso::TypeHasGuid<MyAppTest::MyDetails::IDerivedInterface2>::Value);
  TestAssert::IsTrue(Mso::TypeHasGuid<MyAppTest::MyDetails::MyClass2>::Value);
  TestAssert::IsTrue(Mso::TypeHasGuid<MyAppTest::MyDetails::AlreadyDefinedType21>::Value);
  TestAssert::IsTrue(Mso::TypeHasGuid<MyAppTest::MyDetails::AlreadyDefinedType22>::Value);
  TestAssert::IsFalse(Mso::TypeHasGuid<MyAppTest::MyDetails::TypeWithoutGuid2>::Value);
}

TEST_METHOD(TestTypeGuids_DefineGuidToken) {
  TestAssert::AreEqual(std::string("67A42775-79F7-469D-8A28-7348ED9F8D71"), GuidToString(__uuidof_token(MyProductId)));
  TestAssert::AreEqual(
      std::string("AABFD353-B463-41A2-B04C-9A6AB7541D20"),
      GuidToString(__uuidof_token(MyAppTest::MyDetails::MyProductId2)));
  TestAssert::AreEqual(
      std::string("A39D5FC8-0641-4EEE-8C97-DDEF114D487D"), GuidToString(__uuidof_token(FailingToParseId)));
}
}
;
