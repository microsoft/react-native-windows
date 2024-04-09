// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSI_LONGLIVEDJSIVALUE_
#define MICROSOFT_REACTNATIVE_JSI_LONGLIVEDJSIVALUE_

#include <ReactCommon/LongLivedObject.h>
#include <jsi/jsi.h>

namespace winrt::Microsoft::ReactNative {

// Wrap up JSI Runtime into a LongLivedObject
struct LongLivedJsiRuntime : facebook::react::LongLivedObject {
  static std::weak_ptr<LongLivedJsiRuntime> CreateWeak(facebook::jsi::Runtime &runtime) noexcept {
    auto value = std::shared_ptr<LongLivedJsiRuntime>(new LongLivedJsiRuntime(runtime));
    auto &longLivedObjectCollection = facebook::react::LongLivedObjectCollection::get(runtime);
    longLivedObjectCollection.add(value);
    return value;
  }

  facebook::jsi::Runtime &Runtime() {
    return runtime_;
  }

 protected:
  LongLivedJsiRuntime(facebook::jsi::Runtime &runtime) : LongLivedObject(runtime) {}
  LongLivedJsiRuntime(LongLivedJsiRuntime const &) = delete;
};

// Wrap up a JSI Value into a LongLivedObject.
template <typename TValue>
struct LongLivedJsiValue : facebook::react::LongLivedObject {
  static std::weak_ptr<LongLivedJsiValue<TValue>> CreateWeak(facebook::jsi::Runtime &runtime, TValue &&value) noexcept {
    auto valueWrapper =
        std::shared_ptr<LongLivedJsiValue<TValue>>(new LongLivedJsiValue<TValue>(runtime, std::forward<TValue>(value)));
    auto &longLivedObjectCollection = facebook::react::LongLivedObjectCollection::get(runtime);
    longLivedObjectCollection.add(valueWrapper);
    return valueWrapper;
  }

  TValue &Value() {
    return value_;
  }

 protected:
  template <typename TValue2>
  LongLivedJsiValue(facebook::jsi::Runtime &runtime, TValue2 &&value)
      : LongLivedObject(runtime), value_(std::forward<TValue2>(value)) {}

 private:
  TValue value_;
};

using LongLivedJsiFunction = LongLivedJsiValue<facebook::jsi::Function>;

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSI_LONGLIVEDJSIVALUE_
