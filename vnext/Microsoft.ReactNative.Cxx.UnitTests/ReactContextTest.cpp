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

  IReactDispatcher UIDispatcher() noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  void DispatchEvent(
      Windows::UI::Xaml::FrameworkElement const & /*view*/,
      hstring const & /*eventName*/,
      JSValueArgWriter const & /*eventDataArgWriter*/) noexcept {
    VerifyElseCrashSz(false, "Not implemented");
  }

  void CallJSFunction(
      hstring const &moduleName,
      hstring const &functionName,
      JSValueArgWriter const &paramsArgWriter) noexcept {
    Module = moduleName;
    Method = functionName;
    auto writer = MakeJSValueTreeWriter();
    paramsArgWriter(writer);
    Args = TakeJSValue(writer);
  }

  void EmitJSEvent(
      hstring const &eventEmitterName,
      hstring const &eventName,
      JSValueArgWriter const &paramsArgWriter) noexcept {
    Module = eventEmitterName;
    Method = eventName;
    auto writer = MakeJSValueTreeWriter();
    paramsArgWriter(writer);
    Args = TakeJSValue(writer);
  }

  std::wstring Module;
  std::wstring Method;
  JSValue Args;
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

  TEST_METHOD(Test_CallJSFunction) {
    auto reactContextMock = winrt::make_self<ReactContextStub>();
    ReactContext context{reactContextMock.as<IReactContext>()};
    context.CallJSFunction(L"module1", L"method1");
    TestCheckEqual(L"module1", reactContextMock->Module);
    TestCheckEqual(L"method1", reactContextMock->Method);
    TestCheckEqual(0u, reactContextMock->Args.AsArray().size());

    context.CallJSFunction(L"module1", L"method1", 5);
    TestCheckEqual(1u, reactContextMock->Args.AsArray().size());
    TestCheckEqual(5, reactContextMock->Args[0]);

    context.CallJSFunction(L"module1", L"method1", 14, 17u);
    TestCheckEqual(2u, reactContextMock->Args.AsArray().size());
    TestCheckEqual(14, reactContextMock->Args[0]);
    TestCheckEqual(17u, reactContextMock->Args[1]);

    context.CallJSFunction(L"module1", L"method1", JSValueArray{12, 18u});
    TestCheckEqual(12, reactContextMock->Args[0][0]);
    TestCheckEqual(18u, reactContextMock->Args[0][1]);

    context.CallJSFunction(L"module1", L"method1", JSValueObject{{"prop1", 42}, {"prop2", 77u}});
    TestCheckEqual(42, reactContextMock->Args[0]["prop1"]);
    TestCheckEqual(77u, reactContextMock->Args[0]["prop2"]);

    context.CallJSFunction(L"module1", L"method1", [](IJSValueWriter const &writer) {
      writer.WriteArrayBegin();
      WriteValue(writer, 10u);
      WriteValue(writer, 19);
      writer.WriteArrayEnd();
    });
    TestCheckEqual(10u, reactContextMock->Args[0]);
    TestCheckEqual(19, reactContextMock->Args[1]);
  }

  TEST_METHOD(Test_EmitJSEvent) {
    auto reactContextMock = winrt::make_self<ReactContextStub>();
    ReactContext context{reactContextMock.as<IReactContext>()};
    context.EmitJSEvent(L"module1", L"event1");
    TestCheckEqual(L"module1", reactContextMock->Module);
    TestCheckEqual(L"event1", reactContextMock->Method);
    TestCheckEqual(0u, reactContextMock->Args.AsArray().size());

    context.EmitJSEvent(L"module1", L"event1", 5);
    TestCheckEqual(1u, reactContextMock->Args.AsArray().size());
    TestCheckEqual(5, reactContextMock->Args[0]);

    context.EmitJSEvent(L"module1", L"event1", 14, 17u);
    TestCheckEqual(2u, reactContextMock->Args.AsArray().size());
    TestCheckEqual(14, reactContextMock->Args[0]);
    TestCheckEqual(17u, reactContextMock->Args[1]);

    context.EmitJSEvent(L"module1", L"event1", JSValueArray{12, 18u});
    TestCheckEqual(12, reactContextMock->Args[0][0]);
    TestCheckEqual(18u, reactContextMock->Args[0][1]);

    context.EmitJSEvent(L"module1", L"event1", JSValueObject{{"prop1", 42}, {"prop2", 77u}});
    TestCheckEqual(42, reactContextMock->Args[0]["prop1"]);
    TestCheckEqual(77u, reactContextMock->Args[0]["prop2"]);

    context.EmitJSEvent(L"module1", L"event1", [](IJSValueWriter const &writer) {
      writer.WriteArrayBegin();
      WriteValue(writer, 10u);
      WriteValue(writer, 19);
      writer.WriteArrayEnd();
    });
    TestCheckEqual(10u, reactContextMock->Args[0]);
    TestCheckEqual(19, reactContextMock->Args[1]);
  }
};

} // namespace ReactNativeTests
