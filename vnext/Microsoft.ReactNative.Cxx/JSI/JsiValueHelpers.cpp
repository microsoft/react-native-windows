// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiValueHelpers.h"

namespace winrt::Microsoft::ReactNative {

/*static*/ FacebookJsiValueKind JsiValueKindHelper::ToValueKind(JsiValueKind const &kind) noexcept {
  switch (kind) {
    case JsiValueKind::Undefined:
      return FacebookJsiValueKind::UndefinedKind;
    case JsiValueKind::Null:
      return FacebookJsiValueKind::NullKind;
    case JsiValueKind::Boolean:
      return FacebookJsiValueKind::BooleanKind;
    case JsiValueKind::Number:
      return FacebookJsiValueKind::NumberKind;
    case JsiValueKind::Symbol:
      return FacebookJsiValueKind::SymbolKind;
    case JsiValueKind::String:
      return FacebookJsiValueKind::StringKind;
    case JsiValueKind::Object:
      return FacebookJsiValueKind::ObjectKind;
    case JsiValueKind::BigInt:
      return FacebookJsiValueKind::BigIntKind;
  }
  return FacebookJsiValueKind::UndefinedKind;
}

/*static*/ JsiValueKind JsiValueKindHelper::ToJsiValueKind(FacebookJsiValueKind const &kind) noexcept {
  switch (kind) {
    case FacebookJsiValueKind::UndefinedKind:
      return JsiValueKind::Undefined;
    case FacebookJsiValueKind::NullKind:
      return JsiValueKind::Null;
    case FacebookJsiValueKind::BooleanKind:
      return JsiValueKind::Boolean;
    case FacebookJsiValueKind::NumberKind:
      return JsiValueKind::Number;
    case FacebookJsiValueKind::SymbolKind:
      return JsiValueKind::Symbol;
    case FacebookJsiValueKind::BigIntKind:
      return JsiValueKind::BigInt;
    case FacebookJsiValueKind::StringKind:
      return JsiValueKind::String;
    case FacebookJsiValueKind::ObjectKind:
      return JsiValueKind::Object;
  }
  return JsiValueKind::Undefined;
}

} // namespace winrt::Microsoft::ReactNative
