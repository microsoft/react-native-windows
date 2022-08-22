// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSIVALUEHELPERS
#define MICROSOFT_REACTNATIVE_JSIVALUEHELPERS

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

// CODESYNC: ReactCommon\jsi\jsi\jsi.h
enum FacebookJsiValueKind {
  UndefinedKind,
  NullKind,
  BooleanKind,
  NumberKind,
  SymbolKind,
  BigIntKind,
  StringKind,
  ObjectKind,
  PointerKind = SymbolKind,
};

struct JsiValueKindHelper final {
  static FacebookJsiValueKind ToValueKind(JsiValueKind const &kind) noexcept;
  static JsiValueKind ToJsiValueKind(FacebookJsiValueKind const &kind) noexcept;
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSIVALUEHELPERS