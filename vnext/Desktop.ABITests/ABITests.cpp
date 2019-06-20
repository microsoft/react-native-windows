// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <winrt/facebook.react.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::facebook::react;
using namespace winrt;

namespace ABITests
{

TEST_CLASS(NativeTraceEventTests)
{
	struct TestTraceHandler : public facebook::react::INativeTraceHandler
	{
		// TODO:
		// Figure out what the actual definition of the interface is we're implementing here.

		// void JSBeginSection(param::hstring const& profileName, param::hstring const& args) const override
		// {
		// 	if (this->OnJSBeginSection)
		// 	{
		// 		this->OnJSBeginSection(L"<profileName>", L"<args>");
		// 	}
		// }

		// void JSEndSection() const override {};
		// void JSBeginAsyncSection(param::hstring const& profileName, uint32_t cookie) const override{}
		// void JSEndAsyncSection(param::hstring const& profileName, uint32_t cookie) const override{}
		// void JSCounter(param::hstring const& profileName, uint32_t value) const override{}
		// void NativeBeginSection(param::hstring const& profileName, param::hstring const& args) const override{}
		// void NativeEndSection(param::hstring const& profileName, param::hstring const& args, int64_t durationInNanoseconds) const override{}

		std::function<void(const wchar_t*, const wchar_t*)> OnJSBeginSection;
	};

	TEST_METHOD(NativeTraceEventHandler_Registered)
	{
		init_apartment(winrt::apartment_type::single_threaded);

		TestTraceHandler handler;

		NativeTraceEventSource::InitializeTracing(handler);

		// TODO:
		// Interact with RNW in such a way that it incurs tracing, then verify the respective trace
		// message. To do that, we might have to interact via the existing unsafe ABI, then gradually
		// replace those interactions with the safe ABI as we build it up.
	}
};

}
