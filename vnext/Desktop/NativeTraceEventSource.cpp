#include "pch.h"
#include "NativeTraceEventSource.h"
#include "NativeTraceEventSource.g.cpp"

#include <Tracing.h>
#include <unicode.h>

using namespace ::facebook::react::unicode;

namespace winrt::facebook::react::implementation
{
	class InternalHandler : public ::facebook::react::INativeTraceHandler
	{
	public:
		void SetAbiHandler(::winrt::facebook::react::INativeTraceHandler const& abiHandler)
		{
			m_abiHandler = abiHandler;
		}

		virtual void JSBeginSection(const char* profileName, const char* args) noexcept override
		{
			m_abiHandler.JSBeginSection(utf8ToUtf16(profileName), utf8ToUtf16(args));
		}

		virtual void JSEndSection() noexcept override
		{
			m_abiHandler.JSEndSection();
		}

		virtual void JSBeginAsyncSection(const char* profileName, int cookie) noexcept override
		{
			m_abiHandler.JSBeginAsyncSection(utf8ToUtf16(profileName), cookie);
		}

		virtual void JSEndAsyncSection(const char* profileName, int cookie) noexcept override
		{
			m_abiHandler.JSEndAsyncSection(utf8ToUtf16(profileName), cookie);

		}

		virtual void JSCounter(const char* profileName, int value) noexcept override
		{
			m_abiHandler.JSCounter(utf8ToUtf16(profileName), value);
		}

		virtual void NativeBeginSection(const char* profileName, const char* args) noexcept override
		{
			m_abiHandler.NativeBeginSection(utf8ToUtf16(profileName), utf8ToUtf16(args));
		}

		virtual void NativeEndSection(const char* profileName, const char* args, std::chrono::nanoseconds duration) noexcept override
		{
			m_abiHandler.NativeEndSection(utf8ToUtf16(profileName), utf8ToUtf16(args), duration.count());
		}

	private:
		::winrt::facebook::react::INativeTraceHandler m_abiHandler;

	} g_internalHandler;

	void NativeTraceEventSource::InitializeTracing(::winrt::facebook::react::INativeTraceHandler const& handler)
	{
		g_internalHandler.SetAbiHandler(handler);
		::facebook::react::InitializeTracing(&g_internalHandler);
	}
}
