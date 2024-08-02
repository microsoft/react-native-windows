// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "SampleTurboModule.h"

namespace Microsoft::ReactNative {

void SampleTurboModule::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_reactContext = reactContext;
}

ReactNativeSpecs::SampleTurboModuleSpec_Constants SampleTurboModule::GetConstants() noexcept {
  ReactNativeSpecs::SampleTurboModuleSpec_Constants constants;
  constants.const1 = true;
  constants.const2 = 375;
  constants.const3 = "something";
  return constants;
}

void SampleTurboModule::voidFunc() noexcept {}

bool SampleTurboModule::getBool(bool arg) noexcept {
  return arg;
}

double SampleTurboModule::getEnum(double arg) noexcept {
  return arg;
}

double SampleTurboModule::getNumber(double arg) noexcept {
  return arg;
}

std::string SampleTurboModule::getString(std::string arg) noexcept {
  return std::string(arg);
}

::React::JSValueArray SampleTurboModule::getArray(::React::JSValueArray &&arg) noexcept {
  // TODO: return deep copy
  return arg;
}

::React::JSValue SampleTurboModule::getObject(::React::JSValue &&arg) noexcept {
  // TODO: return deep copy
  return arg;
}

::React::JSValue SampleTurboModule::getUnsafeObject(::React::JSValue &&arg) noexcept {
  // TODO: Proper impl
  return arg;
}

double SampleTurboModule::getRootTag(double arg) noexcept {
  // TODO: Proper impl
  return arg;
}

::React::JSValue SampleTurboModule::getValue(double x, std::string y, ::React::JSValue &&z) noexcept {
  // TODO: Proper impl
  return nullptr;
}

void SampleTurboModule::getValueWithCallback(std::function<void(std::string)> const &callback) noexcept {
  // TODO: Proper impl
}

void SampleTurboModule::getValueWithPromise(bool error, ::React::ReactPromise<std::string> &&result) noexcept {
  // TODO: Proper impl
}

void SampleTurboModule::voidFuncThrows() noexcept {
  // TODO: Proper impl
}

::React::JSValue SampleTurboModule::getObjectThrows(::React::JSValue &&arg) noexcept {
  // TODO: Proper impl
  return arg;
}

void SampleTurboModule::promiseThrows(::React::ReactPromise<void> &&result) noexcept {
  // TODO: Proper impl
}

void SampleTurboModule::voidFuncAssert() noexcept {
  // TODO: Proper impl
}

::React::JSValue SampleTurboModule::getObjectAssert(::React::JSValue &&arg) noexcept {
  // TODO: Proper impl
  return arg;
}

void SampleTurboModule::promiseAssert(::React::ReactPromise<void> &&result) noexcept {
  // TODO: Proper impl
}

} // namespace Microsoft::ReactNative
