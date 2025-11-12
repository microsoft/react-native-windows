/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// [Windows] Brought forward from react-native - switch to using react-native/ReactCxxPlatform/react/threading version
// once we integrate that far

#pragma once

#include <chrono>
#include <functional>
#include <memory>

namespace facebook::react {

/**
 * Representation of a thread looper which can add tasks to a queue and handle
 * the synchronization of callers.
 */
class TaskDispatchThread {
 public:
  using TaskFn = std::function<void()>;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

  TaskDispatchThread(std::string threadName = "", int priorityOffset = 0) noexcept;

  ~TaskDispatchThread() noexcept;

  /** Return true if the current thread is the same as this looper's thread. */
  bool isOnThread() noexcept;

  /** Return true until TaskDispatchThread.quit() is called  */
  bool isRunning() noexcept;

  /** Add task to the queue and return immediately. */
  void runAsync(TaskFn &&task, std::chrono::milliseconds delayMs = std::chrono::milliseconds::zero()) noexcept;

  /** Add task to the queue and wait until it has completed. */
  void runSync(TaskFn &&task) noexcept;

  /** Shut down and clean up the thread. */
  void quit() noexcept;

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

} // namespace facebook::react
