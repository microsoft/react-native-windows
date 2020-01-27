// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_DETAILS_IFUTURE_H
#define MSO_FUTURE_DETAILS_IFUTURE_H

#include "arrayView.h"
#include "dispatchQueue/dispatchQueue.h"
#include "errorCode/errorCode.h"
#include "functional/functor.h"

namespace Mso::Futures {

struct IFuture;

using ByteArrayView = Mso::Async::ArrayView<uint8_t>;

// We use FutureTraits instead of virtual tables to reduce the overall impact on the generated code.
// Instead of having a set of methods for template type parameter set, we are making sure that we have methods
// only for unique template type parameter combinations.

using FutureDestroyCallback = void(const ByteArrayView &obj) noexcept;
using FuturePostCallback = void(const ByteArrayView &taskBuffer, Mso::DispatchTask &&task) noexcept;
using FutureInvokeCallback =
    void(const ByteArrayView &taskBuffer, _In_ IFuture *future, _In_ IFuture *parentFuture) noexcept;
using FutureCatchCallback =
    void(const ByteArrayView &taskBuffer, _In_ IFuture *future, ErrorCode &&parentError) noexcept;

enum class FutureOptions : uint32_t {
  None = 0x00,
  IsShared = 0x01, // Can future have more than one continuation?
  UseParentValue = 0x02, // Value is taken from the parent's future.
  IsMultiPost =
      0x04, // TaskInvoke or TaskCatch can be called inline multiple times. It is used for WhenAll and WhenAny.
  DestroyTaskAfterInvoke = 0x08, // To destroy user lambdas and executors right after the invoke to free resources ASAP.
  CancelIfUnfulfilled = 0x10, // Create CancellationError when future is not succeeded or failed before destructor call.
  CallTaskInvokeOnError = 0x20, // Calls TaskInvoke on error instead of TaskCatch.
};

constexpr inline FutureOptions operator|(FutureOptions left, FutureOptions right) noexcept {
  return static_cast<FutureOptions>(static_cast<uint32_t>(left) | static_cast<uint32_t>(right));
}

constexpr inline bool IsSet(FutureOptions options, FutureOptions value) noexcept {
  return (static_cast<uint32_t>(options) & static_cast<uint32_t>(value)) != 0;
}

// FutureTraits is used as a pseudo virtual table to reduce impact on generated code.
// Unlike a real virtual table we can have some slots to be null, and the same callback can be reused for different
// future traits. Besides pointers to callbacks we also have Options and ValueSize. It allows to share them between
// multiple instances of future.
struct FutureTraits {
  const FutureOptions Options; // Options bits affecting behavior of a future instance.
  const uint32_t ValueSize; // Size is 0 for void values and if UsesParentValue is set in Options.
  FutureDestroyCallback *ValueDestroy; // Destroys value object. Can be null if destructor is trivial.
                                       // It is only called for completed futures.
  FutureDestroyCallback *TaskDestroy; // Destroys task object.
  FuturePostCallback *TaskPost; // Posts for asynchronous execution. It is null for Promise and inline executions.
  FutureInvokeCallback *TaskInvoke; // Invokes a task.
  FutureCatchCallback *TaskCatch; // Catches parent future error.
};

struct IFuture : IUnknown {
  virtual const FutureTraits &GetTraits() const noexcept = 0;
  virtual ByteArrayView GetTask() noexcept = 0;
  virtual ByteArrayView GetValue() noexcept = 0;
  virtual const ErrorCode &GetError() const noexcept = 0;

  virtual void AddContinuation(Mso::CntPtr<IFuture> &&continuation) noexcept = 0;

  _Success_(
      return ) virtual bool TryStartSetValue(_Out_ ByteArrayView &valueBuffer, bool crashIfFailed = false) noexcept = 0;
  virtual void Post() noexcept = 0;
  virtual void StartAwaiting() noexcept = 0;
  virtual bool TrySetSuccess(bool crashIfFailed = false) noexcept = 0;
  virtual bool TrySetError(ErrorCode &&futureError, bool crashIfFailed = false) noexcept = 0;

  virtual bool IsDone() const noexcept = 0;
  virtual bool IsSucceeded() const noexcept = 0;
  virtual bool IsFailed() const noexcept = 0;

  template <class T, class... TArgs>
  void SetValue(TArgs &&... args) noexcept {
    ByteArrayView valueBuffer;
    (void)TryStartSetValue(/*ref*/ valueBuffer, /*crashIfFailed:*/ true);
    ::new (valueBuffer.VoidDataChecked(sizeof(T))) T(std::forward<TArgs>(args)...);
    (void)TrySetSuccess(/*crashIfFailed:*/ true);
  }

  template <class T, class... TArgs>
  bool TrySetValue(TArgs &&... args) noexcept {
    ByteArrayView valueBuffer;
    if (TryStartSetValue(/*ref*/ valueBuffer)) {
      ::new (valueBuffer.VoidDataChecked(sizeof(T))) T(std::forward<TArgs>(args)...);
      (void)TrySetSuccess(/*crashIfFailed:*/ true);
      return true;
    }

    return false;
  }
};

LIBLET_PUBLICAPI Mso::CntPtr<IFuture>
MakeFuture(const FutureTraits &traits, size_t taskSize = 0, _Out_opt_ ByteArrayView *taskBuffer = nullptr) noexcept;

} // namespace Mso::Futures

#endif // MSO_FUTURE_DETAILS_IFUTURE_H
