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
  static std::weak_ptr<LongLivedJsiRuntime> CreateWeak(
      std::shared_ptr<facebook::react::LongLivedObjectCollection> const &longLivedObjectCollection,
      facebook::jsi::Runtime &runtime) noexcept {
    auto value = std::shared_ptr<LongLivedJsiRuntime>(new LongLivedJsiRuntime(longLivedObjectCollection, runtime));
    longLivedObjectCollection->add(value);
    return value;
  }

  facebook::jsi::Runtime &Runtime() {
    return runtime_;
  }

 public: // LongLivedObject overrides
  void allowRelease() override {
    if (auto longLivedObjectCollection = longLivedObjectCollection_.lock()) {
      if (longLivedObjectCollection != nullptr) {
        longLivedObjectCollection->remove(this);
        return;
      }
    }
    LongLivedObject::allowRelease();
  }

 protected:
  LongLivedJsiRuntime(
      std::shared_ptr<facebook::react::LongLivedObjectCollection> const &longLivedObjectCollection,
      facebook::jsi::Runtime &runtime)
      : longLivedObjectCollection_(longLivedObjectCollection), runtime_(runtime) {}

  LongLivedJsiRuntime(LongLivedJsiRuntime const &) = delete;

 private:
  // Use a weak reference to the collection to avoid reference loops
  std::weak_ptr<facebook::react::LongLivedObjectCollection> longLivedObjectCollection_;
  facebook::jsi::Runtime &runtime_;
};

// Wrap up a JSI Value into a LongLivedObject.
template <typename TValue>
struct LongLivedJsiValue : LongLivedJsiRuntime {
  static std::weak_ptr<LongLivedJsiValue<TValue>> CreateWeak(
      std::shared_ptr<facebook::react::LongLivedObjectCollection> const &longLivedObjectCollection,
      facebook::jsi::Runtime &runtime,
      TValue &&value) noexcept {
    auto valueWrapper = std::shared_ptr<LongLivedJsiValue<TValue>>(
        new LongLivedJsiValue<TValue>(longLivedObjectCollection, runtime, std::forward<TValue>(value)));
    longLivedObjectCollection->add(valueWrapper);
    return valueWrapper;
  }

  TValue &Value() {
    return value_;
  }

 protected:
  template <typename TValue2>
  LongLivedJsiValue(
      std::shared_ptr<facebook::react::LongLivedObjectCollection> const &longLivedObjectCollection,
      facebook::jsi::Runtime &runtime,
      TValue2 &&value)
      : LongLivedJsiRuntime(longLivedObjectCollection, runtime), value_(std::forward<TValue2>(value)) {}

 private:
  TValue value_;
};

using LongLivedJsiFunction = LongLivedJsiValue<facebook::jsi::Function>;

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSI_LONGLIVEDJSIVALUE_
