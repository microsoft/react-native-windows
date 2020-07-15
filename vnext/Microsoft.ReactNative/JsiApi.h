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

struct JsiRuntime : implements<JsiRuntime, IJsiRuntime> {
  JsiRuntime(facebook::jsi::Runtime &runtime) noexcept;

 public: // IJsiRuntime
  JsiValueData EvaluateJavaScript(IJsiByteBuffer const &buffer, hstring const &sourceUrl);
  ReactNative::JsiPreparedJavaScript PrepareJavaScript(IJsiByteBuffer const &buffer, hstring const &sourceUrl);
  JsiValueData EvaluatePreparedJavaScript(ReactNative::JsiPreparedJavaScript const &js);
  JsiObjectData Global();
  hstring Description();
  bool IsInspectable();
  JsiSymbolData CloneSymbol(JsiSymbolData symbol);
  JsiStringData CloneString(JsiStringData str);
  JsiObjectData CloneObject(JsiObjectData obj);
  JsiPropertyNameIdData ClonePropertyNameId(JsiPropertyNameIdData propertyNameId);
  JsiPropertyNameIdData CreatePropertyNameIdFromAscii(array_view<uint8_t const> ascii);
  JsiPropertyNameIdData CreatePropertyNameIdFromUtf8(array_view<uint8_t const> utf8);
  JsiPropertyNameIdData CreatePropertyNameIdFromString(JsiStringData str);
  void PropertyNameIdToUtf8(JsiPropertyNameIdData propertyNameId, JsiByteArrayUser const &useUtf8String);
  bool PropertyNameIdEquals(JsiPropertyNameIdData left, JsiPropertyNameIdData right);
  void SymbolToUtf8(JsiSymbolData symbol, JsiByteArrayUser const &useUtf8String);
  JsiStringData CreateStringFromAscii(array_view<uint8_t const> ascii);
  JsiStringData CreateStringFromUtf8(array_view<uint8_t const> utf8);
  void StringToUtf8(JsiStringData str, JsiByteArrayUser const &useUtf8String);
  JsiValueData CreateValueFromJsonUtf8(array_view<uint8_t const> json);
  JsiObjectData CreateObject();
  JsiObjectData CreateObjectWithHostObject(IJsiHostObject const &hostObject);
  IJsiHostObject GetHostObject(JsiObjectData obj);
  JsiHostFunction GetHostFunction(JsiFunctionData func);
  JsiValueData GetPropertyById(JsiObjectData obj, JsiPropertyNameIdData propertyNameId);
  JsiValueData GetPropertyByName(JsiObjectData obj, JsiStringData name);
  bool HasPropertyById(JsiObjectData obj, JsiPropertyNameIdData propertyNameId);
  bool HasPropertyByName(JsiObjectData obj, JsiStringData name);
  void SetPropertyById(JsiObjectData obj, JsiPropertyNameIdData propertyNameId, JsiValueData const &value);
  void SetPropertyByName(JsiObjectData obj, JsiStringData name, JsiValueData const &value);
  bool IsArray(JsiObjectData obj);
  bool IsArrayBuffer(JsiObjectData obj);
  bool IsFunction(JsiObjectData obj);
  bool IsHostObject(JsiObjectData obj);
  bool IsHostFunction(JsiFunctionData obj);
  JsiArrayData GetPropertyNames(JsiObjectData obj);
  JsiWeakObjectData CreateWeakObject(JsiObjectData obj);
  JsiValueData LockWeakObject(JsiWeakObjectData weakObject);
  JsiArrayData CreateArray(uint32_t size);
  uint32_t GetArraySize(JsiArrayData arr);
  uint32_t GetArrayBufferSize(JsiArrayBufferData arrayBuffer);
  void GetArrayBufferData(JsiArrayBufferData arrayBuffer, JsiByteArrayUser const &useArrayBytes);
  JsiValueData GetValueAtIndex(JsiArrayData arr, uint32_t index);
  void SetValueAtIndex(JsiArrayData arr, uint32_t index, JsiValueData const &value);
  JsiFunctionData CreateFunctionFromHostFunction(
      JsiPropertyNameIdData propNameId,
      uint32_t paramCount,
      JsiHostFunction const &hostFunc);
  JsiValueData Call(JsiFunctionData func, JsiValueData const &thisArg, array_view<JsiValueData const> args);
  JsiValueData CallAsConstructor(JsiFunctionData func, array_view<JsiValueData const> args);
  JsiScopeState PushScope();
  void PopScope(JsiScopeState scopeState);
  bool SymbolStrictEquals(JsiSymbolData left, JsiSymbolData right);
  bool StringStrictEquals(JsiStringData left, JsiStringData right);
  bool ObjectStrictEquals(JsiObjectData left, JsiObjectData right);
  bool InstanceOf(JsiObjectData obj, JsiFunctionData constructor);

  void ReleaseSymbol(JsiSymbolData const &symbol);
  void ReleaseString(JsiStringData const &str);
  void ReleaseObject(JsiObjectData const &obj);
  void ReleasePropertyNameId(JsiPropertyNameIdData const &propertyNameId);

 private:
  facebook::jsi::Runtime &m_runtime;
};

} // namespace winrt::Microsoft::ReactNative::implementation
