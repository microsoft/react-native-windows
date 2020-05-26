// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "jsi/jsi.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct JsiReader : implements<JsiReader, IJSValueReader> {
  JsiReader(facebook::jsi::Runtime &runtime, const facebook::jsi::Value &root) noexcept;

 public: // IJSValueReader
  JSValueType ValueType() noexcept;
  bool GetNextObjectProperty(hstring &propertyName) noexcept;
  bool GetNextArrayItem() noexcept;
  hstring GetString() noexcept;
  bool GetBoolean() noexcept;
  int64_t GetInt64() noexcept;
  double GetDouble() noexcept;

 private:
  enum class ContinuationAction {
    MoveToNextObjectProperty,
    MoveToNextArrayElement,
  };

  struct Continuation {
    ContinuationAction Action;
    std::optional<facebook::jsi::Object> CurrentObject; // valid for object
    std::optional<facebook::jsi::Array> PropertyNames; // valid for object
    std::optional<facebook::jsi::Array> CurrentArray; // valid for array
    int Index = -1;

    Continuation(facebook::jsi::Runtime &runtime, facebook::jsi::Object &&value) noexcept
        : Action(ContinuationAction::MoveToNextObjectProperty),
          CurrentObject(std::make_optional<facebook::jsi::Object>(std::move(value))) {
      PropertyNames = CurrentObject.value().getPropertyNames(runtime);
    }

    Continuation(facebook::jsi::Array &&value) noexcept
        : Action(ContinuationAction::MoveToNextArrayElement),
          CurrentArray(std::make_optional<facebook::jsi::Array>(std::move(value))) {}

    Continuation(const Continuation &) = delete;
    Continuation(Continuation &&) = default;
  };

  // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/MIN_SAFE_INTEGER
  static const int64_t MinSafeInteger = -9007199254740991L;
  static const int64_t MaxSafeInteger = 9007199254740991L;

 private:
  void SetValue(const facebook::jsi::Value &value) noexcept;

 private:
  facebook::jsi::Runtime &m_runtime;
  const facebook::jsi::Value &m_root;

  // when m_currentPrimitiveValue is not null, the current value is a primitive value
  // when m_currentPrimitiveValue is null, the current value is the top value of m_nonPrimitiveValues
  std::optional<facebook::jsi::Value> m_currentPrimitiveValue;
  std::vector<Continuation> m_nonPrimitiveValues;
};

} // namespace winrt::Microsoft::ReactNative
