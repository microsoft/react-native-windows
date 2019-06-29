// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <atomic>
#include <cxxreact/MessageQueueThread.h>
#include <Windows.h>
#include <queue>

namespace facebook::react::test {

	class ManualMessageQueueThread : public facebook::react::MessageQueueThread
	{
		class Lock
		{
		public:
			Lock(HANDLE mutex) noexcept;
			~Lock() noexcept;

		private:
			HANDLE m_mutex = NULL;
		};

	public:
		using VoidFunctor = std::function<void()>;

		ManualMessageQueueThread() noexcept;

		#pragma region MessageQueueThread members

		~ManualMessageQueueThread() override;

		void runOnQueue(VoidFunctor&& func) noexcept override;
		void runOnQueueSync(VoidFunctor&& func) noexcept override;
		void quitSynchronous() noexcept override;

		#pragma endregion MessageQueueThread members

		bool DispatchOne(std::chrono::milliseconds timeout);
		bool IsEmpty() const;

	private:
		enum class State
		{
			Running,
			Stopped,
			Destroyed
		};

		std::atomic<State> m_state { State::Running };
		HANDLE m_itemPresent;
		HANDLE m_memberMutex = NULL;
		std::queue<VoidFunctor> m_queue;
	};

} // namespace facebook::react::test
