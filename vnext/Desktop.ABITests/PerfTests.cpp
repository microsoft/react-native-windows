// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <IntegrationTests/TestMessageQueueThread.h>
#include <winrt/facebook.react.h>

#include <ReactWindowsCore/Logging.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::facebook::react;
using namespace winrt;

namespace ABITests
{
	TEST_CLASS(PerfTests)
	{
		static const uint32_t iterations = 100000000;

		static void HandleLogEvents(::winrt::facebook::react::LogLevel l, hstring const& m)
		{}

		TEST_METHOD(TimeNewAbiInitializeLogging01)
		{
			// ensure the DLL has been loaded before starting perf measurements
			uint32_t loggingRegistrationToken = NativeLogEventSource::InitializeLogging(HandleLogEvents);

			LARGE_INTEGER accu{0}, a{0}, b{0};

			NativeLogHandler logHandler = HandleLogEvents;

			QueryPerformanceCounter(&a);

			for(int i = 0; i < iterations; ++i)
			{
				NativeLogEventSource::InitializeLogging(logHandler);
			}

			QueryPerformanceCounter(&b);
			accu.QuadPart = b.QuadPart - a.QuadPart;

			PrintResult("TimeNewAbiInitializeLogging01", iterations, accu.QuadPart);

			NativeLogEventSource::UninitializeLogging(loggingRegistrationToken);
		}

		TEST_METHOD(TimeNewAbiInitializeLogging02)
		{
			// ensure the DLL has been loaded before starting perf measurements
			uint32_t loggingRegistrationToken = NativeLogEventSource::InitializeLogging(HandleLogEvents);

			LARGE_INTEGER accu{0}, a{0}, b{0};

			for(int i = 0; i < iterations; ++i)
			{
				QueryPerformanceCounter(&a);

				NativeLogEventSource::InitializeLogging(HandleLogEvents);

				QueryPerformanceCounter(&b);
				accu.QuadPart += (b.QuadPart - a.QuadPart);
			}

			PrintResult("TimeNewAbiInitializeLogging02", iterations, accu.QuadPart);

			NativeLogEventSource::UninitializeLogging(loggingRegistrationToken);
		}

		TEST_METHOD(TimeOldAbiInitializeLogging01)
		{
			LARGE_INTEGER accu{0}, a{0}, b{0};

			QueryPerformanceCounter(&a);

			for(int i = 0; i < iterations; ++i)
			{
				auto callback = [](::facebook::react::RCTLogLevel l, const char* m){};
				::facebook::react::InitializeLogging(std::move(callback));
			}

			QueryPerformanceCounter(&b);
			accu.QuadPart += b.QuadPart - a.QuadPart;

			PrintResult("TimeOldAbiInitializeLogging01", iterations, accu.QuadPart);
		}

		TEST_METHOD(TimeOldAbiInitializeLogging02)
		{
			LARGE_INTEGER accu{0}, a{0}, b{0};

			for(int i = 0; i < iterations; ++i)
			{
				auto callback = [](::facebook::react::RCTLogLevel l, const char* m){};

				QueryPerformanceCounter(&a);

				::facebook::react::InitializeLogging(std::move(callback));

				QueryPerformanceCounter(&b);
				accu.QuadPart += b.QuadPart - a.QuadPart;
			}

			PrintResult("TimeOldAbiInitializeLogging02", iterations, accu.QuadPart);
		}

		static void PrintResult(const char* testName, uint32_t iterations, LONGLONG accu)
		{
			LARGE_INTEGER freq{0};
			Assert::IsTrue(QueryPerformanceFrequency(&freq));
			std::stringstream ss;

			double time = static_cast<double>(accu)/freq.QuadPart;
			ss << testName << ": its=" << iterations << "; accu=" << accu << "; freq=" << freq.QuadPart << "; tt=" << time << " s; tc=" << time/iterations * std::pow(10, 9) << " ns";
			Logger::WriteMessage(ss.str().c_str());
		}
	};

}
