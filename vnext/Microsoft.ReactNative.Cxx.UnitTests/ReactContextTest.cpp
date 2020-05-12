// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <ReactContext.h>
#include "ReactModuleBuilderMock.h"

using namespace winrt::Microsoft::ReactNative;

struct ReactContextStub : implements<ReactContextStub, IReactContext> {
  IReactPropertyBag Properties() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  void DispatchEvent(
      Windows::UI::Xaml::FrameworkElement const & /*view*/,
      hstring const & /*eventName*/,
      JSValueArgWriter const & /*eventDataArgWriter*/) noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  void CallJSFunction(
      hstring const & /*moduleName*/,
      hstring const & /*functionName*/,
      JSValueArgWriter const & /*paramsArgWriter*/) noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  void EmitJSEvent(
      hstring const & /*eventEmitterName*/,
      hstring const & /*eventName*/,
      JSValueArgWriter const & /*paramsArgWriter*/) noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }
};

namespace ReactNativeTests {

TEST_CLASS (ReactContextTest) {
  TEST_METHOD(Test_ctor_Default) {
    ReactContext context;
    TestCheck(!context);
  }

  TEST_METHOD(Test_ctor_IReactContext) {
    auto reactContextMock = winrt::make<ReactContextStub>();
    ReactContext context{reactContextMock};
    TestCheck(context);
  }

  TEST_METHOD(Test_ctor_copy) {
    auto reactContextMock = winrt::make<ReactContextStub>();
    ReactContext context1{reactContextMock};
    ReactContext context2{context1};
    TestCheck(context1);
    TestCheck(context2);
    TestCheckEqual(context1, context2);
  }

  TEST_METHOD(Test_ctor_move) {
    auto reactContextMock = winrt::make<ReactContextStub>();
    ReactContext context1{reactContextMock};
    ReactContext context2{std::move(context1)};
    TestCheck(!context1);
    TestCheck(context2);
  }

  TEST_METHOD(Test_assign_nullptr) {
    ReactContext context;
    context = nullptr;
    TestCheck(!context);
  }

  TEST_METHOD(Test_assign_copy) {
    auto reactContextMock = winrt::make<ReactContextStub>();
    ReactContext context1{reactContextMock};
    ReactContext context2;
    context2 = context1;
    TestCheck(context1);
    TestCheck(context2);
    TestCheckEqual(context1, context2);
  }

  TEST_METHOD(Test_assign_move) {
    auto reactContextMock = winrt::make<ReactContextStub>();
    ReactContext context1{reactContextMock};
    ReactContext context2;
    context2 = std::move(context1);
    TestCheck(!context1);
    TestCheck(context2);
  }

  TEST_METHOD(Test_compare) {
    auto reactContextMock1 = winrt::make<ReactContextStub>();
    auto reactContextMock2 = winrt::make<ReactContextStub>();
    ReactContext context11{reactContextMock1};
    ReactContext context12{context11};
    ReactContext context2{reactContextMock2};
    ReactContext context3;
    TestCheck(context11 == context12);
    TestCheck(context12 == context11);
    TestCheck(context11 != context2);
    TestCheck(context11 != context3);
    TestCheck(context2 != context11);
    TestCheck(context3 != context11);
    TestCheck(context3 == nullptr);
    TestCheck(nullptr == context3);
    TestCheck(context11 != nullptr);
    TestCheck(nullptr != context11);
  }
};

} // namespace ReactNativeTests
