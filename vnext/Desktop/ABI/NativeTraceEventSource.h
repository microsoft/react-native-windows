#pragma once
#include "NativeTraceEventSource.g.h"

namespace winrt::facebook::react::implementation
{
    struct NativeTraceEventSource
    {
        NativeTraceEventSource() = default;

        static uint32_t InitializeTracing(facebook::react::INativeTraceHandler const& handler);
        static void UninitializeTracing(uint32_t cookie);
    };
}
namespace winrt::facebook::react::factory_implementation
{
    struct NativeTraceEventSource : NativeTraceEventSourceT<NativeTraceEventSource, implementation::NativeTraceEventSource>
    {
    };
}
