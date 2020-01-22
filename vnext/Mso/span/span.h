// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MSO_SPAN_SPAN_H
#define MSO_SPAN_SPAN_H

#include <cstdint>
#include <initializer_list>
#include "crash/verifyElseCrash.h"

//=============================================================================
// Span declaration.
//=============================================================================

namespace Mso {

template <typename T>
struct Span {
  Span() = default;
  template <size_t ArraySize>
  Span(T (&arr)[ArraySize]) noexcept;
  Span(std::initializer_list<T> init) noexcept;
  Span(T const *data, size_t size) noexcept;

  Span(Span const &other) = default;
  Span &operator=(Span const &other) = default;

  T *Data() const noexcept;
  size_t Size() const noexcept;
  void *VoidData() const noexcept;
  void *VoidDataChecked(size_t size) const noexcept;
  void *VoidDataCheckedMin(size_t size) const noexcept;
  template <typename U>
  U *As() const noexcept;
  T *begin() const noexcept;
  T *end() const noexcept;
  T &operator[](size_t index) const noexcept;
  explicit operator bool() const noexcept;

 private:
  T *m_data{nullptr};
  size_t m_size{0};
};

using ByteSpan = Span<uint8_t>;

//=============================================================================
// Span inline implementation.
//=============================================================================

template <typename T>
template <size_t ArraySize>
Span<T>::Span(T (&arr)[ArraySize]) noexcept : m_data{arr}, m_size{ArraySize} {}

template <typename T>
Span<T>::Span(std::initializer_list<T> init) noexcept : m_data{const_cast<T *>(init.begin())}, m_size{init.size()} {}

template <typename T>
Span<T>::Span(T const *data, size_t size) noexcept : m_data{const_cast<T *>(data)}, m_size{size} {}

template <typename T>
T *Span<T>::Data() const noexcept {
  return const_cast<Span *>(this)->m_data;
}

template <typename T>
size_t Span<T>::Size() const noexcept {
  return m_size;
}

template <typename T>
void *Span<T>::VoidData() const noexcept {
  return reinterpret_cast<void *>(const_cast<T *>(m_data));
}

template <typename T>
void *Span<T>::VoidDataChecked(size_t size) const noexcept {
  VerifyElseCrashSz(size == m_size, "Wrong size");
  return reinterpret_cast<void *>(const_cast<T *>(m_data));
}

template <typename T>
void *Span<T>::VoidDataCheckedMin(size_t size) const noexcept {
  VerifyElseCrashSz(size <= m_size, "Wrong size");
  return reinterpret_cast<void *>(const_cast<T *>(m_data));
}

template <typename T>
template <typename U>
U *Span<T>::As() const noexcept {
  VerifyElseCrashSz(sizeof(U) <= m_size, "Wrong size");
  return static_cast<U *>(reinterpret_cast<void *>(const_cast<T *>(m_data)));
}

template <typename T>
T *Span<T>::begin() const noexcept {
  return const_cast<Span *>(this)->m_data;
}

template <typename T>
T *Span<T>::end() const noexcept {
  return const_cast<Span *>(this)->m_data + m_size;
}

template <typename T>
T &Span<T>::operator[](size_t index) const noexcept {
  AssertSz(m_data && index < m_size, "Index out of range!");
  return const_cast<Span *>(this)->m_data[index];
}

template <typename T>
Span<T>::operator bool() const noexcept {
  return m_size > 0;
}

} // namespace Mso

#endif // MSO_SPAN_SPAN_H
