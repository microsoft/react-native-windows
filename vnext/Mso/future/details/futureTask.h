// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_DETAILS_FUTURETASK_H
#define MSO_FUTURE_DETAILS_FUTURETASK_H

#include <type_traits>
#include "errorCode/maybe.h"
#include "executor.h"
#include "future/futureForwardDecl.h"
#include "ifuture.h"

// These are a few principles we used for the FutureTask design to reduce size of compiled code:
// - Eliminate constructors. Use structs with public fields.
// - Post() method is specific for a TExecutor only.
// - ValueDestroy function pointer is null if object has a trivial destructor.
// - Value is allocated separately from a task.
// - Keep FutureTraits as a static field.

namespace Mso {

template <class T>
Mso::Futures::IFuture *GetIFuture(const T &future) noexcept;

namespace Futures {

LIBLET_PUBLICAPI void FutureWait(IFuture &future) noexcept;

template <class TValue, bool IsTrivial = std::is_trivially_destructible<TValue>::value>
struct ValueTraits {
  constexpr static const size_t Size = sizeof(TValue);
  constexpr static FutureDestroyCallback *DestroyPtr = nullptr;
  static_assert(alignof(TValue) <= sizeof(void *) * 2, "Alignment more than two pointer size is not supported");
};

template <class TValue>
struct ValueTraits<TValue, false> {
  static void Destroy(const ByteArrayView &valueBuffer) noexcept {
    valueBuffer.As<TValue>()->~TValue();
  }

  constexpr static const size_t Size = sizeof(TValue);
  constexpr static FutureDestroyCallback *DestroyPtr = &Destroy;
  static_assert(alignof(TValue) <= sizeof(void *) * 2, "Alignment more than two pointer size is not supported");
};

template <>
struct ValueTraits<void, false> {
  constexpr static const size_t Size = 0;
  constexpr static FutureDestroyCallback *DestroyPtr = nullptr;
};

template <class T>
struct PostTraits {
  constexpr static FuturePostCallback *PostPtr = &T::Post;
};

template <>
struct PostTraits<void> {
  constexpr static FuturePostCallback *PostPtr = nullptr;
};

template <class T>
struct InvokeTraits {
  constexpr static FutureInvokeCallback *InvokePtr = &T::Invoke;
};

template <>
struct InvokeTraits<void> {
  constexpr static FutureInvokeCallback *InvokePtr = nullptr;
};

template <class T>
struct CatchTraits {
  constexpr static FutureCatchCallback *CatchPtr = T::CatchPtr;
};

template <>
struct CatchTraits<void> {
  constexpr static FutureCatchCallback *CatchPtr = nullptr;
};

// Provides an instance of FutureTraits for the given template arguments.
// FutureTraits give to FutureImpl all type specific information.
template <FutureOptions Options, class ResultType, class TaskType, class PostType, class InvokeType, class CatchType>
struct FutureTraitsProvider {
  constexpr static const FutureTraits Traits = {
      /*Options:      */ Options,
      /*ValueSize:    */ ValueTraits<ResultType>::Size,
      /*ValueDestroy: */ ValueTraits<ResultType>::DestroyPtr,
      /*TaskDestroy:  */ ValueTraits<TaskType>::DestroyPtr,
      /*TaskPost:     */ PostTraits<PostType>::PostPtr,
      /*TaskInvoke:   */ InvokeTraits<InvokeType>::InvokePtr,
      /*TaskCatch:    */ CatchTraits<CatchType>::CatchPtr,
  };
};

template <FutureOptions Options, class ResultType, class TaskType, class PostType, class InvokeType, class CatchType>
constexpr const FutureTraits
    FutureTraitsProvider<Options, ResultType, TaskType, PostType, InvokeType, CatchType>::Traits;

struct FutureCompletionTask {
  FutureCompletionTask() = delete;
  ~FutureCompletionTask() = delete;

  LIBLET_PUBLICAPI static void
  Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept;
  constexpr static FutureCatchCallback *CatchPtr = &Catch;

  LIBLET_PUBLICAPI static void Destroy(const ByteArrayView &taskBuffer) noexcept;

  Mso::CntPtr<IFuture> FutureToComplete;
};

// ValueTraits specialization to enable use of FutureCompletionTask.
template <>
struct ValueTraits<FutureCompletionTask, false> {
  constexpr static FutureDestroyCallback *DestroyPtr = &FutureCompletionTask::Destroy;
};

template <class TValue>
struct FutureCompletionTaskInvoke {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept {
    // Complete the completion future fist because a continuation is added to the FutureToComplete
    // and everything should be completed by the time the continuation is called.
    future->TrySetSuccess(/*crashIfFailed*/ true);

    auto task = taskBuffer.As<FutureCompletionTask>();
    auto inValue = parentFuture->GetValue().As<TValue>();
    task->FutureToComplete->TrySetValue<TValue>(std::move(*inValue));
  }
};

template <>
struct FutureCompletionTaskInvoke<void> {
  LIBLET_PUBLICAPI static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept;
};

template <class TExecutor, class TCallback>
struct FutureTask {
  // Executor must be first to cast FutureTask to TExecutor in FutureTaskPost
  TExecutor Executor;
  TCallback Callback;
};

template <class TExecutor>
struct FutureTaskPost {
  // Post method only uses TExecutor. It does not use TCallback to reduce compiled code size.
  static void Post(const ByteArrayView &taskBuffer, DispatchTask &&task) noexcept {
    auto executor = taskBuffer.As<TExecutor>();
    static_assert(noexcept(executor->Post(std::move(task))), "Executor Post method must be noexcept");
    executor->Post(std::move(task));
  }
};

template <class T>
struct ResultSetter {
  static void Set(IFuture *future, T &&value) noexcept {
    ByteArrayView valueBuffer;
    future->TryStartSetValue(/*ref*/ valueBuffer, /*crashIfFailed:*/ true);
    ::new (valueBuffer.VoidDataChecked(sizeof(T))) T(std::move(value));
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }
};

template <class T>
struct ResultSetter<Mso::Maybe<T>> {
  static void Set(IFuture *future, Mso::Maybe<T> &&value) noexcept {
    if (value.IsValue()) {
      ByteArrayView valueBuffer;
      future->TryStartSetValue(/*ref*/ valueBuffer, /*crashIfFailed:*/ true);
      ::new (valueBuffer.VoidDataChecked(sizeof(T))) T(value.TakeValue());
      future->TrySetSuccess(/*crashIfFailed:*/ true);
    } else {
      future->TrySetError(value.TakeError(), /*crashIfFailed:*/ true);
    }
  }
};

template <>
struct ResultSetter<Mso::Maybe<void>> {
  LIBLET_PUBLICAPI static void Set(IFuture *future, Mso::Maybe<void> &&value) noexcept;
};

template <class T>
struct ResultSetter<Mso::Future<T>> {
  static void Set(IFuture *future, Mso::Future<T> &&value) noexcept {
    future->StartAwaiting();

    using CompletionTaskType = FutureCompletionTask;
    constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
        /*Options:    */ FutureOptions::UseParentValue,
        /*ResultType: */ void,
        /*TaskType:   */ CompletionTaskType,
        /*PostType:   */ void,
        /*InvokeType: */ FutureCompletionTaskInvoke<T>,
        /*CatchType:  */ CompletionTaskType>::Traits;

    ByteArrayView completionTaskBuffer;
    Mso::CntPtr<IFuture> completionFuture = MakeFuture(futureTraits, sizeof(CompletionTaskType), &completionTaskBuffer);
    ::new (completionTaskBuffer.Data()) Mso::CntPtr<IFuture>(future);

    Mso::GetIFuture(value)->AddContinuation(std::move(completionFuture));
  }
};

template <class T>
struct ResultSetter<Mso::Maybe<Mso::Future<T>>> {
  static void Set(IFuture *future, Mso::Maybe<Mso::Future<T>> &&value) noexcept {
    if (value.IsValue()) {
      future->StartAwaiting();

      using CompletionTaskType = FutureCompletionTask;
      constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
          /*Options:    */ FutureOptions::UseParentValue,
          /*ResultType: */ void,
          /*TaskType:   */ CompletionTaskType,
          /*PostType:   */ void,
          /*InvokeType: */ FutureCompletionTaskInvoke<T>,
          /*CatchType:  */ CompletionTaskType>::Traits;

      ByteArrayView completionTaskBuffer;
      Mso::CntPtr<IFuture> completionFuture =
          MakeFuture(futureTraits, sizeof(CompletionTaskType), &completionTaskBuffer);
      ::new (completionTaskBuffer.Data()) Mso::CntPtr<IFuture>(future);

      Mso::GetIFuture(value.GetValue())->AddContinuation(std::move(completionFuture));
    } else {
      future->TrySetError(value.TakeError(), /*crashIfFailed:*/ true);
    }
  }
};

template <class T>
struct CopyTaskInvoke {
  static void Invoke(const ByteArrayView & /*taskBuffer*/, IFuture *future, IFuture *parentFuture) noexcept {
    const T *inValue = parentFuture->GetValue().As<T>();
    future->SetValue<T>(*inValue);
  }
};

template <>
struct CopyTaskInvoke<void> {
  LIBLET_PUBLICAPI static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept;
};

struct CopyTaskCatch {
  CopyTaskCatch() = delete;
  ~CopyTaskCatch() = delete;

  LIBLET_PUBLICAPI static void
  Catch(const ByteArrayView & /*taskBuffer*/, IFuture *future, ErrorCode &&parentError) noexcept;
  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

// An Invoke callback used for Catch methods that moves value from parent future
template <class T>
struct MoveTaskInvoke {
  static void Invoke(const ByteArrayView & /*taskBuffer*/, IFuture *future, IFuture *parentFuture) noexcept {
    auto inValue = parentFuture->GetValue().As<T>();
    ResultSetter<T>::Set(future, std::move(*inValue));
  }
};

template <>
struct MoveTaskInvoke<void> {
  LIBLET_PUBLICAPI static void
  Invoke(const ByteArrayView & /*taskBuffer*/, IFuture *future, IFuture *parentFuture) noexcept;
};

template <class TExecutor, class TCallback, class TInValue, class TResult, CallbackArgKind argKind>
struct FutureTaskInvoke {};

template <class TExecutor, class TCallback, class TInValue, class TResult>
struct FutureTaskInvoke<TExecutor, TCallback, TInValue, TResult, CallbackArgKind::Value> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    auto inValue = parentFuture->GetValue().As<TInValue>();
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback, std::move(*inValue)));
  }
};

template <class TExecutor, class TCallback, class TInValue, class TResult>
struct FutureTaskInvoke<TExecutor, TCallback, TInValue, TResult, CallbackArgKind::ValueRef> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    auto inValue = parentFuture->GetValue().As<TInValue>();
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback, *inValue));
  }
};

template <class TExecutor, class TCallback, class TInValue, class TResult>
struct FutureTaskInvoke<TExecutor, TCallback, TInValue, TResult, CallbackArgKind::Maybe> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    auto inValue = parentFuture->GetValue().As<TInValue>();
    ResultSetter<TResult>::Set(
        future, task->Executor.Invoke(task->Callback, Mso::Maybe<TInValue>(std::move(*inValue))));
  }
};

template <class TExecutor, class TCallback, class TInValue, class TResult>
struct FutureTaskInvoke<TExecutor, TCallback, TInValue, TResult, CallbackArgKind::MaybeRef> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    auto inValue = parentFuture->GetValue().As<TInValue>();
    Mso::Maybe<TInValue> maybeValue{std::move(*inValue)};
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback, maybeValue));
  }
};

template <class TExecutor, class TCallback, class TResult>
struct FutureTaskInvoke<TExecutor, TCallback, void, TResult, CallbackArgKind::Void> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture * /*parentFuture*/) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback));
  }
};

template <class TExecutor, class TCallback, class TResult>
struct FutureTaskInvoke<TExecutor, TCallback, void, TResult, CallbackArgKind::MaybeVoid> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture * /*parentFuture*/) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback, Mso::Maybe<void>()));
  }
};

template <class TExecutor, class TCallback, class TResult>
struct FutureTaskInvoke<TExecutor, TCallback, void, TResult, CallbackArgKind::MaybeVoidRef> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture * /*parentFuture*/) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    Mso::Maybe<void> maybeValue;
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback, maybeValue));
  }
};

template <class TExecutor, class TCallback, class TInValue>
struct FutureTaskInvoke<TExecutor, TCallback, TInValue, void, CallbackArgKind::Value> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    auto inValue = parentFuture->GetValue().As<TInValue>();
    task->Executor.Invoke(task->Callback, std::move(*inValue));
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }
};

template <class TExecutor, class TCallback, class TInValue>
struct FutureTaskInvoke<TExecutor, TCallback, TInValue, void, CallbackArgKind::ValueRef> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    auto inValue = parentFuture->GetValue().As<TInValue>();
    task->Executor.Invoke(task->Callback, *inValue);
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }
};

template <class TExecutor, class TCallback, class TInValue>
struct FutureTaskInvoke<TExecutor, TCallback, TInValue, void, CallbackArgKind::Maybe> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    auto inValue = parentFuture->GetValue().As<TInValue>();
    task->Executor.Invoke(task->Callback, Mso::Maybe<TInValue>(std::move(*inValue)));
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }
};

template <class TExecutor, class TCallback, class TInValue>
struct FutureTaskInvoke<TExecutor, TCallback, TInValue, void, CallbackArgKind::MaybeRef> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture *parentFuture) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    auto inValue = parentFuture->GetValue().As<TInValue>();
    Mso::Maybe<TInValue> maybeValue{std::move(*inValue)};
    task->Executor.Invoke(task->Callback, maybeValue);
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }
};

template <class TExecutor, class TCallback>
struct FutureTaskInvoke<TExecutor, TCallback, void, void, CallbackArgKind::Void> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture * /*parentFuture*/) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    task->Executor.Invoke(task->Callback);
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }
};

template <class TExecutor, class TCallback>
struct FutureTaskInvoke<TExecutor, TCallback, void, void, CallbackArgKind::MaybeVoid> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture * /*parentFuture*/) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    task->Executor.Invoke(task->Callback, Mso::Maybe<void>());
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }
};

template <class TExecutor, class TCallback>
struct FutureTaskInvoke<TExecutor, TCallback, void, void, CallbackArgKind::MaybeVoidRef> {
  static void Invoke(const ByteArrayView &taskBuffer, IFuture *future, IFuture * /*parentFuture*/) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    Mso::Maybe<void> maybeValue;
    task->Executor.Invoke(task->Callback, maybeValue);
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }
};

template <class TExecutor, class TCallback, class TInValue, class TResult, CallbackArgKind argKind>
struct FutureTaskCatch {
  constexpr static FutureCatchCallback *CatchPtr = nullptr;
};

template <class TExecutor, class TCallback, class TInValue, class TResult>
struct FutureTaskCatch<TExecutor, TCallback, TInValue, TResult, CallbackArgKind::Maybe> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    ResultSetter<TResult>::Set(
        future, task->Executor.Invoke(task->Callback, Mso::Maybe<TInValue>(std::move(parentError))));
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class TExecutor, class TCallback, class TInValue, class TResult>
struct FutureTaskCatch<TExecutor, TCallback, TInValue, TResult, CallbackArgKind::MaybeRef> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    Mso::Maybe<TInValue> maybeValue{std::move(parentError)};
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback, maybeValue));
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class TExecutor, class TCallback, class TInValue, class TResult>
struct FutureTaskCatch<TExecutor, TCallback, TInValue, TResult, CallbackArgKind::ErrorCode> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback, std::move(parentError)));
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class TExecutor, class TCallback, class TResult>
struct FutureTaskCatch<TExecutor, TCallback, void, TResult, CallbackArgKind::MaybeVoid> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback, Mso::Maybe<void>(std::move(parentError))));
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class TExecutor, class TCallback, class TResult>
struct FutureTaskCatch<TExecutor, TCallback, void, TResult, CallbackArgKind::MaybeVoidRef> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    Mso::Maybe<void> maybeValue{std::move(parentError)};
    ResultSetter<TResult>::Set(future, task->Executor.Invoke(task->Callback, maybeValue));
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class TExecutor, class TCallback, class TInValue>
struct FutureTaskCatch<TExecutor, TCallback, TInValue, void, CallbackArgKind::Maybe> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    task->Executor.Invoke(task->Callback, Mso::Maybe<TInValue>(std::move(parentError)));
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class TExecutor, class TCallback, class TInValue>
struct FutureTaskCatch<TExecutor, TCallback, TInValue, void, CallbackArgKind::MaybeRef> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    Mso::Maybe<TInValue> maybeValue{std::move(parentError)};
    task->Executor.Invoke(task->Callback, maybeValue);
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class TExecutor, class TCallback, class TInValue>
struct FutureTaskCatch<TExecutor, TCallback, TInValue, void, CallbackArgKind::ErrorCode> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    task->Executor.Invoke(task->Callback, std::move(parentError));
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class TExecutor, class TCallback>
struct FutureTaskCatch<TExecutor, TCallback, void, void, CallbackArgKind::MaybeVoid> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    task->Executor.Invoke(task->Callback, Mso::Maybe<void>(std::move(parentError)));
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class TExecutor, class TCallback>
struct FutureTaskCatch<TExecutor, TCallback, void, void, CallbackArgKind::MaybeVoidRef> {
  static void Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept {
    auto task = taskBuffer.As<FutureTask<TExecutor, TCallback>>();
    Mso::Maybe<void> maybeValue{std::move(parentError)};
    task->Executor.Invoke(task->Callback, maybeValue);
    future->TrySetSuccess(/*crashIfFailed:*/ true);
  }

  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

} // namespace Futures
} // namespace Mso

#endif // MSO_FUTURE_DETAILS_FUTURETASK_H
