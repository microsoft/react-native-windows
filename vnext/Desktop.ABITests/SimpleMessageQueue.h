// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <cxxreact/MessageQueueThread.h>
#include <winrt/facebook.react.h>
#include <queue>

namespace ABITests {

class SimpleMessageQueue : public winrt::implements<SimpleMessageQueue, ::winrt::facebook::react::IMessageQueue> {
 public:
  void Run(::winrt::facebook::react::QueueItem const &item) const;
  void RunSync(::winrt::facebook::react::QueueItem const &item) const;
  void QuitSync() const;

  bool IsEmpty() const noexcept;
  bool DispatchOne();

 private:
  mutable std::queue<::winrt::facebook::react::QueueItem> m_items;
};

} // namespace ABITests
