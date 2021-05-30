// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_FUTUREWINRT_H
#define MSO_FUTURE_FUTUREWINRT_H

#include <errorCode/hresultErrorProvider.h>
#include <future/future.h>
#include <winrt/Windows.Foundation.h>

namespace Mso {

struct AsyncActionFutureAdapter : winrt::implements<
                                      AsyncActionFutureAdapter,
                                      winrt::Windows::Foundation::IAsyncAction,
                                      winrt::Windows::Foundation::IAsyncInfo> {
  using AsyncStatus = winrt::Windows::Foundation::AsyncStatus;
  using AsyncCompletedHandler = winrt::Windows::Foundation::AsyncActionCompletedHandler;

  AsyncActionFutureAdapter(Mso::Future<void> &&future) noexcept : m_future{std::move(future)} {
    m_future.Then<Mso::Executors::Inline>([weakThis = get_weak()](Mso::Maybe<void> &&result) noexcept {
      if (auto strongThis = weakThis.get()) {
        AsyncCompletedHandler handler;
        {
          winrt::slim_lock_guard const guard(strongThis->m_lock);
          if (strongThis->m_status == AsyncStatus::Started) {
            if (result.IsValue()) {
              strongThis->m_status = AsyncStatus::Completed;
              if (strongThis->m_completedAssigned) {
                handler = std::move(strongThis->m_completed);
              }
            } else {
              strongThis->m_status = AsyncStatus::Error;
              strongThis->m_error = result.GetError();
            }
          }
        }

        if (handler) {
          invoke(handler, *strongThis, AsyncStatus::Completed);
        }
      }
    });
  }

  void Completed(AsyncCompletedHandler const &handler) {
    AsyncStatus status;

    {
      winrt::slim_lock_guard const guard(m_lock);

      if (m_completedAssigned) {
        throw winrt::hresult_illegal_delegate_assignment();
      }

      m_completedAssigned = true;

      if (m_status == AsyncStatus::Started) {
        m_completed = winrt::impl::make_agile_delegate(handler);
        return;
      }

      status = m_status;
    }

    if (handler) {
      invoke(handler, *this, status);
    }
  }

  auto Completed() noexcept {
    winrt::slim_lock_guard const guard(m_lock);
    return m_completed;
  }

  uint32_t Id() const noexcept {
    return 1;
  }

  AsyncStatus Status() noexcept {
    winrt::slim_lock_guard const guard(m_lock);
    return m_status;
  }

  winrt::hresult ErrorCode() noexcept {
    try {
      winrt::slim_lock_guard const guard(m_lock);
      RethrowIfFailed();
      return 0;
    } catch (...) {
      return winrt::to_hresult();
    }
  }

  void Cancel() noexcept {
    winrt::slim_lock_guard const guard(m_lock);

    if (m_status == AsyncStatus::Started) {
      m_status = AsyncStatus::Canceled;
      m_error = Mso::HResultErrorProvider().MakeErrorCode(winrt::impl::error_canceled);
    }
  }

  void Close() noexcept {
    Mso::Future<void> future;
    {
      winrt::slim_lock_guard const guard(m_lock);
      future = std::move(m_future);
    }
  }

  void GetResults() {
    winrt::slim_lock_guard const guard(m_lock);

    if (m_status == AsyncStatus::Completed) {
      return;
    }

    RethrowIfFailed();
    VerifyElseCrash(m_status == AsyncStatus::Started);
    throw winrt::hresult_illegal_method_call();
  }

 private:
  void RethrowIfFailed() const {
    if (m_status == AsyncStatus::Error || m_status == AsyncStatus::Canceled) {
      m_error.Throw();
    }
  }

 private:
  winrt::slim_mutex m_lock;
  Mso::Future<void> m_future;
  AsyncStatus m_status{AsyncStatus::Started};
  Mso::ErrorCode m_error{};
  AsyncCompletedHandler m_completed;
  bool m_completedAssigned{false};
};

} // namespace Mso

#endif // MSO_FUTURE_FUTUREWINRT_H
