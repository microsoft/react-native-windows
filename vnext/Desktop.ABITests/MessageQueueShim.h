#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <winrt/facebook.react.h>

namespace ABITests {
class MessageQueueShim : public winrt::implements<MessageQueueShim, ::winrt::facebook::react::IMessageQueue> {
 public:
  MessageQueueShim();
  MessageQueueShim(std::shared_ptr<::facebook::react::MessageQueueThread> messageQueueThread);
  void Run(::winrt::facebook::react::QueueItem const &item) const;
  void RunSync(::winrt::facebook::react::QueueItem const &item) const;
  void QuitSync() const;

 private:
  std::shared_ptr<::facebook::react::MessageQueueThread> m_messageQueueThread;
};
} // namespace ABITests
