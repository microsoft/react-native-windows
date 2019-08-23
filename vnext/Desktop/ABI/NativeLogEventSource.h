#pragma once
#include "NativeLogEventSource.g.h"

namespace winrt::facebook::react::implementation {
struct NativeLogEventSource {
  NativeLogEventSource() = default;

  static uint32_t InitializeLogging(
      facebook::react::NativeLogHandler const &handler);
  static void UninitializeLogging(uint32_t cookie);
};
} // namespace winrt::facebook::react::implementation
namespace winrt::facebook::react::factory_implementation {
struct NativeLogEventSource : NativeLogEventSourceT<
                                  NativeLogEventSource,
                                  implementation::NativeLogEventSource> {};
} // namespace winrt::facebook::react::factory_implementation
