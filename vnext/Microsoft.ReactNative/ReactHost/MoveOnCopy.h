// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

/**
  Support for Copy on Move pattern, that is needed to implement std:move() semantics
  when capturing values in lambdas.
*/

namespace Mso {

// The purpose of this construct is to wrap a move-able rvalue inside an lvalue.
// Note: this is intended to be used only in lambda captures in order to emulate "capture by move" and wasn't tested for
// any other purpose.
template <typename T>
struct MoveOnCopyWrapper {
  MoveOnCopyWrapper(T &&value) noexcept : m_value{std::move(value)} {}

  MoveOnCopyWrapper(const MoveOnCopyWrapper &other) noexcept : m_value{std::move(other.m_value)} {}

  MoveOnCopyWrapper &operator=(MoveOnCopyWrapper const &other) noexcept {
    m_value = other.MoveValue();
    return *this;
  }

  MoveOnCopyWrapper(MoveOnCopyWrapper &&other) = default;
  MoveOnCopyWrapper &operator=(MoveOnCopyWrapper &&other) = default;

  T &GetValue() const noexcept {
    return m_value;
  }

  T MoveValue() const noexcept {
    return std::move(m_value);
  }

 private:
  mutable T m_value;
};

template <typename T>
MoveOnCopyWrapper<T> MakeMoveOnCopyWrapper(T &&value) {
  return MoveOnCopyWrapper<T>(std::forward<T>(value));
}

} // namespace Mso
