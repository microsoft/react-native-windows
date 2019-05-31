#pragma once
#include "NativeTraceEventSource.g.h"

namespace winrt::facebook::react::implementation
{
    struct NativeTraceEventSource
    {
        NativeTraceEventSource() = default;

        static void InitializeTracing(facebook::react::INativeTraceHandler const& handler);
    };
}
namespace winrt::facebook::react::factory_implementation
{
    struct NativeTraceEventSource : NativeTraceEventSourceT<NativeTraceEventSource, implementation::NativeTraceEventSource>
    {
    };
}
