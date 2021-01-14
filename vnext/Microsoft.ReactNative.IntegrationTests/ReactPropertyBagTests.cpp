// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <ReactPropertyBag.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>
#include <string>

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::Foundation;
using namespace std::string_literals;

namespace ReactNativeIntegrationTests {

TEST_CLASS (ReactPropertyBagTests) {
  TEST_METHOD(StoreNamespace) {
    // Return the same namespace object for the same string.
    auto ns1 = ReactPropertyBagHelper::GetNamespace(L"Foo");
    auto ns2 = ReactPropertyBagHelper::GetNamespace(L"Foo");
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheck(ns1 == ns2);
  }

  TEST_METHOD(WeakNamespace) {
    // Property bag keeps a weak reference to namespaces.
    weak_ref<IReactPropertyNamespace> nsWeak;
    {
      auto ns = ReactPropertyBagHelper::GetNamespace(L"Foo");
      TestCheck(ns);
      nsWeak = ns;
      TestCheck(nsWeak.get());
    }
    TestCheck(!nsWeak.get());
  }

  TEST_METHOD(GlobalNamespace) {
    // Global namespace is the same as the empty string
    auto ns1 = ReactPropertyBagHelper::GlobalNamespace();
    auto ns2 = ReactPropertyBagHelper::GetNamespace(L"");
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheck(ns1 == ns2);
  }

  TEST_METHOD(GlobalNamespaceNull) {
    // Global namespace is the same as null string
    auto ns1 = ReactPropertyBagHelper::GlobalNamespace();
    hstring nullStr{nullptr, take_ownership_from_abi};
    auto ns2 = ReactPropertyBagHelper::GetNamespace(nullStr);
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheck(ns1 == ns2);
  }

  TEST_METHOD(StoreName) {
    // Return the same namespace object for the same string.
    auto ns1 = ReactPropertyBagHelper::GetNamespace(L"Foo");
    auto n11 = ReactPropertyBagHelper::GetName(ns1, L"FooName");
    auto n12 = ReactPropertyBagHelper::GetName(ns1, L"FooName");
    TestCheck(n11);
    TestCheck(n12);
    TestCheck(n11 == n12);
  }

  TEST_METHOD(StoreNameDifferentNamespace) {
    // Return different name objects for the same string in different namespaces.
    auto ns1 = ReactPropertyBagHelper::GetNamespace(L"Foo1");
    auto ns2 = ReactPropertyBagHelper::GetNamespace(L"Foo2");
    auto n11 = ReactPropertyBagHelper::GetName(ns1, L"FooName");
    auto n21 = ReactPropertyBagHelper::GetName(ns2, L"FooName");
    TestCheck(n11);
    TestCheck(n21);
    TestCheck(n11 != n21);
  }

  TEST_METHOD(WeakName) {
    // Property bag keeps a weak reference to namespaces.
    weak_ref<IReactPropertyName> nWeak;
    {
      auto ns = ReactPropertyBagHelper::GetNamespace(L"Foo");
      auto n = ReactPropertyBagHelper::GetName(ns, L"Foo");
      TestCheck(ns);
      TestCheck(n);
      nWeak = n;
      TestCheck(nWeak.get());
    }
    TestCheck(!nWeak.get());
  }

  TEST_METHOD(GlobalNamespaceName) {
    // null namespace is the same as global.
    auto n1 = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    auto n2 = ReactPropertyBagHelper::GetName(ReactPropertyBagHelper::GlobalNamespace(), L"Foo");
    auto n3 = ReactPropertyBagHelper::GetName(ReactPropertyBagHelper::GetNamespace(L""), L"Foo");
    TestCheck(n1 == n2);
    TestCheck(n1 == n3);
  }

  TEST_METHOD(GetProperty_DoesNotExist) {
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    IReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};
    auto value = pb.Get(fooName);
    TestCheck(!value);
  }

  TEST_METHOD(GetProperty_Int) {
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    IReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};
    pb.Set(fooName, box_value(5));
    auto value = pb.Get(fooName);
    TestCheck(value);
    TestCheckEqual(5, unbox_value<int>(value));
  }

  TEST_METHOD(GetOrCreateProperty_Int) {
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    IReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};
    auto value = pb.GetOrCreate(fooName, []() { return box_value(5); });
    TestCheck(value);
    TestCheckEqual(5, unbox_value<int>(value));
  }

  TEST_METHOD(SetProperty_Int) {
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    IReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    auto value1 = pb.Get(fooName);
    TestCheck(!value1);

    pb.Set(fooName, box_value(5));
    auto value2 = pb.Get(fooName);
    TestCheck(value2);
    TestCheckEqual(5, unbox_value<int>(value2));

    pb.Set(fooName, box_value(10));
    auto value3 = pb.Get(fooName);
    TestCheck(value3);
    TestCheckEqual(10, unbox_value<int>(value3));

    pb.Set(fooName, nullptr);
    auto value4 = pb.Get(fooName);
    TestCheck(!value4);
  }

  TEST_METHOD(TwoProperties) {
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    auto barName = ReactPropertyBagHelper::GetName(nullptr, L"Bar");
    IReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    pb.Set(fooName, box_value(5));
    pb.Set(barName, box_value(L"Hello"));

    auto value1 = pb.Get(fooName);
    TestCheck(value1);
    TestCheckEqual(5, unbox_value<int>(value1));

    auto value2 = pb.Get(barName);
    TestCheck(value2);
    TestCheckEqual(L"Hello", unbox_value<hstring>(value2));
  }

  TEST_METHOD(RemoveProperty_Int) {
    auto fooName = ReactPropertyBagHelper::GetName(nullptr, L"Foo");
    IReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    pb.Set(fooName, box_value(5));
    auto value1 = pb.Get(fooName);
    TestCheck(value1);
    TestCheckEqual(5, unbox_value<int>(value1));

    pb.Set(fooName, nullptr);
    auto value2 = pb.Get(fooName);
    TestCheck(!value2);
  }

  TEST_METHOD(PropertyNamespace_ctor_default) {
    ReactPropertyNamespace ns1;
    TestCheck(!ns1);
    TestCheckEqual(L"", ns1.NamespaceName());
    TestCheckEqual(nullptr, ns1.Handle());
  }

  TEST_METHOD(PropertyNamespace_ctor_nullptr) {
    ReactPropertyNamespace ns1{nullptr};
    TestCheck(!ns1);
    TestCheckEqual(L"", ns1.NamespaceName());
    TestCheckEqual(nullptr, ns1.Handle());
  }

  TEST_METHOD(PropertyNamespace_ctor_IReactPropertyNamespace) {
    ReactPropertyNamespace ns1{ReactPropertyBagHelper::GetNamespace(L"Foo")};
    TestCheck(ns1);
    TestCheckEqual(ReactPropertyBagHelper::GetNamespace(L"Foo"), ns1.Handle());
    TestCheckEqual(L"Foo", ns1.Handle().NamespaceName());
    TestCheckEqual(L"Foo", ns1.NamespaceName());
    TestCheck(ReactPropertyBagHelper::GetNamespace(L"Bar") != ns1.Handle());
  }

  TEST_METHOD(PropertyNamespace_ctor_hstring) {
    ReactPropertyNamespace ns1{L"Foo"};
    TestCheck(ns1);
    TestCheckEqual(ReactPropertyBagHelper::GetNamespace(L"Foo"), ns1.Handle());
    TestCheckEqual(L"Foo", ns1.Handle().NamespaceName());
    TestCheckEqual(L"Foo", ns1.NamespaceName());
    TestCheck(ReactPropertyBagHelper::GetNamespace(L"Bar") != ns1.Handle());
  }

  TEST_METHOD(PropertyNamespace_ctor_copy) {
    ReactPropertyNamespace ns1{L"Foo"};
    ReactPropertyNamespace ns2{ns1};
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheckEqual(ReactPropertyBagHelper::GetNamespace(L"Foo"), ns1.Handle());
    TestCheckEqual(ReactPropertyBagHelper::GetNamespace(L"Foo"), ns2.Handle());
  }

  TEST_METHOD(PropertyNamespace_ctor_move) {
    ReactPropertyNamespace ns1{L"Foo"};
    ReactPropertyNamespace ns2{std::move(ns1)};
    TestCheck(!ns1);
    TestCheck(ns2);
    TestCheckEqual(nullptr, ns1.Handle());
    TestCheckEqual(ReactPropertyBagHelper::GetNamespace(L"Foo"), ns2.Handle());
  }

  TEST_METHOD(PropertyNamespace_assign_copy) {
    ReactPropertyNamespace ns1{L"Foo"};
    ReactPropertyNamespace ns2;
    ns2 = ns1;
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheckEqual(ReactPropertyBagHelper::GetNamespace(L"Foo"), ns1.Handle());
    TestCheckEqual(ReactPropertyBagHelper::GetNamespace(L"Foo"), ns2.Handle());
  }

  TEST_METHOD(PropertyNamespace_assign_move) {
    ReactPropertyNamespace ns1{L"Foo"};
    ReactPropertyNamespace ns2;
    ns2 = std::move(ns1);
    TestCheck(!ns1);
    TestCheck(ns2);
    TestCheckEqual(nullptr, ns1.Handle());
    TestCheckEqual(ReactPropertyBagHelper::GetNamespace(L"Foo"), ns2.Handle());
  }

  TEST_METHOD(PropertyNamespace_Global) {
    ReactPropertyNamespace ns1{L""};
    TestCheckEqual(ReactPropertyBagHelper::GlobalNamespace(), ns1.Handle());
    TestCheckEqual(ReactPropertyBagHelper::GlobalNamespace(), ReactPropertyNamespace::Global().Handle());
  }

  TEST_METHOD(PropertyNamespace_Equality) {
    ReactPropertyNamespace ns11{L"Foo"};
    ReactPropertyNamespace ns12{ns11};
    ReactPropertyNamespace ns2{L"Bar"};
    ReactPropertyNamespace ns3;
    TestCheckEqual(ns11, ns12);
    TestCheckEqual(ns12, ns11);
    TestCheck(ns11 != ns2);
    TestCheck(ns2 != ns11);
    TestCheck(ns2 != nullptr);
    TestCheck(nullptr != ns2);
    TestCheck(ns3 == nullptr);
    TestCheck(nullptr == ns3);
  }

  TEST_METHOD(PropertyId_ctor_default) {
    ReactPropertyId<int> name1;
    TestCheck(!name1);
    TestCheckEqual(L"", name1.NamespaceName());
    TestCheckEqual(L"", name1.LocalName());
    TestCheckEqual(nullptr, name1.Handle());
  }

  TEST_METHOD(PropertyId_ctor_nullptr) {
    ReactPropertyId<int> name1{nullptr};
    TestCheck(!name1);
    TestCheckEqual(L"", name1.NamespaceName());
    TestCheckEqual(L"", name1.LocalName());
    TestCheckEqual(nullptr, name1.Handle());
  }

  TEST_METHOD(PropertyId_ctor_IReactPropertyName) {
    ReactPropertyId<int> name1{ReactPropertyBagHelper::GetName(nullptr, L"Foo")};
    TestCheck(name1);
    TestCheckEqual(ReactPropertyBagHelper::GetName(nullptr, L"Foo"), name1.Handle());
    TestCheckEqual(L"", name1.NamespaceName());
    TestCheckEqual(L"Foo", name1.LocalName());
    TestCheckEqual(L"Foo", name1.Handle().LocalName());
    TestCheck(ReactPropertyBagHelper::GetName(nullptr, L"Bar") != name1.Handle());
  }

  TEST_METHOD(PropertyId_ctor_hstring) {
    ReactPropertyId<int> name1{L"Foo"};
    TestCheck(name1);
    TestCheckEqual(ReactPropertyBagHelper::GetName(nullptr, L"Foo"), name1.Handle());
    TestCheckEqual(L"", name1.NamespaceName());
    TestCheckEqual(L"Foo", name1.LocalName());
    TestCheckEqual(L"Foo", name1.Handle().LocalName());
  }

  TEST_METHOD(PropertyId_ctor_ns_hstring) {
    ReactPropertyNamespace ns1{L"Foo"};
    ReactPropertyId<int> name1{ns1, L"Bar"};
    TestCheck(name1);
    TestCheckEqual(
        ReactPropertyBagHelper::GetName(ReactPropertyBagHelper::GetNamespace(L"Foo"), L"Bar"), name1.Handle());
    TestCheckEqual(L"Foo", name1.NamespaceName());
    TestCheckEqual(L"Bar", name1.LocalName());
    TestCheckEqual(L"Bar", name1.Handle().LocalName());
  }

  TEST_METHOD(PropertyId_ctor_hstring_hstring) {
    ReactPropertyId<int> name1{L"Foo", L"Bar"};
    TestCheck(name1);
    TestCheckEqual(
        ReactPropertyBagHelper::GetName(ReactPropertyBagHelper::GetNamespace(L"Foo"), L"Bar"), name1.Handle());
    TestCheckEqual(L"Foo", name1.NamespaceName());
    TestCheckEqual(L"Bar", name1.LocalName());
    TestCheckEqual(L"Bar", name1.Handle().LocalName());
  }

  TEST_METHOD(PropertyId_ctor_copy) {
    ReactPropertyId<int> name1{L"Foo"};
    ReactPropertyId<int> name2{name1};
    TestCheck(name1);
    TestCheck(name2);
    TestCheckEqual(ReactPropertyBagHelper::GetName(nullptr, L"Foo"), name1.Handle());
    TestCheckEqual(ReactPropertyBagHelper::GetName(nullptr, L"Foo"), name2.Handle());
  }

  TEST_METHOD(PropertyId_ctor_move) {
    ReactPropertyId<int> name1{L"Foo"};
    ReactPropertyId<int> name2{std::move(name1)};
    TestCheck(!name1);
    TestCheck(name2);
    TestCheckEqual(nullptr, name1.Handle());
    TestCheckEqual(ReactPropertyBagHelper::GetName(nullptr, L"Foo"), name2.Handle());
  }

  TEST_METHOD(PropertyId_assign_copy) {
    ReactPropertyId<int> name1{L"Foo"};
    ReactPropertyId<int> name2;
    name2 = name1;
    TestCheck(name1);
    TestCheck(name2);
    TestCheckEqual(ReactPropertyBagHelper::GetName(nullptr, L"Foo"), name1.Handle());
    TestCheckEqual(ReactPropertyBagHelper::GetName(nullptr, L"Foo"), name2.Handle());
  }

  TEST_METHOD(PropertyId_assign_move) {
    ReactPropertyId<int> name1{L"Foo"};
    ReactPropertyId<int> name2;
    name2 = std::move(name1);
    TestCheck(!name1);
    TestCheck(name2);
    TestCheckEqual(nullptr, name1.Handle());
    TestCheckEqual(ReactPropertyBagHelper::GetName(nullptr, L"Foo"), name2.Handle());
  }

  TEST_METHOD(PropertyBag_ctor_default) {
    ReactPropertyBag pb1;
    TestCheck(!pb1);
    TestCheckEqual(nullptr, pb1.Handle());
    TestCheck(nullptr == pb1);
    TestCheck(pb1 == nullptr);
  }

  TEST_METHOD(PropertyBag_ctor_nullptr) {
    ReactPropertyBag pb1{nullptr};
    TestCheck(!pb1);
    TestCheckEqual(nullptr, pb1.Handle());
    TestCheck(nullptr == pb1);
    TestCheck(pb1 == nullptr);
  }

  TEST_METHOD(PropertyId_ctor_IReactPropertyBag) {
    IReactPropertyBag ipb1{ReactPropertyBagHelper::CreatePropertyBag()};
    ReactPropertyBag pb1{ipb1};
    TestCheck(pb1);
    TestCheckEqual(ipb1, pb1.Handle());
    TestCheck(pb1.Handle());
    TestCheck(nullptr != pb1);
    TestCheck(pb1 != nullptr);
  }

  TEST_METHOD(PropertyBag_ctor_copy) {
    IReactPropertyBag ipb1{ReactPropertyBagHelper::CreatePropertyBag()};
    ReactPropertyBag pb1{ipb1};
    ReactPropertyBag pb2{pb1};
    TestCheck(pb1);
    TestCheck(pb2);
    TestCheckEqual(ipb1, pb1.Handle());
    TestCheckEqual(ipb1, pb2.Handle());
  }

  TEST_METHOD(PropertyBag_ctor_move) {
    IReactPropertyBag ipb1{ReactPropertyBagHelper::CreatePropertyBag()};
    ReactPropertyBag pb1{ipb1};
    ReactPropertyBag pb2{std::move(pb1)};
    TestCheck(!pb1);
    TestCheck(pb2);
    TestCheckEqual(nullptr, pb1.Handle());
    TestCheckEqual(ipb1, pb2.Handle());
  }

  TEST_METHOD(PropertyBag_assign_copy) {
    IReactPropertyBag ipb1{ReactPropertyBagHelper::CreatePropertyBag()};
    ReactPropertyBag pb1{ipb1};
    ReactPropertyBag pb2;
    pb2 = pb1;
    TestCheck(pb1);
    TestCheck(pb2);
    TestCheckEqual(ipb1, pb1.Handle());
    TestCheckEqual(ipb1, pb2.Handle());
  }

  TEST_METHOD(PropertyBag_assign_move) {
    IReactPropertyBag ipb1{ReactPropertyBagHelper::CreatePropertyBag()};
    ReactPropertyBag pb1{ipb1};
    ReactPropertyBag pb2;
    pb2 = std::move(pb1);
    TestCheck(!pb1);
    TestCheck(pb2);
    TestCheckEqual(nullptr, pb1.Handle());
    TestCheckEqual(ipb1, pb2.Handle());
  }

  TEST_METHOD(PropertyBag_equal) {
    IReactPropertyBag ipb1{ReactPropertyBagHelper::CreatePropertyBag()};
    IReactPropertyBag ipb2{ReactPropertyBagHelper::CreatePropertyBag()};
    ReactPropertyBag pb11{ipb1};
    ReactPropertyBag pb12{ipb1};
    ReactPropertyBag pb21{ipb2};

    TestCheck(pb11 == pb12);
    TestCheck(pb12 == pb11);
    TestCheck(pb11 != pb21);
    TestCheck(pb21 != pb11);
  }

  TEST_METHOD(PropertyBag_Property_int) {
    ReactPropertyId<int> fooName{L"Foo"};
    ReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    TestCheck(!pb.Get(fooName));
    TestCheckEqual(5, *pb.GetOrCreate(fooName, []() { return 5; }));
    TestCheckEqual(5, *pb.Get(fooName));
    pb.Set(fooName, 10);
    TestCheckEqual(10, *pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }

  TEST_METHOD(PropertyBag_Property_optional_int) {
    ReactPropertyId<int> fooName{L"Foo"};
    ReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    TestCheck(!pb.Get(fooName));
    TestCheckEqual(5, *pb.GetOrCreate(fooName, []() { return std::optional<int>{5}; }));
    TestCheckEqual(5, *pb.Get(fooName));
    pb.Set(fooName, std::optional<int>{10});
    TestCheckEqual(10, *pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }

  TEST_METHOD(PropertyBag_Property_enum) {
    // We only support enums defined in IDL. Use underlying type for C++ enums.
    ReactPropertyId<JSValueType> fooName{L"Foo"};
    ReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    TestCheck(!pb.Get(fooName));
    TestCheckEqual(JSValueType::String, *pb.GetOrCreate(fooName, []() { return JSValueType::String; }));
    TestCheckEqual(JSValueType::String, *pb.Get(fooName));
    pb.Set(fooName, JSValueType::Boolean);
    TestCheckEqual(JSValueType::Boolean, *pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }

  TEST_METHOD(PropertyBag_Property_ReactPropertyBag) {
    // Store IInsectable-inherited type
    ReactPropertyId<IReactPropertyBag> fooName{L"Foo"};
    ReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    IReactPropertyBag value1{ReactPropertyBagHelper::CreatePropertyBag()};
    IReactPropertyBag value2{ReactPropertyBagHelper::CreatePropertyBag()};
    TestCheck(!pb.Get(fooName));
    TestCheckEqual(value1, pb.GetOrCreate(fooName, [&value1]() { return value1; }));
    TestCheckEqual(value1, pb.Get(fooName));
    pb.Set(fooName, value2);
    TestCheckEqual(value2, pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }

  TEST_METHOD(PropertyBag_Property_string) {
    ReactPropertyId<hstring> fooName{L"Foo"};
    ReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    TestCheck(!pb.Get(fooName));
    TestCheckEqual(L"Hello", *pb.GetOrCreate(fooName, []() { return L"Hello"; }));
    TestCheckEqual(L"Hello", *pb.Get(fooName));
    pb.Set(fooName, L"World");
    TestCheckEqual(L"World", *pb.Get(fooName));
    TestCheck(L"Hello" != *pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }

  TEST_METHOD(PropertyBag_Property_delegate) {
    ReactPropertyId<ReactCreatePropertyValue> fooName{L"Foo"};
    ReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    TestCheck(!pb.Get(fooName));
    ReactCreatePropertyValue createValue1 = []() { return winrt::box_value(5); };
    TestCheckEqual(createValue1, *pb.GetOrCreate(fooName, [&createValue1]() { return createValue1; }));
    TestCheckEqual(createValue1, *pb.Get(fooName));
    TestCheckEqual(5, winrt::unbox_value<int>((*pb.Get(fooName))()));

    ReactCreatePropertyValue createValue2 = []() { return winrt::box_value(10); };
    pb.Set(fooName, createValue2);
    TestCheckEqual(createValue2, *pb.Get(fooName));
    TestCheckEqual(10, winrt::unbox_value<int>((*pb.Get(fooName))()));
    TestCheck(createValue1 != *pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }

  TEST_METHOD(PropertyBag_Property_Functor) {
    ReactPropertyId<ReactNonAbiValue<Mso::Functor<std::string()>>> fooName{L"Foo"};
    ReactPropertyBag pb{ReactPropertyBagHelper::CreatePropertyBag()};

    TestCheck(!pb.Get(fooName));
    Mso::Functor<std::string()> createValue1 = []() noexcept { return "Hello world!"s; };
    TestCheckEqual(createValue1, *pb.GetOrCreate(fooName, [&createValue1]() { return createValue1; }));
    TestCheckEqual(createValue1, *pb.Get(fooName));
    TestCheckEqual("Hello world!"s, pb.Get(fooName)());

    Mso::Functor<std::string()> createValue2 = []() { return "Hello ReactNative!"s; };
    pb.Set(fooName, createValue2);
    TestCheckEqual(createValue2, *pb.Get(fooName));
    TestCheckEqual("Hello ReactNative!"s, pb.Get(fooName)());
    TestCheck(createValue1 != *pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }
};

} // namespace ReactNativeIntegrationTests
