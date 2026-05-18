// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "CxxModuleUtilities.h"
#include <DynamicReader.h>
#include <DynamicWriter.h>

namespace msrn = winrt::Microsoft::ReactNative;

using folly::dynamic;

namespace Microsoft::React::Modules {

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

msrn::JSValue ToJSValue(dynamic &value) noexcept {
  auto reader = winrt::make<msrn::DynamicReader>(value);
  auto result = msrn::JSValue::ReadFrom(reader);

  return result;
}

dynamic ToDynamic(const msrn::JSValue &value) noexcept {
  auto argWriter = msrn::MakeJSValueArgWriter(value);
  auto result = msrn::DynamicWriter::ToDynamic(argWriter)[0];

  return result;
}

} // namespace Microsoft::React::Modules
