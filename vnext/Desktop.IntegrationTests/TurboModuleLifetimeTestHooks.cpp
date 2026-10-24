// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <TurboModulesProvider.h>
#include <jsi/jsi.h>
#include <winrt/Windows.Foundation.h>

// Compiled into react-native-win32.dll only for EnableV8LifetimeTests builds.
extern "C" __declspec(dllexport) void __cdecl RNWCreateTurboModuleForLifetimeTest(
    facebook::jsi::Runtime *runtime,
    facebook::jsi::Object *result) {
  namespace React = winrt::Microsoft::ReactNative;
  React::TurboModulesProvider provider;
  provider.AddModuleProvider(
      L"LifetimeTest",
      [](React::IReactModuleBuilder const &builder) -> winrt::Windows::Foundation::IInspectable {
        builder.AddSyncMethod(
            L"value", [](React::IJSValueReader const &, React::IJSValueWriter const &writer) { writer.WriteInt64(42); });
        return winrt::box_value(true);
      },
      false);
  *result = facebook::jsi::Object::createFromHostObject(*runtime, provider.getModule("LifetimeTest", nullptr));
}
