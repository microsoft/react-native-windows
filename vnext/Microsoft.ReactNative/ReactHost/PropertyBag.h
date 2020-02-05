// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef JSHOST_REKA_PROPERTYBAG_H
#define JSHOST_REKA_PROPERTYBAG_H

//!
//! PropertyBag is a thread-safe storage of property values.
//! Each property has a name and optional isolation key.
//! Properties with null isolation key are considered global inside of the PropertyBag instance.
//! Isolation key allows to group properties or even hide/isolate them between different components.
//! Property name should be unique for the same isolation key.
//! Different isolation keys may have properties with the same name.
//!
//! The PropertyBag class is a smart pointer to the IPropertyBag ref-counted interface.
//! The IPropertyBag defines three methods: Get, GetOrCreate, and Replace:
//! - Get - returns a property value it exists or null otherwise.
//! - GetOrCreate - tries to find a property and creates if it is not found.
//!                 Since property value creation is called outside of lock, the valueMaker
//!                 could be called multiple times, but only first result is used.
//!                 GetOrCreate may return null if valueMaker returns null.
//! - Replace - replaces current property value with a new one and returns the old value.
//!             Replacing property value with null, removes the property.
//!
//! On top of the untyped IPropertyBag interface, the PropertyBag offers a set of typed property accessors:
//! Get, GetOrCreate, Remove, and Set.
//! There can be two kinds of property value types:
//! - IUnknown inheriting interfaces. Each such type must have an associated GUID and be QI-able from the IUnknown.
//!   Returning values are wrapped up into Mso::CntPtr.
//! - Other types. For other types we use an internal wrapper and we require to associate GUID with each such type.
//!   Macro MSO_PROPERTY_TYPE_GUID can be used to associate property types with GUID. There are some predefined
//!   association in this header file. Values of other types are wrapped up into std::optional.
//!
//! To simplify access to properties we offer NamedProperty and IsolatedNamedProperty types that help to
//! wrap up property type, name, and isolation key into one constant.
//!
//! For example, we can define a property to access UI dispatch queue:
//!
//!     const Mso::JSHost::NamedProperty<Mso::Async::IDispatchQueue> UIQueueProperty{"UIQueue"};
//!
//! then we can use it to set property in Reka context during initialization:
//!
//!     context.Properties().Set(UIQueueProperty, uiQueue);
//!
//! The property can be accessed later in a Reka service:
//!
//!     auto uiQueue = context.Properties().Get(UIQueueProperty);
//!
//! The property can also be given to the RekaServiceWithQueueRegistration class
//! to always call service constructor, destructor, and its methods in UI queue:
//!
//!     Mso::JSHost::RekaServiceWithQueueRegistration<TestService> vTestServce{UIQueueProperty};
//!

#include <optional>
#include <type_traits>
#include "comUtil/qiCastCore.h"
#include "compilerAdapters/cppMacros.h"
#include "functional/functorRef.h"
#include "guid/msoGuid.h"
#include "object/unknownObject.h"

//! To provide strongly typed access to non-IUnknown derived types we are associating
//! each type a with GUID. It is done by declaring a GetPropertyTypeGuid function
//! without implementation that receives a pointer of target type and returns
//! type with a GUID.
//! The MSO_PROPERTY_TYPE_GUID can be used do the associating with simple types.
//! For template specializations with a list of type parameters, you should
//! write the same two lines as in the MSO_PROPERTY_TYPE_GUID macro directly.
#define MSO_PROPERTY_TYPE_GUID(Type, GuidString)        \
  MSO_STRUCT_GUID(Type##_PropertyTypeGuid, GuidString); \
  Type##_PropertyTypeGuid GetPropertyTypeGuid(Type *) noexcept;

namespace Mso::JSHost {
namespace Details {

//! Used for a default case when there is no associated GUID.
struct DefaultPropertyTypeGuid;

//! The default case when type T has no associated GUID.
template <class T>
DefaultPropertyTypeGuid GetPropertyTypeGuid(T *) noexcept;

// Associating GUID with the most common property types.
MSO_PROPERTY_TYPE_GUID(int8_t, "a7997e63-e468-4f4a-ac5e-71389563be98");
MSO_PROPERTY_TYPE_GUID(int16_t, "86d5de2b-b9d9-4908-8f39-ade17a3e3809");
MSO_PROPERTY_TYPE_GUID(int32_t, "40d0fbc8-32fb-4208-bae4-3ab5eef07233");
MSO_PROPERTY_TYPE_GUID(int64_t, "00baf690-91c0-47a8-a6c0-cb448633ff28");
MSO_PROPERTY_TYPE_GUID(uint8_t, "30461cc2-5849-47b6-9c00-bc38608cf823");
MSO_PROPERTY_TYPE_GUID(uint16_t, "e07be325-6fb9-44c5-885d-0cdf3f4dcc58");
MSO_PROPERTY_TYPE_GUID(uint32_t, "b6c3bdd2-ea46-4185-9f04-cad3f57c0edd");
MSO_PROPERTY_TYPE_GUID(uint64_t, "3ed1dbad-d9c3-4989-b6be-6f59470b0034");
MSO_PROPERTY_TYPE_GUID(double, "356c1104-fcd4-4595-a7ca-3aad6f2d51b8");
MSO_PROPERTY_TYPE_GUID(float, "a82a427c-a454-4fd2-a866-c5c0ce3daf26");
MSO_PROPERTY_TYPE_GUID(bool, "e8aa81bd-bbe4-4c55-a18f-edd97d25def4");

//! Associates property type with a GUID.
//! While it is possible to write PropertyTypeGuid<TProperty>::Value specialization
//! for the target type, the recommended way is to associate GUID with a property type
//! using MSO_PROPERTY_TYPE_GUID or declaring GetPropertyTypeGuid function.
template <class TProperty>
struct PropertyTypeGuid {
  static const GUID Value;
};

template <class TProperty>
const GUID PropertyTypeGuid<TProperty>::Value =
    __uuidof(decltype(GetPropertyTypeGuid(static_cast<TProperty *>(nullptr))));

//! Wraps up property values non-inherited from IUnknown into an IUnknown based interface.
MSO_STRUCT_GUID(IPropertyValueWrapper, "8eedb560-d51d-4689-8601-3b02a74ed7a5")
struct IPropertyValueWrapper : IUnknown {
  //! Gets the wrapped value and verifies that it has correct GUID and size.
  virtual void *GetValue(const GUID &propertyTypeId, size_t valueSize) noexcept = 0;
};

//! IPropertyValueWrapper implementation.
template <class TProperty>
struct PropertyValueWrapper : Mso::UnknownObject<IPropertyValueWrapper> {
  PropertyValueWrapper(const TProperty &value) noexcept : m_value{value} {}

  PropertyValueWrapper(TProperty &&value) noexcept : m_value{std::move(value)} {}

  //! Returns the wrapped value if GUID and size are matching.
  //! Otherwise, it crashes the app.
  void *GetValue(const GUID &propertyTypeId, size_t valueSize) noexcept override {
    VerifyElseCrashSzTag(
        propertyTypeId == PropertyTypeGuid<TProperty>::Value, "Incorrect value type", 0x0281d359 /* tag_c63nz */);
    VerifyElseCrashSzTag(valueSize == sizeof(TProperty), "Incorrect value size", 0x0281d35a /* tag_c63n0 */);
    return &m_value;
  }

 private:
  TProperty m_value;
};

//! Property traits for property types not inherited from IUnknown.
template <class TProperty, class TEnable = void>
struct PropertyTraits {
  // Use std::optional for return type to indicate missing values.
  using ReturnType = std::optional<TProperty>;

  static Mso::CntPtr<IUnknown> ToIUnknown(const TProperty &value) noexcept {
    return Mso::Make<PropertyValueWrapper<TProperty>, IUnknown>(value);
  }

  static Mso::CntPtr<IUnknown> ToIUnknown(TProperty &&value) noexcept {
    return Mso::Make<PropertyValueWrapper<TProperty>, IUnknown>(std::move(value));
  }

  static Mso::CntPtr<IUnknown> ToIUnknown(const std::optional<TProperty> &value) noexcept {
    return value ? Mso::Make<PropertyValueWrapper<TProperty>, IUnknown>(*value) : nullptr;
  }

  static Mso::CntPtr<IUnknown> ToIUnknown(std::optional<TProperty> &&value) noexcept {
    return value ? Mso::Make<PropertyValueWrapper<TProperty>, IUnknown>(std::move(*value)) : nullptr;
  }

  static std::optional<TProperty> FromIUnknown(Mso::CntPtr<IUnknown> &&obj) noexcept {
    if (obj) {
      auto &wrapper = query_cast<IPropertyValueWrapper &>(*obj);
      void *valuePtr = wrapper.GetValue(PropertyTypeGuid<TProperty>::Value, sizeof(TProperty));
      return std::optional<TProperty>{*static_cast<TProperty *>(valuePtr)};
    }
    return {};
  }
};

//! Property traits for property types inherited from IUnknown.
//! The type must be used as a pointer.
template <class TProperty>
struct PropertyTraits<TProperty, std::enable_if_t<std::is_base_of<IUnknown, TProperty>::value>> {
  using ReturnType = Mso::CntPtr<TProperty>;

  template <class TValue>
  static Mso::CntPtr<IUnknown> ToIUnknown(TValue &&value) noexcept {
    return Mso::CntPtr<IUnknown>(std::forward<TValue>(value));
  }

  static Mso::CntPtr<TProperty> FromIUnknown(Mso::CntPtr<IUnknown> &&obj) noexcept {
    Mso::CntPtr<TProperty> result;
    if (obj) {
      IUnknown *unknownValue = obj.Get();
      TProperty *typedValue = query_cast<TProperty *>(unknownValue);
      if (typedValue) {
        result.Attach(typedValue);
        obj.Detach();
      } else {
        result = qi_cast_or_crash<TProperty>(unknownValue);
      }
    }

    return result;
  }
};

//! Property traits for property types inherited from IUnknown.
//! The type must be used as an Mso::CntPtr pointer.
template <class TProperty>
struct PropertyTraits<Mso::CntPtr<TProperty>, std::enable_if_t<std::is_base_of<IUnknown, TProperty>::value>> {
  using ReturnType = Mso::CntPtr<TProperty>;

  template <class TValue>
  static Mso::CntPtr<IUnknown> ToIUnknown(TValue &&value) noexcept {
    return Mso::CntPtr<IUnknown>(std::forward<TValue>(value));
  }

  static Mso::CntPtr<TProperty> FromIUnknown(Mso::CntPtr<IUnknown> &&obj) noexcept {
    Mso::CntPtr<TProperty> result;
    if (obj) {
      IUnknown *unknownValue = obj.Get();
      TProperty *typedValue = query_cast<TProperty *>(unknownValue);
      if (typedValue) {
        result.Attach(typedValue);
        obj.Detach();
      } else {
        result = qi_cast_or_crash<TProperty>(unknownValue);
      }
    }

    return result;
  }
};

} // namespace Details

//! Thread-safe untyped property bag interface definition.
//! PropertyBag uses instance of IPropertyBag to keep its content.
//! PropertyBag provides access to strongly typed properties.
//!
//! Properties are uniquely identified by an isolationKey and propertyName.
//! The isolationKey allows to have properties with the same name in the different scopes.
//! It also allows to define properties local to some component that does not share its isolationKey.
//! The isolationKey with a null value enables access to unscoped properties.
MSO_STRUCT_GUID(IPropertyBag, "ffd7e115-472a-45dc-bdaf-0765ab6b2361")
struct DECLSPEC_NOVTABLE IPropertyBag : IUnknown {
  //! Gets value for the provided isolationKey and propertyName.
  //! It returns nullptr if property does not exist.
  virtual Mso::CntPtr<IUnknown> Get(const void *isolationKey, const char *propertyName) const noexcept = 0;

  //! Ensures that property with the provided isolationKey and propertyName exists
  //! by calling valueMaker if needed, and then returns the property value.
  //! The function may return null if valueMaker returns null when called.
  virtual Mso::CntPtr<IUnknown> GetOrCreate(
      const void *isolationKey,
      const char *propertyName,
      const Mso::FunctorRef<Mso::CntPtr<IUnknown>()> &valueMaker) noexcept = 0;

  //! Replaces property value for the provided isolationKey and propertyName.
  //! Returns previously stored value.
  //! If provided value is null, then the property is removed.
  virtual Mso::CntPtr<IUnknown>
  Replace(const void *isolationKey, const char *propertyName, Mso::CntPtr<IUnknown> &&value) noexcept = 0;
};

//! Encapsulates property name and type in the same variable.
//! The typical use is to define properties as constants, and then use them in PropertyBag API.
template <class T>
struct NamedProperty {
  using Type = T;
  const char *Name{""};
};

//! Encapsulates isolation key, property name, and property type in the same variable.
//! The typical use is to define properties as constants, and then use them in PropertyBag API.
template <class T>
struct IsolatedNamedProperty {
  using Type = T;
  const void *IsolationKey{nullptr};
  const char *Name{""};
};

//! PropertyBag implementation that can store strongly-typed properties in a thread-safe way.
//! Types inherited from IUnknown are store directly as Mso::CntPtr<T>.
//! Values of other types are wrapped up into PropertyValueWrapper object.
struct PropertyBag {
  //! Property return type is either Mso::CntPtr<TProperty> or std::optional<TProperty>.
  //! Mso::CntPtr<TProperty> is returned for types inherited from IUnknown.
  //! The std::optional<TProperty> is returned for all other types.
  template <class TProperty>
  using ReturnType = typename Details::PropertyTraits<TProperty>::ReturnType;

  //! Creates a new instance of PropertyBag.
  LIBLET_PUBLICAPI PropertyBag() noexcept;

  //! Creates a new instance of PropertyBag with a null content.
  LIBLET_PUBLICAPI PropertyBag(std::nullptr_t) noexcept;

  //! Creates a new instance of PropertyBag with the provided content.
  LIBLET_PUBLICAPI PropertyBag(Mso::CntPtr<IPropertyBag> &&content) noexcept;

  //! Creates a new instance of PropertyBag with the other PropertyBag content to share.
  LIBLET_PUBLICAPI PropertyBag(const PropertyBag &other) noexcept;

  //! Creates a new instance of PropertyBag with content taken from the other PropertyBag.
  //! The other PropertyBag content becomes null.
  LIBLET_PUBLICAPI PropertyBag(PropertyBag &&other) noexcept;

  //! Destroys instance of PropertyBag.
  LIBLET_PUBLICAPI ~PropertyBag() noexcept;

  //! Assigns the content from the other PropertyBag to share.
  LIBLET_PUBLICAPI PropertyBag &operator=(const PropertyBag &other) noexcept;

  //! Assigns the content taken from the other PropertyBag. The other PropertyBag content becomes null.
  LIBLET_PUBLICAPI PropertyBag &operator=(PropertyBag &&other) noexcept;

  //! True if content is not null.
  explicit operator bool() const noexcept;

  //! Swaps content between two PropertyBags.
  friend void swap(PropertyBag &left, PropertyBag &right) noexcept;

  //! Gets content from the PropertyBag.
  friend IPropertyBag *GetIPropertyBag(const PropertyBag &propertyBag) noexcept;

  //! Gets property value by property name.
  template <class TProperty>
  ReturnType<TProperty> Get(const char *propertyName) const noexcept;

  //! Gets property value by named property.
  template <class TProperty>
  ReturnType<TProperty> Get(const NamedProperty<TProperty> &property) const noexcept;

  //! Gets property value by isolated named property.
  template <class TProperty>
  ReturnType<TProperty> Get(const IsolatedNamedProperty<TProperty> &property) const noexcept;

  //! Gets property value by isolationKey and property name.
  template <class TProperty>
  ReturnType<TProperty> Get(const void *isolationKey, const char *propertyName) const noexcept;

  //! Gets property value by isolationKey and named property.
  template <class TProperty>
  ReturnType<TProperty> Get(const void *isolationKey, const NamedProperty<TProperty> &property) const noexcept;

  //! Ensures that property is created by calling valueMaker if needed, and returns value by property name.
  template <class TProperty>
  ReturnType<TProperty> GetOrCreate(
      const char *propertyName,
      const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept;

  //! Ensures that property is created by calling valueMaker if needed, and returns value by named property.
  template <class TProperty>
  ReturnType<TProperty> GetOrCreate(
      const NamedProperty<TProperty> &property,
      const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept;

  //! Ensures that property is created by calling valueMaker if needed, and returns value by isolated named property.
  template <class TProperty>
  ReturnType<TProperty> GetOrCreate(
      const IsolatedNamedProperty<TProperty> &property,
      const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept;

  //! Ensures that property is created by calling valueMaker if needed, and returns value by isolationKey and property
  //! name.
  template <class TProperty>
  ReturnType<TProperty> GetOrCreate(
      const void *isolationKey,
      const char *propertyName,
      const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept;

  //! Ensures that property is created by calling valueMaker if needed, and returns value by isolationKey and named
  //! property.
  template <class TProperty>
  ReturnType<TProperty> GetOrCreate(
      const void *isolationKey,
      const NamedProperty<TProperty> &property,
      const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept;

  //! Removes property value by property name.
  LIBLET_PUBLICAPI void Remove(const char *propertyName) const noexcept;

  //! Removes property value by named property.
  template <class TProperty>
  void Remove(const NamedProperty<TProperty> &property) const noexcept;

  //! Removes property value by isolated named property.
  template <class TProperty>
  void Remove(const IsolatedNamedProperty<TProperty> &property) const noexcept;

  //! Removes property value by isolationKey and property name.
  LIBLET_PUBLICAPI void Remove(const void *isolationKey, const char *propertyName) const noexcept;

  //! Removes property value by isolationKey and named property.
  template <class TProperty>
  void Remove(const void *isolationKey, const NamedProperty<TProperty> &property) const noexcept;

  //! Sets property value by property type and name.
  template <class TProperty, class TValue>
  void Set(const char *propertyName, TValue &&value) const noexcept;

  //! Sets property value by named property.
  template <class TProperty, class TValue>
  void Set(const NamedProperty<TProperty> &property, TValue &&value) const noexcept;

  //! Sets property value by isolated named property.
  template <class TProperty, class TValue>
  void Set(const IsolatedNamedProperty<TProperty> &property, TValue &&value) const noexcept;

  //! Sets property value by isolationKey, property type and name.
  template <class TProperty, class TValue>
  void Set(const void *isolationKey, const char *propertyName, TValue &&value) const noexcept;

  //! Sets property value by isolationKey and named property.
  template <class TProperty, class TValue>
  void Set(const void *isolationKey, const NamedProperty<TProperty> &property, TValue &&value) const noexcept;

 private:
  Mso::CntPtr<IPropertyBag> m_content;
};

//! Swaps content between two PropertyBags.
inline void swap(PropertyBag &left, PropertyBag &right) noexcept;

//! Gets content from the PropertyBag.
inline IPropertyBag *GetIPropertyBag(const PropertyBag &propertyBag) noexcept;

//! True if two PropertyBags have the same content object instance.
inline bool operator==(const PropertyBag &left, const PropertyBag &right) noexcept;

//! True if two PropertyBags have different content object instances.
inline bool operator!=(const PropertyBag &left, const PropertyBag &right) noexcept;

//! True if left PropertyBag content is null.
inline bool operator==(const PropertyBag &left, std::nullptr_t) noexcept;

//! True if left PropertyBag content is not null.
inline bool operator!=(const PropertyBag &left, std::nullptr_t) noexcept;

//! True if right PropertyBag content is null.
inline bool operator==(std::nullptr_t, const PropertyBag &left) noexcept;

//! True if right PropertyBag content is not null.
inline bool operator!=(std::nullptr_t, const PropertyBag &left) noexcept;

//========================================================================
// PropertyBag inline implementation
//========================================================================

inline PropertyBag::operator bool() const noexcept {
  return m_content != nullptr;
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::Get(const char *propertyName) const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(m_content->Get(nullptr, propertyName));
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::Get(const NamedProperty<TProperty> &property) const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(m_content->Get(nullptr, property.Name));
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::Get(const IsolatedNamedProperty<TProperty> &property) const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(m_content->Get(property.IsolationKey, property.Name));
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::Get(const void *isolationKey, const char *propertyName) const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(m_content->Get(isolationKey, propertyName));
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::Get(const void *isolationKey, const NamedProperty<TProperty> &property)
    const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(m_content->Get(isolationKey, property.Name));
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::GetOrCreate(
    const char *propertyName,
    const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(
      m_content->GetOrCreate(nullptr, propertyName, [&valueMaker]() noexcept {
        return Details::PropertyTraits<TProperty>::ToIUnknown(valueMaker());
      }));
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::GetOrCreate(
    const NamedProperty<TProperty> &property,
    const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(
      m_content->GetOrCreate(nullptr, property.Name, [&valueMaker]() noexcept {
        return Details::PropertyTraits<TProperty>::ToIUnknown(valueMaker());
      }));
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::GetOrCreate(
    const IsolatedNamedProperty<TProperty> &property,
    const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(
      m_content->GetOrCreate(property.IsolationKey, property.Name, [&valueMaker]() noexcept {
        return Details::PropertyTraits<TProperty>::ToIUnknown(valueMaker());
      }));
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::GetOrCreate(
    const void *isolationKey,
    const char *propertyName,
    const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(
      m_content->GetOrCreate(isolationKey, propertyName, [&valueMaker]() noexcept {
        return Details::PropertyTraits<TProperty>::ToIUnknown(valueMaker());
      }));
}

template <class TProperty>
PropertyBag::ReturnType<TProperty> PropertyBag::GetOrCreate(
    const void *isolationKey,
    const NamedProperty<TProperty> &property,
    const Mso::FunctorRef<ReturnType<TProperty>()> &valueMaker) const noexcept {
  return Details::PropertyTraits<TProperty>::FromIUnknown(
      m_content->GetOrCreate(isolationKey, property.Name, [&valueMaker]() noexcept {
        return Details::PropertyTraits<TProperty>::ToIUnknown(valueMaker());
      }));
}

template <class TProperty>
void PropertyBag::Remove(const NamedProperty<TProperty> &property) const noexcept {
  m_content->Replace(nullptr, property.Name, nullptr);
}

template <class TProperty>
void PropertyBag::Remove(const IsolatedNamedProperty<TProperty> &property) const noexcept {
  m_content->Replace(property.IsolationKey, property.Name, nullptr);
}

template <class TProperty>
void PropertyBag::Remove(const void *isolationKey, const NamedProperty<TProperty> &property) const noexcept {
  m_content->Replace(isolationKey, property.Name, nullptr);
}

template <class TProperty, class TValue>
void PropertyBag::Set(const char *propertyName, TValue &&value) const noexcept {
  m_content->Replace(
      nullptr, propertyName, Details::PropertyTraits<TProperty>::ToIUnknown(std::forward<TValue>(value)));
}

template <class TProperty, class TValue>
void PropertyBag::Set(const NamedProperty<TProperty> &property, TValue &&value) const noexcept {
  m_content->Replace(
      nullptr, property.Name, Details::PropertyTraits<TProperty>::ToIUnknown(std::forward<TValue>(value)));
}

template <class TProperty, class TValue>
void PropertyBag::Set(const IsolatedNamedProperty<TProperty> &property, TValue &&value) const noexcept {
  m_content->Replace(
      property.IsolationKey,
      property.Name,
      Details::PropertyTraits<TProperty>::ToIUnknown(std::forward<TValue>(value)));
}

template <class TProperty, class TValue>
void PropertyBag::Set(const void *isolationKey, const char *propertyName, TValue &&value) const noexcept {
  m_content->Replace(
      isolationKey, propertyName, Details::PropertyTraits<TProperty>::ToIUnknown(std::forward<TValue>(value)));
}

template <class TProperty, class TValue>
void PropertyBag::Set(const void *isolationKey, const NamedProperty<TProperty> &property, TValue &&value) const
    noexcept {
  m_content->Replace(
      isolationKey, property.Name, Details::PropertyTraits<TProperty>::ToIUnknown(std::forward<TValue>(value)));
}

inline void swap(PropertyBag &left, PropertyBag &right) noexcept {
  using std::swap;
  swap(left.m_content, right.m_content);
}

inline IPropertyBag *GetIPropertyBag(const PropertyBag &propertyBag) noexcept {
  return propertyBag.m_content.Get();
}

} // namespace Mso::JSHost

#endif // JSHOST_REKA_PROPERTYBAG_H
