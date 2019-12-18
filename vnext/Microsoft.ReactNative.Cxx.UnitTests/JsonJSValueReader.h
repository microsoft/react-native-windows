// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "JsonReader.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct JsonJSValueReader : implements<JsonJSValueReader, IJSValueReader> {
  JsonJSValueReader(std::wstring &&jsonText) noexcept;

 public: // IJSValueReader
  JSValueType ValueType() noexcept;
  bool GetNextObjectProperty(hstring &propertyName) noexcept;
  bool GetNextArrayItem() noexcept;
  hstring GetString() noexcept;
  bool GetBoolean() noexcept;
  int64_t GetInt64() noexcept;
  double GetDouble() noexcept;

 private:
  void SetCurrentValue(JsonParseState parseState) noexcept;
  hstring GetHString() noexcept;
  static bool IsContainerOrValue(JsonParseState parseState) noexcept;

 private:
  const std::wstring m_jsonText;
  JsonReader m_jsonReader;
  JSValueType m_valueType{JSValueType::Null};
  std::vector<JSValueType> m_stack;
  bool m_isIterating{false};
  hstring m_stringValue;
  union {
    bool m_boolValue;
    int64_t m_int64Value;
    double m_doubleValue;
  };
};

} // namespace winrt::Microsoft::ReactNative
