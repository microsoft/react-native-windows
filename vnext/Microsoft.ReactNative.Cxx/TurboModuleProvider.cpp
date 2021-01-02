// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TurboModuleProvider.h"

namespace winrt::Microsoft::ReactNative {

// CallInvoker implementation based on JSDispatcher.
struct AbiCallInvoker final : facebook::react::CallInvoker {
  AbiCallInvoker(IReactDispatcher const &jsDispatcher) : m_jsDispatcher(jsDispatcher) {}

  void invokeAsync(std::function<void()> &&func) override {
    m_jsDispatcher.Post([func = std::move(func)]() { func(); });
  }

  void invokeSync(std::function<void()> &&func) override {
    if (m_jsDispatcher.HasThreadAccess()) {
      func();
    } else {
      std::mutex mutex;
      std::condition_variable cv;
      bool completed{false};
      std::exception_ptr ex;

      auto lock = std::unique_lock{mutex};

      m_jsDispatcher.Post([&func, &mutex, &cv, &completed, &ex]() {
        // Since this method is synchronous, we catch all func exceptions and rethrow them in this thread.
        try {
          func();
        } catch (...) {
          ex = std::current_exception();
        }

        auto lock = std::unique_lock{mutex};
        completed = true;
        cv.notify_all();
      });

      cv.wait(lock, [&] { return completed; });

      if (ex) {
        std::rethrow_exception(ex);
      }
    }

    // Since func is passed as an r-value, we want to clean it up in this method as we would move the value.
    func = nullptr;
  }

 private:
  IReactDispatcher m_jsDispatcher{nullptr};
};

// Creates CallInvoker based on JSDispatcher.
std::shared_ptr<facebook::react::CallInvoker> MakeAbiCallInvoker(IReactDispatcher const &jsDispatcher) noexcept {
  return std::make_shared<AbiCallInvoker>(jsDispatcher);
}

} // namespace winrt::Microsoft::ReactNative
