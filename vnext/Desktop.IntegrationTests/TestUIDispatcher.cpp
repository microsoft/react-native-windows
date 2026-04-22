// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TestUIDispatcher.h"

namespace Microsoft::React::Test {

TestUIDispatcher::TestUIDispatcher(winrt::Microsoft::UI::Dispatching::DispatcherQueue const &dispatcherQueue)
    : m_dispatcherQueue{dispatcherQueue} {
  m_dispatcherQueue.TryEnqueue([self = get_strong()]() noexcept { self->m_threadId = GetCurrentThreadId(); });
}

bool TestUIDispatcher::HasThreadAccess() {
  return m_threadId == GetCurrentThreadId();
}

void TestUIDispatcher::Post(msrn::ReactDispatcherCallback const &callback) {
  m_dispatcherQueue.TryEnqueue([callback]() noexcept { callback(); });
}

} // namespace Microsoft::React::Test
