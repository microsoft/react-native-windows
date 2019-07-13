#include "pch.h"

#include "MemoryTracker.h"

#include "MemoryTracker.g.cpp"

#include "MessageQueueShim.h"

namespace winrt::facebook::react::implementation {
MemoryTracker::MemoryTracker(
    facebook::react::IMessageQueue const &callbackMessageQueue) {
  m_internalMemoryTracker = ::facebook::react::CreateMemoryTracker(
      std::make_shared<::facebook::react::MessageQueueShim>(
          callbackMessageQueue));
}

uint64_t MemoryTracker::CurrentMemoryUsage() {
  return m_internalMemoryTracker->GetCurrentMemoryUsage();
}

uint64_t MemoryTracker::PeakMemoryUsage() {
  return m_internalMemoryTracker->GetPeakMemoryUsage();
}

facebook::react::IMessageQueue MemoryTracker::CallbackMessageQueue() {
  return m_abiCallbackMessageQueue;
}

void MemoryTracker::CallbackMessageQueue(facebook::react::IMessageQueue value) {
  m_internalMemoryTracker->SetCallbackMessageQueueThread(
      std::make_shared<::facebook::react::MessageQueueShim>(value));
}

uint32_t MemoryTracker::AddThresholdHandler(
    uint64_t threshold,
    uint32_t minCallbackIntervalInMilliseconds,
    facebook::react::MemoryThresholdHandler const &handler) {
  return static_cast<uint32_t>(m_internalMemoryTracker->AddThresholdCallback(
      static_cast<size_t>(threshold),
      std::chrono::milliseconds(minCallbackIntervalInMilliseconds),
      handler));
}

bool MemoryTracker::RemoveThresholdHandler(uint32_t token) {
  return m_internalMemoryTracker->RemoveThresholdCallback(token);
}

#pragma region IMemoryTrackerTester

void MemoryTracker::Initialize(uint64_t initialMemoryUsage) {
  m_internalMemoryTracker->Initialize(static_cast<size_t>(initialMemoryUsage));
}

void MemoryTracker::OnAllocation(uint64_t size) {
  m_internalMemoryTracker->OnAllocation(static_cast<size_t>(size));
}

void MemoryTracker::OnDeallocation(uint64_t size) {
  m_internalMemoryTracker->OnDeallocation(static_cast<size_t>(size));
}

#pragma endregion IMemoryTrackerTester
} // namespace winrt::facebook::react::implementation
