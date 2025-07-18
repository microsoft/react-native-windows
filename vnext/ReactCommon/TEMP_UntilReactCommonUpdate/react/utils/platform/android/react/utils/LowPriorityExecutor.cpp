/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Windows version: no pthreads, no sched.h
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <string>
#include <functional>

namespace facebook::react::LowPriorityExecutor {

struct LowPriorityExecutorThread {
  LowPriorityExecutorThread() : thread_{std::thread([this] { run(); })} {
    // Optionally set thread name (Windows 10 1607+)
#if defined(_MSC_VER) && (_MSC_VER >= 1920)
    auto handle = thread_.native_handle();
    // SetThreadDescription is available on Windows 10 1607+.
    // Dynamically load to avoid build break on older SDKs.
    using SetThreadDescriptionFunc = HRESULT(WINAPI*)(HANDLE, PCWSTR);
    HMODULE hKernel32 = GetModuleHandleW(L"Kernel32.dll");
    if (hKernel32) {
      auto setThreadDescription = reinterpret_cast<SetThreadDescriptionFunc>(
          GetProcAddress(hKernel32, "SetThreadDescription"));
      if (setThreadDescription) {
        setThreadDescription(handle, L"LowPriorityExecutorThread");
      }
    }
#endif
    // Setting thread priority to low (optional)
    // ::SetThreadPriority(handle, THREAD_PRIORITY_LOWEST);
  }

  LowPriorityExecutorThread(const LowPriorityExecutorThread&) = delete;
  LowPriorityExecutorThread(LowPriorityExecutorThread&&) = delete;
  LowPriorityExecutorThread& operator=(const LowPriorityExecutorThread&) = delete;
  LowPriorityExecutorThread& operator=(LowPriorityExecutorThread&&) = delete;

  ~LowPriorityExecutorThread() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      running_ = false;
    }
    cv_.notify_one();
    thread_.join();
  }

  void post(std::function<void()>&& workItem) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.emplace(std::move(workItem));
    }
    cv_.notify_one();
  }

 private:
  void run() {
    while (true) {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [this] { return !queue_.empty() || !running_; });

      while (!queue_.empty()) {
        queue_.front()();
        queue_.pop();
      }

      if (!running_) {
        break;
      }
    }
  }

  std::thread thread_;
  std::queue<std::function<void()>> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool running_{true};
};

void execute(std::function<void()>&& workItem) {
  static LowPriorityExecutorThread thread{};
  thread.post(std::move(workItem));
}

} // namespace facebook::react::LowPriorityExecutor
