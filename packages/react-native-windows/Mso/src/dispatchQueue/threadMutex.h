// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include <mutex>

namespace Mso {

//! Mutex wrapper that saves current thread to implement IsLockedByMe() function.
struct ThreadMutex {
  void lock() noexcept;
  void unlock() noexcept;
  bool IsLockedByMe() const;

 private:
  std::mutex m_mutex;
  std::thread::id m_threadId{};
};

//=============================================================================
// QueueMutex inline implementation.
//=============================================================================

inline void ThreadMutex::lock() noexcept {
  m_mutex.lock();
  m_threadId = std::this_thread::get_id();
}

inline void ThreadMutex::unlock() noexcept {
  m_threadId = std::thread::id{};
  m_mutex.unlock();
}

inline bool ThreadMutex::IsLockedByMe() const {
  return m_threadId == std::this_thread::get_id();
}

} // namespace Mso
