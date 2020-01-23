// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_CANCELLATIONTOKEN_H
#define MSO_FUTURE_CANCELLATIONTOKEN_H

/** \file cancellationToken.h

CancellationTokenSource and CancellationToken provide support for cancellation patterns used for futures.

CancellationTokenSource is an owner of the cancellation token state.
It has methods Cancel() and Abandon() which set the cancellation state to true and false.
When all instances of CancellationTokenSource pointing to the same state are destroyed, then its ref count goes to zero
and the state's destructor calls Abandon().
The GetToken() method returns the CancellationToken associated with the CancellationTokenSource instance.

CancellationToken is a class to observe the cancellation token state.
It has IsCanceled, WhenChanged, and WhenCancelled methods to either check the state or to subscribe to its changes.

Typical usage scenario is when the code that may originate the cancellation creates an instance of
CancellationTokenSource.

  CancellationTokenSource tokenSource;

Then to all observers we pass the CancellationToken instance to observe cancellation:

  future.Then(Mso::Async::ConcurrentQueue(), [token = tokenSource.GetToken()](std::vector<int>& data) noexcept
  {
    if (!token.IsCancelled())
    {
      ...
    }
  });

Alternatively, developers can subscribe to the cancellation token state changes:

CancellationToken token = tokenSource.GetToken();
token.WhenCancelled([]() { ... });                       // called only when Cancel() is called
token.WhenChanged().Then([](bool isCancelled) { ... });  // called only when Cancel() or Abandon() are called

The original code can either call Cancel() method to set cancellation state to true:

  tokenSource.Cancel();

Or do nothing and then the state will be set to false when CancellationTokenSource state ref count goes to zero.
To enforce setting cancellation state to false without waiting for the ref counting becoming zero, developers can call
Abandon() method:

  tokenSource.Abandon();

When Cancel() or Abandon() methods are called, any code that subscribed to observe changes in state by using WhenChanged
or WhenCancelled methods is released after execution. So, calling Abandon() may help to release resources earlier if
needed.

Note that only ref count affected by CancellationTokenSource instances defines when the state is released and Abandon()
method is called. The CancellationToken instances can be considered as weak pointers and do not affect when the
Abandon() method is called.

Internally the cancellation token state has two IFuture instances.
One is an IFuture instance pointed by CancellationTokenSource instances.
Another is an observer IFuture instance pointed by CancellationToken instances.
The first IFuture instance owns the second one as a continuation. Since only the first IFuture instance guarantees the
lifetime of the pair, the CancellationTokenSource can be considered as a "strong" pointer, and CancellationToken as a
"weak" pointer.

*/

#include "details/ifuture.h"
#include "futureForwardDecl.h"

namespace Mso {

class CancellationTokenSource;
class CancellationToken;

//! A strong pointer to the cancellation state which is a Boolean variable.
//! The variable is set to true by calling Cancel() method to indicate that the cancellation happened.
//! Or the variable can be set to false by calling Abandon() method to indicate that the cancellation will never happen.
//! The Abandon() method is called automatically when state's ref count becomes zero.
class CancellationTokenSource {
 public:
  //! Creates new CancellationTokenSource with a new non-empty state.
  LIBLET_PUBLICAPI CancellationTokenSource() noexcept;

  //! Creates new CancellationTokenSource with the same state as the other CancellationTokenSource.
  LIBLET_PUBLICAPI CancellationTokenSource(const CancellationTokenSource &other) noexcept;

  //! Creates new CancellationTokenSource with the state taken from the other CancellationTokenSource. The other
  //! CancellationTokenSource state becomes empty.
  LIBLET_PUBLICAPI CancellationTokenSource(CancellationTokenSource &&other) noexcept;

  //! Assigns the state from the other CancellationTokenSource.
  LIBLET_PUBLICAPI CancellationTokenSource &operator=(const CancellationTokenSource &other) noexcept;

  //! Assigns the state taken from the other CancellationTokenSource. The other CancellationTokenSource state becomes
  //! empty.
  LIBLET_PUBLICAPI CancellationTokenSource &operator=(CancellationTokenSource &&other) noexcept;

  //! Swaps states with the other CancellationTokenSource.
  LIBLET_PUBLICAPI void Swap(CancellationTokenSource &other) noexcept;

  //! Makes state empty to reduce ref count for the state.
  LIBLET_PUBLICAPI void Clear() noexcept;

  //! True if state is not empty.
  LIBLET_PUBLICAPI explicit operator bool() const noexcept;

  //! Gets the CancellationToken associated with the CancellationTokenSource instance.
  LIBLET_PUBLICAPI const CancellationToken &GetToken() const noexcept;

  //! Sets the cancellation state to true.
  LIBLET_PUBLICAPI void Cancel() const noexcept;

  //! Tries to set cancellation state to false. It means that the state will be never set to true, and cancellation will
  //! never happen.
  LIBLET_PUBLICAPI void Abandon() const noexcept;

  template <class U>
  friend Mso::Futures::IFuture *GetIFuture(const U &tokenSource) noexcept;

 private:
  //! Points to the first instance in the pair of IFuture instances used for the cancellation state.
  Mso::CntPtr<Mso::Futures::IFuture> m_state;
};

//! True if two CancellationTokenSource have the same state instance.
LIBLET_PUBLICAPI bool operator==(const CancellationTokenSource &left, const CancellationTokenSource &right) noexcept;

//! True if two CancellationTokenSource have different state instance.
LIBLET_PUBLICAPI bool operator!=(const CancellationTokenSource &left, const CancellationTokenSource &right) noexcept;

//! True if left CancellationTokenSource is empty.
LIBLET_PUBLICAPI bool operator==(const CancellationTokenSource &left, std::nullptr_t) noexcept;

//! True if left CancellationTokenSource is not empty.
LIBLET_PUBLICAPI bool operator!=(const CancellationTokenSource &left, std::nullptr_t) noexcept;

//! True if right CancellationTokenSource is empty.
LIBLET_PUBLICAPI bool operator==(std::nullptr_t, const CancellationTokenSource &right) noexcept;

//! True if right CancellationTokenSource is not empty.
LIBLET_PUBLICAPI bool operator!=(std::nullptr_t, const CancellationTokenSource &right) noexcept;

//! CancellationToken is a weak pointer to the cancellation state held by CancellationTokenSource.
//! It allows to observe the state with help of IsCanceled(), WhenCanceled(), and WhenChanged() methods.
class CancellationToken {
  friend CancellationTokenSource;

 public:
  //! Creates new CancellationToken with an empty state.
  LIBLET_PUBLICAPI CancellationToken() noexcept;

  //! Creates new CancellationToken with an empty state.
  LIBLET_PUBLICAPI CancellationToken(std::nullptr_t) noexcept;

  //! Creates new CancellationToken with the same state as the other CancellationToken.
  LIBLET_PUBLICAPI CancellationToken(const CancellationToken &other) noexcept;

  //! Creates new CancellationToken with the state taken from the other CancellationToken. The other CancellationToken
  //! state becomes empty.
  LIBLET_PUBLICAPI CancellationToken(CancellationToken &&other) noexcept;

  //! Assigns the state from the other CancellationToken.
  LIBLET_PUBLICAPI CancellationToken &operator=(const CancellationToken &other) noexcept;

  //! Assigns the state taken from the other CancellationToken. The other CancellationToken state becomes empty.
  LIBLET_PUBLICAPI CancellationToken &operator=(CancellationToken &&other) noexcept;

  //! Swaps states with the other CancellationToken.
  LIBLET_PUBLICAPI void Swap(CancellationToken &other) noexcept;

  //! Makes state empty to reduce weak ref count for the state. Part of the state is kept in memory until weak ref count
  //! is not zero.
  LIBLET_PUBLICAPI void Clear() noexcept;

  //! True if state is not empty.
  LIBLET_PUBLICAPI operator bool() const noexcept;

  //! Returns true if the cancellation token is canceled. It returns false when we are still waiting for the state to be
  //! changed, or when it is abandoned.
  LIBLET_PUBLICAPI bool IsCanceled() const noexcept;

  //! Returns a Future<bool> which will be executed when token is canceled or abandoned.
  //! The continuation future receives true if the token was canceled, or false if it was abandoned and will never be
  //! canceled.
  LIBLET_PUBLICAPI Future<bool> WhenChanged() const noexcept;

  //! Registers an action to be executed when token is canceled.
  //! It is never executed if cancellation token is abandoned.
  LIBLET_PUBLICAPI void WhenCanceled(Mso::VoidFunctor &&action) const noexcept;

  template <class U>
  friend Mso::Futures::IFuture *GetIFuture(const U &token) noexcept;

 private:
  //! Points to the second instance in the pair of IFuture instances used for the cancellation state.
  Mso::CntPtr<Mso::Futures::IFuture> m_state;
};

//! True if two CancellationToken have the same state instance.
LIBLET_PUBLICAPI bool operator==(const CancellationToken &left, const CancellationToken &right) noexcept;

//! True if two CancellationToken have different state instance.
LIBLET_PUBLICAPI bool operator!=(const CancellationToken &left, const CancellationToken &right) noexcept;

//! True if left CancellationToken is empty.
LIBLET_PUBLICAPI bool operator==(const CancellationToken &left, std::nullptr_t) noexcept;

//! True if left CancellationToken is not empty.
LIBLET_PUBLICAPI bool operator!=(const CancellationToken &left, std::nullptr_t) noexcept;

//! True if right CancellationToken is empty.
LIBLET_PUBLICAPI bool operator==(std::nullptr_t, const CancellationToken &right) noexcept;

//! True if right CancellationToken is not empty.
LIBLET_PUBLICAPI bool operator!=(std::nullptr_t, const CancellationToken &right) noexcept;

} // namespace Mso

// std::swap specializations. They must be done in the std namespace because we override it for template classes.
namespace std {

//! Swaps states between CancellationTokenSource instances.
inline void swap(Mso::CancellationTokenSource &tokenSource1, Mso::CancellationTokenSource &tokenSource2) noexcept {
  tokenSource1.Swap(tokenSource2);
}

//! Swaps states between CancellationToken instances.
inline void swap(Mso::CancellationToken &token1, Mso::CancellationToken &token2) noexcept {
  token1.Swap(token2);
}

} // namespace std

#endif // MSO_FUTURE_CANCELLATIONTOKEN_H
