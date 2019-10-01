#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "cxxreact/CxxModule.h"
#include "ABICxxModule.h"
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {

struct NativeModuleBuilder
    : winrt::implements<NativeModuleBuilder, INativeModuleBuilder> {
  NativeModuleBuilder() noexcept;

 public: // INativeModuleBuilder
  void SetName(hstring const &name) noexcept;
  void SetEventEmitterName(hstring const &name) noexcept;
  void AddMethod(
      hstring const &name,
      MethodReturnType returnType,
      MethodDelegate method) noexcept;
  void AddSyncMethod(hstring const &name, SyncMethodDelegate method) noexcept;
  void AddConstantWriter(ConstantWriterDelegate constantWriter) noexcept;
  void AddEventRegister(hstring const &name, EventSetter eventSetter) noexcept;

 public:
  std::unique_ptr<facebook::xplat::module::CxxModule> MakeCxxModule(
      IInspectable &nativeModule) noexcept;

 private:
  static MethodResultCallback MakeMethodResultCallback(
      facebook::xplat::module::CxxModule::Callback callback) noexcept;

 private:
  std::string m_name;
  std::string m_eventEmitterName;
  std::vector<facebook::xplat::module::CxxModule::Method> m_methods;
  std::vector<ConstantWriterDelegate> m_constants;
  std::vector<ABICxxModuleEventSetter> m_eventSetters;
};

} // namespace winrt::Microsoft::ReactNative::Bridge
