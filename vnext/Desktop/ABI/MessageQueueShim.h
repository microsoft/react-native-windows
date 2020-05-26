#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <winrt/Microsoft.React.h>

namespace Microsoft::React {
class MessageQueueShim : public facebook::react::MessageQueueThread {
 public:
  MessageQueueShim(const ::winrt::Microsoft::React::IMessageQueue &abiMessageQueue);
  virtual void runOnQueue(std::function<void()> &&item) override;
  virtual void runOnQueueSync(std::function<void()> &&item) override;
  virtual void quitSynchronous() override;

 private:
  ::winrt::Microsoft::React::IMessageQueue m_abiMessageQueue;
};
} // namespace Microsoft::React
