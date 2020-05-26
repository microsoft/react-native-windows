#include "pch.h"

#include "MessageQueueShim.h"

namespace Microsoft::React {

MessageQueueShim::MessageQueueShim(const ::winrt::Microsoft::React::IMessageQueue &abiMessageQueue)
    : m_abiMessageQueue{abiMessageQueue} {}

void MessageQueueShim::runOnQueue(std::function<void()> &&item) {
  m_abiMessageQueue.Run(std::move(item));
}

void MessageQueueShim::runOnQueueSync(std::function<void()> &&item) {
  m_abiMessageQueue.RunSync(std::move(item));
}

void MessageQueueShim::quitSynchronous() {
  m_abiMessageQueue.QuitSync();
}

} // namespace Microsoft::React
