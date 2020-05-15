// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_REACTNONABIVALUE
#define MICROSOFT_REACTNATIVE_REACTNONABIVALUE

#include <winrt/Microsoft.ReactNative.h>
#include <utility>

namespace winrt::Microsoft::ReactNative {

namespace implementation {

template <class T>
struct ReactNonAbiValue : implements<ReactNonAbiValue<T>, IReactNonAbiValue> {
  template <class... TArgs>
  ReactNonAbiValue(TArgs &&... args) noexcept : m_value{std::forward<TArgs>(args)...} {}

  int64_t GetPtr() const noexcept {
    return reinterpret_cast<int64_t>(&m_value);
  }

 private:
  T m_value{};
};

} // namespace implementation

template <class T>
struct ReactNonAbiValue : Windows::Foundation::IInspectable {
  template <class... TArgs>
  ReactNonAbiValue(std::in_place_t, TArgs &&... args) noexcept
      : IInspectable{make<implementation::ReactNonAbiValue<T>>(std::forward<TArgs>(args)...)} {}
  ReactNonAbiValue(std::nullptr_t = nullptr) noexcept {}
  ReactNonAbiValue(void *ptr, take_ownership_from_abi_t) noexcept : IInspectable(ptr, take_ownership_from_abi) {}

  static T *GetPtrUnsafe(IInspectable const &obj) noexcept {
    if (IReactNonAbiValue temp = obj.try_as<IReactNonAbiValue>()) {
      return reinterpret_cast<T *>(temp.GetPtr());
    } else {
      return nullptr;
    }
  }

  T *GetPtr() const noexcept {
    if (IReactNonAbiValue const &temp =
            *static_cast<IReactNonAbiValue const *>(static_cast<IInspectable const *>(this))) {
      return reinterpret_cast<T *>(temp.GetPtr());
    } else {
      return nullptr;
    }
  }

  T &operator*() const noexcept {
    return *GetPtr();
  }

  T *operator->() const noexcept {
    return GetPtr();
  }

  T &Value() const noexcept {
    return *GetPtr();
  }

  template <class... TArgs>
  auto operator()(TArgs &&... args) const {
    return this->operator*()(std::forward<TArgs>(args)...);
  }
};

template <class T>
struct IsReactNonAbiValue : std::false_type {};

template <class T>
struct IsReactNonAbiValue<ReactNonAbiValue<T>> : std::true_type {};

template <class T>
constexpr bool IsReactNonAbiValueV = IsReactNonAbiValue<T>::value;

} // namespace winrt::Microsoft::ReactNative

namespace winrt::impl {
template <class T>
struct default_interface<Microsoft::ReactNative::ReactNonAbiValue<T>> {
  using type = Microsoft::ReactNative::IReactNonAbiValue;
};
} // namespace winrt::impl

#endif // MICROSOFT_REACTNATIVE_REACTNONABIVALUE
