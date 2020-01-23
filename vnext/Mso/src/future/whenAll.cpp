// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"

namespace Mso {

LIBLET_PUBLICAPI Future<void> WhenAll(Mso::Async::ArrayView<Future<void>> futures) noexcept {
  if (futures.Size() == 0) {
    return MakeSucceededFuture();
  }

  using TaskType = Mso::Futures::WhenAllFutureTask<void>;
  const size_t taskSize = TaskType::GetTaskSize(futures.Size());

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:     */ Mso::Futures::FutureOptions::IsMultiPost,
      /*ResultType:  */ void,
      /*TaskType:    */ TaskType,
      /*PostType:    */ void,
      /*InvokeType:  */ Mso::Futures::WhenAllTaskInvoke<void>,
      /*AbandonType: */ Mso::Futures::WhenAllTaskCatch>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  Mso::CntPtr<Mso::Futures::IFuture> whenAllFuture = Mso::Futures::MakeFuture(futureTraits, taskSize, &taskBuffer);
  TaskType *task = reinterpret_cast<TaskType *>(taskBuffer.VoidDataChecked(taskSize));
  task->CompleteCount = 0;
  task->FutureCount = static_cast<uint32_t>(futures.Size());

  size_t i = 0;
  for (const Future<void> &parentFuture : futures) {
    ::new (&task->ParentFutures[i++]) Mso::Futures::RawOrCntPtr<Mso::Futures::IFuture>(Mso::GetIFuture(parentFuture));
  }

  // Use a separate loop to add whenAllFuture to the parent futures because parent futures may start
  // invoke our whenAllFuture while we still in this function.
  for (const Future<void> &parentFuture : futures) {
    Mso::GetIFuture(parentFuture)->AddContinuation(Mso::CntPtr{whenAllFuture});
  }

  return Future<void>(std::move(whenAllFuture));
}

LIBLET_PUBLICAPI Future<void> WhenAll(std::initializer_list<Future<void>> futures) noexcept {
  return WhenAll(Mso::Async::ArrayView<Future<void>>(futures));
}

LIBLET_PUBLICAPI Future<void> WhenAll(const std::vector<Future<void>> &futures) noexcept {
  return WhenAll(Mso::Async::ArrayView<Future<void>>(futures.data(), futures.size()));
}

namespace Futures {

LIBLET_PUBLICAPI WhenAllFutureTask<void>::WhenAllFutureTask(
    Mso::Futures::IFuture *futureState,
    std::initializer_list<Mso::Futures::IFuture *> init) noexcept
    : CompleteCount{0}, FutureCount(static_cast<uint32_t>(init.size())) {
  size_t i = 0;
  for (Mso::Futures::IFuture *parentFuture : init) {
    ::new (&ParentFutures[i++]) RawOrCntPtr<Mso::Futures::IFuture>(parentFuture);
  }

  for (Mso::Futures::IFuture *parentFuture : init) {
    parentFuture->AddContinuation(Mso::CntPtr{futureState});
  }
}

LIBLET_PUBLICAPI void WhenAllFutureTask<void>::Destroy(const ByteArrayView &obj) noexcept {
  auto task = static_cast<const WhenAllFutureTask *>(obj.VoidData());
  VerifyElseCrashTag(obj.Size() == GetTaskSize(task->FutureCount), 0x01605622 /* tag_byfy8 */);

  for (uint32_t i = 0; i < task->FutureCount; ++i) {
    (&(task->ParentFutures[0]) + i)->~RawOrCntPtr();
  }
}

LIBLET_PUBLICAPI _Callback_ void WhenAllTaskInvoke<void>::Invoke(
    const ByteArrayView &taskBuffer,
    _In_ IFuture *future,
    _In_ IFuture *parentFuture) noexcept {
  auto task = reinterpret_cast<WhenAllFutureTask<void> *>(taskBuffer.VoidData());
  VerifyElseCrashTag(
      taskBuffer.Size() == WhenAllFutureTask<void>::GetTaskSize(task->FutureCount), 0x01605623 /* tag_byfy9 */);

  bool found = false;
  for (size_t i = 0; i < task->FutureCount; ++i) {
    auto &storedFuture = task->ParentFutures[i];
    if (parentFuture == storedFuture.Get()) {
      storedFuture.ConvertToCntPtr();
      found = true;
      break;
    }
  }

  VerifyElseCrashSzTag(found, "parent future is not found", 0x01605640 /* tag_byfza */);

  if (++task->CompleteCount == task->FutureCount) {
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }
}

LIBLET_PUBLICAPI void
WhenAllTaskCatch::Catch(const ByteArrayView & /*taskBuffer*/, IFuture *future, ErrorCode &&parentError) noexcept {
  // We should not crash because multiple parents may decide to set error. We only return the first error.
  future->TrySetError(std::move(parentError), /*crashIfFailed:*/ false);
}

} // namespace Futures
} // namespace Mso
