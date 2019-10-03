// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeModuleBuilder.h"
#include "ABICxxModule.h"
#include "DynamicWriter.h"

using namespace facebook::xplat::module;

namespace winrt::Microsoft::ReactNative::Bridge {

//===========================================================================
// NativeModuleBuilder implementation
//===========================================================================

NativeModuleBuilder::NativeModuleBuilder() noexcept {}

void NativeModuleBuilder::SetName(hstring const &name) noexcept {
  m_name = to_string(name);
}

void NativeModuleBuilder::SetEventEmitterName(hstring const &name) noexcept {
  m_eventEmitterName = to_string(name);
}

void NativeModuleBuilder::AddMethod(
    hstring const &name,
    MethodReturnType returnType,
    MethodDelegate method) noexcept {
  CxxModule::Method cxxMethod(
      to_string(name),
      [method = std::move(method)](
          folly::dynamic args,
          CxxModule::Callback resolve,
          CxxModule::Callback reject) mutable noexcept {
        DynamicReader argReader{args};
        DynamicWriter resultWriter;
        auto resolveCallback = MakeMethodResultCallback(std::move(resolve));
        auto rejectCallback = MakeMethodResultCallback(std::move(reject));
        method(argReader, resultWriter, resolveCallback, rejectCallback);
      });

  switch (returnType) {
    case MethodReturnType::Callback:
      cxxMethod.callbacks = 1;
      cxxMethod.isPromise = false;
      break;
    case MethodReturnType::TwoCallbacks:
      cxxMethod.callbacks = 2;
      cxxMethod.isPromise = false;
      break;
    case MethodReturnType::Promise:
      cxxMethod.callbacks = 2;
      cxxMethod.isPromise = true;
      break;
    default:
      cxxMethod.callbacks = 0;
      cxxMethod.isPromise = false;
  }

  m_methods.push_back(std::move(cxxMethod));
}

void NativeModuleBuilder::AddSyncMethod(
    hstring const &name,
    SyncMethodDelegate method) noexcept {
  CxxModule::Method cxxMethod(
      to_string(name),
      [method = std::move(method)](folly::dynamic args) mutable noexcept {
        DynamicReader argReader{args};
        DynamicWriter resultWriter;
        method(argReader, resultWriter);
        return resultWriter.TakeValue();
      },
      CxxModule::SyncTag);

  m_methods.push_back(std::move(cxxMethod));
}

void NativeModuleBuilder::AddConstantWriter(
    ConstantWriterDelegate constantWriter) noexcept {
  m_constants.push_back(constantWriter);
}

void NativeModuleBuilder::AddEventRegister(
    hstring const &name,
    EventSetter eventSetter) noexcept {
  m_eventSetters.push_back(
      ABICxxModuleEventSetter{winrt::to_string(name), eventSetter});
}

/*static*/ MethodResultCallback NativeModuleBuilder::MakeMethodResultCallback(
    CxxModule::Callback callback) noexcept {
  if (callback) {
    return [callback = std::move(callback)](
        const IJSValueWriter &outputWriter) mutable noexcept {
      if (outputWriter) {
        folly::dynamic argArray = outputWriter.as<DynamicWriter>()->TakeValue();
        callback(std::vector<folly::dynamic>(argArray.begin(), argArray.end()));
      } else {
        callback(std::vector<folly::dynamic>{});
      }
    };
  }

  return {};
}

std::unique_ptr<CxxModule> NativeModuleBuilder::MakeCxxModule(
    IInspectable &nativeModule) noexcept {
  return std::make_unique<ABICxxModule>(
      nativeModule,
      m_name,
      m_eventEmitterName,
      m_methods,
      m_constants,
      m_eventSetters);
}

} // namespace winrt::Microsoft::ReactNative::Bridge
