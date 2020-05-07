#include "pch.h"

#include <PropertyBag.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

TEST_CLASS (PropertyBagTests) {
  TEST_METHOD(StoreNamespace) {
    // Return the same namespace object for the same string.
    auto ns1 = ReactPropertyBag::GetNamespace(L"Foo");
    auto ns2 = ReactPropertyBag::GetNamespace(L"Foo");
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheck(ns1 == ns2);
  }

  TEST_METHOD(WeakNamespace) {
    // Property bag keeps a weak reference to namespaces.
    weak_ref<IReactPropertyNamespace> nsWeak;
    {
      auto ns = ReactPropertyBag::GetNamespace(L"Foo");
      TestCheck(ns);
      nsWeak = ns;
      TestCheck(nsWeak.get());
    }
    TestCheck(!nsWeak.get());
  }

  TEST_METHOD(GlobalNamespace) {
    // Global namespace is the same as the empty string
    auto ns1 = ReactPropertyBag::GlobalNamespace();
    auto ns2 = ReactPropertyBag::GetNamespace(L"");
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheck(ns1 == ns2);
  }

  TEST_METHOD(GlobalNamespaceNull) {
    // Global namespace is the same as null string
    auto ns1 = ReactPropertyBag::GlobalNamespace();
    hstring nullStr{nullptr, take_ownership_from_abi};
    auto ns2 = ReactPropertyBag::GetNamespace(nullStr);
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheck(ns1 == ns2);
  }

  TEST_METHOD(WeakGlobalNamespace) {
    // Property bag keeps a weak reference to the Global namespace.
    weak_ref<IReactPropertyNamespace> globalWeak;
    {
      auto global = ReactPropertyBag::GlobalNamespace();
      TestCheck(global);
      globalWeak = global;
      TestCheck(globalWeak.get());
    }
    TestCheck(!globalWeak.get());
  }

  TEST_METHOD(StoreName) {
    // Return the same namespace object for the same string.
    auto ns1 = ReactPropertyBag::GetNamespace(L"Foo");
    auto n11 = ReactPropertyBag::GetName(ns1, L"FooName");
    auto n12 = ReactPropertyBag::GetName(ns1, L"FooName");
    TestCheck(n11);
    TestCheck(n12);
    TestCheck(n11 == n12);
  }

  TEST_METHOD(StoreNameDifferentNamespace) {
    // Return different name objects for the same string in different namespaces.
    auto ns1 = ReactPropertyBag::GetNamespace(L"Foo1");
    auto ns2 = ReactPropertyBag::GetNamespace(L"Foo2");
    auto n11 = ReactPropertyBag::GetName(ns1, L"FooName");
    auto n21 = ReactPropertyBag::GetName(ns2, L"FooName");
    TestCheck(n11);
    TestCheck(n21);
    TestCheck(n11 != n21);
  }

  TEST_METHOD(WeakName) {
    // Property bag keeps a weak reference to namespaces.
    weak_ref<IReactPropertyName> nWeak;
    {
      auto ns = ReactPropertyBag::GetNamespace(L"Foo");
      auto n = ReactPropertyBag::GetName(ns, L"Foo");
      TestCheck(ns);
      TestCheck(n);
      nWeak = n;
      TestCheck(nWeak.get());
    }
    TestCheck(!nWeak.get());
  }

  TEST_METHOD(GlobalNamespaceName) {
    // null namespace is the same as global.
    auto n1 = ReactPropertyBag::GetName(nullptr, L"Foo");
    auto n2 = ReactPropertyBag::GetName(ReactPropertyBag::GlobalNamespace(), L"Foo");
    auto n3 = ReactPropertyBag::GetName(ReactPropertyBag::GetNamespace(L""), L"Foo");
    TestCheck(n1 == n2);
    TestCheck(n1 == n3);
  }

  TEST_METHOD(GetProperty_DoesNotExist) {
    auto fooName = ReactPropertyBag::GetName(nullptr, L"Foo");
    ReactPropertyBag pb;
    auto value = pb.GetProperty(fooName);
    TestCheck(!value);
  }

  TEST_METHOD(GetProperty_Int) {
    auto fooName = ReactPropertyBag::GetName(nullptr, L"Foo");
    ReactPropertyBag pb;
    pb.SetProperty(fooName, box_value(5));
    auto value = pb.GetProperty(fooName);
    TestCheck(value);
    TestCheckEqual(5, unbox_value<int>(value));
  }

  TEST_METHOD(GetOrCreateProperty_Int) {
    auto fooName = ReactPropertyBag::GetName(nullptr, L"Foo");
    ReactPropertyBag pb;
    auto value = pb.GetOrCreateProperty(fooName, []() { return box_value(5); });
    TestCheck(value);
    TestCheckEqual(5, unbox_value<int>(value));
  }

  TEST_METHOD(SetProperty_Int) {
    auto fooName = ReactPropertyBag::GetName(nullptr, L"Foo");
    ReactPropertyBag pb;

    auto value1 = pb.GetProperty(fooName);
    TestCheck(!value1);

    pb.SetProperty(fooName, box_value(5));
    auto value2 = pb.GetProperty(fooName);
    TestCheck(value2);
    TestCheckEqual(5, unbox_value<int>(value2));

    pb.SetProperty(fooName, box_value(10));
    auto value3 = pb.GetProperty(fooName);
    TestCheck(value3);
    TestCheckEqual(10, unbox_value<int>(value3));

    pb.SetProperty(fooName, nullptr);
    auto value4 = pb.GetProperty(fooName);
    TestCheck(!value4);
  }

  TEST_METHOD(TwoProperties) {
    auto fooName = ReactPropertyBag::GetName(nullptr, L"Foo");
    auto barName = ReactPropertyBag::GetName(nullptr, L"Bar");
    ReactPropertyBag pb;

    pb.SetProperty(fooName, box_value(5));
    pb.SetProperty(barName, box_value(L"Hello"));

    auto value1 = pb.GetProperty(fooName);
    TestCheck(value1);
    TestCheckEqual(5, unbox_value<int>(value1));

    auto value2 = pb.GetProperty(barName);
    TestCheck(value2);
    TestCheckEqual(L"Hello", unbox_value<hstring>(value2));
  }

  TEST_METHOD(RemoveProperty_Int) {
    auto fooName = ReactPropertyBag::GetName(nullptr, L"Foo");
    ReactPropertyBag pb;

    pb.SetProperty(fooName, box_value(5));
    auto value1 = pb.GetProperty(fooName);
    TestCheck(value1);
    TestCheckEqual(5, unbox_value<int>(value1));

    pb.RemoveProperty(fooName);
    auto value2 = pb.GetProperty(fooName);
    TestCheck(!value2);
  }

  TEST_METHOD(PropertyNamespace_ctor_default) {
    PropertyNamespace ns1;
    TestCheck(!ns1);
    TestCheckEqual(L"", ns1.NamespaceName());
    TestCheckEqual(nullptr, ns1.Get());
  }

  TEST_METHOD(PropertyNamespace_ctor_nullptr) {
    PropertyNamespace ns1{nullptr};
    TestCheck(!ns1);
    TestCheckEqual(L"", ns1.NamespaceName());
    TestCheckEqual(nullptr, ns1.Get());
  }

  TEST_METHOD(PropertyNamespace_ctor_IReactPropertyNamespace) {
    PropertyNamespace ns1{ReactPropertyBag::GetNamespace(L"Foo")};
    TestCheck(ns1);
    TestCheckEqual(ReactPropertyBag::GetNamespace(L"Foo"), ns1.Get());
    TestCheckEqual(L"Foo", ns1.Get().NamespaceName());
    TestCheckEqual(L"Foo", ns1.NamespaceName());
    TestCheck(ReactPropertyBag::GetNamespace(L"Bar") != ns1.Get());
  }

  TEST_METHOD(PropertyNamespace_ctor_hstring) {
    PropertyNamespace ns1{L"Foo"};
    TestCheck(ns1);
    TestCheckEqual(ReactPropertyBag::GetNamespace(L"Foo"), ns1.Get());
    TestCheckEqual(L"Foo", ns1.Get().NamespaceName());
    TestCheckEqual(L"Foo", ns1.NamespaceName());
    TestCheck(ReactPropertyBag::GetNamespace(L"Bar") != ns1.Get());
  }

  TEST_METHOD(PropertyNamespace_ctor_copy) {
    PropertyNamespace ns1{L"Foo"};
    PropertyNamespace ns2{ns1};
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheckEqual(ReactPropertyBag::GetNamespace(L"Foo"), ns1.Get());
    TestCheckEqual(ReactPropertyBag::GetNamespace(L"Foo"), ns2.Get());
  }

  TEST_METHOD(PropertyNamespace_ctor_move) {
    PropertyNamespace ns1{L"Foo"};
    PropertyNamespace ns2{std::move(ns1)};
    TestCheck(!ns1);
    TestCheck(ns2);
    TestCheckEqual(nullptr, ns1.Get());
    TestCheckEqual(ReactPropertyBag::GetNamespace(L"Foo"), ns2.Get());
  }

  TEST_METHOD(PropertyNamespace_assign_copy) {
    PropertyNamespace ns1{L"Foo"};
    PropertyNamespace ns2;
    ns2 = ns1;
    TestCheck(ns1);
    TestCheck(ns2);
    TestCheckEqual(ReactPropertyBag::GetNamespace(L"Foo"), ns1.Get());
    TestCheckEqual(ReactPropertyBag::GetNamespace(L"Foo"), ns2.Get());
  }

  TEST_METHOD(PropertyNamespace_assign_move) {
    PropertyNamespace ns1{L"Foo"};
    PropertyNamespace ns2;
    ns2 = std::move(ns1);
    TestCheck(!ns1);
    TestCheck(ns2);
    TestCheckEqual(nullptr, ns1.Get());
    TestCheckEqual(ReactPropertyBag::GetNamespace(L"Foo"), ns2.Get());
  }

  TEST_METHOD(PropertyNamespace_Global) {
    PropertyNamespace ns1{L""};
    TestCheckEqual(ReactPropertyBag::GlobalNamespace(), ns1.Get());
    TestCheckEqual(ReactPropertyBag::GlobalNamespace(), PropertyNamespace::Global().Get());
  }

  TEST_METHOD(PropertyName_ctor_default) {
    PropertyName<int> name1;
    TestCheck(!name1);
    TestCheckEqual(L"", name1.NamespaceName());
    TestCheckEqual(L"", name1.LocalName());
    TestCheckEqual(nullptr, name1.Get());
  }

  TEST_METHOD(PropertyName_ctor_nullptr) {
    PropertyName<int> name1{nullptr};
    TestCheck(!name1);
    TestCheckEqual(L"", name1.NamespaceName());
    TestCheckEqual(L"", name1.LocalName());
    TestCheckEqual(nullptr, name1.Get());
  }

  TEST_METHOD(PropertyName_ctor_IReactPropertyName) {
    PropertyName<int> name1{ReactPropertyBag::GetName(nullptr, L"Foo")};
    TestCheck(name1);
    TestCheckEqual(ReactPropertyBag::GetName(nullptr, L"Foo"), name1.Get());
    TestCheckEqual(L"", name1.NamespaceName());
    TestCheckEqual(L"Foo", name1.LocalName());
    TestCheckEqual(L"Foo", name1.Get().LocalName());
    TestCheck(ReactPropertyBag::GetName(nullptr, L"Bar") != name1.Get());
  }

  TEST_METHOD(PropertyName_ctor_hstring) {
    PropertyName<int> name1{L"Foo"};
    TestCheck(name1);
    TestCheckEqual(ReactPropertyBag::GetName(nullptr, L"Foo"), name1.Get());
    TestCheckEqual(L"", name1.NamespaceName());
    TestCheckEqual(L"Foo", name1.LocalName());
    TestCheckEqual(L"Foo", name1.Get().LocalName());
  }

  TEST_METHOD(PropertyName_ctor_ns_hstring) {
    PropertyNamespace ns1{L"Foo"};
    PropertyName<int> name1{ns1, L"Bar"};
    TestCheck(name1);
    TestCheckEqual(ReactPropertyBag::GetName(ReactPropertyBag::GetNamespace(L"Foo"), L"Bar"), name1.Get());
    TestCheckEqual(L"Foo", name1.NamespaceName());
    TestCheckEqual(L"Bar", name1.LocalName());
    TestCheckEqual(L"Bar", name1.Get().LocalName());
  }

  TEST_METHOD(PropertyName_ctor_hstring_hstring) {
    PropertyName<int> name1{L"Foo", L"Bar"};
    TestCheck(name1);
    TestCheckEqual(ReactPropertyBag::GetName(ReactPropertyBag::GetNamespace(L"Foo"), L"Bar"), name1.Get());
    TestCheckEqual(L"Foo", name1.NamespaceName());
    TestCheckEqual(L"Bar", name1.LocalName());
    TestCheckEqual(L"Bar", name1.Get().LocalName());
  }

  TEST_METHOD(PropertyName_ctor_copy) {
    PropertyName<int> name1{L"Foo"};
    PropertyName<int> name2{name1};
    TestCheck(name1);
    TestCheck(name2);
    TestCheckEqual(ReactPropertyBag::GetName(nullptr, L"Foo"), name1.Get());
    TestCheckEqual(ReactPropertyBag::GetName(nullptr, L"Foo"), name2.Get());
  }

  TEST_METHOD(PropertyName_ctor_move) {
    PropertyName<int> name1{L"Foo"};
    PropertyName<int> name2{std::move(name1)};
    TestCheck(!name1);
    TestCheck(name2);
    TestCheckEqual(nullptr, name1.Get());
    TestCheckEqual(ReactPropertyBag::GetName(nullptr, L"Foo"), name2.Get());
  }

  TEST_METHOD(PropertyName_assign_copy) {
    PropertyName<int> name1{L"Foo"};
    PropertyName<int> name2;
    name2 = name1;
    TestCheck(name1);
    TestCheck(name2);
    TestCheckEqual(ReactPropertyBag::GetName(nullptr, L"Foo"), name1.Get());
    TestCheckEqual(ReactPropertyBag::GetName(nullptr, L"Foo"), name2.Get());
  }

  TEST_METHOD(PropertyName_assign_move) {
    PropertyName<int> name1{L"Foo"};
    PropertyName<int> name2;
    name2 = std::move(name1);
    TestCheck(!name1);
    TestCheck(name2);
    TestCheckEqual(nullptr, name1.Get());
    TestCheckEqual(ReactPropertyBag::GetName(nullptr, L"Foo"), name2.Get());
  }

  TEST_METHOD(PropertyBag_ctor_default) {
    PropertyBag pb1;
    TestCheck(!pb1);
    TestCheckEqual(nullptr, pb1.Get());
    TestCheck(nullptr == pb1);
    TestCheck(pb1 == nullptr);
  }

  TEST_METHOD(PropertyBag_ctor_nullptr) {
    PropertyBag pb1{nullptr};
    TestCheck(!pb1);
    TestCheckEqual(nullptr, pb1.Get());
    TestCheck(nullptr == pb1);
    TestCheck(pb1 == nullptr);
  }

  TEST_METHOD(PropertyName_ctor_IReactPropertyBag) {
    ReactPropertyBag rpg1;
    PropertyBag pb1{rpg1};
    TestCheck(pb1);
    TestCheckEqual(rpg1, pb1.Get());
    TestCheck(pb1.Get());
    TestCheck(nullptr != pb1);
    TestCheck(pb1 != nullptr);
  }

  TEST_METHOD(PropertyBag_ctor_copy) {
    ReactPropertyBag rpg1;
    PropertyBag pb1{rpg1};
    PropertyBag pb2{pb1};
    TestCheck(pb1);
    TestCheck(pb2);
    TestCheckEqual(rpg1, pb1.Get());
    TestCheckEqual(rpg1, pb2.Get());
  }

  TEST_METHOD(PropertyBag_ctor_move) {
    ReactPropertyBag rpg1;
    PropertyBag pb1{rpg1};
    PropertyBag pb2{std::move(pb1)};
    TestCheck(!pb1);
    TestCheck(pb2);
    TestCheckEqual(nullptr, pb1.Get());
    TestCheckEqual(rpg1, pb2.Get());
  }

  TEST_METHOD(PropertyBag_assign_copy) {
    ReactPropertyBag rpg1;
    PropertyBag pb1{rpg1};
    PropertyBag pb2;
    pb2 = pb1;
    TestCheck(pb1);
    TestCheck(pb2);
    TestCheckEqual(rpg1, pb1.Get());
    TestCheckEqual(rpg1, pb2.Get());
  }

  TEST_METHOD(PropertyBag_assign_move) {
    ReactPropertyBag rpg1;
    PropertyBag pb1{rpg1};
    PropertyBag pb2;
    pb2 = std::move(pb1);
    TestCheck(!pb1);
    TestCheck(pb2);
    TestCheckEqual(nullptr, pb1.Get());
    TestCheckEqual(rpg1, pb2.Get());
  }

  TEST_METHOD(PropertyBag_equal) {
    ReactPropertyBag rpg1;
    ReactPropertyBag rpg2;
    PropertyBag pb11{rpg1};
    PropertyBag pb12{rpg1};
    PropertyBag pb21{rpg2};

    TestCheck(pb11 == pb12);
    TestCheck(pb12 == pb11);
    TestCheck(pb11 != pb21);
    TestCheck(pb21 != pb11);
  }

  TEST_METHOD(PropertyBag_Property_int) {
    PropertyName<int> fooName{L"Foo"};
    PropertyBag pb{ReactPropertyBag{}};

    TestCheck(!pb.Get(fooName));
    TestCheckEqual(5, *pb.GetOrCreate(fooName, []() { return 5; }));
    TestCheckEqual(5, *pb.Get(fooName));
    pb.Set(fooName, 10);
    TestCheckEqual(10, *pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }

  TEST_METHOD(PropertyBag_Property_optional_int) {
    PropertyName<int> fooName{L"Foo"};
    PropertyBag pb{ReactPropertyBag{}};

    TestCheck(!pb.Get(fooName));
    TestCheckEqual(5, *pb.GetOrCreate(fooName, []() { return std::optional<int>{5}; }));
    TestCheckEqual(5, *pb.Get(fooName));
    pb.Set(fooName, std::optional<int>{10});
    TestCheckEqual(10, *pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }

  TEST_METHOD(PropertyBag_Property_enum) {
    // We only support enums defined in IDL.
    PropertyName<JSValueType> fooName{L"Foo"};
    PropertyBag pb{ReactPropertyBag{}};

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
    PropertyName<ReactPropertyBag> fooName{L"Foo"};
    PropertyBag pb{ReactPropertyBag{}};

    ReactPropertyBag value1;
    ReactPropertyBag value2;
    TestCheck(!pb.Get(fooName));
    TestCheckEqual(value1, pb.GetOrCreate(fooName, [&value1]() { return value1; }));
    TestCheckEqual(value1, pb.Get(fooName));
    pb.Set(fooName, value2);
    TestCheckEqual(value2, pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }

  TEST_METHOD(PropertyBag_Property_string) {
    // We only support enums defined in IDL.
    PropertyName<hstring> fooName{L"Foo"};
    PropertyBag pb{ReactPropertyBag{}};

    TestCheck(!pb.Get(fooName));
    TestCheckEqual(L"Hello", *pb.GetOrCreate(fooName, []() { return L"Hello"; }));
    TestCheckEqual(L"Hello", *pb.Get(fooName));
    pb.Set(fooName, L"World");
    TestCheckEqual(L"World", *pb.Get(fooName));
    TestCheck(L"Hello" != *pb.Get(fooName));
    pb.Remove(fooName);
    TestCheck(!pb.Get(fooName));
  }
};

} // namespace ReactNativeIntegrationTests
