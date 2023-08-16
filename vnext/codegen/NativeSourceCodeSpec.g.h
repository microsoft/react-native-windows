
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

struct SourceCodeSpec_Constants {
    std::string scriptURL;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(SourceCodeSpec_Constants*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"scriptURL", &SourceCodeSpec_Constants::scriptURL},
    };
    return fieldMap;
}

struct SourceCodeSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<SourceCodeSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{

  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, SourceCodeSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, SourceCodeSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "SourceCodeSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) SourceCodeSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static SourceCodeSpec_Constants GetConstants() noexcept {/*implementation*/}\n");


  }
};

} // namespace Microsoft::ReactNativeSpecs
