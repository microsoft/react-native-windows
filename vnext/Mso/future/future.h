// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_FUTURE_H
#define MSO_FUTURE_FUTURE_H

#include <chrono>
#include <memory>
#include <vector>
#include "compilerAdapters/managedCpp.h"
#include "errorCode/maybe.h"
#include "object/weakPtr.h"

MSO_PRAGMA_MANAGED_PUSH_OFF

#include "details/cancellationErrorProvider.h"
#include "details/executor.h"
#include "details/futureTask.h"
#include "details/ifuture.h"
#include "details/resultTraits.h"
#include "details/timeoutException.h"
#include "future/cancellationToken.h"
#include "future/futureForwardDecl.h"

/** \file Future.h

Office's implementation of Futures/Promises, which are a useful abstraction for writing asynchronous code. It has
several advantages over the STL's implementation of futures:

- Full support for executors: async operations default to running concurrently in the thread pool, but all of them
  allow you to specify an executor to control where the code runs.

- Always non-blocking: Future doesn't provide any direct support for waiting on the result, like std::future::get().
  Instead, Future uses continuations (which std::future doesn't support). This encourages a programming style that
  makes deadlocks much less likely.

- Integrated non-throwing error handling: Office generally prefers not to use exceptions, because they impact our
  binary size. Future lets you transparently use Mso::ErrorCode and Mso::Maybe<T> for error handling instead of
  exceptions, and it gives you a lot of control over which callbacks actually need to handle error cases.

- Support for move semantics: Future only allows a single continuation by default, which allows it to efficiently
  pass movable and move-only types to continuations. Multiple continuations are possible, but must be explicitly
  opted-into by using SharedFuture.

- Optimized for code size: Future and Promise are structured so that most of the implementation can live in a MSO
  DLL, rather than being inlined everywhere it's used.

- Optimized for memory usage: All internal state of the Future lives in a single heap allocation, which is shared
  between the Promise and the Future bound to it. Also, because the shared state is already ref-counted, Future
  and Promise can be efficiently copied without needing to wrap them in another layer of smart pointers.

Class summary:

- Promise is a mutable variable that can be set only once.

- Future is a construct that allows to schedule a task and observe its result.
  In our implementation Future may have only a single continuation created with a .Then() method.
  This restriction allows to support move semantic for values where the continuation can move result of the
  previous future instead of copying it.

- SharedFuture is a Future that allows to have multiple continuations, but in expense of not supporting the move
semantic for the returning values. To create one use Future::Share() method.
*/

namespace Mso {

//=============================================================================
// Mso::Future<T> creation functions.
//=============================================================================

//! Create an instance of Mso::Future<T> based on a callback, and post it to executor.
template <class TExecutor = Mso::Executors::Concurrent, class TCallback>
auto PostFuture(TCallback &&callback) noexcept;

//! Create an instance of a future based on a callback, and post it to executor.
template <class TExecutor, class TCallback>
auto PostFuture(TExecutor &&executor, TCallback &&callback) noexcept;

//! Create an instance of completed Mso::Future<T> from a provided value.
template <class T>
auto MakeCompletedFuture(T &&value) noexcept;

//! Create an instance of completed Mso::Future<void>.
Future<void> MakeCompletedFuture() noexcept;

//! Create an instance of completed Mso::Future<T> from a value created in-place.
template <class T, class... TArgs>
Future<T> MakeCompletedFutureEmplaced(TArgs &&... args) noexcept;

//! Create an instance of completed Mso::Future<T> from a value created in-place.
template <class T, class U, class... TArgs>
Future<T> MakeCompletedFutureEmplaced(std::initializer_list<U> il, TArgs &&... args) noexcept;

//! Create an instance of succeeded Mso::Future<T> from a provided value.
template <class T>
auto MakeSucceededFuture(T &&value) noexcept;

//! Create an instance of succeeded Mso::Future<void>.
Future<void> MakeSucceededFuture() noexcept;

//! Create an instance of completed Mso::Future<T> from a value created in-place.
template <class T, class... TArgs>
Future<T> MakeSucceededFutureEmplaced(TArgs &&... args) noexcept;

//! Create an instance of completed Mso::Future<T> from a value created in-place.
template <class T, class U, class... TArgs>
Future<T> MakeSucceededFutureEmplaced(std::initializer_list<U> il, TArgs &&... args) noexcept;

//! Create an instance of Mso::Future<T> with error code.
template <class T>
Future<T> MakeFailedFuture(Mso::ErrorCode &&error) noexcept;

//! Create an instance of Mso::Future<T> with error code.
template <class T>
Future<T> MakeFailedFuture(const Mso::ErrorCode &error) noexcept;

//=============================================================================
// Mso::WhenAll overloads.
//=============================================================================

//! WhenAll returns a future which is completed when all input futures are completed.
//! Receives an array view of Future instances. The input array view can be empty.
//! Returns Future<Mso::Async::ArrayView<T>> where we have a view to an array of input future results.
template <class T>
Future<Mso::Async::ArrayView<T>> WhenAll(Mso::Async::ArrayView<Future<T>> futures) noexcept;

//! WhenAll returns a future which is completed when all input futures are completed.
//! Receives an initializer list of Future instances. The list can be empty.
//! Returns Future<Mso::Async::ArrayView<T>> where we have a view to an array of input future results.
template <class T>
Future<Mso::Async::ArrayView<T>> WhenAll(std::initializer_list<Future<T>> futures) noexcept;

//! WhenAll returns a future which is completed when all input futures are completed.
//! Receives an array of Future instances.
//! Returns Future<Mso::Async::ArrayView<T>> where we have a view to an array of input future results.
template <class T, size_t size>
Future<Mso::Async::ArrayView<T>> WhenAll(Future<T> (&futures)[size]) noexcept;

//! WhenAll returns a future which is completed when all input futures are completed.
//! Receives a std::vector of Future instances. The vector can be empty.
//! Returns Future<Mso::Async::ArrayView<T>> where we have a view to an array of input future results.
template <class T>
Future<Mso::Async::ArrayView<T>> WhenAll(const std::vector<Future<T>> &futures) noexcept;

//! WhenAll returns a future which is completed when all input futures are completed.
//! Receives an array view of Future instances. The input array view can be empty.
//! Returns Future<void>.
LIBLET_PUBLICAPI Future<void> WhenAll(Mso::Async::ArrayView<Future<void>> futures) noexcept;

//! WhenAll returns a future which is completed when all input futures are completed.
//! Receives an initializer list of Future instances. The list can be empty.
//! Returns Future<void>.
LIBLET_PUBLICAPI Future<void> WhenAll(std::initializer_list<Future<void>> futures) noexcept;

//! WhenAll returns a future which is completed when all input futures are completed.
//! Receives an array of Future instances.
//! Returns Future<void>.
template <size_t size>
Future<void> WhenAll(Future<void> (&futures)[size]) noexcept;

//! WhenAll returns a future which is completed when all input futures are completed.
//! Receives a std::vector of Future instances. The vector can be empty.
//! Returns Future<void>.
LIBLET_PUBLICAPI Future<void> WhenAll(const std::vector<Future<void>> &futures) noexcept;

//! WhenAll returns a future which is completed when all input futures are completed.
//! Receives a non-empty list of Future<T>. E.g. WhenAll(future1, future2, future3);
//! Each future may have its own return type.
//! Returns Future<std::tuple<Ts...>>.
template <class T0, class... Ts>
Future<std::tuple<T0, Ts...>> WhenAll(const Future<T0> &future0, const Future<Ts> &... futures) noexcept;

//=============================================================================
// Mso::WhenAny overloads.
//=============================================================================

//! WhenAny returns a future which is completed when one of the input futures is completed.
//! Receives an array view of Future instances. The input array view must not be empty.
//! Returns Future<T> with the value or error of the first completed future.
template <class T>
Future<T> WhenAny(Mso::Async::ArrayView<Future<T>> futures) noexcept;

//! WhenAny returns a future which is completed when one of the input futures is completed.
//! Receives an initializer list of Future instances. The input list must not be empty.
//! Returns Future<T> with the value or error of the first completed future.
template <class T>
Future<T> WhenAny(std::initializer_list<Future<T>> futures) noexcept;

//! WhenAny returns a future which is completed when one of the input futures is completed.
//! Receives an array of Future instances.
//! Returns Future<T> with the value or error of the first completed future.
template <class T, size_t size>
Future<T> WhenAny(Future<T> (&futures)[size]) noexcept;

//! WhenAny returns a future which is completed when one of the input futures is completed.
//! Receives an std::vector of Future instances. The vector must not be empty.
//! Returns Future<T> with the value or error of the first completed future.
template <class T>
Future<T> WhenAny(const std::vector<Future<T>> &futures) noexcept;

//! WhenAny returns a future which is completed when one of the input futures is completed.
//! Receives an array view of Future instances. The input array view must not be empty.
//! Returns Future<void> with the result of the first completed future.
LIBLET_PUBLICAPI Future<void> WhenAny(Mso::Async::ArrayView<Future<void>> futures) noexcept;

//! WhenAny returns a future which is completed when one of the input futures is completed.
//! Receives an initializer list of Future instances. The input list must not be empty.
//! Returns Future<void> with the result of the first completed future.
LIBLET_PUBLICAPI Future<void> WhenAny(std::initializer_list<Future<void>> futures) noexcept;

//! WhenAny returns a future which is completed when one of the input futures is completed.
//! Receives an array of Future instances.
//! Returns Future<void> with the result of the first completed future.
template <size_t size>
Future<void> WhenAny(Future<void> (&futures)[size]) noexcept;

//! WhenAny returns a future which is completed when one of the input futures is completed.
//! Receives an std::vector of Future instances. The vector must not be empty.
//! Returns Future<void> with the result of the first completed future.
LIBLET_PUBLICAPI Future<void> WhenAny(const std::vector<Future<void>> &futures) noexcept;

//=============================================================================
// Mso::WhenDoneOrTimeout declaration.
//=============================================================================

//! WhenDoneOrTimeout returns a Future which is completed when either the given Future
//! completes, or after a timeout.
//! If the timeout expires, the resulting Future will contain an ErrorCode from the
//! Mso::Async::TimeoutError provider.
// TODO: implement
// template <class T>
// Mso::Future<T> WhenDoneOrTimeout(const Mso::Future<T>& future, std::chrono::milliseconds timeout) noexcept;

//=============================================================================
// Mso::GetIFuture declaration.
//=============================================================================

template <class T>
extern Mso::Futures::IFuture *GetIFuture(const T &future) noexcept;

//=============================================================================
// Definition of Promise, Future, and SharedFuture classes.
//=============================================================================

//! Promise is a mutable variable that can be set at a later point of time.
//! Use GetFuture to observe changes in the Promise instance.
//! Promise is essentially an Mso::CntPtr to an instance of IFuture.
//! The Promise can be empty or non-empty depending on the pointer being null or not null. Calling methods requiring
//! non-empty Promise will cause crash.
//! The value can be only set once. SetValue and EmplaceValue set the value and change state to Completed.
//! If value cannot be set because it is already set or the Promise is abandoned then we crash.
//! In cases when the value can be set multiple times and you need only the first value then you can use
//! the TrySetValue or TryEmplaceValue methods to avoid crash. They also return false if Promise is abandoned.
//! The Abandoned state means that none of the continuation futures will be invoked. Though they will be notified
//! about the state change to Abandoned. Future or SharedFuture will crash when its source Promise or Future
//! is abandoned. Only OptionalFuture and OptionalSharedFuture can be abandoned without crash.
//! If Promise is not completed or abandoned before destruction then its state is changed to Abandoned in destructor.
//! The TryAbandon method can be used to explicitly abandon the Promise if it is not completed or abandoned yet.
template <class T>
struct Promise {
  /// Creates new Promise with non-empty state.
  Promise() noexcept;

  /// Creates new Promise with empty state.
  Promise(std::nullptr_t) noexcept;

  /// Creates new Promise with provided state.
  explicit Promise(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept;

  /// Creates new Promise with the same state as the other Promise.
  Promise(const Promise &other) noexcept;

  /// Creates new Promise with the state taken from the other Promise. The other Promise state becomes empty.
  Promise(Promise &&other) noexcept;

  /// Assigns the state from the other promise.
  Promise &operator=(const Promise &other) noexcept;

  /// Assigns the state taken from the other promise. The other Promise state becomes empty.
  Promise &operator=(Promise &&other) noexcept;

  /// Swaps states with the other Promise.
  void Swap(Promise &other) noexcept;

  /// True if state is not empty.
  explicit operator bool() const noexcept;

  /// Casts Promise to Future.
  Future<T> AsFuture() const noexcept;

  /// Sets the value and completes the Promise. It can be called only once. Otherwise it crashes the app.
  void SetValue(const T &value) const noexcept;

  /// Sets the value and completes the Promise. It can be called only once. Otherwise it crashes the app.
  void SetValue(T &&value) const noexcept;

  /// Tries to set the value if Promise is not completed or abandoned yet. Returns true on success.
  bool TrySetValue(const T &value) const noexcept;

  /// Tries to set the value if Promise is not completed or abandoned yet. Returns true on success.
  bool TrySetValue(T &&value) const noexcept;

  /// Sets the value and completes the Promise. It can be called only once. Otherwise it crashes the app.
  void SetValue(const Mso::Maybe<T> &value) const noexcept;

  /// Sets the value and completes the Promise. It can be called only once. Otherwise it crashes the app.
  void SetValue(Mso::Maybe<T> &&value) const noexcept;

  /// Tries to set the value if Promise is not completed or abandoned yet. Returns true on success.
  bool TrySetValue(const Mso::Maybe<T> &value) const noexcept;

  /// Tries to set the value if Promise is not completed or abandoned yet. Returns true on success.
  bool TrySetValue(Mso::Maybe<T> &&value) const noexcept;

  /// Sets the value in-place and completes the Promise. It can be called only once. Otherwise it crashes the app.
  template <class... TArgs>
  void EmplaceValue(TArgs &&... args) const noexcept;

  /// Sets the value in-place and completes the PromiseGroup. It can be called only once. Otherwise it VECs.
  template <class TArg, class... TArgs>
  void EmplaceValue(std::initializer_list<TArg> init, TArgs &&... args) const noexcept;

  /// Tries to set the value in-place if Promise is not completed or abandoned yet. Returns true on success.
  template <class... TArgs>
  bool TryEmplaceValue(TArgs &&... args) const noexcept;

  /// Tries to set the value in-place if PromiseGroup is not completed or abandoned yet. Returns true on success.
  template <class TArg, class... TArgs>
  bool TryEmplaceValue(std::initializer_list<TArg> init, TArgs &&... args) const noexcept;

  /// Tries to set Promise to Failed state with a CancellationError. Returns true if cancel succeeded.
  bool TryCancel() const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. It crashes app if error code cannot be set.
  void SetError(const ErrorCode &errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. It crashes app if error code cannot be set.
  void SetError(ErrorCode &&errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. Returns true on success.
  bool TrySetError(const ErrorCode &errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. Returns true on success.
  bool TrySetError(ErrorCode &&errorCode) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It crashes app if value cannot be set.
  void SetMaybe(const Mso::Maybe<T> &value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It crashes app if value cannot be set.
  void SetMaybe(Mso::Maybe<T> &&value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It returns true on success.
  bool TrySetMaybe(const Mso::Maybe<T> &value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It returns true on success.
  bool TrySetMaybe(Mso::Maybe<T> &&value) const noexcept;

  template <class U>
  friend Mso::Futures::IFuture *GetIFuture(const U &promise) noexcept;

 private:
  Mso::CntPtr<Mso::Futures::IFuture> m_state;
};

//! Promise<void> is a mutable variable that can be completed at a later point of time.
//! Use GetFuture or GetOptionalFuture to observe changes in the Promise.
//! Promise is essentially an Mso::CntPtr to an instance of IFuture.
//! The Promise can be empty or non-empty depending on the pointer being null or not null. Calling methods requiring
//! non-empty Promise will cause crash.
//! The Promise can be completed only once. Complete() method changes state to Completed.
//! If Promise cannot be completed because it is already completed or the Promise is abandoned then we crash.
//! In cases when the Complete() can be called multiple times and you want ignore extra calls then you can use
//! the TryComplete method to avoid crash. It also returns false if state is Abandoned.
//! The Abandoned state means that none of the continuation futures will be invoked. Though they will be notified
//! about the state change to Abandoned. Future or SharedFuture will crash when its source Promise or Future
//! is abandoned. Only OptionalFuture and OptionalSharedFuture can be abandoned without crash.
//! If Promise is not completed or abandoned before destruction then its state is changed to Abandoned.
//! The TryAbandon method can be used to explicitly abandon the Promise if it is not completed or abandoned yet.
template <>
struct Promise<void> {
  /// Creates new Promise with non-empty state.
  LIBLET_PUBLICAPI Promise() noexcept;

  /// Creates new Promise with empty state.
  LIBLET_PUBLICAPI Promise(std::nullptr_t) noexcept;

  /// Creates new Promise with provided state.
  LIBLET_PUBLICAPI explicit Promise(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept;

  /// Creates new Promise with the same state as the other Promise.
  LIBLET_PUBLICAPI Promise(const Promise &other) noexcept;

  /// Creates new Promise with the state taken from the other Promise. The other Promise state becomes empty.
  LIBLET_PUBLICAPI Promise(Promise &&other) noexcept;

  /// Assigns the state from the other Promise.
  LIBLET_PUBLICAPI Promise &operator=(const Promise &other) noexcept;

  /// Assigns the state taken from the other Promise. The other Promise state becomes empty.
  LIBLET_PUBLICAPI Promise &operator=(Promise &&other) noexcept;

  /// Swaps states with the other Promise.
  LIBLET_PUBLICAPI void Swap(Promise &other) noexcept;

  /// True if state is not empty.
  LIBLET_PUBLICAPI explicit operator bool() const noexcept;

  /// Casts Promise to Future.
  LIBLET_PUBLICAPI Future<void> AsFuture() const noexcept;

  /// Sets Promise to the Succeeded state. It can be called only once. Otherwise it crashes the app.
  LIBLET_PUBLICAPI void SetValue() const noexcept;

  /// Tries to set Promise to Succeeded state. Returns true on success.
  LIBLET_PUBLICAPI bool TrySetValue() const noexcept;

  /// Sets the value and completes the Promise. It can be called only once. Otherwise it crashes the app.
  LIBLET_PUBLICAPI void SetValue(const Mso::Maybe<void> &value) const noexcept;

  /// Sets the value and completes the Promise. It can be called only once. Otherwise it crashes the app.
  LIBLET_PUBLICAPI void SetValue(Mso::Maybe<void> &&value) const noexcept;

  /// Tries to set the value if Promise is not completed or abandoned yet. Returns true on success.
  LIBLET_PUBLICAPI bool TrySetValue(const Mso::Maybe<void> &value) const noexcept;

  /// Tries to set the value if Promise is not completed or abandoned yet. Returns true on success.
  LIBLET_PUBLICAPI bool TrySetValue(Mso::Maybe<void> &&value) const noexcept;

  /// Tries to set Promise to Failed state with a CancellationError. Returns true if cancel succeeded.
  LIBLET_PUBLICAPI bool TryCancel() const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. It crashes app if error code cannot be set.
  LIBLET_PUBLICAPI void SetError(const ErrorCode &errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. It crashes app if error code cannot be set.
  LIBLET_PUBLICAPI void SetError(ErrorCode &&errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. Returns true on success.
  LIBLET_PUBLICAPI bool TrySetError(const ErrorCode &errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. Returns true on success.
  LIBLET_PUBLICAPI bool TrySetError(ErrorCode &&errorCode) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It crashes app if value cannot be set.
  LIBLET_PUBLICAPI void SetMaybe(const Mso::Maybe<void> &value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It crashes app if value cannot be set.
  LIBLET_PUBLICAPI void SetMaybe(Mso::Maybe<void> &&value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It returns true on success.
  LIBLET_PUBLICAPI bool TrySetMaybe(const Mso::Maybe<void> &value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It returns true on success.
  LIBLET_PUBLICAPI bool TrySetMaybe(Mso::Maybe<void> &&value) const noexcept;

  template <class U>
  friend Mso::Futures::IFuture *GetIFuture(const U &promise) noexcept;

 private:
  Mso::CntPtr<Mso::Futures::IFuture> m_state;
};

//! Promise<Maybe<T>> is the same as the Promise<T>
template <class T>
struct Promise<Mso::Maybe<T>> : public Promise<T> {
  // TODO: Add static_assert

  using Super = Promise<T>;

 public:
  /// Creates new Promise with empty state.
  Promise() noexcept;

  /// Creates new Promise with empty state.
  Promise(std::nullptr_t) noexcept;

  /// Creates new Promise from another with the same type
  Promise(const Mso::Promise<T> &other) noexcept;

  /// Creates new Promise from another with the same type
  Promise(Mso::Promise<T> &&other) noexcept;

  /// Creates new Promise with provided state.
  explicit Promise(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept;

  /// Creates new Promise with the same state as the other Promise.
  Promise(const Promise &other) noexcept;

  /// Creates new Promise with the state taken from the other Promise. The other Promise state becomes empty.
  Promise(Promise &&other) noexcept;

  /// Assigns the state from the other Promise.
  Promise &operator=(const Promise &other) noexcept;

  /// Assigns the state taken from the other Promise. The other Promise state becomes empty.
  Promise &operator=(Promise &&other) noexcept;
};

//! Promise<ErrorCode> is the same as the Promise<void>
template <>
struct Promise<Mso::ErrorCode> : public Promise<void> {
  // TODO: Add static_assert
};

/// True if two Promise have the same state instance.
template <class T>
bool operator==(const Promise<T> &left, const Promise<T> &right) noexcept;

/// True if two Promise have different state instance.
template <class T>
bool operator!=(const Promise<T> &left, const Promise<T> &right) noexcept;

/// True if left Promise is empty.
template <class T>
bool operator==(const Promise<T> &left, std::nullptr_t) noexcept;

/// True if left Promise is not empty.
template <class T>
bool operator!=(const Promise<T> &left, std::nullptr_t) noexcept;

/// True is right Promise is empty.
template <class T>
bool operator==(std::nullptr_t, const Promise<T> &right) noexcept;

/// True is right Promise is not empty.
template <class T>
bool operator!=(std::nullptr_t, const Promise<T> &right) noexcept;

//! A task which can be completed in a future.
//! New Future can be created by calling:
//! - Mso::MakeFuture(executor, lambda);
//! - promise.AsFuture(); // Exposing promise as Future
//! - future.Then(executor, lambda);
//! - sharedFuture.Then(executor, lambda);
template <class T>
struct Future {
  using ResultType = T;

  /// Creates new Future with empty state.
  Future() noexcept;

  /// Creates new Future with empty state.
  Future(std::nullptr_t) noexcept;

  /// Creates new Future with provided state.
  explicit Future(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept;

  /// Creates new Future with the same state as the other Future.
  Future(const Future &other) noexcept;

  /// Creates new Future with the state taken from the other Future. The other Future state becomes empty.
  Future(Future &&other) noexcept;

  /// Assigns the state from the other Future.
  Future &operator=(const Future &other) noexcept;

  /// Assigns the state taken from the other Future. The other Future state becomes empty.
  Future &operator=(Future &&other) noexcept;

  /// Swaps states with the other Future.
  void Swap(Future &other) noexcept;

  /// True if state is not empty.
  explicit operator bool() const noexcept;

  //! Schedules a continuation to run after this Future is completed.
  //! TCallback is a function or callable object. It needs to take either a T or a Mso::Maybe<T>, or a reference to one
  //! of those, as an argument. The result of Then() will be a Mso::Future<T2>, where T2 is the return type of
  //! TCallback. If the callback returns Mso::Maybe<T2> or Mso::Future<T2>, the result will still be Mso::Future<T2>,
  //! because Future collapses the extra layers into itself. Executes the callback in the concurrent queue, which runs
  //! work in the thread pool. Pass an explicit executor to Future<T>::Then(TExecutor, TCallback) to control which
  //! thread the callback will run in.
  template <class TExecutor = Mso::Executors::Concurrent, class TCallback>
  auto Then(TCallback &&callback) const noexcept;

  //! Schedules a continuation to run after this Future is completed.
  //! TExecutor is an object which controls how the callback is executed. It must be something that can be passed to
  //! GetExecutorType, such as an IDispatchQueue.
  //! See Future<T>::Then(TCallback) for a description of the TCallback parameter.
  template <class TExecutor, class TCallback>
  auto Then(TExecutor &&executor, TCallback &&callback) const noexcept;

  /// Adds a continuation that handles errors to the Future.
  template <class TExecutor = Mso::Executors::Inline, class TCallback>
  Mso::Future<T> Catch(TCallback &&callback) const noexcept;

  /// Adds a continuation that handles errors to the Future.
  template <class TExecutor, class TCallback>
  Mso::Future<T> Catch(TExecutor &&executor, TCallback &&callback) const noexcept;

  Mso::SharedFuture<T> Share() const noexcept;

  template <class U>
  friend Mso::Futures::IFuture *GetIFuture(const U &future) noexcept;
  template <class U>
  friend struct Mso::Future;

 private:
  Mso::CntPtr<Mso::Futures::IFuture> m_state;
};

//! Future<Maybe<T>> is the same as the Future<T>
template <class T>
struct Future<Mso::Maybe<T>> : public Future<T> {
  // TODO: Add static_assert to prohibit such specialization

  using Super = Future<T>;

  /// Creates new Future with empty state.
  Future() noexcept;

  /// Creates new Future with empty state.
  Future(std::nullptr_t) noexcept;

  /// Creates new Future from another with the same type
  Future(const Mso::Future<T> &other) noexcept;

  /// Creates new Future from another with the same type
  Future(Mso::Future<T> &&other) noexcept;

  /// Creates new Future with provided state.
  explicit Future(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept;

  /// Creates new Future with the same state as the other Future.
  Future(const Future &other) noexcept;

  /// Creates new Future with the state taken from the other Future. The other Future state becomes empty.
  Future(Future &&other) noexcept;

  /// Assigns the state from the other Future.
  Future &operator=(const Future &other) noexcept;

  /// Assigns the state taken from the other Future. The other Future state becomes empty.
  Future &operator=(Future &&other) noexcept;
};

//! Future<ErrorCode> is the same as the Future<void>
template <>
struct Future<Mso::ErrorCode> : public Future<void> {
  // TODO: Add static_assert
};

/// True if two Future have the same state instance.
template <class T>
bool operator==(const Future<T> &left, const Future<T> &right) noexcept;

/// True if two Future have different state instance.
template <class T>
bool operator!=(const Future<T> &left, const Future<T> &right) noexcept;

/// True if left Future is empty.
template <class T>
bool operator==(const Future<T> &left, std::nullptr_t) noexcept;

/// True if left Future is not empty.
template <class T>
bool operator!=(const Future<T> &left, std::nullptr_t) noexcept;

/// True is right Promise is empty.
template <class T>
bool operator==(std::nullptr_t, const Future<T> &right) noexcept;

/// True is right Promise is not empty.
template <class T>
bool operator!=(std::nullptr_t, const Future<T> &right) noexcept;

//! Enables multiple continuations for a Future by disabling its move semantic.
//! New SharedFuture can be created by calling:
//! - future.Share();
template <class T>
struct SharedFuture {
  /// Creates new SharedFuture with empty state.
  SharedFuture() noexcept;

  /// Creates new SharedFuture with empty state.
  SharedFuture(std::nullptr_t) noexcept;

  /// Creates new SharedFuture with provided state.
  explicit SharedFuture(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept;

  /// Creates new SharedFuture with the same state as the other SharedFuture.
  SharedFuture(const SharedFuture &other) noexcept;

  /// Creates new SharedFuture with the state taken from the other SharedFuture. The other SharedFuture state becomes
  /// empty.
  SharedFuture(SharedFuture &&other) noexcept;

  /// Assigns the state from the other SharedFuture.
  SharedFuture &operator=(const SharedFuture &other) noexcept;

  /// Assigns the state taken from the other SharedFuture. The other SharedFuture state becomes empty.
  SharedFuture &operator=(SharedFuture &&other) noexcept;

  /// Swaps states with the other SharedFuture.
  void Swap(SharedFuture &other) noexcept;

  /// True if state is not empty.
  explicit operator bool() const noexcept;

  /// Adds a continuation to the Future.
  template <class TExecutor = Mso::Executors::Concurrent, class TCallback>
  auto Then(TCallback &&callback) const noexcept;

  /// Adds a continuation to the Future.
  template <class TExecutor, class TCallback>
  auto Then(TExecutor &&executor, TCallback &&callback) const noexcept;

  /// Adds a continuation that handles errors to the Future.
  template <class TExecutor = Mso::Executors::Inline, class TCallback>
  Mso::Future<T> Catch(TCallback &&callback) const noexcept;

  /// Adds a continuation that handles errors to the Future.
  template <class TExecutor, class TCallback>
  Mso::Future<T> Catch(TExecutor &&executor, TCallback &&callback) const noexcept;

  template <class U>
  friend Mso::Futures::IFuture *GetIFuture(const U &future) noexcept;

 private:
  Mso::CntPtr<Mso::Futures::IFuture> m_state;
};

/// True if two SharedFuture have the same state instance.
template <class T>
bool operator==(const SharedFuture<T> &left, const SharedFuture<T> &right) noexcept;

/// True if two SharedFuture have different state instance.
template <class T>
bool operator!=(const SharedFuture<T> &left, const SharedFuture<T> &right) noexcept;

/// True if left SharedFuture is empty.
template <class T>
bool operator==(const SharedFuture<T> &left, std::nullptr_t) noexcept;

/// True if left SharedFuture is not empty.
template <class T>
bool operator!=(const SharedFuture<T> &left, std::nullptr_t) noexcept;

/// True is right Promise is empty.
template <class T>
bool operator==(std::nullptr_t, const SharedFuture<T> &right) noexcept;

/// True is right Promise is not empty.
template <class T>
bool operator!=(std::nullptr_t, const SharedFuture<T> &right) noexcept;

//! PromiseGroup is a set of Promise instances which are set to a copy of the value assigned to the PropertyGroup.
//! Use AddPromise() to add a new promise to the group and return it.
//! The PromiseGroup can be empty or non-empty depending on the m_state pointer being null or not null.
//! Calling methods that require non-empty PromiseGroup will cause a crash.
//! The value can be only set once. SetValue and EmplaceValue set the value and then set the copy of the value to all
//! promises in the group. If value cannot be set because it is already set or the PromiseGroup is abandoned then we
//! crash. In cases when the value can be set multiple times and you need only the first value then you can use the
//! TrySetValue or TryEmplaceValue methods to avoid crash. They also return false if Promise is abandoned. The Abandoned
//! state means that none of the continuation futures will be invoked. Though they will be notified about the state
//! change to Abandoned. Future may crash when its source Promise or Future is abandoned. To avoid the crash you should
//! use optional executors. If PromiseGroup is not completed or abandoned before destruction then its state is changed
//! to Abandoned in destructor. The TryAbandon method can be used to explicitly abandon the PromiseGroup if it is not
//! completed or abandoned yet.
template <class T>
struct PromiseGroup {
  static_assert(std::is_copy_constructible<T>::value, "PromiseGroup may only work with copy constructible types.");

 public:
  /// Creates new PromiseGroup with non-empty state.
  PromiseGroup() noexcept;

  /// Creates new PromiseGroup with empty state.
  PromiseGroup(std::nullptr_t) noexcept;

  /// Creates new PromiseGroup with provided state.
  explicit PromiseGroup(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept;

  /// Creates new PromiseGroup with the same state as the other PromiseGroup.
  PromiseGroup(const PromiseGroup &other) noexcept;

  /// Creates new PromiseGroup with the state taken from the other PromiseGroup. The other PromiseGroup state becomes
  /// empty.
  PromiseGroup(PromiseGroup &&other) noexcept;

  /// Assigns the state from the other promise group.
  PromiseGroup &operator=(const PromiseGroup &other) noexcept;

  /// Assigns the state taken from the other promise group. The other PromiseGroup state becomes empty.
  PromiseGroup &operator=(PromiseGroup &&other) noexcept;

  /// Swaps states with the other PromiseGroup.
  void Swap(PromiseGroup &other) noexcept;

  /// True if state is not empty.
  explicit operator bool() const noexcept;

  /// Adds a new Future to observe the group and returns it. The future is going to return a copy of the value assigned
  /// to the PromiseGroup.
  Mso::Future<T> AddFuture() const noexcept;

  /// Sets the value and completes the PromiseGroup. It can be called only once. Otherwise it VECs.
  void SetValue(const T &value) const noexcept;

  /// Sets the value and completes the PromiseGroup. It can be called only once. Otherwise it VECs.
  void SetValue(T &&value) const noexcept;

  /// Tries to set the value if PromiseGroup is not completed or abandoned yet. Returns true on success.
  bool TrySetValue(const T &value) const noexcept;

  /// Tries to set the value if PromiseGroup is not completed or abandoned yet. Returns true on success.
  bool TrySetValue(T &&value) const noexcept;

  /// Sets the value in-place and completes the PromiseGroup. It can be called only once. Otherwise it VECs.
  template <class... TArgs>
  void EmplaceValue(TArgs &&... args) const noexcept;

  /// Sets the value in-place and completes the PromiseGroup. It can be called only once. Otherwise it VECs.
  template <class TArg, class... TArgs>
  void EmplaceValue(std::initializer_list<TArg> init, TArgs &&... args) const noexcept;

  /// Tries to set the value in-place if PromiseGroup is not completed or abandoned yet. Returns true on success.
  template <class... TArgs>
  bool TryEmplaceValue(TArgs &&... args) const noexcept;

  /// Tries to set the value in-place if PromiseGroup is not completed or abandoned yet. Returns true on success.
  template <class TArg, class... TArgs>
  bool TryEmplaceValue(std::initializer_list<TArg> init, TArgs &&... args) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. It crashes app if error code cannot be set.
  void SetError(const ErrorCode &errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. It crashes app if error code cannot be set.
  void SetError(ErrorCode &&errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. Returns true on success.
  bool TrySetError(const ErrorCode &errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. Returns true on success.
  bool TrySetError(ErrorCode &&errorCode) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It crashes app if value cannot be set.
  void SetMaybe(const Mso::Maybe<T> &value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It crashes app if value cannot be set.
  void SetMaybe(Mso::Maybe<T> &&value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It returns true on success.
  bool TrySetMaybe(const Mso::Maybe<T> &value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It returns true on success.
  bool TrySetMaybe(Mso::Maybe<T> &&value) const noexcept;

  template <class U>
  friend Mso::Futures::IFuture *GetIFuture(const U &promise) noexcept;

 private:
  Mso::CntPtr<Mso::Futures::IFuture> m_state;
};

//! PromiseGroup<void> is a set of Promise<void> instances which are completed or abandoned then the group state is
//! changed. The PromiseGroup can be empty or non-empty depending on the m_state pointer being null or not null. Calling
//! methods requiring non-empty PromiseGroup will cause crash. The PromiseGroup can be completed only once. SetValue()
//! method changes state to Completed. If PromiseGroup cannot be completed because it is already completed or the
//! PromiseGroup is abandoned then we crash. In cases when the SetValue() can be called multiple times and you want
//! ignore extra calls then you can use the TrySetValue method to avoid crash. It also returns false if state is
//! Abandoned. The Abandoned state means that none of the continuation futures will be invoked. Though they will be
//! notified about the state change to Abandoned. Future will crash when its source Promise or Future is abandoned. To
//! avoid the crash use special optional executors. If PromiseGroup is not completed or abandoned before destruction
//! then its state is changed to Abandoned. The TryAbandon method can be used to explicitly abandon the PromiseGroup if
//! it is not completed or abandoned yet.
template <>
struct PromiseGroup<void> {
  /// Creates new PromiseGroup with non-empty state.
  LIBLET_PUBLICAPI PromiseGroup() noexcept;

  /// Creates new PromiseGroup with empty state.
  LIBLET_PUBLICAPI PromiseGroup(std::nullptr_t) noexcept;

  /// Creates new PromiseGroup with provided state.
  LIBLET_PUBLICAPI explicit PromiseGroup(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept;

  /// Creates new PromiseGroup with the same state as the other PromiseGroup.
  LIBLET_PUBLICAPI PromiseGroup(const PromiseGroup &other) noexcept;

  /// Creates new PromiseGroup with the state taken from the other PromiseGroup. The other PromiseGroup state becomes
  /// empty.
  LIBLET_PUBLICAPI PromiseGroup(PromiseGroup &&other) noexcept;

  /// Assigns the state from the other promise group.
  LIBLET_PUBLICAPI PromiseGroup &operator=(const PromiseGroup &other) noexcept;

  /// Assigns the state taken from the other promise group. The other PromiseGroup state becomes empty.
  LIBLET_PUBLICAPI PromiseGroup &operator=(PromiseGroup &&other) noexcept;

  /// Swaps states with the other PromiseGroup.
  LIBLET_PUBLICAPI void Swap(PromiseGroup &other) noexcept;

  /// True if state is not empty.
  LIBLET_PUBLICAPI explicit operator bool() const noexcept;

  /// Adds a new Future to the group and returns it. The new future is going to have a copy of the value assigned to the
  /// PromiseGroup.
  LIBLET_PUBLICAPI Mso::Future<void> AddFuture() const noexcept;

  /// Sets the value and completes the PromiseGroup. It can be called only once. Otherwise it VECs.
  LIBLET_PUBLICAPI void SetValue() const noexcept;

  /// Tries to set the value if PromiseGroup is not completed or abandoned yet. Returns true on success.
  LIBLET_PUBLICAPI bool TrySetValue() const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. It crashes app if error code cannot be set.
  LIBLET_PUBLICAPI void SetError(const ErrorCode &errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. It crashes app if error code cannot be set.
  LIBLET_PUBLICAPI void SetError(ErrorCode &&errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. Returns true on success.
  LIBLET_PUBLICAPI bool TrySetError(const ErrorCode &errorCode) const noexcept;

  /// Tries to set Promise to Failed state with the provided error code. Returns true on success.
  LIBLET_PUBLICAPI bool TrySetError(ErrorCode &&errorCode) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It crashes app if value cannot be set.
  LIBLET_PUBLICAPI void SetMaybe(const Mso::Maybe<void> &value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It crashes app if value cannot be set.
  LIBLET_PUBLICAPI void SetMaybe(Mso::Maybe<void> &&value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It returns true on success.
  LIBLET_PUBLICAPI bool TrySetMaybe(const Mso::Maybe<void> &value) const noexcept;

  /// Sets Promise to Succeeded or Failed state depending on Maybe value. It returns true on success.
  LIBLET_PUBLICAPI bool TrySetMaybe(Mso::Maybe<void> &&value) const noexcept;

  template <class U>
  friend Mso::Futures::IFuture *GetIFuture(const U &promise) noexcept;

 private:
  Mso::CntPtr<Mso::Futures::IFuture> m_state;
};

/// True if two PromiseGroup have the same state instance.
template <class T>
bool operator==(const PromiseGroup<T> &left, const PromiseGroup<T> &right) noexcept;

/// True if two PromiseGroup have different state instance.
template <class T>
bool operator!=(const PromiseGroup<T> &left, const PromiseGroup<T> &right) noexcept;

/// True if left PromiseGroup is empty.
template <class T>
bool operator==(const PromiseGroup<T> &left, std::nullptr_t) noexcept;

/// True if left PromiseGroup is not empty.
template <class T>
bool operator!=(const PromiseGroup<T> &left, std::nullptr_t) noexcept;

/// True is right Promise is empty.
template <class T>
bool operator==(std::nullptr_t, const PromiseGroup<T> &right) noexcept;

/// True is right Promise is not empty.
template <class T>
bool operator!=(std::nullptr_t, const PromiseGroup<T> &right) noexcept;

// base class of WeakPtr<Future<T>>
struct FutureWeakPtrBase {
  template <class T>
  friend class Mso::WeakPtr;

  LIBLET_PUBLICAPI void Reset() noexcept;

  LIBLET_PUBLICAPI bool IsEmpty() const noexcept;

  LIBLET_PUBLICAPI bool IsExpired() const noexcept;

 protected:
  LIBLET_PUBLICAPI FutureWeakPtrBase() noexcept;

  LIBLET_PUBLICAPI FutureWeakPtrBase(_In_opt_ void *ptr, bool shouldAddWeakRef = true) noexcept;

  LIBLET_PUBLICAPI ~FutureWeakPtrBase() noexcept;

  LIBLET_PUBLICAPI void Assign(_In_opt_ void *ptr) noexcept;

  LIBLET_PUBLICAPI void Attach(FutureWeakPtrBase &&from) noexcept;

 private:
  LIBLET_PUBLICAPI static bool IncrementRefCountIfNotZero(_In_opt_ const void *ptr) noexcept;

  LIBLET_PUBLICAPI static void CheckedAddWeakRef(_In_opt_ const void *ptr) noexcept;

  LIBLET_PUBLICAPI static void CheckedReleaseWeakRef(_In_opt_ const void *ptr) noexcept;

 private:
  void *m_ptr{nullptr};
};

// WeakPtr specialization for Future<T>
template <class T>
class WeakPtr<Future<T>> final : public FutureWeakPtrBase {
  using Super = FutureWeakPtrBase;

 public:
  WeakPtr() noexcept;

  WeakPtr(std::nullptr_t) noexcept;

  WeakPtr(const Future<T> &future) noexcept;

  WeakPtr(const WeakPtr &from) noexcept;

  WeakPtr(WeakPtr &&from) noexcept;

  WeakPtr &operator=(std::nullptr_t) noexcept;

  WeakPtr &operator=(const Mso::Future<T> &future) noexcept;

  WeakPtr &operator=(const WeakPtr &from) noexcept;

  WeakPtr &operator=(WeakPtr &&from) noexcept;

  Mso::Future<T> GetFuture() const noexcept;

  void Swap(WeakPtr &other) noexcept;
};

} // namespace Mso

// std::swap specializations. They must be done in the std namespace because we override it for template classes.
namespace std {

/// Swaps states between Promise instances.
template <class T>
inline void swap(Mso::Promise<T> &promise1, Mso::Promise<T> &promise2) noexcept {
  promise1.Swap(promise2);
}

/// Swaps states between Future instances.
template <class T>
inline void swap(Mso::Future<T> &future1, Mso::Future<T> &future2) noexcept {
  future1.Swap(future2);
}

/// Swaps states between SharedFuture instances.
template <class T>
inline void swap(Mso::SharedFuture<T> &future1, Mso::SharedFuture<T> &future2) noexcept {
  future1.Swap(future2);
}

/// Swaps states between PromiseGroup instances.
template <class T>
inline void swap(Mso::PromiseGroup<T> &promiseGroup1, Mso::PromiseGroup<T> &promiseGroup2) noexcept {
  promiseGroup1.Swap(promiseGroup2);
}

/// Swaps states between Mso::WeakPtr<Mso::Future<T>> instances.
template <class T>
inline void swap(Mso::WeakPtr<Mso::Future<T>> &weakPtr1, Mso::WeakPtr<Mso::Future<T>> &weakPtr2) noexcept {
  weakPtr1.Swap(weakPtr2);
}

} // namespace std

#define MSO_FUTURE_INLINE_DEFS
#include "details/futureFuncInl.h"
#include "details/futureInl.h"
#include "details/futureWeakPtrInl.h"
#include "details/promiseGroupInl.h"
#include "details/promiseInl.h"
#include "details/sharedFutureInl.h"
#include "details/whenAllInl.h"
#include "details/whenAnyInl.h"
#undef MSO_FUTURE_INLINE_DEFS

MSO_PRAGMA_MANAGED_POP

#endif // MSO_FUTURE_FUTURE_H
