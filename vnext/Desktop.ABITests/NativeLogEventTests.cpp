// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <vector>
#include <winrt/facebook.react.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::facebook::react;
using namespace winrt;

namespace ABITests
{

TEST_CLASS(NativeLogEventTests)
{
	struct TestTraceHandler : public winrt::implements<TestTraceHandler, ::winrt::facebook::react::INativeTraceHandler>
	{
		void JSBeginSection(param::hstring const& profileName, param::hstring const& args) const
		{
			if (this->OnJSBeginSection)
			{
				this->OnJSBeginSection(((hstring) profileName).c_str(), ((hstring) args).c_str());
			}
		}

		void JSEndSection() const
		{
			if (this->OnJSEndSection)
			{
				this->OnJSEndSection();
			}
		};

		void JSBeginAsyncSection(param::hstring const& profileName, uint32_t cookie) const
		{
			if (this->OnJSBeginAsyncSection)
			{
				this->OnJSBeginAsyncSection(((hstring) profileName).c_str(), cookie);
			}
		}

		void JSEndAsyncSection(param::hstring const& profileName, uint32_t cookie) const
		{
			if (this->OnJSEndAsyncSection)
			{
				this->OnJSEndAsyncSection(((hstring) profileName).c_str(), cookie);
			}
		}

		void JSCounter(param::hstring const& profileName, uint32_t value) const
		{
			if (this->OnJSCounter)
			{
				this->OnJSCounter(((hstring) profileName).c_str(), value);
			}

		}

		void NativeBeginSection(param::hstring const& profileName, param::hstring const& args) const
		{
			if (this->OnNativeBeginSection)
			{
				this->OnNativeBeginSection(((hstring) profileName).c_str(), ((hstring) args).c_str());
			}

		}

		void NativeEndSection(param::hstring const& profileName, param::hstring const& args, int64_t durationInNanoseconds) const
		{
			if (this->OnNativeEndSection)
			{
				this->OnNativeEndSection(((hstring) profileName).c_str(), ((hstring) args).c_str(), durationInNanoseconds);
			}
		}

		std::function<void(const wchar_t*, const wchar_t*)> OnJSBeginSection;
		std::function<void()> OnJSEndSection;
		std::function<void(const wchar_t*, uint32_t)> OnJSBeginAsyncSection;
		std::function<void(const wchar_t*, uint32_t)> OnJSEndAsyncSection;
		std::function<void(const wchar_t*, uint32_t)> OnJSCounter;
		std::function<void(const wchar_t*, const wchar_t*)> OnNativeBeginSection;
		std::function<void(const wchar_t*, const wchar_t*, int64_t)> OnNativeEndSection;
	};

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

	TEST_METHOD(NativeLogEventHandler_Registered)
	{
		init_apartment(winrt::apartment_type::single_threaded);

		// anticipatory, see TODO below
		std::vector<std::pair<::winrt::facebook::react::LogLevel, std::wstring>> logMessages;

		NativeLogHandler handler(
			[&logMessages](::winrt::facebook::react::LogLevel l, hstring const& m)
			{
				logMessages.emplace_back(l, m.c_str());
			}
		);

		NativeLogInitializationGuard initializationGuard(handler);

		// TODO:
		// Interact with RNW in such a way that it incurs logging, then verify the respective log
		// message(s). To do that, we might have to interact via the existing unsafe ABI, then
		// gradually replace those interactions with the safe ABI as we build it up.
	}
};

}
