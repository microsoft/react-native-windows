// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>

//#include <Windows.h>
#include <winrt/base.h>
#include <winrt/facebook.react.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace winrt::facebook::react;
using namespace winrt;
// using namespace Windows::Foundation;

TEST_CLASS(ABITests)
{


	struct Handler : public facebook::react::INativeTraceHandler
	{
	};

	TEST_METHOD(NativeTraceEventHandler_Registered)
	{
		init_apartment(winrt::apartment_type::single_threaded);

		Handler handler;

		NativeTraceEventSource::InitializeTracing(handler);
	}
};
