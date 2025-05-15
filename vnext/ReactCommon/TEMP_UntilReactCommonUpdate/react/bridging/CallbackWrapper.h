/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <jsi/jsi.h>

#include <memory>

#include <LongLivedObject.h> // [Windows #13391]

namespace facebook::react {

class CallInvoker;

// Helper for passing jsi::Function arg to other methods.
class CallbackWrapper : public LongLivedObject {
 private:
  CallbackWrapper(
      jsi::Function&& callback,
      jsi::Runtime& runtime,
      std::shared_ptr<CallInvoker> jsInvoker)
      : LongLivedObject(runtime),
        callback_(std::move(callback)),
        jsInvoker_(std::move(jsInvoker)) {}

  CallbackWrapper(
      std::shared_ptr<LongLivedObjectCollection> /*longLivedObjectCollection*/, // [Windows]
      jsi::Function &&callback,
      jsi::Runtime &runtime,
      std::shared_ptr<CallInvoker> jsInvoker)
      : LongLivedObject(runtime),
        callback_(std::move(callback)),
        jsInvoker_(std::move(jsInvoker)) {}

  // Use a weak_ptr to avoid a retain cycle: LongLivedObjectCollection owns all
  // CallbackWrappers. So, CallbackWrapper cannot own its
  // LongLivedObjectCollection.
  std::weak_ptr<LongLivedObjectCollection> longLivedObjectCollection_;
  jsi::Function callback_;
  std::shared_ptr<CallInvoker> jsInvoker_;

 public:
  static std::weak_ptr<CallbackWrapper> createWeak(
      jsi::Function &&callback,
      jsi::Runtime &runtime,
      std::shared_ptr<CallInvoker> jsInvoker) {
    auto wrapper = std::shared_ptr<CallbackWrapper>(new CallbackWrapper(
        std::move(callback), runtime, std::move(jsInvoker)));
    LongLivedObjectCollection::get(runtime).add(wrapper);
    return wrapper;
  }

  static std::weak_ptr<CallbackWrapper> createWeak(
      std::shared_ptr<LongLivedObjectCollection> longLivedObjectCollection,
      jsi::Function &&callback,
      jsi::Runtime &runtime,
      std::shared_ptr<CallInvoker> jsInvoker) {
    auto wrapper = std::shared_ptr<CallbackWrapper>(new CallbackWrapper(
        longLivedObjectCollection, std::move(callback), runtime, jsInvoker));
    longLivedObjectCollection->add(wrapper);
    return wrapper;
  }

  // Delete the enclosed jsi::Function
  void destroy() {
    allowRelease();
  }

  jsi::Function &callback() {
    return callback_;
  }

  jsi::Runtime &runtime() {
    return runtime_;
  }

  CallInvoker &jsInvoker() {
    return *(jsInvoker_);
  }

  std::shared_ptr<CallInvoker> jsInvokerPtr() {
    return jsInvoker_;
  }

  void allowRelease() {
    if (auto longLivedObjectCollection = longLivedObjectCollection_.lock()) {
      if (longLivedObjectCollection != nullptr) {
        longLivedObjectCollection->remove(this);
        return;
      }
    }
    LongLivedObject::allowRelease();
  }
};

} // namespace facebook::react
