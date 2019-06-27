#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <winrt/facebook.react.h>

namespace facebook::react
{
	class MessageQueueShim : public MessageQueueThread
	{
	public:
		MessageQueueShim(const ::winrt::facebook::react::IMessageQueue& abiMessageQueue);
		virtual void runOnQueue(std::function<void()>&& item) override;
		virtual void runOnQueueSync(std::function<void()>&& item) override;
		virtual void quitSynchronous() override;

	private:
		::winrt::facebook::react::IMessageQueue m_abiMessageQueue;
	};
}
