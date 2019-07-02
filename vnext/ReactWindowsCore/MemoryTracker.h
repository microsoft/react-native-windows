// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <chrono>
#include <functional>

#include <cxxreact/MessageQueueThread.h>

namespace facebook {
namespace react {

/**
 * @brief Type of memory threshold event handlers, @see
 * MemoryTracker::AddThresholdCallback
 */
using MemoryThresholdCallback = std::function<void(size_t currentlyUsedMemory)>;

/**
 * @brief Identifier for callback registrations, @see
 * MemoryTracker::AddThresholdCallback.
 */
using CallbackRegistrationCookie = size_t;

/**
 * @class MemoryTracker
 *
 * @brief Provides properties and events related to the memory consumption of a
 * JS engine instance.
 */
class MemoryTracker {
 public:
  /**
   * @brief Gets the amount of memory currently used by the JS engine.
   *
   * @returns Amount of memory currently used by the JS engine.
   */
  virtual size_t GetCurrentMemoryUsage() const noexcept = 0;

  /**
   * @brief Gets the peak amount of memory the JS engine instance has used
   * during its lifetime.
   *
   * @returns Peak amount of memory the used by the JS engine instance.
   */
  virtual size_t GetPeakMemoryUsage() const noexcept = 0;

  /**
   * @brief Gets the message queue thread on which event handlers are be being
   * called on.
   *
   * @returns Message queue thread to call event handlers on.
   */
  virtual std::shared_ptr<MessageQueueThread> GetCallbackMessageQueueThread()
      const noexcept = 0;

  /**
   * @brief Sets the message queue thread on which event handlers are be being
   * called on.
   *
   * @param messageQueueThread Message queue thread to call event handlers on.
   */
  virtual void SetCallbackMessageQueueThread(
      std::shared_ptr<MessageQueueThread> &&messageQueueThread) noexcept = 0;

  /**
   * @brief Adds a memory threshold callback.
   *
   * The callback is invoked when a) the JS engine consumes memory beyond the
   * specified threshold and b) the specified amount of time has passed since
   * the last callback invocation.
   *
   * @param threshold Memory threshold (number of bytes).
   * @param minCallbackInterval Minimal time that needs to pass between callback
   * invocations.
   *
   * @returns Identifier that can be used to remove the threshold callback.
   */
  virtual CallbackRegistrationCookie AddThresholdCallback(
      size_t threshold,
      std::chrono::milliseconds minCallbackInterval,
      MemoryThresholdCallback &&callback) noexcept = 0;

  /**
   * @brief Removes a memory threshold callback.
   *
   * @param cookie Identifier returned from @see AddThresholdCallback
   *
   * @returns True if the callback was removed, false if no callback is
   * registered under the given identifier.
   */
  virtual bool RemoveThresholdCallback(
      CallbackRegistrationCookie cookie) noexcept = 0;

  /**
   * @brief Initializes memory tracking.
   *
   * @param initialMemoryUsage Amount of memory consumed by the JS engine after
   * its initialization.
   *
   * @remarks This a ReactNative internal method and not meant be called by
   * ReactNative users.
   */
  virtual void Initialize(size_t initialMemoryUsage) noexcept = 0;

  /**
   * @brief Tracks an allocation by the JS engine.
   *
   * @param size Allocation amount (number of bytes).
   *
   * @remarks This a ReactNative internal method and not meant be called by
   * ReactNative users.
   */
  virtual void OnAllocation(size_t size) noexcept = 0;

  /**
   * @brief Tracks a deallocation by the JS engine.
   *
   * @param size Deallocation amount (number of bytes).
   *
   * @remarks This a ReactNative internal method and not meant be called by
   * ReactNative users.
   */
  virtual void OnDeallocation(size_t size) noexcept = 0;
};

/**
 * @brief Creates a memory tracker object.
 *
 * @param callbackMessageQueueThread Message queue thread on which event
 * handlers are be being called on.
 *
 * @returns Memory tracker object.
 *
 * @remarks If not specified when calling this method, the message queue thread
 * must be set via MemoryTracker::SetCallbackMessageQueueThread before callbacks
 * are going to be invoked.
 */
std::shared_ptr<MemoryTracker> CreateMemoryTracker(
    std::shared_ptr<MessageQueueThread> &&callbackMessageQueueThread =
        nullptr) noexcept;

} // namespace react
} // namespace facebook
