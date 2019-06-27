// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <winrt/facebook.react.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::facebook::react;
using namespace winrt;

namespace ABITests
{

TEST_CLASS(MemoryTrackerTests)
{
	// RAII helper to ensure log handlers get unregistered
	struct NativeLogInitializationGuard
	{
		NativeLogInitializationGuard(::winrt::facebook::react::NativeLogHandler const& handler) noexcept
		{
			m_registrationCookie = NativeLogEventSource::InitializeLogging(handler);
		}

		~NativeLogInitializationGuard() noexcept
		{
			NativeLogEventSource::UninitializeLogging(m_registrationCookie);
		}

	private:
		uint32_t m_registrationCookie;
	};

	TEST_METHOD(MemoryTracker_Created)
	{
		init_apartment(winrt::apartment_type::single_threaded);

		// TODO: implement
		// MemoryTracker tracker();
	}
};

}
