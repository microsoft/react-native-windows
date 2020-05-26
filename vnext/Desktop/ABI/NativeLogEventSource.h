#pragma once
#include "Microsoft.React.NativeLogEventSource.g.h"

namespace winrt::Microsoft::React::implementation {
struct NativeLogEventSource {
  NativeLogEventSource() = default;

  static uint32_t InitializeLogging(Microsoft::React::NativeLogHandler const &handler);
  static void UninitializeLogging(uint32_t cookie);
};
} // namespace winrt::Microsoft::React::implementation
namespace winrt::Microsoft::React::factory_implementation {
struct NativeLogEventSource : NativeLogEventSourceT<NativeLogEventSource, implementation::NativeLogEventSource> {};
} // namespace winrt::Microsoft::React::factory_implementation
