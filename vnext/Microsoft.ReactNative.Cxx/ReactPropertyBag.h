// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_PROPERTYBAG
#define MICROSOFT_REACTNATIVE_PROPERTYBAG

//
// ReactPropertyBag is a thread-safe storage of property values.
// Internally the value is IInspectable and the key is a name object that represents an
// atomized string name. Each name object is defined in the context of a namespace object.
// The null or empty namespace object is a global namespace object.
// The property name is unique for the same namespace object.
// Different namespaces may have properties with the same local names.
//
// The ReactPropertyBag struct is a wrapper around the ABI-safe IReactPropertyBag interface.
// The IReactPropertyBag represents all values as IInspectable object which can wrap any type.
// On top of the untyped IReactPropertyBag values, the ReactPropertyBag offers a set of typed
// property accessors: Get, GetOrCreate, Remove, and Set.
//
// To simplify access to properties we offer ReactPropertyId type that helps to
// wrap up property name and type into one variable.
//
// For example, we can define a property to access an integer value:
//
//     const React::ReactPropertyId<int> MyIntProperty{"MyInt"};
//
// then we can use it to set property in settings properties during initialization:
//
//     settings.Properties().Set(MyIntProperty, 42);
//
// The property can be accessed later in a native modules:
//
//     std::optional<int> myInt = context.Properties().Get(MyIntProperty);
//
// Note that types inherited from IInspectable are returned
// directly because their null value may indicated absent property value.
// For other types we return std::optional<T>. It has std::nullopt value in case if
// no property value is found or if it has a wrong type.
// To pass values through the ABI boundary the non-IInspectable types must be WinRT types
// which are described here:
// https://docs.microsoft.com/en-us/uwp/api/windows.foundation.propertytype?view=winrt-18362
//

#include <winrt/Microsoft.ReactNative.h>
#include <optional>
#include "BoxedValue.h"

namespace winrt::Microsoft::ReactNative {

// Encapsulates the IReactPropertyNamespace
struct ReactPropertyNamespace {
  ReactPropertyNamespace(std::nullptr_t = nullptr) noexcept {}

  explicit ReactPropertyNamespace(IReactPropertyNamespace const &handle) noexcept : m_handle{handle} {}

  explicit ReactPropertyNamespace(param::hstring const &ns) noexcept
      : m_handle{ReactPropertyBagHelper::GetNamespace(ns)} {}

  static ReactPropertyNamespace Global() noexcept {
    return ReactPropertyNamespace{ReactPropertyBagHelper::GlobalNamespace()};
  }

  hstring NamespaceName() const noexcept {
    return m_handle ? m_handle.NamespaceName() : hstring{};
  }

  IReactPropertyNamespace const &Handle() const noexcept {
    return m_handle;
  }

  explicit operator bool() const noexcept {
    return static_cast<bool>(m_handle);
  }

 private:
  IReactPropertyNamespace m_handle;
};

// Encapsulates the IReactPropertyName and the property type
template <class T>
struct ReactPropertyId {
  using PropertyType = T;

  ReactPropertyId(std::nullptr_t = nullptr) noexcept {}

  explicit ReactPropertyId(IReactPropertyName const &handle) noexcept : m_handle{handle} {}

  explicit ReactPropertyId(hstring const &localName) noexcept
      : m_handle{ReactPropertyBagHelper::GetName(nullptr, localName)} {}

  ReactPropertyId(ReactPropertyNamespace const &ns, hstring const &localName) noexcept
      : m_handle{ReactPropertyBagHelper::GetName(ns.Handle(), localName)} {}

  ReactPropertyId(hstring const &ns, hstring const &localName) noexcept
      : m_handle{ReactPropertyBagHelper::GetName(ReactPropertyBagHelper::GetNamespace(ns), localName)} {}

  hstring NamespaceName() const noexcept {
    return m_handle ? m_handle.Namespace().NamespaceName() : hstring{};
  }

  hstring LocalName() const noexcept {
    return m_handle ? m_handle.LocalName() : hstring{};
  }

  IReactPropertyName const &Handle() const noexcept {
    return m_handle;
  }

  explicit operator bool() const noexcept {
    return static_cast<bool>(m_handle);
  }

 private:
  IReactPropertyName m_handle;
};

// ReactPropertyBag is a wrapper for IReactPropertyBag to store strongly-typed properties in a thread-safe way.
// Types inherited from IInspectable are stored directly.
// Values of other types are boxed with help of winrt::box_value.
// Non-WinRT types are wrapped with the help of BoxedValue template.
struct ReactPropertyBag {
  // Property result type is either T or std::optional<T>.
  // T is returned for types inherited from IInspectable.
  // The std::optional<T> is returned for all other types.
  template <class T>
  using ResultType = std::conditional_t<std::is_base_of_v<Windows::Foundation::IInspectable, T>, T, std::optional<T>>;

  // Create a new empty instance of ReactPropertyBag.
  ReactPropertyBag(std::nullptr_t = nullptr) noexcept {}

  // Creates a new instance of ReactPropertyBag with the provided handle.
  explicit ReactPropertyBag(IReactPropertyBag const &handle) noexcept : m_handle{handle} {}

  // True if handle is not null.
  explicit operator bool() const noexcept {
    return static_cast<bool>(m_handle);
  }

  // Get ReactPropertyBag handle.
  IReactPropertyBag const &Handle() const noexcept {
    return m_handle;
  }

  // Get property value by property name.
  template <class T>
  static ResultType<T> Get(IReactPropertyBag const &handle, ReactPropertyId<T> const &propertyId) noexcept {
    Windows::Foundation::IInspectable propertyValue = handle ? handle.Get(propertyId.Handle()) : nullptr;
    return FromObject<T>(propertyValue);
  }

  // Ensure that property is created by calling valueCreator if needed, and return value by property name.
  // The TCreateValue must return either T or std::optional<T>. It must have no parameters.
  template <class T, class TCreateValue>
  static ResultType<T> GetOrCreate(
      IReactPropertyBag const &handle,
      ReactPropertyId<T> const &propertyId,
      TCreateValue const &createValue) noexcept {
    Windows::Foundation::IInspectable propertyValue = handle
        ? handle.GetOrCreate(propertyId.Handle(), [&createValue]() noexcept { return ToObject<T>(createValue()); })
        : nullptr;
    return FromObject<T>(propertyValue);
  }

  // Set property value by property name.
  template <class T, class TValue>
  static void Set(IReactPropertyBag const &handle, ReactPropertyId<T> const &propertyId, TValue const &value) noexcept {
    if (handle) {
      auto propertyValue = ToObject<T>(value);
      handle.Set(propertyId.Handle(), propertyValue);
    }
  }

  // Removes property value by property name.
  template <class T>
  static void Remove(IReactPropertyBag const &handle, ReactPropertyId<T> const &propertyId) noexcept {
    if (handle) {
      handle.Set(propertyId.Handle(), nullptr);
    }
  }

  // Get property value by property name.
  template <class T>
  ResultType<T> Get(ReactPropertyId<T> const &propertyId) const noexcept {
    return Get(m_handle, propertyId);
  }

  // Ensure that property is created by calling valueCreator if needed, and return value by property name.
  // The TCreateValue must return either T or std::optional<T>. It must have no parameters.
  template <class T, class TCreateValue>
  ResultType<T> GetOrCreate(ReactPropertyId<T> const &propertyId, TCreateValue const &createValue) const noexcept {
    return GetOrCreate(m_handle, propertyId, createValue);
  }

  // Set property value by property name.
  template <class T, class TValue>
  void Set(ReactPropertyId<T> const &propertyId, TValue const &value) const noexcept {
    Set(m_handle, propertyId, value);
  }

  // Removes property value by property name.
  template <class T>
  void Remove(ReactPropertyId<T> const &propertyId) const noexcept {
    Remove(m_handle, propertyId);
  }

  // True if two ReactPropertyBags have the same handle.
  friend bool operator==(const ReactPropertyBag &left, const ReactPropertyBag &right) noexcept {
    return left.m_handle == right.m_handle;
  }

  // True if two ReactPropertyBags have different handles.
  friend bool operator!=(const ReactPropertyBag &left, const ReactPropertyBag &right) noexcept {
    return left.m_handle != right.m_handle;
  }

  // True if left ReactPropertyBag handle is null.
  friend bool operator==(const ReactPropertyBag &left, std::nullptr_t) noexcept {
    return !static_cast<bool>(left.m_handle);
  }

  // True if left ReactPropertyBag handle is not null.
  friend bool operator!=(const ReactPropertyBag &left, std::nullptr_t) noexcept {
    return static_cast<bool>(left.m_handle);
  }

  // True if right ReactPropertyBag handle is null.
  friend bool operator==(std::nullptr_t, const ReactPropertyBag &right) noexcept {
    return !static_cast<bool>(right.m_handle);
  }

  // True if right ReactPropertyBag handle is not null.
  friend bool operator!=(std::nullptr_t, const ReactPropertyBag &right) noexcept {
    return static_cast<bool>(right.m_handle);
  }

 private:
  template <class T>
  static Windows::Foundation::IInspectable ToObject(T const &value) noexcept {
    if constexpr (impl::has_category_v<T>) {
      return box_value(value);
    } else {
      return make<BoxedValue<T>>(value);
    }
  }

  template <class T>
  static Windows::Foundation::IInspectable ToObject(std::optional<T> const &value) noexcept {
    return value ? ToObject(*value) : nullptr;
  }

  template <class T>
  static auto FromObject(Windows::Foundation::IInspectable const &obj) noexcept {
    if constexpr (std::is_base_of_v<Windows::Foundation::IInspectable, T>) {
      return obj.try_as<T>();
    } else if constexpr (impl::has_category_v<T>) {
      if (obj) {
#ifdef WINRT_IMPL_IUNKNOWN_DEFINED
        if constexpr (std::is_same_v<T, GUID>) {
          if (auto temp = obj.try_as<Windows::Foundation::IReference<guid>>()) {
            return std::optional<T>{temp.Value()};
          }
        }
#endif
        if (auto temp = obj.try_as<Windows::Foundation::IReference<T>>()) {
          return std::optional<T>{temp.Value()};
        }

        if constexpr (std::is_enum_v<T>) {
          if (auto temp = obj.try_as<Windows::Foundation::IReference<std::underlying_type_t<T>>>()) {
            return std::optional<T>{static_cast<T>(temp.Value())};
          }
        }
      }

      return std::optional<T>{};
    } else {
      if (obj) {
        if (auto temp = obj.try_as<IBoxedValue>()) {
          return std::optional<T>{BoxedValue<T>::GetValueUnsafe(temp)};
        }
      }

      return std::optional<T>{};
    }
  }

 private:
  IReactPropertyBag m_handle;
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_PROPERTYBAG
