// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <winrt/base.h>

// Copied from "vnext\Shared\Utils\CppWinrtLessExceptions.h" until we have a good way to share between packages

namespace winrt::NodeRpc {

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

      auto callback = [](winrt::impl::com_callback_args *args) noexcept -> int32_t {
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

} // namespace winrt::NodeRpc
