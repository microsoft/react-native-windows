// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactModuleBuilder.h"
#include <IReactContext.h>
#include <cassert>

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// ReactModuleBuilder implementation
//===========================================================================

ReactModuleBuilder::ReactModuleBuilder(IReactContext const &reactContext) noexcept : m_reactContext{reactContext} {}

void ReactModuleBuilder::AddInitializer(InitializerDelegate const &initializer) noexcept {
  m_initializers.push_back(initializer);
}

void ReactModuleBuilder::AddJsiInitializer(JsiInitializerDelegate const &initializer) noexcept {
  m_jsiinitializers.push_back(initializer);
}

void ReactModuleBuilder::AddConstantProvider(ConstantProviderDelegate const &constantProvider) noexcept {
  m_constantProviders.push_back(constantProvider);
}

void ReactModuleBuilder::AddMethod(
    hstring const & /*name*/,
    MethodReturnType /*returnType*/,
    MethodDelegate const & /*method*/) noexcept {
  // This method previously built CxxModule::Method for legacy module registration.
  // With New Architecture, use TurboModules instead.
  assert(
      false &&
      "Legacy module registration removed. Use packageBuilder.AddTurboModule() instead of packageBuilder.AddModule().");
  std::terminate();
}

void ReactModuleBuilder::AddSyncMethod(hstring const & /*name*/, SyncMethodDelegate const & /*method*/) noexcept {
  // This method previously built CxxModule::Method for legacy sync method registration.
  // With New Architecture, use TurboModules instead.
  assert(
      false &&
      "Legacy module registration removed. Use packageBuilder.AddTurboModule() instead of packageBuilder.AddModule().");
  std::terminate();
}

void ReactModuleBuilder::AddEventEmitter(hstring const &, EventEmitterInitializerDelegate const &) {
  // This method required TurboModule registration.
  assert(
      false &&
      "This module requires TurboModule registration. Use packageBuilder.AddTurboModule() instead of packageBuilder.AddModule().");
  std::terminate();
}

} // namespace winrt::Microsoft::ReactNative
