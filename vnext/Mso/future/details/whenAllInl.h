// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// We do not use pragma once because the file is empty if FUTURE_INLINE_DEFS is not defined
#ifdef MSO_FUTURE_INLINE_DEFS

#ifndef MSO_FUTURE_DETAILS_WHENALLINL_H
#define MSO_FUTURE_DETAILS_WHENALLINL_H

namespace Mso {
namespace Futures {

template <class T>
struct RawOrCntPtr {
  RawOrCntPtr() = default;
  RawOrCntPtr(const RawOrCntPtr &) = delete;
  RawOrCntPtr &operator=(const RawOrCntPtr &) = delete;

  RawOrCntPtr(T *ptr) noexcept
      : m_ptr((reinterpret_cast<uintptr_t>(ptr) >> 1) | (static_cast<uintptr_t>(1) << (sizeof(uintptr_t) * 8 - 1))) {}

  ~RawOrCntPtr() noexcept {
    if (!IsRawPtr()) {
      T *ptr = Get();
      if (ptr) {
        ptr->Release();
      }
    }

    m_ptr = 0;
  }

  T *Get() const noexcept {
    return const_cast<T *>(reinterpret_cast<const T *>(m_ptr << 1));
  }

  bool IsRawPtr() const noexcept {
    return (m_ptr >> (sizeof(uintptr_t) * 8 - 1)) != 0;
  }

  void ConvertToCntPtr() noexcept {
    if (IsRawPtr()) {
      T *ptr = Get();
      if (ptr) {
        ptr->AddRef();
        m_ptr = reinterpret_cast<uintptr_t>(ptr) >> 1;
      }
    }
  }

 private:
  uintptr_t m_ptr{0};
};

template <class T>
struct WhenAllFutureTask {
  std::atomic<uint32_t> CompleteCount;
  uint32_t FutureCount;

  // List of pointers to parent futures. We use these pointers to copy values when setting result value.
  // This field must be the last one in the struct because we assume that other array elements are allocated after the
  // WhenAllFutureTask struct. Also, after this array we allocate an array for result values.
  RawOrCntPtr<Mso::Futures::IFuture> ParentFutures[1];

  // Used by Mso::WhenAll that returns tuple. We only have specialization for void type T.
  WhenAllFutureTask(Mso::Futures::IFuture *futureState, std::initializer_list<Mso::Futures::IFuture *> init) noexcept;

  WhenAllFutureTask() = delete;
  ~WhenAllFutureTask() = delete;

  static constexpr size_t GetAlignedSize(size_t size) noexcept {
    return (size + std::alignment_of<T>::value - 1) & ~(std::alignment_of<T>::value - 1);
  }

  static constexpr size_t GetTaskSize(size_t futureCount) noexcept {
    return (futureCount > 0)
        ? GetAlignedSize(sizeof(WhenAllFutureTask) + (futureCount - 1) * sizeof(RawOrCntPtr<Mso::Futures::IFuture>)) +
            futureCount * sizeof(T)
        : sizeof(WhenAllFutureTask);
  }

  T *GetValuePtr() noexcept {
    T *ptr = reinterpret_cast<T *>(
        reinterpret_cast<uint8_t *>(this) +
        GetAlignedSize(sizeof(WhenAllFutureTask) + (FutureCount - 1) * sizeof(RawOrCntPtr<Mso::Futures::IFuture>)));
    VerifyElseCrashSzTag(
        (reinterpret_cast<uintptr_t>(ptr) & (std::alignment_of<T>::value - 1)) == 0,
        "WhenAll value is not aligned",
        0x016056db /* tag_byf11 */);
    return ptr;
  }

  static void Destroy(const ByteArrayView &obj) noexcept {
    auto task = static_cast<const WhenAllFutureTask *>(obj.VoidData());
    VerifyElseCrashTag(obj.Size() == GetTaskSize(task->FutureCount), 0x016056dc /* tag_byf12 */);

    for (uint32_t i = 0; i < task->FutureCount; ++i) {
      (&(task->ParentFutures[0]) + i)->~RawOrCntPtr();
    }

    // Check if all futures were successful and we have stored result array.
    if (task->CompleteCount == task->FutureCount) {
      T *valuePtr = const_cast<WhenAllFutureTask *>(task)->GetValuePtr();
      for (size_t i = 0; i < task->FutureCount; ++i) {
        valuePtr[i].~T();
      }
    }
  }
};

template <>
struct WhenAllFutureTask<void> {
  std::atomic<uint32_t> CompleteCount;
  uint32_t FutureCount;

  // List of pointers to parent futures. We use these pointers to copy values when setting result value.
  // This field must be the last one in the struct because we assume that other array elements are allocated after the
  // WhenAllFutureTask struct. We do not store result array because type is void.
  RawOrCntPtr<Mso::Futures::IFuture> ParentFutures[1];

  // Used by Mso::WhenAll that returns tuple.
  LIBLET_PUBLICAPI WhenAllFutureTask(
      Mso::Futures::IFuture *futureState,
      std::initializer_list<Mso::Futures::IFuture *> init) noexcept;

  WhenAllFutureTask() = delete;
  ~WhenAllFutureTask() = delete;

  static constexpr size_t GetTaskSize(size_t futureCount) noexcept {
    return (futureCount > 0)
        ? sizeof(WhenAllFutureTask) + (futureCount - 1) * sizeof(RawOrCntPtr<Mso::Futures::IFuture>)
        : sizeof(WhenAllFutureTask);
  }

  LIBLET_PUBLICAPI static void Destroy(const ByteArrayView &obj) noexcept;
};

// ValueTraits specialization to enable use of WhenAllFutureTask in FutureTraitsProvider.
template <class T>
struct ValueTraits<WhenAllFutureTask<T>, false> {
  constexpr static FutureDestroyCallback *DestroyPtr = &WhenAllFutureTask<T>::Destroy;
};

template <class T>
struct WhenAllTaskInvoke {
  static void Invoke(const ByteArrayView &taskBuffer, _In_ IFuture *future, _In_ IFuture *parentFuture) noexcept {
    auto task = reinterpret_cast<WhenAllFutureTask<T> *>(taskBuffer.VoidData());
    VerifyElseCrashTag(
        taskBuffer.Size() == WhenAllFutureTask<T>::GetTaskSize(task->FutureCount), 0x016056dd /* tag_byf13 */);

    bool found = false;
    for (size_t i = 0; i < task->FutureCount; ++i) {
      auto &storedFuture = task->ParentFutures[i];
      if (parentFuture == storedFuture.Get()) {
        storedFuture.ConvertToCntPtr();
        found = true;
        break;
      }
    }

    VerifyElseCrashSzTag(found, "parent future is not found", 0x012ca410 /* tag_blkqq */);

    if (++task->CompleteCount == task->FutureCount) {
      // All parent futures completed: copy results to the WhenAllFutureTask value storage.
      ByteArrayView valueBuffer;
      (void)future->TryStartSetValue(/*ref*/ valueBuffer, /*crashIfFailed:*/ true);
      T *valuePtr = task->GetValuePtr();
      for (size_t i = 0; i < task->FutureCount; ++i) {
        ::new (std::addressof(valuePtr[i]))
            T(std::move(*reinterpret_cast<T *>(task->ParentFutures[i].Get()->GetValue().VoidData())));
      }
      ::new (valueBuffer.VoidData()) Mso::Async::ArrayView<T>(valuePtr, task->FutureCount);
      (void)future->TrySetSuccess(/*crashIfFailed:*/ true);
    }
  }
};

template <>
struct WhenAllTaskInvoke<void> {
  LIBLET_PUBLICAPI _Callback_ static void
  Invoke(const ByteArrayView &taskBuffer, _In_ IFuture *future, _In_ IFuture *parentFuture) noexcept;
};

struct WhenAllTaskCatch {
  WhenAllTaskCatch() = delete;
  ~WhenAllTaskCatch() = delete;

  LIBLET_PUBLICAPI static void
  Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept;
  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

template <class... Ts, size_t... I>
_Callback_ inline void CreateTuple(
    ByteArrayView &valueBuffer,
    RawOrCntPtr<Mso::Futures::IFuture> *futures,
    std::integer_sequence<size_t, I...>) noexcept {
  ::new (valueBuffer.VoidData())
      std::tuple<Ts...>(std::move(*reinterpret_cast<Ts *>(futures[I].Get()->GetValue().VoidData()))...);
}

template <class... Ts>
struct WhenAllTupleTaskInvoke {
  static void Invoke(const ByteArrayView &taskBuffer, _In_ IFuture *future, _In_ IFuture *parentFuture) noexcept {
    constexpr const size_t futureCount = sizeof...(Ts);
    constexpr const size_t taskSize = WhenAllFutureTask<void>::GetTaskSize(futureCount);
    auto task = reinterpret_cast<WhenAllFutureTask<void> *>(taskBuffer.VoidDataChecked(taskSize));
    bool found = false;
    for (size_t i = 0; i < futureCount; ++i) {
      auto &storedFuture = task->ParentFutures[i];
      if (parentFuture == storedFuture.Get()) {
        storedFuture.ConvertToCntPtr();
        found = true;
        break;
      }
    }

    VerifyElseCrashSzTag(found, "parent future is not found", 0x012ca412 /* tag_blkqs */);

    if (++task->CompleteCount == futureCount) {
      ByteArrayView valueBuffer;
      (void)future->TryStartSetValue(/*ref*/ valueBuffer, /*crashIfFailed:*/ true);
      CreateTuple<Ts...>(/*ref*/ valueBuffer, task->ParentFutures, std::make_index_sequence<futureCount>());
      (void)future->TrySetSuccess(/*crashIfFailed:*/ true);
    }
  }
};

} // namespace Futures

template <class T>
inline Future<Mso::Async::ArrayView<T>> WhenAll(Mso::Async::ArrayView<Future<T>> futures) noexcept {
  if (futures.Size() == 0) {
    return MakeSucceededFutureEmplaced<Mso::Async::ArrayView<T>>();
  }

  using TaskType = Mso::Futures::WhenAllFutureTask<T>;

  // Along with TaskType we allocate space for pointers to parent futures, and the array of results.
  const size_t taskSize = TaskType::GetTaskSize(futures.Size());

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::IsMultiPost,
      /*ResultType: */ Mso::Async::ArrayView<T>,
      /*TaskType:   */ TaskType,
      /*PostType:   */ void,
      /*InvokeType: */ Mso::Futures::WhenAllTaskInvoke<T>,
      /*CatchType:  */ Mso::Futures::WhenAllTaskCatch>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  Mso::CntPtr<Mso::Futures::IFuture> whenAllFuture = Mso::Futures::MakeFuture(futureTraits, taskSize, &taskBuffer);
  TaskType *task = reinterpret_cast<TaskType *>(taskBuffer.VoidDataChecked(taskSize));
  task->CompleteCount = 0;
  task->FutureCount = static_cast<uint32_t>(futures.Size());

  size_t i = 0;
  for (const Future<T> &parentFuture : futures) {
    // Set raw pointer to parent futures.
    ::new (&task->ParentFutures[i++]) Mso::Futures::RawOrCntPtr<Mso::Futures::IFuture>(Mso::GetIFuture(parentFuture));
  }

  // Use a separate loop to add whenAllFuture to the parent futures because parent futures may start
  // invoke our whenAllFuture while we still in this function.
  for (const Future<T> &parentFuture : futures) {
    Mso::GetIFuture(parentFuture)->AddContinuation(Mso::CntPtr{whenAllFuture});
  }

  return Future<Mso::Async::ArrayView<T>>(std::move(whenAllFuture));
}

template <class T>
inline Future<Mso::Async::ArrayView<T>> WhenAll(std::initializer_list<Future<T>> futures) noexcept {
  return WhenAll(Mso::Async::ArrayView<Future<T>>(futures));
}

template <class T, size_t size>
inline Future<Mso::Async::ArrayView<T>> WhenAll(Future<T> (&futures)[size]) noexcept {
  return WhenAll(Mso::Async::ArrayView<Future<T>>(futures));
}

template <class T>
inline Future<Mso::Async::ArrayView<T>> WhenAll(const std::vector<Mso::Future<T>> &futures) noexcept {
  return WhenAll(Mso::Async::ArrayView<Future<T>>(futures.data(), futures.size()));
}

template <size_t size>
inline Future<void> WhenAll(Future<void> (&futures)[size]) noexcept {
  return WhenAll(Mso::Async::ArrayView<Future<void>>(futures));
}

template <class T0, class... Ts>
Future<std::tuple<T0, Ts...>> WhenAll(const Future<T0> &future0, const Future<Ts> &... futures) noexcept {
  using ResultType = std::tuple<T0, Ts...>;
  using TaskType = Mso::Futures::WhenAllFutureTask<void>;
  const size_t taskSize = TaskType::GetTaskSize(1 + sizeof...(Ts));

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:     */ Mso::Futures::FutureOptions::IsMultiPost,
      /*ResultType:  */ ResultType,
      /*TaskType:    */ TaskType,
      /*PostType:    */ void,
      /*InvokeType:  */ Mso::Futures::WhenAllTupleTaskInvoke<T0, Ts...>,
      /*AbandonType: */ Mso::Futures::WhenAllTaskCatch>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  Mso::CntPtr<Mso::Futures::IFuture> whenAllFuture = Mso::Futures::MakeFuture(futureTraits, taskSize, &taskBuffer);
  ::new (taskBuffer.VoidData()) TaskType(whenAllFuture.Get(), {Mso::GetIFuture(future0), Mso::GetIFuture(futures)...});

  return Future<ResultType>(std::move(whenAllFuture));
}

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_WHENALLINL_H
#endif // MSO_FUTURE_INLINE_DEFS
