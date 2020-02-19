#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ABICxxModule.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct ReactModuleBuilder : winrt::implements<ReactModuleBuilder, IReactModuleBuilder> {
  ReactModuleBuilder(IReactContext const &reactContext) noexcept;

 public: // IReactModuleBuilder
  void AddInitializer(InitializerDelegate const &initializer) noexcept;
  void AddConstantProvider(ConstantProviderDelegate const &constantProvider) noexcept;
  void AddMethod(hstring const &name, MethodReturnType returnType, MethodDelegate const &method) noexcept;
  void AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept;

 public:
  std::unique_ptr<facebook::xplat::module::CxxModule> MakeCxxModule(
      std::string const &name,
      IInspectable const &nativeModule) noexcept;

 private:
  static MethodResultCallback MakeMethodResultCallback(
      facebook::xplat::module::CxxModule::Callback &&callback) noexcept;

 private:
  IReactContext m_reactContext;
  std::vector<InitializerDelegate> m_initializers;
  std::vector<ConstantProviderDelegate> m_constantProviders;
  std::vector<facebook::xplat::module::CxxModule::Method> m_methods;
};

} // namespace winrt::Microsoft::ReactNative
