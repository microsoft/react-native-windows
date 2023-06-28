// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "CxxModuleUtilities.h"

namespace msrn = winrt::Microsoft::ReactNative;

using facebook::react::Instance;
using folly::dynamic;
using std::string;
using std::weak_ptr;

namespace {

msrn::JSValueArray ToJSValueArray(dynamic& array) noexcept {
  auto result = msrn::JSValueArray{};
  for (auto& item : array) {
    result.emplace_back(Microsoft::React::Modules::ToJSValue(item));
  }

  return result;
}

msrn::JSValueObject ToJSValueObject(dynamic& object) noexcept {
  auto result = msrn::JSValueObject{};
  for (auto &entry : object.items()) {
    result[entry.first.asString()] = Microsoft::React::Modules::ToJSValue(entry.second);
  }

  return result;
}

dynamic ToDynamicArray(const msrn::JSValue& value) noexcept {
  auto result = dynamic::array();
  for (auto& item : value.AsArray()) {
    result.push_back(Microsoft::React::Modules::ToDynamic(item));
  }

  return result;
}

dynamic ToDynamicObject(const msrn::JSValue& value) noexcept {
  auto result = dynamic::object();
  for (auto& entry : value.AsObject()) {
    result(entry.first, Microsoft::React::Modules::ToDynamic(entry.second));
  }

  return result;
}

} // namespace <anonymous>

namespace Microsoft::React::Modules {

void SendEvent(weak_ptr<Instance> weakReactInstance, string &&eventName, dynamic &&args) {
  if (auto instance = weakReactInstance.lock()) {
    instance->callJSFunction("RCTDeviceEventEmitter", "emit", dynamic::array(std::move(eventName), std::move(args)));
  }
}

void SendEvent(
    msrn::ReactContext const &reactContext,
    std::wstring_view &&eventName,
    msrn::JSValueObject &&args) noexcept {
  reactContext.EmitJSEvent(L"RCTDeviceEventEmitter", std::move(eventName), std::move(args));
}

void SendEvent(
    msrn::ReactContext const &reactContext,
    std::wstring_view &&eventName,
    msrn::JSValueArray &&args) noexcept {
  reactContext.EmitJSEvent(L"RCTDeviceEventEmitter", std::move(eventName), std::move(args));
}

msrn::JSValue ToJSValue(dynamic& value) noexcept {
  using msrn::JSValue;

  switch (value.type()) {
    case dynamic::Type::BOOL:
      return JSValue{value.asBool()};
    case dynamic::Type::DOUBLE:
      return JSValue{value.asDouble()};
    case dynamic::Type::INT64:
      return JSValue{value.asInt()};
    case dynamic::Type::STRING:
      return JSValue{value.asString()};
    case dynamic::Type::NULLT:
      return JSValue{nullptr};
    case dynamic::Type::ARRAY:
      return ToJSValueArray(value);
    case dynamic::Type::OBJECT:
      return ToJSValueObject(value);
  }

  // This should not happen.
  return JSValue{nullptr};
}

dynamic ToDynamic(const msrn::JSValue& value) noexcept {
  using msrn::JSValueType;

  switch (value.Type())
  {
  case JSValueType::Boolean:
    return dynamic{ value.AsBoolean() };
  case JSValueType::Double:
    return dynamic{ value.AsDouble() };
  case JSValueType::Int64:
    return dynamic{ value.AsInt64() };
  case JSValueType::String:
    return dynamic{ value.AsString() };
  case JSValueType::Null:
    return dynamic{nullptr};
  case JSValueType::Array:
    return ToDynamicArray(value);
  case JSValueType::Object:
    return ToDynamicObject(value);
  }

  return dynamic{ nullptr };
}

} // namespace Microsoft::React::Modules
