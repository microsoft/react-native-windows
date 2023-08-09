
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

struct ShareModuleSpec_share_content {
    std::optional<std::string> title;
    std::optional<std::string> message;
};

struct ShareModuleSpec_share_returnType {
    std::string action;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ShareModuleSpec_share_content*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"title", &ShareModuleSpec_share_content::title},
        {L"message", &ShareModuleSpec_share_content::message},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ShareModuleSpec_share_returnType*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"action", &ShareModuleSpec_share_returnType::action},
    };
    return fieldMap;
}

struct ShareModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(ShareModuleSpec_share_content, std::string, Promise<ShareModuleSpec_share_returnType>) noexcept>{0, L"share"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ShareModuleSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "share",
          "    REACT_METHOD(share) void share(ShareModuleSpec_share_content && content, std::string dialogTitle, ::React::ReactPromise<ShareModuleSpec_share_returnType> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(share) static void share(ShareModuleSpec_share_content && content, std::string dialogTitle, ::React::ReactPromise<ShareModuleSpec_share_returnType> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
