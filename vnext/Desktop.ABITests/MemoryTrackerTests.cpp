// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <IntegrationTests/TestMessageQueueThread.h>
#include <winrt/facebook.react.h>

#include "MessageQueueShim.h"

using namespace Microsoft::React::Test;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::facebook::react;

namespace ABITests
{
	TEST_CLASS(MemoryTrackerTests)
	{
		TEST_METHOD(Handler_AddedAndRemoved)
		{
			init_apartment(winrt::apartment_type::single_threaded);
			IMessageQueue callbackMessageQueue = ::winrt::make<MessageQueueShim>();
			MemoryTracker tracker { callbackMessageQueue };

			uint32_t registrationToken = tracker.AddThresholdHandler(/* threshold */ 100, /* minCallbackIntervalInMilliseconds */ 100, [](uint64_t currentUsage){});
			Assert::IsTrue(tracker.RemoveThresholdHandler(registrationToken));
		}

		TEST_METHOD(CurrentMemoryUsage_ReturnsInitialValue)
		{
			init_apartment(winrt::apartment_type::single_threaded);
			IMessageQueue callbackMessageQueue = ::winrt::make<MessageQueueShim>();
			MemoryTracker tracker { callbackMessageQueue };

			tracker.Initialize(1000);
			Assert::AreEqual(1000ull, tracker.CurrentMemoryUsage(), L"CurrentMemoryUsage");
		}

		TEST_METHOD(PeakMemoryUsage_ReturnsInitialValue)
		{
			init_apartment(winrt::apartment_type::single_threaded);
			IMessageQueue callbackMessageQueue = ::winrt::make<MessageQueueShim>();
			MemoryTracker tracker { callbackMessageQueue };

			tracker.Initialize(1000);
			Assert::AreEqual(1000ull, tracker.PeakMemoryUsage(), L"PeakMemoryUsage");
		}

		TEST_METHOD(ThresholdHandler_Called)
		{
			init_apartment(winrt::apartment_type::single_threaded);

			DWORD testThreadId = GetCurrentThreadId();

			auto manualMessageQueue = std::make_shared<TestMessageQueueThread>(TestMessageQueueThread::Mode::ManualDispatch);

			IMessageQueue callbackMessageQueue = ::winrt::make<MessageQueueShim>(manualMessageQueue);
			MemoryTracker tracker { callbackMessageQueue };

			tracker.Initialize(500);

			std::vector<uint64_t> actualCallbacks;
			DWORD callbackThreadId;
			uint32_t registrationToken = tracker.AddThresholdHandler(
				/* threshold */ 1000,
				/* minCallbackIntervalInMilliseconds */ 100,
				[&actualCallbacks, &callbackThreadId]
				(uint64_t currentUsage)
				{
					actualCallbacks.push_back(currentUsage);
					callbackThreadId = GetCurrentThreadId();
				});

			tracker.OnAllocation(1000);

			// allow the callback to get dispatched
			Assert::IsTrue(manualMessageQueue->DispatchOne(std::chrono::milliseconds(500)));
			Assert::IsTrue(manualMessageQueue->IsEmpty());

			Assert::AreEqual(1500ull, tracker.CurrentMemoryUsage(), L"CurrentMemoryUsage");
			Assert::AreEqual(1500ull, tracker.PeakMemoryUsage(), L"PeakMemoryUsage");

			Assert::AreEqual(static_cast<size_t>(1), actualCallbacks.size());
			Assert::AreEqual(1500ull, actualCallbacks[0]);

			Assert::AreNotEqual(testThreadId, callbackThreadId);

			Assert::IsTrue(tracker.RemoveThresholdHandler(registrationToken));
		}
	};
}
