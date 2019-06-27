#pragma once

#include <IntegrationTests/TestMessageQueueThread.h>
#include <winrt/facebook.react.h>

namespace ABITests
{
	class MessageQueueShim : public  winrt::implements<MessageQueueShim, ::winrt::facebook::react::IMessageQueue>
	{
	public:
		MessageQueueShim();
		MessageQueueShim(std::unique_ptr<::facebook::react::test::TestMessageQueueThread> messageQueueThread);
		void Run(::winrt::facebook::react::QueueItem const& item) const;
		void RunSync(::winrt::facebook::react::QueueItem const& item) const;
		void QuitSync() const;

	private:
		std::unique_ptr<::facebook::react::test::TestMessageQueueThread> m_messageQueueThread;
	};

}
