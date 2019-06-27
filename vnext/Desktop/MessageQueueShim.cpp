#include "pch.h"
#include "MessageQueueShim.h"

namespace facebook::react {

	MessageQueueShim::MessageQueueShim(const ::winrt::facebook::react::IMessageQueue& abiMessageQueue) :
		m_abiMessageQueue { abiMessageQueue }
	{
	}

		void MessageQueueShim::runOnQueue(std::function<void()>&& item)
	{
		m_abiMessageQueue.Run(std::move(item));
	}

		void MessageQueueShim::runOnQueueSync(std::function<void()>&& item)
	{
		m_abiMessageQueue.RunSync(std::move(item));
	}

	void MessageQueueShim::quitSynchronous()
	{
		m_abiMessageQueue.QuitSync();
	}

}
