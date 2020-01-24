// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "MsoUtils.h"
#include "activeObject/activeObject.h"
#include "future/future.h"
#include "object/refCountedObject.h"

namespace Mso {

inline winrt::Windows::Foundation::IAsyncAction FutureToAsyncAction(const Mso::Future<void> &future) noexcept {
  struct AsyncActionImpl final : winrt::impl::promise_base<AsyncActionImpl, winrt::Windows::Foundation::IAsyncAction> {
    void return_void() const noexcept {}
  };

  auto asyncActionImpl = winrt::make_self<AsyncActionImpl>();
  auto asyncAction = asyncActionImpl.try_as<winrt::Windows::Foundation::IAsyncAction>();

  future.Then(Mso::Executors::Inline{}, [asyncActionImpl](Mso::Maybe<void> &&result) noexcept {
    if (result.IsValue()) {
      asyncActionImpl->set_completed();
    } else {
      try {
        if (auto cancellation = result.GetError().HandleAs(Mso::CancellationErrorProvider())) {
          throw winrt::hresult_canceled{};
        } else {
          result.GetError().HandleAndThrow();
        }
      } catch (...) {
        asyncActionImpl->unhandled_exception();
      }
    }
  });

  return asyncAction;
}

} // namespace Mso
