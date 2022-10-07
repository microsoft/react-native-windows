
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include "NativeModules.h"
#include <tuple>

namespace SampleLibraryCodegen {

REACT_STRUCT(MyModuleSpec_Constants)
struct MyModuleSpec_Constants {
    REACT_FIELD(const1)
    bool const1;
    REACT_FIELD(const2)
    double const2;
    REACT_FIELD(const3)
    std::string const3;
};

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
      Method<void(bool, Promise<::React::JSValue>) noexcept>{8, L"getValueWithPromise"},
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
          "    REACT_METHOD(getValueWithPromise) void getValueWithPromise(bool error, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getValueWithPromise) static void getValueWithPromise(bool error, ::React::ReactPromise<::React::JSValue> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace SampleLibraryCodegen
