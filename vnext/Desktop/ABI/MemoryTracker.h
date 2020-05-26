#pragma once
#include "Microsoft.React.MemoryTracker.g.h"

#include <ReactWindowsCore/MemoryTracker.h>

namespace winrt::Microsoft::React::implementation {
struct MemoryTracker : MemoryTrackerT<MemoryTracker> {
  MemoryTracker() = default;

  MemoryTracker(Microsoft::React::IMessageQueue const &callbackMessageQueue);
  uint64_t CurrentMemoryUsage();
  uint64_t PeakMemoryUsage();
  Microsoft::React::IMessageQueue CallbackMessageQueue();
  void CallbackMessageQueue(Microsoft::React::IMessageQueue value);
  uint32_t AddThresholdHandler(
      uint64_t threshold,
      uint32_t minCallbackIntervalInMilliseconds,
      Microsoft::React::MemoryThresholdHandler const &handler);
  bool RemoveThresholdHandler(uint32_t token);

#pragma region IMemoryTrackerTester

  void Initialize(uint64_t initialMemoryUsage);
  void OnAllocation(uint64_t size);
  void OnDeallocation(uint64_t size);

#pragma endregion IMemoryTrackerTester

 private:
  std::shared_ptr<::facebook::react::MemoryTracker> m_internalMemoryTracker;
  ::winrt::Microsoft::React::IMessageQueue m_abiCallbackMessageQueue;
};
} // namespace winrt::Microsoft::React::implementation
namespace winrt::Microsoft::React::factory_implementation {
struct MemoryTracker : MemoryTrackerT<MemoryTracker, implementation::MemoryTracker> {};
} // namespace winrt::Microsoft::React::factory_implementation
