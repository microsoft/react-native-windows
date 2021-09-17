
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

namespace Microsoft::ReactNativeSpecs {

REACT_STRUCT(AsyncSQLiteDBStorageSpec_multiGet_callback_errors_element)
struct AsyncSQLiteDBStorageSpec_multiGet_callback_errors_element {
    REACT_FIELD(message)
    std::string message;
    REACT_FIELD(key)
    std::optional<std::string> key;
};

REACT_STRUCT(AsyncSQLiteDBStorageSpec_multiSet_callback_errors_element)
struct AsyncSQLiteDBStorageSpec_multiSet_callback_errors_element {
    REACT_FIELD(message)
    std::string message;
    REACT_FIELD(key)
    std::optional<std::string> key;
};

REACT_STRUCT(AsyncSQLiteDBStorageSpec_multiMerge_callback_errors_element)
struct AsyncSQLiteDBStorageSpec_multiMerge_callback_errors_element {
    REACT_FIELD(message)
    std::string message;
    REACT_FIELD(key)
    std::optional<std::string> key;
};

REACT_STRUCT(AsyncSQLiteDBStorageSpec_multiRemove_callback_errors_element)
struct AsyncSQLiteDBStorageSpec_multiRemove_callback_errors_element {
    REACT_FIELD(message)
    std::string message;
    REACT_FIELD(key)
    std::optional<std::string> key;
};

REACT_STRUCT(AsyncSQLiteDBStorageSpec_clear_callback_error)
struct AsyncSQLiteDBStorageSpec_clear_callback_error {
    REACT_FIELD(message)
    std::string message;
    REACT_FIELD(key)
    std::optional<std::string> key;
};

REACT_STRUCT(AsyncSQLiteDBStorageSpec_getAllKeys_callback_error)
struct AsyncSQLiteDBStorageSpec_getAllKeys_callback_error {
    REACT_FIELD(message)
    std::string message;
    REACT_FIELD(key)
    std::optional<std::string> key;
};

struct AsyncSQLiteDBStorageSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(std::vector<std::string>, Callback<std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiGet_callback_errors_element>>, std::optional<std::vector<std::vector<std::string>>>>) noexcept>{0, L"multiGet"},
      Method<void(std::vector<std::vector<std::string>>, Callback<std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiSet_callback_errors_element>>>) noexcept>{1, L"multiSet"},
      Method<void(std::vector<std::vector<std::string>>, Callback<std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiMerge_callback_errors_element>>>) noexcept>{2, L"multiMerge"},
      Method<void(std::vector<std::string>, Callback<std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiRemove_callback_errors_element>>>) noexcept>{3, L"multiRemove"},
      Method<void(Callback<AsyncSQLiteDBStorageSpec_clear_callback_error>) noexcept>{4, L"clear"},
      Method<void(Callback<std::optional<AsyncSQLiteDBStorageSpec_getAllKeys_callback_error>, std::optional<std::vector<std::string>>>) noexcept>{5, L"getAllKeys"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, AsyncSQLiteDBStorageSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "multiGet",
          "    REACT_METHOD(multiGet) void multiGet(std::vector<std::string> const & keys, std::function<void(std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiGet_callback_errors_element>>, std::optional<std::vector<std::vector<std::string>>>)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(multiGet) static void multiGet(std::vector<std::string> const & keys, std::function<void(std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiGet_callback_errors_element>>, std::optional<std::vector<std::vector<std::string>>>)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "multiSet",
          "    REACT_METHOD(multiSet) void multiSet(std::vector<std::vector<std::string>> const & kvPairs, std::function<void(std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiSet_callback_errors_element>>)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(multiSet) static void multiSet(std::vector<std::vector<std::string>> const & kvPairs, std::function<void(std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiSet_callback_errors_element>>)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "multiMerge",
          "    REACT_METHOD(multiMerge) void multiMerge(std::vector<std::vector<std::string>> const & kvPairs, std::function<void(std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiMerge_callback_errors_element>>)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(multiMerge) static void multiMerge(std::vector<std::vector<std::string>> const & kvPairs, std::function<void(std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiMerge_callback_errors_element>>)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "multiRemove",
          "    REACT_METHOD(multiRemove) void multiRemove(std::vector<std::string> const & keys, std::function<void(std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiRemove_callback_errors_element>>)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(multiRemove) static void multiRemove(std::vector<std::string> const & keys, std::function<void(std::optional<std::vector<AsyncSQLiteDBStorageSpec_multiRemove_callback_errors_element>>)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "clear",
          "    REACT_METHOD(clear) void clear(std::function<void(AsyncSQLiteDBStorageSpec_clear_callback_error const &)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(clear) static void clear(std::function<void(AsyncSQLiteDBStorageSpec_clear_callback_error const &)> const & callback) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "getAllKeys",
          "    REACT_METHOD(getAllKeys) void getAllKeys(std::function<void(std::optional<AsyncSQLiteDBStorageSpec_getAllKeys_callback_error>, std::optional<std::vector<std::string>>)> const & callback) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(getAllKeys) static void getAllKeys(std::function<void(std::optional<AsyncSQLiteDBStorageSpec_getAllKeys_callback_error>, std::optional<std::vector<std::string>>)> const & callback) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
