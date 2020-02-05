// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "activeObject/activeObject.h"
#include "future/future.h"

namespace Mso {

//! A simple function that calls a copy constructor for T.
//! It is more compact to use comparing with a constructor call.
//! For example if we have a function that accepts an r-value:
//!
//! SetValue(Mso::CntPtr<IValue>&& value);
//!
//! and we want to give it a value which is constant in our context,
//! then we can call the Mso::Copy:
//!
//! const Mso::CntPtr<IValue> m_value;
//!
//! SetValue(Mso::Copy(m_value));
//!
//! It is more compact and better conveys semantic than the copy constructor call:
//!
//! SetValue(Mso::TCntPtr<IValue>{m_value});
//!
template <class T>
T Copy(const T &obj) noexcept(noexcept(T{obj})) {
  return T{obj};
}

//! Creates an Mso::Functor for a member function, that captures 'this'
//! as a weak pointer Mso::WeakPtr.
//! The member function is only called when the weak pointer is successfully
//! resolved to a strong pointer at the call time.
template <class T, class TReturn, class... TArgs>
Mso::Functor<TReturn(TArgs...)> MakeWeakMemberFunctor(T *self, TReturn (T::*memberFunc)(TArgs...)) noexcept {
  return [ weakSelf = Mso::WeakPtr<T>{self}, memberFunc ](TArgs... args) noexcept {
    if (auto strongSelf = weakSelf.GetStrongPtr()) {
      (strongSelf.Get()->*memberFunc)(args...);
    }
  };
}

//! Creates an std::function for a member function, that captures 'this'
//! as a weak pointer Mso::WeakPtr.
//! The member function is only called when the weak pointer is successfully
//! resolved to a strong pointer at the call time.
template <class T, class TReturn, class... TArgs>
std::function<TReturn(TArgs...)> MakeWeakMemberStdFunction(T *self, TReturn (T::*memberFunc)(TArgs...)) noexcept {
  return std::function<TReturn(TArgs...)>([ weakSelf = Mso::WeakPtr<T>{self}, memberFunc ](TArgs... args) noexcept {
    if (auto strongSelf = weakSelf.GetStrongPtr()) {
      (strongSelf.Get()->*memberFunc)(args...);
    }
  });
}

struct InvokeElsePostExecutor : Mso::Executors::Internal::ExecutorInvoker {
  InvokeElsePostExecutor(Mso::DispatchQueue const &queue) noexcept : m_queue{queue} {}

  void Post(Mso::DispatchTask &&task) noexcept {
    m_queue.InvokeElsePost(std::move(task));
  }

 private:
  Mso::DispatchQueue m_queue;
};

//! Returns future that indicates that all provided futures are completed.
//! In case if completed futures have errors, then the returned future contains only the
//! first error based on the list of provided futures.
Mso::Future<void> WhenAllCompleted(const std::vector<Mso::Future<void>> &futures) noexcept;

//! Creates a failed future using the Mso::CancellationErrorProvider().
Mso::Future<void> MakeCanceledFuture() noexcept;

//! Sets value of the promise to the result of the valueSource Future.
void SetPromiseValue(Mso::Promise<void> &&promise, const Mso::Future<void> &valueSource) noexcept;

//! Invokes lambda for each element in the collection and gives it item and index as parameters.
template <class TCollection, class TLambda>
void ForEach(TCollection &&collection, TLambda &&lambda) noexcept {
  size_t index = 0;
  for (const auto &item : collection) {
    lambda(item, index++);
  }
}

} // namespace Mso
