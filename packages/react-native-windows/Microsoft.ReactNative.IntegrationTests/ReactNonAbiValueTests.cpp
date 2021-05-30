// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <ReactNonAbiValue.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.h>
#include <string>

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::Foundation;
using namespace std::string_literals;

namespace ReactNativeIntegrationTests {

TEST_CLASS (ReactNonAbiValueTests) {
  TEST_METHOD(NonAbiValue_ctor_Default) {
    ReactNonAbiValue<std::string> v1;
    TestCheck(!v1);
  }

  TEST_METHOD(NonAbiValue_ctor_EmptyStr) {
    ReactNonAbiValue<std::string> v1{std::in_place};
    TestCheck(v1);
    TestCheckEqual(""s, *v1);
  }

  TEST_METHOD(NonAbiValue_ctor_NonEmptyStr) {
    ReactNonAbiValue<std::string> v1{std::in_place, "Hello"};
    TestCheck(v1);
    TestCheckEqual("Hello"s, *v1);
  }

  TEST_METHOD(NonAbiValue_ctor_Copy) {
    ReactNonAbiValue<std::string> v1{std::in_place, "Hello"};
    ReactNonAbiValue<std::string> v2{v1};
    TestCheck(v1);
    TestCheck(v2);
    TestCheckEqual(v1.GetPtr(), v2.GetPtr());
    TestCheckEqual("Hello"s, *v1);
    TestCheckEqual("Hello"s, *v2);
  }

  TEST_METHOD(NonAbiValue_ctor_Move) {
    ReactNonAbiValue<std::string> v1{std::in_place, "Hello"};
    ReactNonAbiValue<std::string> v2{std::move(v1)};
    TestCheck(!v1);
    TestCheck(v2);
    TestCheckEqual("Hello"s, *v2);
  }

  TEST_METHOD(NonAbiValue_assignment_Copy) {
    ReactNonAbiValue<std::string> v1{std::in_place, "Hello"};
    ReactNonAbiValue<std::string> v2;
    v2 = v1;
    TestCheck(v1);
    TestCheck(v2);
    TestCheckEqual(v1.GetPtr(), v2.GetPtr());
    TestCheckEqual("Hello"s, *v1);
    TestCheckEqual("Hello"s, *v2);
  }

  TEST_METHOD(NonAbiValue_assignment_Move) {
    ReactNonAbiValue<std::string> v1{std::in_place, "Hello"};
    ReactNonAbiValue<std::string> v2;
    v2 = std::move(v1);
    TestCheck(!v1);
    TestCheck(v2);
    TestCheckEqual("Hello"s, *v2);
  }

  TEST_METHOD(NonAbiValue_swap) {
    using std::swap;
    ReactNonAbiValue<std::string> v1{std::in_place, "Hello"};
    ReactNonAbiValue<std::string> v2{std::in_place, "World"};
    swap(v1, v2);
    TestCheckEqual("Hello"s, *v2);
    TestCheckEqual("World"s, *v1);
  }

  TEST_METHOD(NonAbiValue_reset) {
    ReactNonAbiValue<std::string> v1{std::in_place, "Hello"};
    TestCheck(v1);
    v1 = nullptr;
    TestCheck(!v1);
  }

  TEST_METHOD(NonAbiValue_equality) {
    ReactNonAbiValue<std::string> v11{std::in_place, "Hello"};
    ReactNonAbiValue<std::string> v12{v11};
    ReactNonAbiValue<std::string> v2{std::in_place, "World"};
    ReactNonAbiValue<std::string> v3;
    TestCheck(v11 == v12);
    TestCheck(v12 == v11);
    TestCheck(v11 != v2);
    TestCheck(v11 != v3);
    TestCheck(v2 != v11);
    TestCheck(v3 != v11);
    TestCheck(v3 == nullptr);
    TestCheck(nullptr == v3);
    TestCheck(v11 != nullptr);
    TestCheck(nullptr != v11);
  }

  TEST_METHOD(NonAbiValue_operator_arrow) {
    ReactNonAbiValue<std::string> v1{std::in_place, "Hello"};
    TestCheckEqual(5u, v1->length());
  }

  TEST_METHOD(NonAbiValue_operator_call) {
    ReactNonAbiValue<std::function<int(int, int)>> v1{std::in_place, [](int x, int y) { return x + y; }};
    TestCheckEqual(42, v1(37, 5));
  }

  TEST_METHOD(NonAbiValue_moveonly_value) {
    ReactNonAbiValue<std::unique_ptr<int>> v1{std::in_place, new int{42}};
    TestCheck(v1);
    TestCheck(v1->get());
    TestCheckEqual(42, *v1->get());
    std::unique_ptr<int> ptr = std::move(*v1);
    TestCheckEqual(42, *ptr);
    TestCheck(v1);
    TestCheck(!v1->get());
  }

  TEST_METHOD(NonAbiValue_Value) {
    ReactNonAbiValue<std::unique_ptr<int>> v1{std::in_place, new int{42}};
    TestCheck(v1.Value());
    TestCheckEqual(42, *v1.Value());
    std::unique_ptr<int> ptr{std::move(v1.Value())};
    TestCheckEqual(42, *ptr);
    TestCheck(v1);
    TestCheck(!v1.Value());
  }
};

} // namespace ReactNativeIntegrationTests
