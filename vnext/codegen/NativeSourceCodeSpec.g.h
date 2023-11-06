
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

struct SourceCodeSpec_SourceCodeConstants {
    std::string scriptURL;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(SourceCodeSpec_SourceCodeConstants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"scriptURL", &SourceCodeSpec_SourceCodeConstants::scriptURL},
    };
    return fieldMap;
}

struct SourceCodeSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, SourceCodeSpec>();


  }
};

} // namespace Microsoft::ReactNativeSpecs
