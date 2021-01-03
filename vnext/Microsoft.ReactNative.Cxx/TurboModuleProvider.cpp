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
    // Move the func into the local value to make sure it is destroyed here after the execution.
    std::function<void()> localFunc{std::move(func)};

    if (m_jsDispatcher.HasThreadAccess()) {
      localFunc();
    } else {
      std::mutex mutex;
      std::condition_variable cv;
      bool completed{false};
      std::exception_ptr ex;

      auto lock = std::unique_lock{mutex};

      m_jsDispatcher.Post([&localFunc, &mutex, &cv, &completed, &ex]() {
        // Since this method is synchronous, we catch all func exceptions and rethrow them in this thread.
        try {
          localFunc();
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
  }

 private:
  IReactDispatcher m_jsDispatcher{nullptr};
};

// Creates CallInvoker based on JSDispatcher.
std::shared_ptr<facebook::react::CallInvoker> MakeAbiCallInvoker(IReactDispatcher const &jsDispatcher) noexcept {
  return std::make_shared<AbiCallInvoker>(jsDispatcher);
}

} // namespace winrt::Microsoft::ReactNative
