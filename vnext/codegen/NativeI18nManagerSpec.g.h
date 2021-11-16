
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

REACT_STRUCT(I18nManagerSpec_Constants)
struct I18nManagerSpec_Constants {
    REACT_FIELD(isRTL)
    bool isRTL;
    REACT_FIELD(doLeftAndRightSwapInRTL)
    bool doLeftAndRightSwapInRTL;
    REACT_FIELD(localeIdentifier)
    std::optional<std::string> localeIdentifier;
};

struct I18nManagerSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<I18nManagerSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void(bool) noexcept>{0, L"allowRTL"},
      Method<void(bool) noexcept>{1, L"forceRTL"},
      Method<void(bool) noexcept>{2, L"swapLeftAndRightInRTL"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, I18nManagerSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, I18nManagerSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "I18nManagerSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) I18nManagerSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static I18nManagerSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "allowRTL",
          "    REACT_METHOD(allowRTL) void allowRTL(bool allowRTL) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(allowRTL) static void allowRTL(bool allowRTL) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "forceRTL",
          "    REACT_METHOD(forceRTL) void forceRTL(bool forceRTL) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(forceRTL) static void forceRTL(bool forceRTL) noexcept { /* implementation */ }}\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "swapLeftAndRightInRTL",
          "    REACT_METHOD(swapLeftAndRightInRTL) void swapLeftAndRightInRTL(bool flipStyles) noexcept { /* implementation */ }}\n"
          "    REACT_METHOD(swapLeftAndRightInRTL) static void swapLeftAndRightInRTL(bool flipStyles) noexcept { /* implementation */ }}\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
