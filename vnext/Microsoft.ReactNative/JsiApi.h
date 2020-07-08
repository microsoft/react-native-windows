// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "JsiPreparedJavaScript.g.h"
#include "winrt/Microsoft.ReactNative.h"

namespace facebook::jsi {
class Runtime;
class Pointer;
} // namespace facebook::jsi

namespace winrt::Microsoft::ReactNative::implementation {

using JsiPointerHandle = uint64_t;
using JsiScopeHandle = uint64_t;

struct JsiRuntime : implements<JsiRuntime, IJsiRuntime> {
  JsiRuntime(facebook::jsi::Runtime &runtime) noexcept;

 public: // IJsiRuntime
  Microsoft::ReactNative::JsiValueData EvaluateJavaScript(
      Microsoft::ReactNative::IJsiBuffer const &buffer,
      hstring const &sourceUrl);
  Microsoft::ReactNative::JsiPreparedJavaScript PrepareJavaScript(
      Microsoft::ReactNative::IJsiBuffer const &buffer,
      hstring const &sourceUrl);
  Microsoft::ReactNative::JsiValueData EvaluatePreparedJavaScript(
      Microsoft::ReactNative::JsiPreparedJavaScript const &js);
  uint64_t Global() noexcept;
  hstring Description();
  bool IsInspectable();
  JsiPointerHandle CloneSymbol(JsiPointerHandle symbol);
  JsiPointerHandle CloneString(JsiPointerHandle str);
  JsiPointerHandle CloneObject(JsiPointerHandle obj);
  JsiPointerHandle ClonePropertyNameId(JsiPointerHandle propertyNameId);
  JsiPointerHandle CreatePropertyNameIdFromAscii(array_view<uint8_t const> ascii);
  JsiPointerHandle CreatePropertyNameIdFromUtf8(array_view<uint8_t const> utf8);
  JsiPointerHandle CreatePropertyNameIdFromString(JsiPointerHandle str);
  void PropertyNameIdToUtf8(JsiPointerHandle propertyNameId, Microsoft::ReactNative::JsiDataHandler const &utf8Handler);
  bool ComparePropertyNameIds(JsiPointerHandle left, JsiPointerHandle right);
  void SymbolToUtf8(JsiPointerHandle symbol, Microsoft::ReactNative::JsiDataHandler const &utf8Handler);
  JsiPointerHandle CreateStringFromAscii(array_view<uint8_t const> ascii);
  JsiPointerHandle CreateStringFromUtf8(array_view<uint8_t const> utf8);
  void StringToUtf8(JsiPointerHandle str, Microsoft::ReactNative::JsiDataHandler const &utf8Handler);
  Microsoft::ReactNative::JsiValueData CreateValueFromJsonUtf8(array_view<uint8_t const> json);
  JsiPointerHandle CreateObject();
  JsiPointerHandle CreateObjectWithHostObject(Microsoft::ReactNative::IJsiHostObject const &hostObject);
  Microsoft::ReactNative::IJsiHostObject GetHostObject(JsiPointerHandle obj);
  Microsoft::ReactNative::JsiHostFunction GetHostFunction(JsiPointerHandle func);
  Microsoft::ReactNative::JsiValueData GetProperty(JsiPointerHandle obj, JsiPointerHandle propertyNameId);
  Microsoft::ReactNative::JsiValueData GetPropertyWithString(JsiPointerHandle obj, JsiPointerHandle name);
  bool HasProperty(JsiPointerHandle obj, JsiPointerHandle propertyNameId);
  bool HasPropertyWithString(JsiPointerHandle obj, JsiPointerHandle name);
  void
  SetProperty(JsiPointerHandle obj, JsiPointerHandle propertyNameId, Microsoft::ReactNative::JsiValueData const &value);
  void
  SetPropertyWithString(JsiPointerHandle obj, JsiPointerHandle name, Microsoft::ReactNative::JsiValueData const &value);
  bool IsArray(JsiPointerHandle obj);
  bool IsArrayBuffer(JsiPointerHandle obj);
  bool IsFunction(JsiPointerHandle obj);
  bool IsHostObject(JsiPointerHandle obj);
  bool IsHostFunction(JsiPointerHandle obj);
  JsiPointerHandle GetPropertyNameArray(JsiPointerHandle obj);
  JsiPointerHandle CreateWeakObject(JsiPointerHandle obj);
  Microsoft::ReactNative::JsiValueData LockWeakObject(JsiPointerHandle weakObject);
  JsiPointerHandle CreateArray(uint32_t size);
  uint32_t GetArraySize(JsiPointerHandle arr);
  uint32_t GetArrayBufferSize(JsiPointerHandle arrayBuffer);
  void ArrayBufferToUtf8(JsiPointerHandle arrayBuffer, Microsoft::ReactNative::JsiDataHandler const &utf8Handler);
  Microsoft::ReactNative::JsiValueData GetValueAtIndex(JsiPointerHandle arr, uint32_t index);
  void SetValueAtIndex(JsiPointerHandle arr, uint32_t index, Microsoft::ReactNative::JsiValueData const &value);
  JsiPointerHandle CreateFunctionFromHostFunction(
      JsiPointerHandle propNameId,
      uint32_t paramCount,
      Microsoft::ReactNative::JsiHostFunction const &hostFunc);
  Microsoft::ReactNative::JsiValueData Call(
      JsiPointerHandle func,
      Microsoft::ReactNative::JsiValueData const &thisValue,
      array_view<Microsoft::ReactNative::JsiValueData const> args);
  Microsoft::ReactNative::JsiValueData CallAsConstructor(
      JsiPointerHandle func,
      array_view<Microsoft::ReactNative::JsiValueData const> args);
  JsiScopeHandle PushScope();
  void PopScope(JsiScopeHandle scopeState);
  bool SymbolStrictEquals(JsiPointerHandle left, JsiPointerHandle right);
  bool StringStrictEquals(JsiPointerHandle left, JsiPointerHandle right);
  bool ObjectStrictEquals(JsiPointerHandle left, JsiPointerHandle right);
  bool InstanceOf(JsiPointerHandle obj, JsiPointerHandle constructor);

 private:
  facebook::jsi::Runtime &m_runtime;
};

} // namespace winrt::Microsoft::ReactNative::implementation
