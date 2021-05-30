// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "future/future.h"

namespace Mso {

//=============================================================================
// Promise<void> implementation
//=============================================================================

LIBLET_PUBLICAPI Promise<void>::Promise() noexcept {
  constexpr const auto &promiseTraits = Mso::Futures::FutureTraitsProvider<
      /*Options:     */ Mso::Futures::FutureOptions::CancelIfUnfulfilled,
      /*ResultType:  */ void,
      /*TaskType:    */ void,
      /*PostType:    */ void,
      /*InvokeType:  */ void,
      /*AbandonType: */ void>::Traits;

  m_state = Mso::Futures::MakeFuture(promiseTraits, 0, nullptr);
}

LIBLET_PUBLICAPI Promise<void>::Promise(std::nullptr_t) noexcept {}

LIBLET_PUBLICAPI Promise<void>::Promise(Mso::CntPtr<Mso::Futures::IFuture> &&state) noexcept
    : m_state(std::move(state)) {}

LIBLET_PUBLICAPI Promise<void>::Promise(const Promise &other) noexcept : m_state(other.m_state) {}

LIBLET_PUBLICAPI Promise<void>::Promise(Promise &&other) noexcept : m_state(std::move(other.m_state)) {}

LIBLET_PUBLICAPI Promise<void> &Promise<void>::operator=(const Promise<void> &other) noexcept {
  m_state = other.m_state;
  return *this;
}

LIBLET_PUBLICAPI Promise<void> &Promise<void>::operator=(Promise<void> &&other) noexcept {
  m_state = std::move(other.m_state);
  return *this;
}

LIBLET_PUBLICAPI void Promise<void>::Swap(Promise &other) noexcept {
  using std::swap;
  swap(m_state, other.m_state);
}

LIBLET_PUBLICAPI Promise<void>::operator bool() const noexcept {
  return !m_state.IsEmpty();
}

LIBLET_PUBLICAPI Future<void> Promise<void>::AsFuture() const noexcept {
  return Future<void>{Mso::CntPtr{m_state}};
}

LIBLET_PUBLICAPI void Promise<void>::SetValue() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605605 /* tag_byfyf */);
  (void)m_state->TrySetSuccess(/*crashIfFailed*/ true);
}

LIBLET_PUBLICAPI bool Promise<void>::TrySetValue() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605606 /* tag_byfyg */);
  return m_state->TrySetSuccess(/*crashIfFailed*/ false);
}

LIBLET_PUBLICAPI void Promise<void>::SetValue(const Mso::Maybe<void> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605607 /* tag_byfyh */);
  if (!value.IsError()) {
    (void)m_state->TrySetSuccess(/*crashIfFailed*/ true);
  } else {
    (void)m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ true);
  }
}

LIBLET_PUBLICAPI void Promise<void>::SetValue(Mso::Maybe<void> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605608 /* tag_byfyi */);
  if (!value.IsError()) {
    (void)m_state->TrySetSuccess(/*crashIfFailed*/ true);
  } else {
    (void)m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ true);
  }
}

LIBLET_PUBLICAPI bool Promise<void>::TrySetValue(const Mso::Maybe<void> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605609 /* tag_byfyj */);
  if (!value.IsError()) {
    return m_state->TrySetSuccess(/*crashIfFailed*/ false);
  } else {
    return m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ false);
  }
}

LIBLET_PUBLICAPI bool Promise<void>::TrySetValue(Mso::Maybe<void> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160560a /* tag_byfyk */);
  if (!value.IsError()) {
    return m_state->TrySetSuccess(/*crashIfFailed*/ false);
  } else {
    return m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ false);
  }
}

LIBLET_PUBLICAPI bool Promise<void>::TryCancel() const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160560b /* tag_byfyl */);
  return m_state->TrySetError(Mso::CancellationErrorProvider().MakeErrorCode(true), /*crashIfFailed:*/ false);
}

LIBLET_PUBLICAPI void Promise<void>::SetError(const ErrorCode &errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160560c /* tag_byfym */);
  (void)m_state->TrySetError(Mso::ErrorCode(errorCode), /*crashIfFailed:*/ true);
}

LIBLET_PUBLICAPI void Promise<void>::SetError(ErrorCode &&errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160560d /* tag_byfyn */);
  (void)m_state->TrySetError(std::move(errorCode), /*crashIfFailed:*/ true);
}

LIBLET_PUBLICAPI bool Promise<void>::TrySetError(const ErrorCode &errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160560e /* tag_byfyo */);
  return m_state->TrySetError(Mso::ErrorCode(errorCode), /*crashIfFailed:*/ false);
}

LIBLET_PUBLICAPI bool Promise<void>::TrySetError(ErrorCode &&errorCode) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x0160560f /* tag_byfyp */);
  return m_state->TrySetError(std::move(errorCode), /*crashIfFailed:*/ false);
}

LIBLET_PUBLICAPI void Promise<void>::SetMaybe(const Mso::Maybe<void> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605610 /* tag_byfyq */);
  if (!value.IsError()) {
    (void)m_state->TrySetSuccess(/*crashIfFailed*/ true);
  } else {
    (void)m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ true);
  }
}

LIBLET_PUBLICAPI void Promise<void>::SetMaybe(Mso::Maybe<void> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605611 /* tag_byfyr */);
  if (!value.IsError()) {
    (void)m_state->TrySetSuccess(/*crashIfFailed*/ true);
  } else {
    (void)m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ true);
  }
}

LIBLET_PUBLICAPI bool Promise<void>::TrySetMaybe(const Mso::Maybe<void> &value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605612 /* tag_byfys */);
  if (!value.IsError()) {
    return m_state->TrySetSuccess(/*crashIfFailed*/ false);
  } else {
    return m_state->TrySetError(Mso::ErrorCode(value.GetError()), /*crashIfFailed:*/ false);
  }
}

LIBLET_PUBLICAPI bool Promise<void>::TrySetMaybe(Mso::Maybe<void> &&value) const noexcept {
  VerifyElseCrashSzTag(!m_state.IsEmpty(), "State is empty.", 0x01605613 /* tag_byfyt */);
  if (!value.IsError()) {
    return m_state->TrySetSuccess(/*crashIfFailed*/ false);
  } else {
    return m_state->TrySetError(value.TakeError(), /*crashIfFailed:*/ false);
  }
}

} // namespace Mso
