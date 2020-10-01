// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

//
// Helper file for places where the winrt API throws exceptions in 'expected'
// cases and we want to avoid them to allow developers to have break on
// exceptions enabled to find bugs
//

// This assumes RS5 winrtcpp SDK.  Set the define DEFAULT_CPPWINRT_EXCEPTIONS
// if winrtcpp internals change and break this lessthrow_await_adapter -
// based on await_adapter but doesn't throw from
// await_resume for APIs that fail in normal usage.
//
// When we can assume Windows 10 20H1 new APIs which may exist with less
// throwing by default (TryConnectAsync, httpClient.TryGetAsync APIs may exist
// then), this could go away (or cppwinrt API could add nothrow accessors)
//

#include <winrt/base.h>

// #define DEFAULT_CPPWINRT_EXCEPTIONS
#ifndef DEFAULT_CPPWINRT_EXCEPTIONS

// Adapted from winrt/base.h await_adapter
template <typename Async>
struct lessthrow_await_adapter {
  Async const &async;

  bool await_ready() const {
    return async.Status() == winrt::Windows::Foundation::AsyncStatus::Completed;
  }

  void await_suspend(std::experimental::coroutine_handle<> handle) const {
    auto context = winrt::capture<winrt::impl::IContextCallback>(WINRT_IMPL_CoGetObjectContext);

    async.Completed([handle, context = std::move(context)](auto const &, winrt::Windows::Foundation::AsyncStatus) {
      winrt::impl::com_callback_args args{};
      args.data = handle.address();

      auto callback = [](winrt::impl::com_callback_args * args) noexcept->int32_t {
        std::experimental::coroutine_handle<>::from_address(args->data)();
        return S_OK;
      };

      winrt::check_hresult(context->ContextCallback(
          callback, &args, winrt::guid_of<winrt::impl::ICallbackWithNoReentrancyToApplicationSTA>(), 5, nullptr));
    });
  }

  void await_resume() const {
    // Don't check for a failure result here
    return;
  }
};
#endif
