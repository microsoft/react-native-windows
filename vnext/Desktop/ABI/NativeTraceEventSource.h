#pragma once
#include "Microsoft.React.NativeTraceEventSource.g.h"

namespace winrt::Microsoft::React::implementation {
struct NativeTraceEventSource {
  NativeTraceEventSource() = default;

  static uint32_t InitializeTracing(Microsoft::React::INativeTraceHandler const &handler);
  static void UninitializeTracing(uint32_t cookie);
};
} // namespace winrt::Microsoft::React::implementation
namespace winrt::Microsoft::React::factory_implementation {
struct NativeTraceEventSource
    : NativeTraceEventSourceT<NativeTraceEventSource, implementation::NativeTraceEventSource> {};
} // namespace winrt::Microsoft::React::factory_implementation
