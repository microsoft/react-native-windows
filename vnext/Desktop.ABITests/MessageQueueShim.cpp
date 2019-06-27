#include "MessageQueueShim.h"

namespace ABITests
{
	MessageQueueShim::MessageQueueShim()
	: m_messageQueueThread { std::make_unique<::facebook::react::test::TestMessageQueueThread>() }
	{
	}

	MessageQueueShim::MessageQueueShim(std::unique_ptr<::facebook::react::test::TestMessageQueueThread> messageQueueThread)
	: m_messageQueueThread { std::move(messageQueueThread) }
	{
	}

	void MessageQueueShim::Run(::winrt::facebook::react::QueueItem const& item) const
	{
		m_messageQueueThread->runOnQueue(item);
	}

	void MessageQueueShim::RunSync(::winrt::facebook::react::QueueItem const& item) const
	{
		m_messageQueueThread->runOnQueueSync(item);
	}

	void MessageQueueShim::QuitSync() const
	{
		m_messageQueueThread->quitSynchronous();
	}
}
