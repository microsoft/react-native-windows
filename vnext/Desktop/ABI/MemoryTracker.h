#pragma once
#include "facebook.react.MemoryTracker.g.h"

#include <Shared/MemoryTracker.h>

namespace winrt::facebook::react::implementation {
struct MemoryTracker : MemoryTrackerT<MemoryTracker> {
  MemoryTracker() = default;

  MemoryTracker(facebook::react::IMessageQueue const &callbackMessageQueue);
  uint64_t CurrentMemoryUsage();
  uint64_t PeakMemoryUsage();
  facebook::react::IMessageQueue CallbackMessageQueue();
  void CallbackMessageQueue(facebook::react::IMessageQueue value);
  uint32_t AddThresholdHandler(
      uint64_t threshold,
      uint32_t minCallbackIntervalInMilliseconds,
      facebook::react::MemoryThresholdHandler const &handler);
  bool RemoveThresholdHandler(uint32_t token);

#pragma region IMemoryTrackerTester

  void Initialize(uint64_t initialMemoryUsage);
  void OnAllocation(uint64_t size);
  void OnDeallocation(uint64_t size);

#pragma endregion IMemoryTrackerTester

 private:
  std::shared_ptr<::facebook::react::MemoryTracker> m_internalMemoryTracker;
  ::winrt::facebook::react::IMessageQueue m_abiCallbackMessageQueue;
};
} // namespace winrt::facebook::react::implementation
namespace winrt::facebook::react::factory_implementation {
struct MemoryTracker : MemoryTrackerT<MemoryTracker, implementation::MemoryTracker> {};
} // namespace winrt::facebook::react::factory_implementation
