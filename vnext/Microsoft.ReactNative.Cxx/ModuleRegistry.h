#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ReactMemberInfo.h"

namespace winrt::Microsoft::ReactNative {

struct ModuleRegistry {
  template <class TClass, int I>
  void RegisterModule(wchar_t const *moduleName, wchar_t const *eventEmitterName, ReactMemberId<I>) noexcept;

  template <class TClass, int I>
  void RegisterModule(wchar_t const *moduleName, wchar_t const *eventEmitterName, ReactMemberId<I>) noexcept;
};

} // namespace winrt::Microsoft::ReactNative
