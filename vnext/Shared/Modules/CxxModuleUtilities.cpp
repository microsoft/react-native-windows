// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "CxxModuleUtilities.h"

namespace msrn = winrt::Microsoft::ReactNative;

using facebook::react::Instance;
using folly::dynamic;
using std::string;
using std::weak_ptr;

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

} // namespace Microsoft::React::Modules
