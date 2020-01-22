// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "future/future.h"
#include "testCheck.h"

namespace FutureTests {

enum class ExecutorInvoke {
  NoParam,
  RefParam,
  ConstRefParam,
};

struct MockExecutor {
  MockExecutor(ExecutorInvoke invoke) noexcept : m_invoke(invoke) {}

  void Post(Mso::DispatchTask &&task) noexcept {
    Mso::DispatchQueue::ConcurrentQueue().Post(std::move(task));
  }

  template <class Callback>
  auto Invoke(Callback &&callback) noexcept {
    static_assert(noexcept(callback()), "Callback must not throw.");
    TestCheck(m_invoke == ExecutorInvoke::NoParam);
    return callback();
  }

  template <class Callback, class T>
  auto Invoke(Callback &&callback, T &value) noexcept {
    static_assert(noexcept(callback(value)), "Callback must not throw.");
    TestCheck(m_invoke == ExecutorInvoke::RefParam);
    return callback(value);
  }

  template <class Callback, class T>
  auto Invoke(Callback &&callback, const T &value) noexcept {
    static_assert(noexcept(callback(value)), "Callback must not throw.");
    TestCheck(m_invoke == ExecutorInvoke::ConstRefParam);
    return callback(value);
  }

 private:
  ExecutorInvoke m_invoke;
};

struct MockExecutorOnCancel {
  MockExecutorOnCancel(bool shouldVEC, bool *isCancelCalled = nullptr) noexcept
      : m_shouldVEC(shouldVEC), m_isCancelCalled(isCancelCalled) {}

  void Post(Mso::DispatchTask &&task) noexcept {
    auto &cancellation = query_cast<Mso::ICancellationListener &>(*task.Get());
    if (m_shouldVEC) {
      TestCheckCrash(cancellation.OnCancel());
    } else {
      cancellation.OnCancel();
    }

    task = nullptr;

    if (m_isCancelCalled) {
      *m_isCancelCalled = true;
    }
  }

  template <class Callback, class... TArgs>
  auto Invoke(Callback &&callback, TArgs &&... args) noexcept {
    TestCheckFail("Must not be invoked");
    return callback(std::forward<TArgs>(args)...);
  }

 private:
  bool m_shouldVEC;
  bool *m_isCancelCalled{nullptr};
};

struct MockInlineExecutor {
  void Post(Mso::DispatchTask &&task) noexcept {
    task.Get()->Invoke();
    task = nullptr;
  }

  template <class Callback, class... TArgs>
  auto Invoke(Callback &&callback, TArgs &&... args) noexcept {
    return callback(std::forward<TArgs>(args)...);
  }
};

inline Mso::DispatchQueue MakeTestDispatchQueue() noexcept {
  return Mso::DispatchQueue::MakeConcurrentQueue(0);
}

} // namespace FutureTests
