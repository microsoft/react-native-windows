// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicReader.h"
#include "DynamicWriter.h"
#include "IReactModuleBuilder.h"
#include "TestController.h"

#include "Microsoft.Internal.TestController.g.cpp"

using namespace winrt;

namespace {

// Extends the 'DynamicReader' class with the ability the manage the lifetime of the folly::dynamic value it's reading
// from.
struct DynamicWrapperReader : public winrt::implements<DynamicWrapperReader, Microsoft::ReactNative::IJSValueReader> {
  DynamicWrapperReader(folly::dynamic &&value) : m_value{std::move(value)} {
    m_innerReader = make<Microsoft::ReactNative::DynamicReader>(m_value);
  }

  Microsoft::ReactNative::JSValueType ValueType() noexcept {
    return m_innerReader.ValueType();
  }

  bool GetNextObjectProperty(hstring &propertyName) noexcept {
    return m_innerReader.GetNextObjectProperty(propertyName);
  }

  bool GetNextArrayItem() noexcept {
    return m_innerReader.GetNextArrayItem();
  }

  hstring GetString() noexcept {
    return m_innerReader.GetString();
  }

  bool GetBoolean() noexcept {
    return m_innerReader.GetBoolean();
  }

  int64_t GetInt64() noexcept {
    return m_innerReader.GetInt64();
  }

  double GetDouble() noexcept {
    return m_innerReader.GetDouble();
  }

 private:
  Microsoft::ReactNative::IJSValueReader m_innerReader;
  folly::dynamic m_value;
};

struct ReactModuleBuilderWrapper
    : public winrt::implements<ReactModuleBuilderWrapper, Microsoft::ReactNative::IReactModuleBuilder> {
  ReactModuleBuilderWrapper(Microsoft::ReactNative::IReactContext context) : m_context{context} {
    m_innerBuilder = make<Microsoft::ReactNative::ReactModuleBuilder>(m_context);
  }
  void AddInitializer(Microsoft::ReactNative::InitializerDelegate const &initializer) noexcept {
    m_innerBuilder.AddInitializer(initializer);
  }
  void AddConstantProvider(Microsoft::ReactNative::ConstantProviderDelegate const &constantProvider) noexcept {
    m_innerBuilder.AddConstantProvider(constantProvider);
  }
  void AddMethod(
      hstring const &name,
      Microsoft::ReactNative::MethodReturnType returnType,
      Microsoft::ReactNative::MethodDelegate const &method) noexcept {
    m_innerBuilder.AddMethod(name, returnType, method);
  }
  void AddSyncMethod(hstring const &name, Microsoft::ReactNative::SyncMethodDelegate const &method) noexcept {
    m_innerBuilder.AddSyncMethod(name, method);
  }

 private:
  Microsoft::ReactNative::IReactModuleBuilder m_innerBuilder;
  Microsoft::ReactNative::IReactContext m_context;
};

struct TestContext : public winrt::implements<TestContext, Microsoft::ReactNative::IReactContext> {
  Microsoft::ReactNative::IReactPropertyBag Properties() noexcept {
    return nullptr;
  }
  Microsoft::ReactNative::IReactNotificationService Notifications() noexcept {
    return nullptr;
  }
  Microsoft::ReactNative::IReactDispatcher UIDispatcher() noexcept {
    return nullptr;
  }
  Microsoft::ReactNative::IReactDispatcher JSDispatcher() noexcept {
    return nullptr;
  }
  void CallJSFunction(
      hstring const &moduleName,
      hstring const &methodName,
      Microsoft::ReactNative::JSValueArgWriter const &paramsArgWriter) noexcept {}
  void EmitJSEvent(
      hstring const &eventEmitterName,
      hstring const &eventName,
      Microsoft::ReactNative::JSValueArgWriter const &paramsArgWriter) noexcept {}
};

} // namespace

namespace winrt::Microsoft::Internal::implementation {

Microsoft::ReactNative::IJSValueReader TestController::CreateDynamicReader(
    Microsoft::ReactNative::IJSValueWriter writer) {
  auto dw = writer.as<Microsoft::ReactNative::DynamicWriter>();
  return make<DynamicWrapperReader>(dw->TakeValue());
}

Microsoft::ReactNative::IJSValueWriter TestController::CreateDynamicWriter() {
  return make<winrt::Microsoft::ReactNative::DynamicWriter>();
}

Microsoft::ReactNative::IReactContext TestController::CreateTestContext() {
  return make<TestContext>();
}
Microsoft::ReactNative::IReactModuleBuilder TestController::CreateReactModuleBuilder(
    Microsoft::ReactNative::IReactContext context) {
  return make<ReactModuleBuilderWrapper>(context);
}

} // namespace winrt::Microsoft::Internal::implementation
