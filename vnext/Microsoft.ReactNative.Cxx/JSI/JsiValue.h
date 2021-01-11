// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSIVALUE
#define MICROSOFT_REACTNATIVE_JSIVALUE

// This is a new experimental implementation of JsiValue.
// It is work in progress - no implementation yet.

#include <winrt/Microsoft.ReactNative.h>
#include "Crash.h"

namespace winrt::Microsoft::ReactNative {

struct JsiValue {
  //! JsiValue with JsiValueKind::Undefined.
  static JsiValue const Undefined;

  //! JsiValue with JsiValueKind::Null.
  static JsiValue const Null;

  JsiValue() noexcept;
  JsiValue(JsiValue &&other) noexcept;
  JsiValue &operator=(JsiValue &&other) noexcept;

  ~JsiValue() noexcept;

  JsiValue(JsiValue const &other) = delete;
  JsiValue &operator=(JsiValue const &other) = delete;

  JsiValue Clone();

  bool IsUndefined();
  bool IsNull();
  bool IsBoolean();
  bool IsNumber();
  bool IsString();
  bool IsSymbol();
  bool IsObject();
  bool IsArray();
  bool IsFunction();

  bool IsStrictlyEqual(JsiValue const &other);
  bool IsEqualWithTypeCoercion(JsiValue const &other);
  bool IsInstanceOf(JsiValue const &value);

  JsiValue Call(winrt::array_view<JsiValue> args);
  JsiValue Construct(winrt::array_view<JsiValue> args);
  JsiValue InvokeMethod(std::string_view methodName, winrt::array_view<JsiValue> args);
  JsiValue InvokeMethod(JsiValue const &methodName, winrt::array_view<JsiValue> args);

  void DefineProperty(std::string_view propertyName, JsiValue const &descriptor);
  void DefineProperty(JsiValue const &propertyName, JsiValue const &descriptor);
  bool HasProperty(std::string_view propertyName);
  bool HasProperty(JsiValue const &propertyName);
  bool DeleteProperty(std::string_view propertyName);
  bool DeleteProperty(JsiValue const &propertyName);
  JsiValue GetProperty(std::string_view propertyName);
  JsiValue GetProperty(JsiValue const &propertyName);
  void SetProperty(std::string_view propertyName, JsiValue const &value);
  void SetProperty(JsiValue const &propertyName, JsiValue const &value);
  JsiValue GetArrayItem(uint32_t index);
  void SetArrayItem(uint32_t index, JsiValue const &value);

  IJsiRuntime Runtime();

  // TODO: add operator []

 private:
  JsiValueKind m_kind{JsiValueKind::Undefined};
  IJsiRuntime m_runtime;
  union {
    bool m_boolValue;
    double m_numberValue;
    void *m_ptrValue;
  };
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSIVALUE
