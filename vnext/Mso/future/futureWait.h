// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_FUTUREWAIT_H
#define MSO_FUTURE_FUTUREWAIT_H

/** \file futureWait.h

FutureWait functions block a thread until completion of a future.
These functions should be used sparingly and judiciously, and only where a task must complete in a blocking manner (e.g.
waiting for asynchronous work to complete on shutdown or suspend) Mso::Future does not implement a Wait method
intentionally, to avoid accidental use or misuse; these functions are intended to supplement that for the rare cases
where a blocking Wait is required.
*/

#include "eventWaitHandle/eventWaitHandle.h"
#include "future/Future.h"

namespace Mso {

//=============================================================================
// Mso::Future synchronous blocking functions.
//=============================================================================

template <class T>
Mso::Maybe<T> FutureWait(const Mso::Future<T> &future) noexcept {
  Mso::Maybe<T> value;
  Mso::ManualResetEvent finished;
  future.Then(Mso::Executors::Inline{}, [&finished, &value ](Mso::Maybe<T> && result) noexcept {
    value = std::move(result);
    finished.Set();
  });
  finished.Wait();
  return value;
}

template <class T>
inline T FutureWaitAndGetValue(const Mso::Future<T> &future) noexcept {
  return FutureWait(future).TakeValue();
}

template <class T>
inline Mso::ErrorCode FutureWaitAndGetError(const Mso::Future<T> &future) noexcept {
  return FutureWait(future).TakeError();
}

template <class T>
inline bool FutureWaitIsSucceeded(const Mso::Future<T> &future) noexcept {
  return FutureWait(future).IsValue();
}

template <class T>
inline bool FutureWaitIsFailed(const Mso::Future<T> &future) noexcept {
  return FutureWait(future).IsError();
}

} // namespace Mso

#endif // MSO_FUTURE_FUTUREWAIT_H
