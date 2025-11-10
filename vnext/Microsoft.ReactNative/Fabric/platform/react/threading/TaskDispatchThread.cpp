/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// [Windows] Brought forward from react-native - switch to using react-native/ReactCxxPlatform/react/threading version
// once we integrate that far

#include "TaskDispatchThread.h"

#include <folly/portability/SysResource.h>
#include <folly/system/ThreadName.h>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

#include <glog/logging.h>

#ifdef ANDROID
#include <fbjni/fbjni.h>
#include <sys/syscall.h>
#endif

namespace facebook::react {

class TaskDispatchThread::Impl : public std::enable_shared_from_this<TaskDispatchThread::Impl> {
 public:
  Impl(std::string &&threadName) noexcept;
  ~Impl() noexcept;

  void start() noexcept;
  bool isOnThread() noexcept;
  bool isRunning() noexcept;
  void runAsync(TaskFn &&task, std::chrono::milliseconds delayMs = std::chrono::milliseconds::zero()) noexcept;
  void runSync(TaskFn &&task) noexcept;
  void quit() noexcept;
  void loop() noexcept;

 private:
  struct Task {
    TimePoint dispatchTime;
    TaskFn fn;

    Task(TimePoint dispatchTime, TaskFn &&fn) : dispatchTime(dispatchTime), fn(std::move(fn)) {}

    bool operator<(const Task &other) const {
      // Have the earliest tasks be at the front of the queue.
      return dispatchTime > other.dispatchTime;
    }
  };

  std::mutex queueLock_;
  std::condition_variable loopCv_;
  std::priority_queue<Task> queue_;
  std::atomic<bool> running_{true};
  std::string threadName_;
  std::thread thread_;
};

TaskDispatchThread::TaskDispatchThread(std::string threadName, int /*priorityOffset*/) noexcept
    : impl_(std::make_shared<Impl>(std::move(threadName))) {
  impl_->start();
}

TaskDispatchThread::~TaskDispatchThread() noexcept {
  impl_->quit();
}

bool TaskDispatchThread::isOnThread() noexcept {
  return impl_->isOnThread();
}

bool TaskDispatchThread::isRunning() noexcept {
  return impl_->isRunning();
}

void TaskDispatchThread::runAsync(TaskFn &&task, std::chrono::milliseconds delayMs) noexcept {
  impl_->runAsync(std::move(task), delayMs);
}

void TaskDispatchThread::runSync(TaskFn &&task) noexcept {
  impl_->runSync(std::move(task));
}

void TaskDispatchThread::quit() noexcept {
  impl_->quit();
}

TaskDispatchThread::Impl::Impl(std::string &&threadName) noexcept : threadName_(std::move(threadName)) {}

TaskDispatchThread::Impl::~Impl() noexcept {
  quit();
}

void TaskDispatchThread::Impl::start() noexcept {
  thread_ = std::thread([self = shared_from_this()]() { self->loop(); });
}

bool TaskDispatchThread::Impl::isOnThread() noexcept {
  return std::this_thread::get_id() == thread_.get_id();
}

bool TaskDispatchThread::Impl::isRunning() noexcept {
  return running_;
}

void TaskDispatchThread::Impl::runAsync(TaskFn &&task, std::chrono::milliseconds delayMs) noexcept {
  if (!running_) {
    return;
  }
  std::lock_guard<std::mutex> guard(queueLock_);
  auto dispatchTime = std::chrono::system_clock::now() + delayMs;
  queue_.emplace(dispatchTime, std::move(task));
  loopCv_.notify_one();
}

void TaskDispatchThread::Impl::runSync(TaskFn &&task) noexcept {
  std::promise<void> promise;
  runAsync([&]() {
    if (running_) {
      task();
    }
    promise.set_value();
  });
  promise.get_future().wait();
}

void TaskDispatchThread::Impl::quit() noexcept {
  if (!running_) {
    return;
  }
  running_ = false;
  loopCv_.notify_one();
  if (thread_.joinable()) {
    if (!isOnThread()) {
      thread_.join();
    } else {
      thread_.detach();
    }
  }
}

void TaskDispatchThread::Impl::loop() noexcept {
  if (!threadName_.empty()) {
    folly::setThreadName(threadName_);
  }
  while (running_) {
    std::unique_lock<std::mutex> lock(queueLock_);
    loopCv_.wait(lock, [&]() { return !running_ || !queue_.empty(); });
    while (!queue_.empty()) {
      auto task = queue_.top();
      auto now = std::chrono::system_clock::now();
      if (task.dispatchTime > now) {
        if (running_) {
          loopCv_.wait_until(lock, task.dispatchTime);
        } else {
          // Shutting down, skip all the delayed tasks that are not to be
          // executed yet
          queue_.pop();
        }
        continue;
      }

      queue_.pop();
      lock.unlock();
      task.fn();
      lock.lock();
    }
  }
}

} //  namespace facebook::react
