// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "codegen/NativeSampleTurboModuleSpec.g.h"
#include <NativeModules.h>

namespace Microsoft::ReactNative {

REACT_TURBO_MODULE(SampleTurboModule)
struct SampleTurboModule {
  using ModuleSpec = ReactNativeSpecs::SampleTurboModuleSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_EVENT(onPress)
  std::function<void()> onPress;

  REACT_EVENT(onClick)
  std::function<void(std::string)> onClick;

  REACT_EVENT(onChange)
  std::function<void(ReactNativeSpecs::SampleTurboModuleSpec_ObjectStruct)> onChange;

  REACT_EVENT(onSubmit)
  std::function<void(winrt::Microsoft::ReactNative::JSValueArray)> onSubmit;

  REACT_GET_CONSTANTS(GetConstants)
  ReactNativeSpecs::SampleTurboModuleSpec_Constants GetConstants() noexcept;

  REACT_METHOD(voidFunc)
  void voidFunc() noexcept;

  REACT_SYNC_METHOD(getBool)
  bool getBool(bool arg) noexcept;

  REACT_SYNC_METHOD(getEnum)
  double getEnum(double arg) noexcept;

  REACT_SYNC_METHOD(getNumber)
  double getNumber(double arg) noexcept;

  REACT_SYNC_METHOD(getString)
  std::string getString(std::string arg) noexcept;

  REACT_SYNC_METHOD(getArray)
  ::React::JSValueArray getArray(::React::JSValueArray &&arg) noexcept;

  REACT_SYNC_METHOD(getObject)
  ::React::JSValue getObject(::React::JSValue &&arg) noexcept;

  REACT_SYNC_METHOD(getUnsafeObject)
  ::React::JSValue getUnsafeObject(::React::JSValue &&arg) noexcept;

  REACT_SYNC_METHOD(getRootTag)
  double getRootTag(double arg) noexcept;

  REACT_SYNC_METHOD(getValue)
  ::React::JSValue getValue(double x, std::string y, ::React::JSValue &&z) noexcept;

  REACT_METHOD(getValueWithCallback)
  void getValueWithCallback(std::function<void(std::string)> const &callback) noexcept;

  REACT_METHOD(getValueWithPromise)
  void getValueWithPromise(bool error, ::React::ReactPromise<std::string> &&result) noexcept;

  REACT_METHOD(voidFuncThrows)
  void voidFuncThrows() noexcept;

  REACT_SYNC_METHOD(getObjectThrows)
  ::React::JSValue getObjectThrows(::React::JSValue &&arg) noexcept;

  REACT_METHOD(promiseThrows)
  void promiseThrows(::React::ReactPromise<void> &&result) noexcept;

  REACT_METHOD(voidFuncAssert)
  void voidFuncAssert() noexcept;

  REACT_SYNC_METHOD(getObjectAssert)
  ::React::JSValue getObjectAssert(::React::JSValue &&arg) noexcept;

  REACT_METHOD(promiseAssert)
  void promiseAssert(::React::ReactPromise<void> &&result) noexcept;

  REACT_METHOD(getImageUrl)
  void getImageUrl(::React::ReactPromise<void> &&result) noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
};

} // namespace Microsoft::ReactNative
