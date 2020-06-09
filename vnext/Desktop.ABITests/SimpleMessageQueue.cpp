// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "SimpleMessageQueue.h"

namespace ABITests {

void SimpleMessageQueue::Run(::winrt::facebook::react::QueueItem const &item) const {
  m_items.push(item);
}

void SimpleMessageQueue::RunSync(::winrt::facebook::react::QueueItem const &item) const {
  item();
}

void SimpleMessageQueue::QuitSync() const {}

bool SimpleMessageQueue::IsEmpty() const noexcept {
  return m_items.empty();
}

bool SimpleMessageQueue::DispatchOne() {
  if (m_items.empty())
    return false;

  m_items.front()();
  m_items.pop();

  return true;
}

} // namespace ABITests
