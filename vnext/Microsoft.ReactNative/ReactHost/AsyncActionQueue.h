// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "MsoUtils.h"
#include "activeObject/activeObject.h"
#include "future/future.h"
#include "object/refCountedObject.h"

namespace Mso::React {

//! The async action is a Functor that returns void Future.
using AsyncAction = Mso::Functor<Mso::Future<void>()>;

//! The queue that executes actions in the sequential order.
//! Each action returns Mso::Future<void> to indicate its completion.
//! The next action does not start until the previous action is completed.
struct AsyncActionQueue final : Mso::RefCountedObjectNoVTable<Mso::RefCountStrategy::WeakRef, AsyncActionQueue> {
  //! Creates a new AsyncActionQueue that is based on the provided sequential queue.
  AsyncActionQueue(Mso::DispatchQueue const &queue) noexcept;
  AsyncActionQueue(AsyncActionQueue const &other) = delete;
  AsyncActionQueue &operator=(AsyncActionQueue const &other) = delete;

  //! Posts a new action to the AsyncActionQueue and returns Future indicating when it is completed.
  Mso::Future<void> PostAction(AsyncAction &&action) noexcept;

  //! Posts a new action to the AsyncActionQueue and returns Future indicating when the last one is completed.
  //! For the empty list it returns succeeded Future immediately.
  Mso::Future<void> PostActions(std::initializer_list<AsyncAction> actions) noexcept;

  //! Returns the queue associated with the AsyncActionQueue.
  Mso::DispatchQueue const &Queue() noexcept;

 private:
  struct Entry {
    AsyncAction Action;
    Mso::Promise<void> Result;
  };

 private:
  //! Invokes action in the m_queue and observes its result.
  void InvokeAction(Entry &&entry) noexcept;

  //! Completes the action, and then starts the next one.
  void CompleteAction(Entry &&entry, Mso::Maybe<void> &&result) noexcept;

 private:
  const Mso::DispatchQueue m_queue;
  const Mso::InvokeElsePostExecutor m_executor{m_queue};
  std::vector<Entry> m_actions;
  bool m_isInvoking{false};
};

} // namespace Mso::React
