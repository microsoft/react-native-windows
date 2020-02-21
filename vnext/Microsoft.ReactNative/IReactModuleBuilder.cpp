// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactModuleBuilder.h"
#include "DynamicWriter.h"
#include "ReactHost/MsoUtils.h"

using namespace facebook::xplat::module;

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// ReactModuleBuilder implementation
//===========================================================================

ReactModuleBuilder::ReactModuleBuilder(IReactContext const &reactContext) noexcept : m_reactContext{reactContext} {}

void ReactModuleBuilder::AddInitializer(InitializerDelegate const &initializer) noexcept {
  m_initializers.push_back(initializer);
}

void ReactModuleBuilder::AddConstantProvider(ConstantProviderDelegate const &constantProvider) noexcept {
  m_constantProviders.push_back(constantProvider);
}

void ReactModuleBuilder::AddMethod(
    hstring const &name,
    MethodReturnType returnType,
    MethodDelegate const &method) noexcept {
  CxxModule::Method cxxMethod(
      to_string(name), [method](folly::dynamic args, CxxModule::Callback resolve, CxxModule::Callback reject) noexcept {
        auto argReader = make<DynamicReader>(args);
        auto resultWriter = make<DynamicWriter>();
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
      [method](folly::dynamic args) noexcept {
        auto argReader = make<DynamicReader>(args);
        auto resultWriter = make<DynamicWriter>();
        method(argReader, resultWriter);
        return get_self<DynamicWriter>(resultWriter)->TakeValue();
      },
      CxxModule::SyncTag);

  m_methods.push_back(std::move(cxxMethod));
}

/*static*/ MethodResultCallback ReactModuleBuilder::MakeMethodResultCallback(CxxModule::Callback &&callback) noexcept {
  if (callback) {
    return [callback = std::move(callback)](const IJSValueWriter &outputWriter) noexcept {
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
    IInspectable const &nativeModule) noexcept {
  for (auto &initializer : m_initializers) {
    initializer(m_reactContext);
  }
  return std::make_unique<ABICxxModule>(
      nativeModule, Mso::Copy(name), Mso::Copy(m_constantProviders), Mso::Copy(m_methods));
}

} // namespace winrt::Microsoft::ReactNative
