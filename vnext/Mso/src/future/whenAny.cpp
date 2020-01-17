// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"

namespace Mso {

LIBLET_PUBLICAPI Future<void> WhenAny(Mso::Async::ArrayView<Future<void>> futures) noexcept {
  VerifyElseCrashSzTag(futures.Size() > 0, "Must have at least one parent future.", 0x01605641 /* tag_byfzb */);

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::IsMultiPost,
      /*ResultType: */ void,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ Mso::Futures::WhenAnyTaskInvoke<void>,
      /*CatchType:  */ Mso::Futures::WhenAnyTaskCatch>::Traits;

  Mso::CntPtr<Mso::Futures::IFuture> whenAnyFuture = Mso::Futures::MakeFuture(futureTraits, 0, nullptr);

  for (const Future<void> &parentFuture : futures) {
    Mso::GetIFuture(parentFuture)->AddContinuation(Mso::CntPtr{whenAnyFuture});
  }

  return Future<void>(std::move(whenAnyFuture));
}

LIBLET_PUBLICAPI Future<void> WhenAny(std::initializer_list<Future<void>> futures) noexcept {
  return WhenAny(Mso::Async::ArrayView<Future<void>>(futures));
}

LIBLET_PUBLICAPI Future<void> WhenAny(const std::vector<Future<void>> &futures) noexcept {
  return WhenAny(Mso::Async::ArrayView<Future<void>>(futures.data(), futures.size()));
}

namespace Futures {

LIBLET_PUBLICAPI _Callback_ void WhenAnyTaskInvoke<void>::Invoke(
    const ByteArrayView & /*taskBuffer*/,
    _In_ IFuture *future,
    _In_ IFuture * /*parentFuture*/) noexcept {
  future->TrySetSuccess(/*crashIfFailed:*/ false);
}

LIBLET_PUBLICAPI void
WhenAnyTaskCatch::Catch(const ByteArrayView & /*taskBuffer*/, IFuture *future, ErrorCode &&parentError) noexcept {
  future->TrySetError(std::move(parentError), /*crashIfFailed:*/ false);
}

} // namespace Futures
} // namespace Mso
