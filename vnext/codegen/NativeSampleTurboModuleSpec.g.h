
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

struct SampleTurboModuleSpec_Constants {
    bool const1;
    double const2;
    std::string const3;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(SampleTurboModuleSpec_Constants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"const1", &SampleTurboModuleSpec_Constants::const1},
        {L"const2", &SampleTurboModuleSpec_Constants::const2},
        {L"const3", &SampleTurboModuleSpec_Constants::const3},
    };
    return fieldMap;
}

struct SampleTurboModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<SampleTurboModuleSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"voidFunc"},
      SyncMethod<bool(bool) noexcept>{1, L"getBool"},
      SyncMethod<double(double) noexcept>{2, L"getEnum"},
      SyncMethod<double(double) noexcept>{3, L"getNumber"},
      SyncMethod<std::string(std::string) noexcept>{4, L"getString"},
      SyncMethod<::React::JSValueArray(::React::JSValueArray) noexcept>{5, L"getArray"},
      SyncMethod<::React::JSValue(::React::JSValue) noexcept>{6, L"getObject"},
      SyncMethod<::React::JSValue(::React::JSValue) noexcept>{7, L"getUnsafeObject"},
      SyncMethod<double(double) noexcept>{8, L"getRootTag"},
      SyncMethod<::React::JSValue(double, std::string, ::React::JSValue) noexcept>{9, L"getValue"},
      Method<void(Callback<std::string>) noexcept>{10, L"getValueWithCallback"},
      Method<void(bool, Promise<std::string>) noexcept>{11, L"getValueWithPromise"},
      Method<void() noexcept>{12, L"voidFuncThrows"},
      SyncMethod<::React::JSValue(::React::JSValue) noexcept>{13, L"getObjectThrows"},
      Method<void(Promise<void>) noexcept>{14, L"promiseThrows"},
      Method<void() noexcept>{15, L"voidFuncAssert"},
      SyncMethod<::React::JSValue(::React::JSValue) noexcept>{16, L"getObjectAssert"},
      Method<void(Promise<void>) noexcept>{17, L"promiseAssert"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, SampleTurboModuleSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, SampleTurboModuleSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "SampleTurboModuleSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) SampleTurboModuleSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static SampleTurboModuleSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "voidFunc",
          "    REACT_METHOD(voidFunc) void voidFunc() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(voidFunc) static void voidFunc() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getBool",
          "    REACT_SYNC_METHOD(getBool) bool getBool(bool arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getBool) static bool getBool(bool arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "getEnum",
          "    REACT_SYNC_METHOD(getEnum) double getEnum(double arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getEnum) static double getEnum(double arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getNumber",
          "    REACT_SYNC_METHOD(getNumber) double getNumber(double arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getNumber) static double getNumber(double arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "getString",
          "    REACT_SYNC_METHOD(getString) std::string getString(std::string arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getString) static std::string getString(std::string arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getArray",
          "    REACT_SYNC_METHOD(getArray) ::React::JSValueArray getArray(::React::JSValueArray && arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getArray) static ::React::JSValueArray getArray(::React::JSValueArray && arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "getObject",
          "    REACT_SYNC_METHOD(getObject) ::React::JSValue getObject(::React::JSValue && arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getObject) static ::React::JSValue getObject(::React::JSValue && arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "getUnsafeObject",
          "    REACT_SYNC_METHOD(getUnsafeObject) ::React::JSValue getUnsafeObject(::React::JSValue && arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getUnsafeObject) static ::React::JSValue getUnsafeObject(::React::JSValue && arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "getRootTag",
          "    REACT_SYNC_METHOD(getRootTag) double getRootTag(double arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getRootTag) static double getRootTag(double arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "getValue",
          "    REACT_SYNC_METHOD(getValue) ::React::JSValue getValue(double x, std::string y, ::React::JSValue && z) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getValue) static ::React::JSValue getValue(double x, std::string y, ::React::JSValue && z) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "getValueWithCallback",
          "    REACT_METHOD(getValueWithCallback) void getValueWithCallback(std::function<void(std::string)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getValueWithCallback) static void getValueWithCallback(std::function<void(std::string)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "getValueWithPromise",
          "    REACT_METHOD(getValueWithPromise) void getValueWithPromise(bool error, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getValueWithPromise) static void getValueWithPromise(bool error, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "voidFuncThrows",
          "    REACT_METHOD(voidFuncThrows) void voidFuncThrows() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(voidFuncThrows) static void voidFuncThrows() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "getObjectThrows",
          "    REACT_SYNC_METHOD(getObjectThrows) ::React::JSValue getObjectThrows(::React::JSValue && arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getObjectThrows) static ::React::JSValue getObjectThrows(::React::JSValue && arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "promiseThrows",
          "    REACT_METHOD(promiseThrows) void promiseThrows(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(promiseThrows) static void promiseThrows(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "voidFuncAssert",
          "    REACT_METHOD(voidFuncAssert) void voidFuncAssert() noexcept { /* implementation */ }\n"
          "    REACT_METHOD(voidFuncAssert) static void voidFuncAssert() noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "getObjectAssert",
          "    REACT_SYNC_METHOD(getObjectAssert) ::React::JSValue getObjectAssert(::React::JSValue && arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getObjectAssert) static ::React::JSValue getObjectAssert(::React::JSValue && arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "promiseAssert",
          "    REACT_METHOD(promiseAssert) void promiseAssert(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(promiseAssert) static void promiseAssert(::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
