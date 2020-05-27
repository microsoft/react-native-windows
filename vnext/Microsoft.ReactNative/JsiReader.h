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
  enum class ContainerType {
    Object,
    Array,
  };

  struct Container {
    ContainerType Type;
    std::optional<facebook::jsi::Object> CurrentObject; // valid for object
    std::optional<facebook::jsi::Array> PropertyNames; // valid for object
    std::optional<facebook::jsi::Array> CurrentArray; // valid for array
    int Index = -1;

    Container(facebook::jsi::Runtime &runtime, facebook::jsi::Object &&value) noexcept
        : Type(ContainerType::Object), CurrentObject(std::make_optional<facebook::jsi::Object>(std::move(value))) {
      PropertyNames = CurrentObject.value().getPropertyNames(runtime);
    }

    Container(facebook::jsi::Array &&value) noexcept
        : Type(ContainerType::Array), CurrentArray(std::make_optional<facebook::jsi::Array>(std::move(value))) {}

    Container(const Container &) = delete;
    Container(Container &&) = default;
  };

 private:
  void SetValue(const facebook::jsi::Value &value) noexcept;

 private:
  facebook::jsi::Runtime &m_runtime;
  const facebook::jsi::Value &m_root;

  // when m_currentPrimitiveValue is not null, the current value is a primitive value
  // when m_currentPrimitiveValue is null, the current value is the top value of m_nonPrimitiveValues
  std::optional<facebook::jsi::Value> m_currentPrimitiveValue;
  std::vector<Container> m_containers;
};

} // namespace winrt::Microsoft::ReactNative
