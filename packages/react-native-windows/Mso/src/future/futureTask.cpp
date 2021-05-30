// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"

namespace Mso::Futures {

LIBLET_PUBLICAPI void CopyTaskInvoke<void>::Invoke(
    const ByteArrayView & /*taskBuffer*/,
    IFuture *future,
    IFuture * /*parentFuture*/) noexcept {
  future->TrySetSuccess(/*crashIfFailed:*/ true);
}

LIBLET_PUBLICAPI void
CopyTaskCatch::Catch(const ByteArrayView & /*taskBuffer*/, IFuture *future, ErrorCode &&parentError) noexcept {
  future->TrySetError(ErrorCode(parentError));
}

LIBLET_PUBLICAPI void MoveTaskInvoke<void>::Invoke(
    const ByteArrayView & /*taskBuffer*/,
    IFuture *future,
    IFuture * /*parentFuture*/) noexcept {
  future->TrySetSuccess(/*crashIfFailed:*/ true);
}

LIBLET_PUBLICAPI void
FutureCompletionTask::Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
  // We do not handle error in the completion task, and instead propagating it to the future that waits for completion.
  auto task = taskBuffer.As<FutureCompletionTask>();
  task->FutureToComplete->TrySetError(ErrorCode(parentError), /*crashIfFailed:*/ false);
  task->FutureToComplete.Clear();
  future->TrySetError(std::move(parentError), /*crashIfFailed:*/ true);
}

LIBLET_PUBLICAPI void FutureCompletionTask::Destroy(const ByteArrayView &taskBuffer) noexcept {
  taskBuffer.As<FutureCompletionTask>()->FutureToComplete.Clear();
}

LIBLET_PUBLICAPI void FutureCompletionTaskInvoke<void>::Invoke(
    const ByteArrayView &taskBuffer,
    IFuture *future,
    IFuture * /*parentFuture*/) noexcept {
  // Complete the completion future first because a continuation is added to the FutureToComplete
  // and everything should be completed by the time the continuation is called.
  future->TrySetSuccess();

  auto task = taskBuffer.As<FutureCompletionTask>();
  task->FutureToComplete->TrySetSuccess();
}

LIBLET_PUBLICAPI void ResultSetter<Mso::Maybe<void>>::Set(IFuture *future, Mso::Maybe<void> &&value) noexcept {
  if (value.IsValue()) {
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  } else {
    future->TrySetError(value.TakeError(), /*crashIfFailed:*/ true);
  }
}

} // namespace Mso::Futures
