// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_BOXEDVALUE
#define MICROSOFT_REACTNATIVE_BOXEDVALUE

#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative {

template <class T>
struct BoxedValue : implements<BoxedValue<T>, IBoxedValue> {
  template <class... TArgs>
  BoxedValue(TArgs &&... args) noexcept : m_value(std::forward<TArgs>(args)...) {}

  int64_t GetPtr() const noexcept {
    return reinterpret_cast<int64_t>(&m_value);
  }

  static T &GetValueUnsafe(IBoxedValue const &boxedValue) noexcept {
    return *reinterpret_cast<T *>(boxedValue.GetPtr());
  }

 private:
  T m_value{};
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_BOXEDVALUE
