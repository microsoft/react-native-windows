// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_DETAILS_ARRAYVIEW_H
#define MSO_FUTURE_DETAILS_ARRAYVIEW_H

#include <cstdint>
#include <initializer_list>
#include "crash/verifyElseCrash.h"

//=============================================================================
// ArrayView declaration.
//=============================================================================

namespace Mso::Async {

template <class T>
struct ArrayView {
  ArrayView() noexcept {}

  template <size_t size>
  ArrayView(T (&arr)[size]) noexcept : m_data(arr), m_size(size) {}

  ArrayView(std::initializer_list<T> init) noexcept : m_data(const_cast<T *>(init.begin())), m_size(init.size()) {}

  ArrayView(const T *data, size_t size) noexcept : m_data(const_cast<T *>(data)), m_size(size) {}

  ArrayView(const ArrayView &other) noexcept : m_data(other.m_data), m_size(other.m_size) {}

  ArrayView &operator=(const ArrayView &other) noexcept {
    if (this != &other) {
      m_data = other.m_data;
      m_size = other.m_size;
    }

    return *this;
  }

  T *Data() noexcept {
    return m_data;
  }

  const T *Data() const noexcept {
    return m_data;
  }

  size_t Size() const noexcept {
    return m_size;
  }

  void *VoidData() const noexcept {
    return reinterpret_cast<void *>(const_cast<T *>(m_data));
  }

  void *VoidDataChecked(size_t size) const noexcept {
    VerifyElseCrashSzTag(size == m_size, "Wrong size", 0x012ca3d8 /* tag_blkpy */);
    return reinterpret_cast<void *>(const_cast<T *>(m_data));
  }

  void *VoidDataCheckedMin(size_t size) const noexcept {
    VerifyElseCrashSzTag(size <= m_size, "Wrong size", 0x012ca3d9 /* tag_blkpz */);
    return reinterpret_cast<void *>(const_cast<T *>(m_data));
  }

  template <class U>
  U *As() const noexcept {
    VerifyElseCrashSzTag(sizeof(U) <= m_size, "Wrong size", 0x01605691 /* tag_byf0r */);
    return static_cast<U *>(reinterpret_cast<void *>(const_cast<T *>(m_data)));
  }

  T *begin() noexcept {
    return m_data;
  }

  T *end() noexcept {
    return m_data + m_size;
  }

  const T *begin() const noexcept {
    return m_data;
  }

  const T *end() const noexcept {
    return m_data + m_size;
  }

  T &operator[](size_t index) noexcept {
    AssertSzTag(m_data && index < m_size, "Index out of range!", 0x012ca3da /* tag_blkp0 */);
    return m_data[index];
  }

  const T &operator[](size_t index) const noexcept {
    AssertSzTag(m_data && index < m_size, "Index out of range!", 0x012ca3db /* tag_blkp1 */);
    return m_data[index];
  }

  explicit operator bool() const noexcept {
    return m_size > 0;
  }

 private:
  T *m_data{nullptr};
  size_t m_size{0};
};

} // namespace Mso::Async

#endif // MSO_FUTURE_DETAILS_ARRAYVIEW_H
