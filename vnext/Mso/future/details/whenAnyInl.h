// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// We do not use pragma once because the file is empty if FUTURE_INLINE_DEFS is not defined
#ifdef MSO_FUTURE_INLINE_DEFS

#ifndef MSO_FUTURE_DETAILS_WHENANYINL_H
#define MSO_FUTURE_DETAILS_WHENANYINL_H

namespace Mso {
namespace Futures {

template <class T>
struct WhenAnyTaskInvoke {
  static void Invoke(const ByteArrayView & /*taskBuffer*/, _In_ IFuture *future, _In_ IFuture *parentFuture) noexcept {
    ByteArrayView valueBuffer;
    if (future->TryStartSetValue(/*ref*/ valueBuffer)) {
      auto value = reinterpret_cast<T *>(parentFuture->GetValue().VoidDataChecked(sizeof(T)));
      ::new (valueBuffer.VoidDataChecked(sizeof(T))) T(std::move(*value));
      future->TrySetSuccess(/*crashIfFailed:*/ true);
    }
  }
};

template <>
struct WhenAnyTaskInvoke<void> {
  LIBLET_PUBLICAPI _Callback_ static void
  Invoke(const ByteArrayView &taskBuffer, _In_ IFuture *future, _In_ IFuture *parentFuture) noexcept;
};

struct WhenAnyTaskCatch {
  WhenAnyTaskCatch() = delete;
  ~WhenAnyTaskCatch() = delete;

  LIBLET_PUBLICAPI static void
  Catch(const ByteArrayView &taskBuffer, IFuture *future, ErrorCode &&parentError) noexcept;
  constexpr static FutureCatchCallback *CatchPtr = &Catch;
};

} // namespace Futures

template <class T>
Future<T> WhenAny(Mso::Async::ArrayView<Future<T>> futures) noexcept {
  VerifyElseCrashSzTag(futures.Size() > 0, "Must have at least one parent future.", 0x012ca416 /* tag_blkqw */);

  constexpr const auto &futureTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::IsMultiPost,
      /*ResultType: */ T,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ Mso::Futures::WhenAnyTaskInvoke<T>,
      /*CatchType:  */ Mso::Futures::WhenAnyTaskCatch>::Traits;

  Mso::CntPtr<Mso::Futures::IFuture> whenAnyFuture = Mso::Futures::MakeFuture(futureTraits, 0, nullptr);

  for (const Future<T> &parentFuture : futures) {
    Mso::GetIFuture(parentFuture)->AddContinuation(Mso::CntPtr{whenAnyFuture});
  }

  return Future<T>(std::move(whenAnyFuture));
}

template <class T>
inline Future<T> WhenAny(std::initializer_list<Future<T>> futures) noexcept {
  return WhenAny(Mso::Async::ArrayView<Future<T>>(futures));
}

template <class T, size_t size>
inline Future<T> WhenAny(Future<T> (&futures)[size]) noexcept {
  return WhenAny(Mso::Async::ArrayView<Future<T>>(futures));
}

template <class T>
inline Future<T> WhenAny(const std::vector<Future<T>> &futures) noexcept {
  return WhenAny(Mso::Async::ArrayView<Future<T>>(futures.data(), futures.size()));
}

template <size_t size>
inline Future<void> WhenAny(Future<void> (&futures)[size]) noexcept {
  return WhenAny(Mso::Async::ArrayView<Future<void>>(futures));
}

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_WHENANYINL_H
#endif // MSO_FUTURE_INLINE_DEFS
