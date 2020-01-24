// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "AsyncActionQueue.h"

namespace Mso::React {

//=============================================================================================
// AsyncActionQueue implementation
//=============================================================================================

AsyncActionQueue::AsyncActionQueue(Mso::DispatchQueue const &queue) noexcept : m_queue{queue} {}

Mso::Future<void> AsyncActionQueue::PostAction(AsyncAction &&action) noexcept {
  Entry entry{std::move(action), Mso::Promise<void>{}};
  Mso::Future<void> result = entry.Result.AsFuture();
  auto &actionQueue = m_actions.Load();
  if (!m_isInvoking.Load() && actionQueue.empty()) {
    InvokeAction(std::move(entry));
  } else {
    actionQueue.push_back(std::move(entry));
  }

  return result;
}

Mso::Future<void> AsyncActionQueue::PostActions(std::initializer_list<AsyncAction> actions) noexcept {
  // Return Future for the last action in the list
  Mso::Future<void> result;
  for (auto &action : actions) {
    // We must copy action because the initialize_list is read-only (a bug in the standard?).
    result = PostAction(Mso::Copy(action));
  }

  if (!result) {
    result = Mso::MakeSucceededFuture();
  }

  return result;
}

void AsyncActionQueue::InvokeAction(Entry &&entry) noexcept {
  Mso::Internal::VerifyIsInQueueElseCrash(m_queue);

  m_isInvoking.Store(true);
  auto actionResult = entry.Action();
  actionResult.Then(
      m_executor, [entry = std::move(entry), spThis = Mso::CntPtr{this}](Mso::Maybe<void> &&result) mutable noexcept {
        spThis->CompleteAction(std::move(entry), std::move(result));
      });
}

void AsyncActionQueue::CompleteAction(Entry &&entry, Mso::Maybe<void> &&result) noexcept {
  // Complete the action
  entry.Result.SetValue(std::move(result));
  entry = {nullptr, nullptr}; // Release the action before the next one starts

  // Start the next action from the queue.
  auto &actionQueue = m_actions.Load();
  if (!actionQueue.empty()) {
    auto nextActionEntry = std::move(actionQueue[0]);
    actionQueue.erase(actionQueue.begin());
    InvokeAction(std::move(nextActionEntry));
  } else {
    m_isInvoking.Store(false);
  }
}

Mso::DispatchQueue const &AsyncActionQueue::Queue() noexcept {
  return m_queue;
}

} // namespace Mso::React
