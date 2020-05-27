// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <cxxreact/MessageQueueThread.h>
#include <winrt/Microsoft.React.h>
#include <queue>

namespace ABITests {

class SimpleMessageQueue : public winrt::implements<SimpleMessageQueue, ::winrt::Microsoft::React::IMessageQueue> {
 public:
  void Run(::winrt::Microsoft::React::QueueItem const &item) const;
  void RunSync(::winrt::Microsoft::React::QueueItem const &item) const;
  void QuitSync() const;

  bool IsEmpty() const noexcept;
  bool DispatchOne();

 private:
  mutable std::queue<::winrt::Microsoft::React::QueueItem> m_items;
};

} // namespace ABITests
