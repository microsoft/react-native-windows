#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ABICxxModule.h"
#include "cxxreact/CxxModule.h"
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {

struct ReactModuleBuilder
    : winrt::implements<ReactModuleBuilder, IReactModuleBuilder> {
  ReactModuleBuilder() noexcept;

 public: // IReactModuleBuilder
  void SetEventEmitterName(hstring const &name) noexcept;
  void AddMethod(
      hstring const &name,
      MethodReturnType returnType,
      MethodDelegate const &method) noexcept;
  void AddSyncMethod(
      hstring const &name,
      SyncMethodDelegate const &method) noexcept;
  void AddConstantProvider(ConstantProvider const &constantProvider) noexcept;
  void AddEventSetter(
      hstring const &name,
      EventSetter const &eventSetter) noexcept;

 public:
  std::unique_ptr<facebook::xplat::module::CxxModule> MakeCxxModule(
      std::string const &name,
      IInspectable &nativeModule) noexcept;

 private:
  static MethodResultCallback MakeMethodResultCallback(
      facebook::xplat::module::CxxModule::Callback callback) noexcept;

 private:
  std::string m_eventEmitterName;
  std::vector<facebook::xplat::module::CxxModule::Method> m_methods;
  std::vector<ConstantProvider> m_constants;
  std::vector<ABICxxModuleEventSetter> m_eventSetters;
};

} // namespace winrt::Microsoft::ReactNative::Bridge
