#include "pch.h"

#include <V8JsiRuntime.h>
#include "V8JSIRuntimeHolder.h"

#include <atomic>
#include <queue>

using namespace facebook;
using namespace facebook::react;

namespace facebook {
namespace react {

class ReactQueueBackedTaskRunner {
 public:
  ReactQueueBackedTaskRunner(std::shared_ptr<facebook::react::MessageQueueThread> jsQueue)
      : jsQueue_(std::move(jsQueue)) {
    // This thread handles the delayed tasks
    std::thread(&ReactQueueBackedTaskRunner::TimerFunc, this).detach();
  }

  ~ReactQueueBackedTaskRunner() {
    stop_requested_ = true;

    delayed_tasks_available_cond_.notify_all();

    // Wait until the thread is done
    std::unique_lock<std::mutex> worker_stopped_lock(delayed_queue_access_mutex_);
    delayed_tasks_available_cond_.wait(worker_stopped_lock, [this]() { return timer_stopped_; });
  }

  void PostTask(std::unique_ptr<v8runtime::JSITask> task) {
    std::shared_ptr<v8runtime::JSITask> shared_task(task.release());
    jsQueue_->runOnQueue([shared_task2 = std::move(shared_task)]() { shared_task2->run(); });
  }

  void PostDelayedTask(std::unique_ptr<v8runtime::JSITask> task, double delay_in_seconds) {
    if (delay_in_seconds == 0) {
      PostTask(std::move(task));
      return;
    } else
      return;

    double deadline = std::chrono::steady_clock::now().time_since_epoch().count() +
        delay_in_seconds * std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

    std::lock_guard<std::mutex> lock(delayed_queue_access_mutex_);
    delayed_task_queue_.push(std::make_pair(deadline, std::move(task)));
    delayed_tasks_available_cond_.notify_all();
  }

  void PostIdleTask(std::unique_ptr<v8runtime::JSIIdleTask> task) {
    std::abort(); // Not supported as of now.
  }

  bool IdleTasksEnabled() {
    return false;
  };

 private:
  void TimerFunc() {
    while (true) {
      std::unique_lock<std::mutex> delayed_lock(delayed_queue_access_mutex_);
      delayed_tasks_available_cond_.wait(
          delayed_lock, [this]() { return !delayed_task_queue_.empty() || stop_requested_; });

      if (stop_requested_)
        break;

      if (delayed_task_queue_.empty())
        continue; // Loop back and block the thread.

      std::queue<std::unique_ptr<v8runtime::JSITask>> new_ready_tasks;

      do {
        const DelayedEntry &delayed_entry = delayed_task_queue_.top();
        if (delayed_entry.first < std::chrono::steady_clock::now().time_since_epoch().count()) {
          new_ready_tasks.push(std::move(const_cast<DelayedEntry &>(delayed_entry).second));
          delayed_task_queue_.pop();
        } else {
          // The rest are not ready
          break;
        }

      } while (!delayed_task_queue_.empty());

      delayed_lock.unlock();

      if (!new_ready_tasks.empty()) {
        do {
          PostTask(std::move(new_ready_tasks.front()));
        } while (!new_ready_tasks.empty());
      }

      if (!delayed_task_queue_.empty()) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for a second and loop back and recompute again.
      } // else loop back and block the thread.
    }

    timer_stopped_ = true;
    delayed_tasks_available_cond_.notify_all();
  }

 private:
  using DelayedEntry = std::pair<double, std::unique_ptr<v8runtime::JSITask>>;

  // Define a comparison operator for the delayed_task_queue_ to make sure
  // that the unique_ptr in the DelayedEntry is not accessed in the priority
  // queue. This is necessary because we have to reset the unique_ptr when we
  // remove a DelayedEntry from the priority queue.
  struct DelayedEntryCompare {
    bool operator()(DelayedEntry &left, DelayedEntry &right) {
      return left.first > right.first;
    }
  };

  std::shared_ptr<facebook::react::MessageQueueThread> jsQueue_;

  std::priority_queue<DelayedEntry, std::vector<DelayedEntry>, DelayedEntryCompare> delayed_task_queue_;

  std::mutex delayed_queue_access_mutex_;
  std::condition_variable delayed_tasks_available_cond_;

  std::atomic<bool> stop_requested_{false};
  bool timer_stopped_{false};
};

class TaskRunnerAdapter : public v8runtime::JSITaskRunner {
 public:
  TaskRunnerAdapter(std::shared_ptr<ReactQueueBackedTaskRunner> taskRunner) : taskRunner_(std::move(taskRunner)) {}

  void postTask(std::unique_ptr<v8runtime::JSITask> task) override {
    taskRunner_->PostTask(std::move(task));
  }

  void postDelayedTask(std::unique_ptr<v8runtime::JSITask> task, double delay_in_seconds) override {
    taskRunner_->PostDelayedTask(std::move(task), delay_in_seconds);
  }

  void postIdleTask(std::unique_ptr<v8runtime::JSIIdleTask> task) override {
    std::abort();
  }

  bool IdleTasksEnabled() override {
    return false;
  }

 private:
  TaskRunnerAdapter(const TaskRunnerAdapter &) = delete;
  TaskRunnerAdapter &operator=(const TaskRunnerAdapter &) = delete;

  std::shared_ptr<ReactQueueBackedTaskRunner> taskRunner_;
};

std::shared_ptr<facebook::jsi::Runtime> V8JSIRuntimeHolder::getRuntime() noexcept {
  std::call_once(once_flag_, [this]() { initRuntime(); });

  if (!runtime_)
    std::terminate();

  // V8JsiRuntime is not thread safe as of now.
  if (own_thread_id_ != std::this_thread::get_id())
    std::terminate();

  return runtime_;
}

void V8JSIRuntimeHolder::initRuntime() noexcept {
  v8runtime::V8RuntimeArgs args{};

  if (debuggerPort_ > 0)
    args.inspectorPort = debuggerPort_;

  args.enableInspector = useDirectDebugger_;
  args.waitForDebugger = debuggerBreakOnNextLine_;

  args.foreground_task_runner =
      std::make_unique<TaskRunnerAdapter>(std::make_shared<ReactQueueBackedTaskRunner>(jsQueue_));
  args.scriptStore = std::move(scriptStore_);
  args.preparedScriptStore = std::move(preparedScriptStore_);

  runtime_ = v8runtime::makeV8Runtime(std::move(args));

  own_thread_id_ = std::this_thread::get_id();
}

} // namespace react
} // namespace facebook
