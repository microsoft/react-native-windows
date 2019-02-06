// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <cassert>
#include <limits>

#include <MemoryTracker.h>

namespace facebook { namespace react {

MemoryTracker::MemoryTracker(std::shared_ptr<MessageQueueThread>&& callbackMessageQueueThread) noexcept
	: m_callbackMessageQueueThread { std::move(callbackMessageQueueThread) }
{
}

size_t MemoryTracker::GetCurrentMemoryUsage() const noexcept
{
	std::lock_guard<std::recursive_mutex> lockGuard { m_mutex };
	assert(m_isInitialized);
	return m_currentMemoryUsage;
}

size_t MemoryTracker::GetPeakMemoryUsage() const noexcept
{
	std::lock_guard<std::recursive_mutex> lockGuard { m_mutex };
	assert(m_isInitialized);
	return m_peakMemoryUsage;
}

std::shared_ptr<MessageQueueThread> MemoryTracker::GetCallbackMessageQueueThread() const noexcept
{
	return m_callbackMessageQueueThread;
}

void MemoryTracker::SetCallbackMessageQueueThread(std::shared_ptr<MessageQueueThread>&& messageQueueThread) noexcept
{
	assert(messageQueueThread);
	assert(!m_isInitialized);
	m_callbackMessageQueueThread = std::move(messageQueueThread);
}

CallbackRegistrationCookie MemoryTracker::AddThresholdCallback(size_t threshold, std::chrono::milliseconds minCallbackInterval, MemoryThresholdCallback&& callback) noexcept
{
	std::lock_guard<std::recursive_mutex> lockGuard { m_mutex };
	assert(m_nextCookie < std::numeric_limits<decltype(m_nextCookie)>::max());
	CallbackRegistrationCookie cookie = m_nextCookie++;
#if DEBUG
	bool success = false;
	std::tie(std::ignore, success) =
#endif
	m_thresholdCallbackRecords.emplace(cookie, ThresholdCallbackRecord{threshold, minCallbackInterval, std::move(callback)});
#if DEBUG
	assert(success);
#endif
	return cookie;
}

bool MemoryTracker::RemoveThresholdCallback(CallbackRegistrationCookie cookie) noexcept
{
	std::lock_guard<std::recursive_mutex> lockGuard{ m_mutex };
	return m_thresholdCallbackRecords.erase(cookie) == 1;
}

void MemoryTracker::Initialize(size_t initialMemoryUsage) noexcept
{
	std::lock_guard<std::recursive_mutex> lockGuard { m_mutex };
	assert(!m_isInitialized);
	m_isInitialized = true;
	OnAllocation(initialMemoryUsage);
}

void MemoryTracker::OnAllocation(size_t size) noexcept
{
	std::lock_guard<std::recursive_mutex> lockGuard{ m_mutex };
	// detect overflow
	assert(std::numeric_limits<decltype(m_currentMemoryUsage)>::max() - m_currentMemoryUsage >= size);

	assert(m_isInitialized);
	assert(m_callbackMessageQueueThread);

	m_currentMemoryUsage += size;

	if (m_currentMemoryUsage > m_peakMemoryUsage)
	{
		m_peakMemoryUsage = m_currentMemoryUsage;
	}

	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

	for (auto& record : m_thresholdCallbackRecords)
	{
		if (m_currentMemoryUsage >= record.second.Threshold &&
			currentTime > record.second.LastNotificationTime + record.second.MinCallbackInterval)
		{
			m_callbackMessageQueueThread->runOnQueue([callback = record.second.Callback, currentMemoryUsage = m_currentMemoryUsage]{ callback(currentMemoryUsage); });
			record.second.LastNotificationTime = currentTime;
		}
	}
}

void MemoryTracker::OnDeallocation(size_t size) noexcept
{
	std::lock_guard<std::recursive_mutex> lockGuard{ m_mutex };
	assert(m_isInitialized);
	assert(size <= m_currentMemoryUsage);
	m_currentMemoryUsage -= size;
}

MemoryTracker::ThresholdCallbackRecord::ThresholdCallbackRecord(size_t threshold, std::chrono::milliseconds minCallbackInterval, MemoryThresholdCallback&& callback) noexcept
	: Threshold{ threshold}
	, MinCallbackInterval{ minCallbackInterval }
	, Callback{std::move(callback) }
{
}

std::shared_ptr<MemoryTracker> CreateMemoryTracker(std::shared_ptr<MessageQueueThread>&& callbackMessageQueueThread) noexcept
{
	return std::make_shared<MemoryTracker>(std::move(callbackMessageQueueThread));
}

}}
