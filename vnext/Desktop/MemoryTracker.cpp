#include "pch.h"
#include "MemoryTracker.h"
#include "MemoryTracker.g.cpp"
#include "MessageQueueShim.h"

namespace winrt::facebook::react::implementation
{
	MemoryTracker::MemoryTracker(facebook::react::IMessageQueue const& callbackMessageQueue)
	{
		m_internalMemoryTracker = ::facebook::react::CreateMemoryTracker(std::make_shared<::facebook::react::MessageQueueShim>(callbackMessageQueue));
	}

	uint64_t MemoryTracker::CurrentMemoryUsage()
	{
		return m_internalMemoryTracker->GetCurrentMemoryUsage();
	}

	uint64_t MemoryTracker::PeakMemoryUsage()
	{
		return m_internalMemoryTracker->GetPeakMemoryUsage();
	}

	facebook::react::IMessageQueue MemoryTracker::CallbackMessageQueue()
	{
		return m_abiCallbackMessageQueue;
	}

	void MemoryTracker::CallbackMessageQueue(facebook::react::IMessageQueue value)
	{
		m_internalMemoryTracker->SetCallbackMessageQueueThread(std::make_shared<::facebook::react::MessageQueueShim>(value));
	}

	uint32_t MemoryTracker::AddThresholdHandler(uint64_t threshold, uint32_t minCallbackIntervalInMilliseconds, facebook::react::MemoryThresholdHandler const& handler)
	{
		return static_cast<uint32_t>(m_internalMemoryTracker->AddThresholdCallback(
			threshold,
			std::chrono::milliseconds(minCallbackIntervalInMilliseconds),
			handler));
	}

	bool MemoryTracker::RemoveThresholdHandler(uint32_t token)
	{
		return m_internalMemoryTracker->RemoveThresholdCallback(token);
	}
}
