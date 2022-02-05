// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <dispatchQueue/dispatchQueue.h>

// Windows API
#include <winrt/base.h>

// Standard Library
#include <sstream>

namespace Microsoft::React::Utilities {

///
/// Implements an awaiter for Mso::DispatchQueue
///
auto resume_in_queue(const Mso::DispatchQueue &queue) noexcept {
  struct awaitable {
    awaitable(const Mso::DispatchQueue &queue) noexcept : m_queue{queue} {}

    bool await_ready() const noexcept {
      return false;
    }

    void await_resume() const noexcept {}

    void await_suspend(std::experimental::coroutine_handle<> resume) noexcept {
      m_callback = [context = resume.address()]() noexcept {
        std::experimental::coroutine_handle<>::from_address(context)();
      };
      m_queue.Post(std::move(m_callback));
    }

   private:
    Mso::DispatchQueue m_queue;
    Mso::VoidFunctor m_callback;
  };

  return awaitable{queue};
} // resume_in_queue

std::string HResultToString(winrt::hresult_error const &e);

std::string HResultToString(winrt::hresult &&result);

} // namespace Microsoft::React::Utilities
