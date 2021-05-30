// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/cancellationToken.h"
#include "future/details/cancellationErrorProvider.h"
#include "future/details/cancellationException.h"
#include "future/details/timeoutException.h"
#include "future/future.h"
#include "futureImpl.h"

namespace Mso {

struct CancellationTokenSourceTask {
  CancellationToken CancellationToken;
};

struct CancellationTokenTask {
  static void Invoke(
      const Mso::Futures::ByteArrayView & /*taskBuffer*/,
      Mso::Futures::IFuture *future,
      Mso::Futures::IFuture * /*parentFuture*/) noexcept {
    future->SetValue<bool>(true);
  }

  static void Catch(
      const Mso::Futures::ByteArrayView & /*taskBuffer*/,
      Mso::Futures::IFuture *future,
      Mso::ErrorCode && /*parentError*/) noexcept {
    future->SetValue<bool>(false);
  }

  constexpr static Mso::Futures::FutureCatchCallback *CatchPtr = &Catch;
};

//=============================================================================
// CancellationTokenSource implementation
//=============================================================================

LIBLET_PUBLICAPI CancellationTokenSource::CancellationTokenSource() noexcept {
  constexpr const auto &tokenSourceTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::CancelIfUnfulfilled,
      /*ResultType: */ void,
      /*TaskType:   */ CancellationTokenSourceTask,
      /*PostType:   */ void,
      /*InvokeType: */ void,
      /*CatchType:  */ void>::Traits;

  constexpr const auto &tokenTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::IsShared,
      /*ResultType: */ bool,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ CancellationTokenTask,
      /*CatchType:  */ CancellationTokenTask>::Traits;

  Mso::Futures::ByteArrayView taskBuffer;
  m_state = Mso::Futures::MakeFuture(tokenSourceTraits, sizeof(CancellationTokenSourceTask), &taskBuffer);
  Mso::CntPtr<Mso::Futures::IFuture> tokenState = Mso::Futures::MakeFuture(tokenTraits, 0, nullptr);
  CancellationToken token;
  token.m_state = tokenState;
  ::new (taskBuffer.VoidData()) CancellationTokenSourceTask{std::move(token)};
  m_state->AddContinuation(std::move(tokenState));
}

LIBLET_PUBLICAPI CancellationTokenSource::CancellationTokenSource(const CancellationTokenSource &other) noexcept
    : m_state(other.m_state) {}

LIBLET_PUBLICAPI CancellationTokenSource::CancellationTokenSource(CancellationTokenSource &&other) noexcept
    : m_state(std::move(other.m_state)) {}

LIBLET_PUBLICAPI CancellationTokenSource &CancellationTokenSource::operator=(
    const CancellationTokenSource &other) noexcept {
  m_state = other.m_state;
  return *this;
}

LIBLET_PUBLICAPI CancellationTokenSource &CancellationTokenSource::operator=(CancellationTokenSource &&other) noexcept {
  m_state = std::move(other.m_state);
  return *this;
}

LIBLET_PUBLICAPI void CancellationTokenSource::Swap(CancellationTokenSource &other) noexcept {
  using std::swap;
  swap(m_state, other.m_state);
}

LIBLET_PUBLICAPI void CancellationTokenSource::Clear() noexcept {
  m_state.Clear();
}

LIBLET_PUBLICAPI CancellationTokenSource::operator bool() const noexcept {
  return !m_state.IsEmpty();
}

LIBLET_PUBLICAPI const CancellationToken &CancellationTokenSource::GetToken() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0130f546 /* tag_bmpvg */);
  return reinterpret_cast<CancellationTokenSourceTask *>(Mso::GetIFuture(*this)->GetTask().VoidData())
      ->CancellationToken;
}

LIBLET_PUBLICAPI void CancellationTokenSource::Cancel() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0130f547 /* tag_bmpvh */);
  m_state->TrySetSuccess();
}

LIBLET_PUBLICAPI void CancellationTokenSource::Abandon() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0130f548 /* tag_bmpvi */);
  m_state->TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true));
}

/// True if two CancellationTokenSource have the same state instance.
LIBLET_PUBLICAPI bool operator==(const CancellationTokenSource &left, const CancellationTokenSource &right) noexcept {
  return GetIFuture(left) == GetIFuture(right);
}

/// True if two CancellationTokenSource have different state instance.
LIBLET_PUBLICAPI bool operator!=(const CancellationTokenSource &left, const CancellationTokenSource &right) noexcept {
  return GetIFuture(left) != GetIFuture(right);
}

/// True if left CancellationTokenSource is empty.
LIBLET_PUBLICAPI bool operator==(const CancellationTokenSource &left, std::nullptr_t) noexcept {
  return GetIFuture(left) == nullptr;
}

/// True if left CancellationTokenSource is not empty.
LIBLET_PUBLICAPI bool operator!=(const CancellationTokenSource &left, std::nullptr_t) noexcept {
  return GetIFuture(left) != nullptr;
}

/// True is right CancellationTokenSource is empty.
LIBLET_PUBLICAPI bool operator==(std::nullptr_t, const CancellationTokenSource &right) noexcept {
  return GetIFuture(right) == nullptr;
}

/// True is right CancellationTokenSource is not empty.
LIBLET_PUBLICAPI bool operator!=(std::nullptr_t, const CancellationTokenSource &right) noexcept {
  return GetIFuture(right) != nullptr;
}

//=============================================================================
// CancellationToken implementation
//=============================================================================

LIBLET_PUBLICAPI CancellationToken::CancellationToken() noexcept {}

LIBLET_PUBLICAPI CancellationToken::CancellationToken(std::nullptr_t) noexcept {}

LIBLET_PUBLICAPI CancellationToken::CancellationToken(const CancellationToken &other) noexcept
    : m_state(other.m_state) {}

LIBLET_PUBLICAPI CancellationToken::CancellationToken(CancellationToken &&other) noexcept
    : m_state(std::move(other.m_state)) {}

LIBLET_PUBLICAPI CancellationToken &CancellationToken::operator=(const CancellationToken &other) noexcept {
  m_state = other.m_state;
  return *this;
}

LIBLET_PUBLICAPI CancellationToken &CancellationToken::operator=(CancellationToken &&other) noexcept {
  m_state = std::move(other.m_state);
  return *this;
}

LIBLET_PUBLICAPI void CancellationToken::Swap(CancellationToken &other) noexcept {
  using std::swap;
  swap(m_state, other.m_state);
}

LIBLET_PUBLICAPI void CancellationToken::Clear() noexcept {
  m_state.Clear();
}

LIBLET_PUBLICAPI CancellationToken::operator bool() const noexcept {
  return !m_state.IsEmpty();
}

/// Returns true if the cancellation token is canceled.
LIBLET_PUBLICAPI bool CancellationToken::IsCanceled() const noexcept {
  if (m_state->IsDone()) {
    return *static_cast<bool *>(m_state->GetValue().VoidDataChecked(sizeof(bool)));
  }

  return false;
}

/// Returns a Future<bool> which will be executed when token is canceled or destroyed.
/// The continuation future receives true if the token was canceled, or false if it was not canceled and destroyed.
LIBLET_PUBLICAPI Mso::Future<bool> CancellationToken::WhenChanged() const noexcept {
  return Mso::Future<bool>{Mso::CntPtr{m_state}};
}

/// Registers an action to be executed when token is canceled.
/// It is never executed if cancellation token is destroyed before cancellation.
LIBLET_PUBLICAPI void CancellationToken::WhenCanceled(Mso::VoidFunctor &&action) const noexcept {
  WhenChanged().Then(Mso::Executors::Inline{}, [act = std::move(action)](bool isCanceled) noexcept {
    if (isCanceled) {
      act();
    }
  });
}

/// True if two CancellationToken have the same state instance.
LIBLET_PUBLICAPI bool operator==(const CancellationToken &left, const CancellationToken &right) noexcept {
  return GetIFuture(left) == GetIFuture(right);
}

/// True if two CancellationToken have different state instance.
LIBLET_PUBLICAPI bool operator!=(const CancellationToken &left, const CancellationToken &right) noexcept {
  return GetIFuture(left) != GetIFuture(right);
}

/// True if left CancellationToken is empty.
LIBLET_PUBLICAPI bool operator==(const CancellationToken &left, std::nullptr_t) noexcept {
  return GetIFuture(left) == nullptr;
}

/// True if left CancellationToken is not empty.
LIBLET_PUBLICAPI bool operator!=(const CancellationToken &left, std::nullptr_t) noexcept {
  return GetIFuture(left) != nullptr;
}

/// True is right CancellationToken is empty.
LIBLET_PUBLICAPI bool operator==(std::nullptr_t, const CancellationToken &right) noexcept {
  return GetIFuture(right) == nullptr;
}

/// True is right CancellationToken is not empty.
LIBLET_PUBLICAPI bool operator!=(std::nullptr_t, const CancellationToken &right) noexcept {
  return GetIFuture(right) != nullptr;
}

//=============================================================================
// CancellationErrorProvider implementation
//=============================================================================

template <>
LIBLET_PUBLICAPI void ErrorProvider<bool, CancellationErrorProviderGuid>::Throw(
    const ErrorCode &errorCode,
    bool shouldHandle) const {
  if (TryGetErrorInfo(errorCode, shouldHandle)) {
    throw Mso::Async::CancellationException();
  }
}

ErrorProvider<bool, CancellationErrorProviderGuid> s_cancellationErrorProvider{};

LIBLET_PUBLICAPI const ErrorProvider<bool, CancellationErrorProviderGuid> &CancellationErrorProvider() noexcept {
  return s_cancellationErrorProvider;
}

//=============================================================================
// TimeoutErrorProvider implementation
//=============================================================================

namespace Async {

Mso::ErrorProvider<int, TimeoutErrorGuid> s_timeoutError{};

LIBLET_PUBLICAPI const Mso::ErrorProvider<int, TimeoutErrorGuid> &TimeoutError() noexcept {
  return s_timeoutError;
}

} // namespace Async

template <>
LIBLET_PUBLICAPI void ErrorProvider<int, Async::TimeoutErrorGuid>::Throw(const ErrorCode &errorCode, bool shouldHandle)
    const {
  if (TryGetErrorInfo(errorCode, shouldHandle)) {
    throw Mso::Async::TimeoutException("Timeout error.");
  }
}

} // namespace Mso
