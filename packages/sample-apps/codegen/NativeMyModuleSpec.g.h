
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

// #include "NativeMyModuleDataTypes.g.h" before this file to use the generated type definition
#include <NativeModules.h>
#include <tuple>

namespace SampleLibraryCodegen {

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(MyModuleSpec_Constants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"const1", &MyModuleSpec_Constants::const1},
        {L"const2", &MyModuleSpec_Constants::const2},
        {L"const3", &MyModuleSpec_Constants::const3},
    };
    return fieldMap;
}

struct MyModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<MyModuleSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void() noexcept>{0, L"voidFunc"},
      SyncMethod<bool(bool) noexcept>{1, L"getBool"},
      SyncMethod<double(double) noexcept>{2, L"getNumber"},
      SyncMethod<std::string(std::string) noexcept>{3, L"getString"},
      SyncMethod<::React::JSValueArray(::React::JSValueArray) noexcept>{4, L"getArray"},
      SyncMethod<::React::JSValue(::React::JSValue) noexcept>{5, L"getObject"},
      SyncMethod<::React::JSValue(double, std::string, ::React::JSValue) noexcept>{6, L"getValue"},
      Method<void(Callback<std::string>) noexcept>{7, L"getValueWithCallback"},
      Method<void(bool, Promise<std::string>) noexcept>{8, L"getValueWithPromise"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, MyModuleSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, MyModuleSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "MyModuleSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) MyModuleSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static MyModuleSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

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
          "getNumber",
          "    REACT_SYNC_METHOD(getNumber) double getNumber(double arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getNumber) static double getNumber(double arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "getString",
          "    REACT_SYNC_METHOD(getString) std::string getString(std::string arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getString) static std::string getString(std::string arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "getArray",
          "    REACT_SYNC_METHOD(getArray) ::React::JSValueArray getArray(::React::JSValueArray && arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getArray) static ::React::JSValueArray getArray(::React::JSValueArray && arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getObject",
          "    REACT_SYNC_METHOD(getObject) ::React::JSValue getObject(::React::JSValue && arg) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getObject) static ::React::JSValue getObject(::React::JSValue && arg) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "getValue",
          "    REACT_SYNC_METHOD(getValue) ::React::JSValue getValue(double x, std::string y, ::React::JSValue && z) noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getValue) static ::React::JSValue getValue(double x, std::string y, ::React::JSValue && z) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "getValueWithCallback",
          "    REACT_METHOD(getValueWithCallback) void getValueWithCallback(std::function<void(std::string)> const & callback) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getValueWithCallback) static void getValueWithCallback(std::function<void(std::string)> const & callback) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "getValueWithPromise",
          "    REACT_METHOD(getValueWithPromise) void getValueWithPromise(bool error, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getValueWithPromise) static void getValueWithPromise(bool error, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace SampleLibraryCodegen
