// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactModuleBuilder.h"
#include "ABICxxModule.h"
#include "DynamicWriter.h"

using namespace facebook::xplat::module;

namespace winrt::Microsoft::ReactNative::Bridge {

//===========================================================================
// ReactModuleBuilder implementation
//===========================================================================

ReactModuleBuilder::ReactModuleBuilder() noexcept {}

void ReactModuleBuilder::SetEventEmitterName(hstring const &name) noexcept {
  m_eventEmitterName = to_string(name);
}

void ReactModuleBuilder::AddMethod(
    hstring const &name,
    MethodReturnType returnType,
    MethodDelegate const &method) noexcept {
  CxxModule::Method cxxMethod(
      to_string(name),
      [method = std::move(method)](
          folly::dynamic args, CxxModule::Callback resolve, CxxModule::Callback reject) mutable noexcept {
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

void ReactModuleBuilder::AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept {
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

void ReactModuleBuilder::AddConstantProvider(ConstantProvider const &constantProvider) noexcept {
  m_constants.push_back(constantProvider);
}

void ReactModuleBuilder::AddEventHandlerSetter(
    hstring const &name,
    ReactEventHandlerSetter const &eventHandlerSetter) noexcept {
  m_eventHandlerSetters.push_back(ABICxxModuleEventHandlerSetter{winrt::to_string(name), eventHandlerSetter});
}

/*static*/ MethodResultCallback ReactModuleBuilder::MakeMethodResultCallback(CxxModule::Callback callback) noexcept {
  if (callback) {
    return [callback = std::move(callback)](const IJSValueWriter &outputWriter) mutable noexcept {
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

std::unique_ptr<CxxModule> ReactModuleBuilder::MakeCxxModule(
    std::string const &name,
    IInspectable &nativeModule) noexcept {
  return std::make_unique<ABICxxModule>(
      nativeModule, name, m_eventEmitterName, m_methods, m_constants, m_eventHandlerSetters);
}

} // namespace winrt::Microsoft::ReactNative::Bridge
