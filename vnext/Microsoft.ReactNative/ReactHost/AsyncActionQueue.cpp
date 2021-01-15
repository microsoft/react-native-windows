// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AsyncActionQueue.h"

namespace Mso::React {

//=============================================================================================
// AsyncActionQueue implementation
//=============================================================================================

AsyncActionQueue::AsyncActionQueue(Mso::DispatchQueue const &queue) noexcept : m_queue{queue} {}

Mso::Future<void> AsyncActionQueue::PostAction(AsyncAction &&action) noexcept {
  Mso::Internal::VerifyIsInQueueElseCrash(m_queue);

  Entry entry{std::move(action), Mso::Promise<void>{}};
  Mso::Future<void> result = entry.Result.AsFuture();
  if (!m_isInvoking && m_actions.empty()) {
    InvokeAction(std::move(entry));
  } else {
    m_actions.push_back(std::move(entry));
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
  m_isInvoking = true;
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
  if (!m_actions.empty()) {
    auto nextActionEntry = std::move(m_actions[0]);
    m_actions.erase(m_actions.begin());
    InvokeAction(std::move(nextActionEntry));
  } else {
    m_isInvoking = false;
  }
}

Mso::DispatchQueue const &AsyncActionQueue::Queue() noexcept {
  return m_queue;
}

} // namespace Mso::React
