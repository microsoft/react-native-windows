// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <winrt/facebook.react.h>

#include "MessageQueueShim.h"

using namespace ::facebook::react::test;
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
	};
}
