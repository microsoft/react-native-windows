
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off

#include <NativeModules.h>
#include <tuple>

namespace Microsoft::ReactNativeSpecs {

struct ReactDevToolsRuntimeSettingsModuleSpec_PartialReloadAndProfileConfig {
    std::optional<bool> shouldReloadAndProfile;
    std::optional<bool> recordChangeDescriptions;
};

struct ReactDevToolsRuntimeSettingsModuleSpec_ReloadAndProfileConfig {
    bool shouldReloadAndProfile;
    bool recordChangeDescriptions;
};


inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ReactDevToolsRuntimeSettingsModuleSpec_PartialReloadAndProfileConfig*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"shouldReloadAndProfile", &ReactDevToolsRuntimeSettingsModuleSpec_PartialReloadAndProfileConfig::shouldReloadAndProfile},
        {L"recordChangeDescriptions", &ReactDevToolsRuntimeSettingsModuleSpec_PartialReloadAndProfileConfig::recordChangeDescriptions},
    };
    return fieldMap;
}

inline winrt::Microsoft::ReactNative::FieldMap GetStructInfo(ReactDevToolsRuntimeSettingsModuleSpec_ReloadAndProfileConfig*) noexcept {
    winrt::Microsoft::ReactNative::FieldMap fieldMap {
        {L"shouldReloadAndProfile", &ReactDevToolsRuntimeSettingsModuleSpec_ReloadAndProfileConfig::shouldReloadAndProfile},
        {L"recordChangeDescriptions", &ReactDevToolsRuntimeSettingsModuleSpec_ReloadAndProfileConfig::recordChangeDescriptions},
    };
    return fieldMap;
}

struct ReactDevToolsRuntimeSettingsModuleSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(ReactDevToolsRuntimeSettingsModuleSpec_PartialReloadAndProfileConfig) noexcept>{0, L"setReloadAndProfileConfig"},
      SyncMethod<ReactDevToolsRuntimeSettingsModuleSpec_ReloadAndProfileConfig() noexcept>{1, L"getReloadAndProfileConfig"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, ReactDevToolsRuntimeSettingsModuleSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "setReloadAndProfileConfig",
          "    REACT_METHOD(setReloadAndProfileConfig) void setReloadAndProfileConfig(ReactDevToolsRuntimeSettingsModuleSpec_PartialReloadAndProfileConfig && config) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setReloadAndProfileConfig) static void setReloadAndProfileConfig(ReactDevToolsRuntimeSettingsModuleSpec_PartialReloadAndProfileConfig && config) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "getReloadAndProfileConfig",
          "    REACT_SYNC_METHOD(getReloadAndProfileConfig) ReactDevToolsRuntimeSettingsModuleSpec_ReloadAndProfileConfig getReloadAndProfileConfig() noexcept { /* implementation */ }\n"
          "    REACT_SYNC_METHOD(getReloadAndProfileConfig) static ReactDevToolsRuntimeSettingsModuleSpec_ReloadAndProfileConfig getReloadAndProfileConfig() noexcept { /* implementation */ }\n");
  }
};

} // namespace Microsoft::ReactNativeSpecs
