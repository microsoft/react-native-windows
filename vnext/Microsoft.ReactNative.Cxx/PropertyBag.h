// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_PROPERTYBAG
#define MICROSOFT_REACTNATIVE_PROPERTYBAG

//!
//! PropertyBag is a thread-safe storage of property values.
//! Internally the value is IInspectable and the key is a name object that represents an
//! atomized string name. Each name object is defined in the context of a namespace object.
//! The null or empty namespace object is a global namespace object.
//! The property name is unique for the same namespace object.
//! Different namespaces may have properties with the same local names.
//!
//! The PropertyBag struct is a wrapper around the ABI-safe IReactPropertyBag interface.
//! The IReactPropertyBag represents all values as IInspectable object which can wrap any type.
//! On top of the untyped IReactPropertyBag values, the PropertyBag offers a set of typed property accessors:
//! Get, GetOrCreate, Remove, and Set.
//!
//! To simplify access to properties we offer PropertyName type that helps to
//! wrap up property name and type into one variable.
//!
//! For example, we can define a property to access an integer value:
//!
//!     const React::PropertyName<int> MyIntProperty{"MyInt"};
//!
//! then we can use it to set property in application settings during initialization:
//!
//!     settings.ApplicationProperties().Set(MyIntProperty, 42);
//!
//! The property can be accessed later in a native modules:
//!
//!     std::optional<int> myInt = context.ApplicationProperties().Get(MyIntProperty);
//!
//! Note that types inherited from winrt::Windows::Framework::IInspectable are returned
//! directly because their null value may indicated absent property value.
//! For other types we return std::optional<T>. It has std::nullopt in case if
//! no property value is found or if it has a wrong type.
//! To avoid compilation errors the non-IInspectable types must be WinRT types which are described here:
//! https://docs.microsoft.com/en-us/uwp/api/windows.foundation.propertytype?view=winrt-18362
//!

#include <winrt/Microsoft.ReactNative.h>
#include <optional>

namespace winrt::Microsoft::ReactNative {

//! Encapsulates the IReactPropertyNamespace
struct PropertyNamespace {
  PropertyNamespace(std::nullptr_t = nullptr) noexcept {}

  explicit PropertyNamespace(IReactPropertyNamespace const &ns) noexcept : m_namespace{ns} {}

  explicit PropertyNamespace(param::hstring const &ns) noexcept : m_namespace{ReactPropertyBag::GetNamespace(ns)} {}

  static PropertyNamespace Global() noexcept {
    return PropertyNamespace{ReactPropertyBag::GlobalNamespace()};
  }

  hstring NamespaceName() const noexcept {
    return m_namespace ? m_namespace.NamespaceName() : hstring{};
  }

  IReactPropertyNamespace const &Get() const noexcept {
    return m_namespace;
  }

  explicit operator bool() const noexcept {
    return static_cast<bool>(m_namespace);
  }

 private:
  IReactPropertyNamespace m_namespace;
};

//! Encapsulates the IReactPropertyName and the property type
template <class T>
struct PropertyName {
  using PropertyType = T;

  PropertyName(std::nullptr_t = nullptr) noexcept {}

  explicit PropertyName(IReactPropertyName const &name) noexcept : m_name{name} {}

  explicit PropertyName(hstring const &localName) noexcept : m_name{ReactPropertyBag::GetName(nullptr, localName)} {}

  PropertyName(PropertyNamespace const &ns, hstring const &localName) noexcept
      : m_name{ReactPropertyBag::GetName(ns.Get(), localName)} {}

  PropertyName(hstring const &ns, hstring const &localName) noexcept
      : m_name{ReactPropertyBag::GetName(ReactPropertyBag::GetNamespace(ns), localName)} {}

  hstring NamespaceName() const noexcept {
    return m_name ? m_name.Namespace().NamespaceName() : hstring{};
  }

  hstring LocalName() const noexcept {
    return m_name ? m_name.LocalName() : hstring{};
  }

  IReactPropertyName const &Get() const noexcept {
    return m_name;
  }

  explicit operator bool() const noexcept {
    return static_cast<bool>(m_name);
  }

 private:
  IReactPropertyName m_name;
};

//! PropertyBag is a wrapper for IReactPropertyBag that stores strongly-typed properties in a thread-safe way.
//! Types inherited from IInspectable are stored directly.
//! Values of other types are boxed with help of winrt::box_value.
struct PropertyBag {
  //! Property return type is either T or std::optional<T>.
  //! T is returned for types inherited from IInspectable.
  //! The std::optional<T> is returned for all other types.
  template <class T>
  using ResultType = std::conditional_t<std::is_base_of_v<Windows::Foundation::IInspectable, T>, T, std::optional<T>>;

  //! Create a new empty instance of PropertyBag.
  PropertyBag(std::nullptr_t = nullptr) noexcept {}

  //! Creates a new instance of PropertyBag with the provided content.
  explicit PropertyBag(IReactPropertyBag const &content) noexcept : m_content{content} {}

  //! True if content is not null.
  explicit operator bool() const noexcept {
    return static_cast<bool>(m_content);
  }

  //! Get content from the PropertyBag.
  IReactPropertyBag const &Get() const noexcept {
    return m_content;
  }

  //! Get property value by property name.
  template <class T>
  ResultType<T> Get(PropertyName<T> const &propertyName) const noexcept {
    Windows::Foundation::IInspectable propertyValue = m_content ? m_content.GetProperty(propertyName.Get()) : nullptr;
    return FromObject<T>(propertyValue);
  }

  //! Ensure that property is created by calling valueCreator if needed, and return value by property name.
  //! The TCreateValue must return either T or std::optional<T>. It must have no parameters.
  template <class T, class TCreateValue>
  ResultType<T> GetOrCreate(PropertyName<T> const &propertyName, TCreateValue const &createValue) const noexcept {
    Windows::Foundation::IInspectable propertyValue = m_content
        ? m_content.GetOrCreateProperty(
              propertyName.Get(), [&createValue]() noexcept { return ToObject<T>(createValue()); })
        : nullptr;
    return FromObject<T>(propertyValue);
  }

  //! Removes property value by property name.
  template <class T>
  void Remove(PropertyName<T> const &propertyName) const noexcept {
    if (m_content) {
      m_content.RemoveProperty(propertyName.Get());
    }
  }

  //! Set property value by property name.
  template <class T, class TValue>
  void Set(PropertyName<T> const &propertyName, TValue const &value) const noexcept {
    if (m_content) {
      auto propertyValue = ToObject<T>(value);
      m_content.SetProperty(propertyName.Get(), propertyValue);
    }
  }

  //! True if two PropertyBags have the same content object instance.
  friend bool operator==(const PropertyBag &left, const PropertyBag &right) noexcept {
    return left.Get() == right.Get();
  }

  //! True if two PropertyBags have different content object instances.
  friend bool operator!=(const PropertyBag &left, const PropertyBag &right) noexcept {
    return left.Get() != right.Get();
  }

  //! True if left PropertyBag content is null.
  friend bool operator==(const PropertyBag &left, std::nullptr_t) noexcept {
    return !static_cast<bool>(left);
  }

  //! True if left PropertyBag content is not null.
  friend bool operator!=(const PropertyBag &left, std::nullptr_t) noexcept {
    return static_cast<bool>(left);
  }

  //! True if right PropertyBag content is null.
  friend bool operator==(std::nullptr_t, const PropertyBag &right) noexcept {
    return !static_cast<bool>(right);
  }

  //! True if right PropertyBag content is not null.
  friend bool operator!=(std::nullptr_t, const PropertyBag &right) noexcept {
    return static_cast<bool>(right);
  }

 private:
  template <class T>
  static Windows::Foundation::IInspectable ToObject(T const &value) noexcept {
    return winrt::box_value(value);
  }

  template <class T>
  static Windows::Foundation::IInspectable ToObject(std::optional<T> const &value) noexcept {
    return value ? ToObject(*value) : nullptr;
  }

  template <class T>
  static auto FromObject(Windows::Foundation::IInspectable const &obj) noexcept {
    if constexpr (std::is_base_of_v<Windows::Foundation::IInspectable, T>) {
      return obj.try_as<T>();
    } else {
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
    }
  }

 private:
  IReactPropertyBag m_content;
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_PROPERTYBAG
