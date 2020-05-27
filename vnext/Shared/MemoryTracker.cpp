// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <cassert>
#include <limits>
#include <unordered_map>

#include <MemoryTracker.h>

namespace facebook {
namespace react {

class MemoryTrackerImpl : public MemoryTracker {
 public:
  MemoryTrackerImpl(std::shared_ptr<MessageQueueThread> &&callbackMessageQueueThread) noexcept;
  size_t GetCurrentMemoryUsage() const noexcept override;
  size_t GetPeakMemoryUsage() const noexcept override;
  std::shared_ptr<MessageQueueThread> GetCallbackMessageQueueThread() const noexcept override;
  void SetCallbackMessageQueueThread(std::shared_ptr<MessageQueueThread> &&messageQueueThread) noexcept override;
  CallbackRegistrationCookie AddThresholdCallback(
      size_t threshold,
      std::chrono::milliseconds minCallbackInterval,
      MemoryThresholdCallback &&callback) noexcept override;
  bool RemoveThresholdCallback(CallbackRegistrationCookie cookie) noexcept override;
  void Initialize(size_t initialMemoryUsage) noexcept override;
  void OnAllocation(size_t size) noexcept override;
  void OnDeallocation(size_t size) noexcept override;

 private:
  struct ThresholdCallbackRecord {
    ThresholdCallbackRecord(
        size_t threshold,
        std::chrono::milliseconds minCallbackInterval,
        MemoryThresholdCallback &&callback) noexcept;

    const size_t Threshold;
    const std::chrono::milliseconds MinCallbackInterval;
    const MemoryThresholdCallback Callback;
    std::chrono::steady_clock::time_point LastNotificationTime;
  };

  bool m_isInitialized = false;
  mutable std::recursive_mutex m_mutex;
  size_t m_currentMemoryUsage = 0;
  size_t m_peakMemoryUsage = 0;
  CallbackRegistrationCookie m_nextCookie = 0;
  std::unordered_map<CallbackRegistrationCookie, ThresholdCallbackRecord> m_thresholdCallbackRecords;
  std::shared_ptr<MessageQueueThread> m_callbackMessageQueueThread;
};

MemoryTrackerImpl::MemoryTrackerImpl(std::shared_ptr<MessageQueueThread> &&callbackMessageQueueThread) noexcept
    : m_callbackMessageQueueThread{std::move(callbackMessageQueueThread)} {}

size_t MemoryTrackerImpl::GetCurrentMemoryUsage() const noexcept {
  std::lock_guard<std::recursive_mutex> lockGuard{m_mutex};
  assert(m_isInitialized);
  return m_currentMemoryUsage;
}

size_t MemoryTrackerImpl::GetPeakMemoryUsage() const noexcept {
  std::lock_guard<std::recursive_mutex> lockGuard{m_mutex};
  assert(m_isInitialized);
  return m_peakMemoryUsage;
}

std::shared_ptr<MessageQueueThread> MemoryTrackerImpl::GetCallbackMessageQueueThread() const noexcept {
  return m_callbackMessageQueueThread;
}

void MemoryTrackerImpl::SetCallbackMessageQueueThread(
    std::shared_ptr<MessageQueueThread> &&messageQueueThread) noexcept {
  assert(messageQueueThread);
  assert(!m_isInitialized);
  m_callbackMessageQueueThread = std::move(messageQueueThread);
}

CallbackRegistrationCookie MemoryTrackerImpl::AddThresholdCallback(
    size_t threshold,
    std::chrono::milliseconds minCallbackInterval,
    MemoryThresholdCallback &&callback) noexcept {
  std::lock_guard<std::recursive_mutex> lockGuard{m_mutex};
  assert(m_nextCookie < std::numeric_limits<decltype(m_nextCookie)>::max());
  CallbackRegistrationCookie cookie = m_nextCookie++;
#if DEBUG
  bool success = false;
  std::tie(std::ignore, success) =
#endif
      m_thresholdCallbackRecords.emplace(
          cookie, ThresholdCallbackRecord{threshold, minCallbackInterval, std::move(callback)});
#if DEBUG
  assert(success);
#endif
  return cookie;
}

bool MemoryTrackerImpl::RemoveThresholdCallback(CallbackRegistrationCookie cookie) noexcept {
  std::lock_guard<std::recursive_mutex> lockGuard{m_mutex};
  return m_thresholdCallbackRecords.erase(cookie) == 1;
}

void MemoryTrackerImpl::Initialize(size_t initialMemoryUsage) noexcept {
  std::lock_guard<std::recursive_mutex> lockGuard{m_mutex};
  assert(!m_isInitialized);
  m_isInitialized = true;
  OnAllocation(initialMemoryUsage);
}

void MemoryTrackerImpl::OnAllocation(size_t size) noexcept {
  std::lock_guard<std::recursive_mutex> lockGuard{m_mutex};
  // detect overflow
  assert(std::numeric_limits<decltype(m_currentMemoryUsage)>::max() - m_currentMemoryUsage >= size);

  assert(m_isInitialized);
  assert(m_callbackMessageQueueThread);

  m_currentMemoryUsage += size;

  if (m_currentMemoryUsage > m_peakMemoryUsage) {
    m_peakMemoryUsage = m_currentMemoryUsage;
  }

  std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

  for (auto &record : m_thresholdCallbackRecords) {
    if (m_currentMemoryUsage >= record.second.Threshold &&
        currentTime > record.second.LastNotificationTime + record.second.MinCallbackInterval) {
      m_callbackMessageQueueThread->runOnQueue(
          [callback = record.second.Callback, currentMemoryUsage = m_currentMemoryUsage] {
            callback(currentMemoryUsage);
          });
      record.second.LastNotificationTime = currentTime;
    }
  }
}

void MemoryTrackerImpl::OnDeallocation(size_t size) noexcept {
  std::lock_guard<std::recursive_mutex> lockGuard{m_mutex};
  assert(m_isInitialized);
  assert(size <= m_currentMemoryUsage);
  m_currentMemoryUsage -= size;
}

MemoryTrackerImpl::ThresholdCallbackRecord::ThresholdCallbackRecord(
    size_t threshold,
    std::chrono::milliseconds minCallbackInterval,
    MemoryThresholdCallback &&callback) noexcept
    : Threshold{threshold}, MinCallbackInterval{minCallbackInterval}, Callback{std::move(callback)} {}

std::shared_ptr<MemoryTracker> CreateMemoryTracker(
    std::shared_ptr<MessageQueueThread> &&callbackMessageQueueThread) noexcept {
  return std::make_shared<MemoryTrackerImpl>(std::move(callbackMessageQueueThread));
}

} // namespace react
} // namespace facebook
