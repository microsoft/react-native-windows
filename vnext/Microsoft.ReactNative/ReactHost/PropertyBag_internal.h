// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <map>
#include <mutex>
#include <string>
#include "PropertyBag.h"
#include "object/unknownObject.h"

namespace Mso::JSHost {

struct IsolatedId {
  intptr_t IsolationKey;
  std::string PropertyName;
};

struct IsolatedIdComparer {
  bool operator()(const IsolatedId &left, const IsolatedId &right) const noexcept;
};

// IPropertyBag implementation that uses thread safe storage
struct ThreadSafePropertyBag final : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IPropertyBag> {
  Mso::CntPtr<IUnknown> GetOrCreate(
      const void *isolationKey,
      const char *propertyName,
      const Mso::FunctorRef<Mso::CntPtr<IUnknown>()> &valueMaker) noexcept override;

  Mso::CntPtr<IUnknown> Get(const void *isolationKey, const char *propertyName) const noexcept override;

  Mso::CntPtr<IUnknown>
  Replace(const void *isolationKey, const char *propertyName, Mso::CntPtr<IUnknown> &&value) noexcept override;

 private:
  Mso::CntPtr<IUnknown> GetInternal(const IsolatedId &isolatedId) const noexcept;

 private:
  mutable std::mutex m_lock;
  std::map<const IsolatedId, Mso::CntPtr<IUnknown>, IsolatedIdComparer> m_isolatedObjects;
};

struct WeakPropertyBagProxy final : Mso::UnknownObject<IPropertyBag> {
  WeakPropertyBagProxy(const PropertyBag &propertyBag) noexcept;

  Mso::CntPtr<IUnknown> GetOrCreate(
      const void *isolationKey,
      const char *propertyName,
      const Mso::FunctorRef<Mso::CntPtr<IUnknown>()> &valueMaker) noexcept override;

  Mso::CntPtr<IUnknown> Get(const void *isolationKey, const char *propertyName) const noexcept override;

  Mso::CntPtr<IUnknown>
  Replace(const void *isolationKey, const char *propertyName, Mso::CntPtr<IUnknown> &&value) noexcept override;

 private:
  Mso::WeakPtr<IPropertyBag> m_weakPropertyBag;
};

// IPropertyBag non ref-counting proxy to another IPropertyBag.
// This is intended to be allocate on a call stack or by std::make_unique.
// It does not implement ref counting for itself and does not change ref count for provided IPropertyBag.
struct PropertyBagLocalProxy final : Mso::UnknownObject<Mso::RefCountStrategy::NoRefCount, IPropertyBag> {
  PropertyBagLocalProxy(IPropertyBag &propertyBag) noexcept;

  Mso::CntPtr<IUnknown> GetOrCreate(
      const void *isolationKey,
      const char *propertyName,
      const Mso::FunctorRef<Mso::CntPtr<IUnknown>()> &valueMaker) noexcept override;

  Mso::CntPtr<IUnknown> Get(const void *isolationKey, const char *propertyName) const noexcept override;

  Mso::CntPtr<IUnknown>
  Replace(const void *isolationKey, const char *propertyName, Mso::CntPtr<IUnknown> &&value) noexcept override;

 private:
  IPropertyBag &m_propertyBag;
};

} // namespace Mso::JSHost
