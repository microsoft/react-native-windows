// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ManualMessageQueueThread.h"

#include <assert.h>

namespace facebook::react::test {

#pragma region ManualMessageQueueThread members

ManualMessageQueueThread::Lock::Lock(HANDLE mutex) noexcept : m_mutex { mutex }
{
	assert(mutex != NULL);
	DWORD waitResult = WaitForSingleObject(m_mutex, INFINITE);
	assert(waitResult == WAIT_OBJECT_0);
}

ManualMessageQueueThread::Lock::~Lock() noexcept
{
	ReleaseMutex(m_mutex);
	m_mutex = NULL;
}

ManualMessageQueueThread::ManualMessageQueueThread() noexcept
{
	m_memberMutex = CreateMutex(
		/* lpMutexAttributes */ NULL,
		/* bInitialOwner */ FALSE,
		/* lpName */ nullptr);
	assert(m_memberMutex != NULL);

	m_itemPresent = CreateEvent(
		/* lpEventAttributes */ NULL,
		/* bManualReset */ FALSE,
		/* bInitialState */ FALSE,
		/* lpName */ nullptr);
	assert(m_itemPresent != NULL);
}

ManualMessageQueueThread::~ManualMessageQueueThread()
{
	{
		Lock memberLock { m_memberMutex };
		m_state = State::Destroyed;
		assert(m_queue.size() == 0);

		CloseHandle(m_itemPresent);
		m_itemPresent = NULL;
	}

	CloseHandle(m_memberMutex);
	m_memberMutex = NULL;
}

void ManualMessageQueueThread::runOnQueue(VoidFunctor&& func) noexcept
{
	Lock memberLock { m_memberMutex };
	assert(m_state == State::Running);
	m_queue.push(move(func));
	SetEvent(m_itemPresent);
}

void ManualMessageQueueThread::runOnQueueSync(VoidFunctor&& func) noexcept
{
	{
		Lock memberLock { m_memberMutex };
		assert(m_state == State::Running);
	}
	func();
}

void ManualMessageQueueThread::quitSynchronous() noexcept
{
	Lock memberLock { m_memberMutex };
	assert(m_state == State::Running);
	m_state = State::Stopped;
}

#pragma endregion ManualMessageQueueThread members

bool ManualMessageQueueThread::DispatchOne(std::chrono::milliseconds timeout)
{
	switch (WaitForSingleObject(m_itemPresent, static_cast<DWORD>(timeout.count())))
	{
		case WAIT_OBJECT_0:
			{
				VoidFunctor func;
				{
					Lock queueLock(m_memberMutex);
					assert(m_queue.size() > 0);
					func = m_queue.front();
					m_queue.pop();
					if (m_queue.size() > 0)
					{
						SetEvent(m_itemPresent);
					}
				}
				func();
			}
			return true;

		default:
			return false;
	}
}

bool ManualMessageQueueThread::IsEmpty() const
{
	Lock memberLock { m_memberMutex };
	return m_queue.size() > 0;
}

} // namespace facebook::react::test
