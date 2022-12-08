// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>
#include <JSValue.h>
#include <React.h>
#include <stdint.h>

namespace Microsoft::ReactNative {

using namespace std;

template <typename T>
inline T asEnum(winrt::Microsoft::ReactNative::JSValue const &obj) {
  return static_cast<T>(obj.AsInt64());
}

bool IsRS3OrHigher();
bool IsRS4OrHigher();
bool IsRS5OrHigher();
bool Is19H1OrHigher();
bool Is21H1OrHigher();

bool IsXamlIsland();
bool IsWinUI3Island();
bool IsFabricEnabled(winrt::Microsoft::ReactNative::IReactPropertyBag const &properties);

} // namespace Microsoft::ReactNative
