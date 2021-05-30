// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"

namespace Mso {

//=============================================================================
// PromiseGroup<void> implementation
//=============================================================================

LIBLET_PUBLICAPI PromiseGroup<void>::PromiseGroup() noexcept {
  constexpr const auto &promiseGroupTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:     */ Mso::Futures::FutureOptions::IsShared | Mso::Futures::FutureOptions::CancelIfUnfulfilled,
      /*ResultType:  */ void,
      /*TaskType:    */ void,
      /*PostType:    */ void,
      /*InvokeType:  */ void,
      /*AbandonType: */ void>::Traits;

  m_state = Mso::Futures::MakeFuture(promiseGroupTraits, 0, nullptr);
}

LIBLET_PUBLICAPI PromiseGroup<void>::PromiseGroup(std::nullptr_t) noexcept {}

LIBLET_PUBLICAPI PromiseGroup<void>::PromiseGroup(const PromiseGroup &other) noexcept : m_state(other.m_state) {}

LIBLET_PUBLICAPI PromiseGroup<void>::PromiseGroup(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept
    : m_state(std::move(state)) {}

LIBLET_PUBLICAPI PromiseGroup<void>::PromiseGroup(PromiseGroup &&other) noexcept : m_state(std::move(other.m_state)) {}

LIBLET_PUBLICAPI PromiseGroup<void> &PromiseGroup<void>::operator=(const PromiseGroup<void> &other) noexcept {
  m_state = other.m_state;
  return *this;
}

LIBLET_PUBLICAPI PromiseGroup<void> &PromiseGroup<void>::operator=(PromiseGroup<void> &&other) noexcept {
  m_state = std::move(other.m_state);
  return *this;
}

LIBLET_PUBLICAPI void PromiseGroup<void>::Swap(PromiseGroup &other) noexcept {
  using std::swap;
  swap(m_state, other.m_state);
}

LIBLET_PUBLICAPI PromiseGroup<void>::operator bool() const noexcept {
  return !m_state.IsEmpty();
}

LIBLET_PUBLICAPI Future<void> PromiseGroup<void>::AddFuture() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605617 /* tag_byfyx */);
  const auto &promiseTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:    */ Mso::Futures::FutureOptions::None,
      /*ResultType: */ void,
      /*TaskType:   */ void,
      /*PostType:   */ void,
      /*InvokeType: */ Mso::Futures::CopyTaskInvoke<void>,
      /*CatchType:  */ Mso::Futures::CopyTaskCatch>::Traits;

  Mso::CntPtr<Mso::Futures::IFuture> promise = Mso::Futures::MakeFuture(promiseTraits, 0, nullptr);
  m_state->AddContinuation(Mso::CntPtr{promise});
  return Mso::Future<void>(std::move(promise));
}

LIBLET_PUBLICAPI void PromiseGroup<void>::SetValue() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605618 /* tag_byfyy */);
  m_state->TrySetSuccess(/*crashIfFailed:*/ true);
}

LIBLET_PUBLICAPI bool PromiseGroup<void>::TrySetValue() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605619 /* tag_byfyz */);
  return m_state->TrySetSuccess(/*crashIfFailed:*/ false);
}

LIBLET_PUBLICAPI void PromiseGroup<void>::SetError(const ErrorCode &errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160561a /* tag_byfy0 */);
  m_state->TrySetError(Mso::ErrorCode(errorCode), /*crashIfFailed:*/ true);
}

LIBLET_PUBLICAPI void PromiseGroup<void>::SetError(ErrorCode &&errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160561b /* tag_byfy1 */);
  m_state->TrySetError(std::move(errorCode), /*crashIfFailed:*/ true);
}

LIBLET_PUBLICAPI bool PromiseGroup<void>::TrySetError(const ErrorCode &errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160561c /* tag_byfy2 */);
  return m_state->TrySetError(Mso::ErrorCode(errorCode), /*crashIfFailed:*/ false);
}

LIBLET_PUBLICAPI bool PromiseGroup<void>::TrySetError(ErrorCode &&errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160561d /* tag_byfy3 */);
  return m_state->TrySetError(std::move(errorCode), /*crashIfFailed:*/ false);
}

LIBLET_PUBLICAPI void PromiseGroup<void>::SetMaybe(const Mso::Maybe<void> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160561e /* tag_byfy4 */);
  if (!value.IsError()) {
    m_state->TrySetSuccess(/*crashIfFailed:*/ true);
  } else {
    m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ true);
  }
}

LIBLET_PUBLICAPI void PromiseGroup<void>::SetMaybe(Mso::Maybe<void> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160561f /* tag_byfy5 */);
  if (!value.IsError()) {
    m_state->TrySetSuccess(/*crashIfFailed:*/ true);
  } else {
    m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ true);
  }
}

LIBLET_PUBLICAPI bool PromiseGroup<void>::TrySetMaybe(const Mso::Maybe<void> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605620 /* tag_byfy6 */);
  if (!value.IsError()) {
    return m_state->TrySetSuccess(/*crashIfFailed:*/ false);
  } else {
    return m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ false);
  }
}

LIBLET_PUBLICAPI bool PromiseGroup<void>::TrySetMaybe(Mso::Maybe<void> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605621 /* tag_byfy7 */);
  if (!value.IsError()) {
    return m_state->TrySetSuccess(/*crashIfFailed:*/ false);
  } else {
    return m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ false);
  }
}

} // namespace Mso
