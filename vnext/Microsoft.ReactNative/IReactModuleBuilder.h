#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct ReactModuleBuilder : winrt::implements<ReactModuleBuilder, IReactModuleBuilder> {
  ReactModuleBuilder(IReactContext const &reactContext) noexcept;

 public: // IReactModuleBuilder
  void AddInitializer(InitializerDelegate const &initializer) noexcept;
  void AddJsiInitializer(JsiInitializerDelegate const &initializer) noexcept;
  void AddConstantProvider(ConstantProviderDelegate const &constantProvider) noexcept;
  void AddMethod(hstring const &name, MethodReturnType returnType, MethodDelegate const &method) noexcept;
  void AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept;
  void AddEventEmitter(hstring const &name, EventEmitterInitializerDelegate const &emitter);

 private:
  IReactContext m_reactContext;
  std::vector<InitializerDelegate> m_initializers;
  std::vector<JsiInitializerDelegate> m_jsiinitializers;
  std::vector<ConstantProviderDelegate> m_constantProviders;
};

} // namespace winrt::Microsoft::ReactNative
