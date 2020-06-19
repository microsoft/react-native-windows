// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_REACTDISPATCHER
#define MICROSOFT_REACTNATIVE_REACTDISPATCHER

#include <winrt/Microsoft.ReactNative.h>
#include "ReactHandleHelper.h"

namespace winrt::Microsoft::ReactNative {

// Represents a dispatcher queue to invoke work item asynchronously.
// It wraps up the IReactDispatcher and adds convenience methods for
// working with C++ types.
struct ReactDispatcher {
  ReactDispatcher(std::nullptr_t = nullptr) noexcept {}

  explicit ReactDispatcher(IReactDispatcher const &handle) noexcept : m_handle{handle} {}

  IReactDispatcher const &Handle() const noexcept {
    return m_handle;
  }

  explicit operator bool() const noexcept {
    return m_handle ? true : false;
  }

  void Post(ReactDispatcherCallback const &callback) const noexcept {
    if (m_handle) {
      m_handle.Post(callback);
    }
  }

  bool HasThreadAccess() const noexcept {
    return m_handle ? m_handle.HasThreadAccess() : false;
  }

  static ReactDispatcher CreateSerialDispatcher() noexcept {
    return ReactDispatcher{ReactDispatcherHelper::CreateSerialDispatcher()};
  }

 private:
  IReactDispatcher m_handle;
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_REACTDISPATCHER
