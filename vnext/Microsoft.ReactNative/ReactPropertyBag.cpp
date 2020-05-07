// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactPropertyBag.h"
#include "ReactPropertyBag.g.cpp"
#include <functional/functorRef.h>
#include <object/refCountedObject.h>
#include <shared_mutex>

namespace winrt::Microsoft::ReactNative {

template <class TValue>
struct ReactNametable : Mso::RefCountedObjectNoVTable<ReactNametable<TValue>> {
  com_ptr<TValue> GetOrCreate(hstring const &key, Mso::FunctorRef<com_ptr<TValue>()> const &createValue) noexcept {
    com_ptr<TValue> result;

    {
      // Try to retrieve value.
      std::scoped_lock lock{m_mutex};
      result = m_entries[key].get();
    }

    if (!result) {
      // If value cannot be retrieved, then create it outside of lock.
      auto newValue = createValue();
      std::scoped_lock lock{m_mutex};
      // Make sure that the value was not inserted while we were unlocked
      auto &entry = m_entries[key];
      result = entry.get();
      if (!result) {
        result = std::move(newValue);
        entry = result;
      }
    }

    return result;
  }

 private:
  std::mutex m_mutex;
  std::unordered_map<hstring, weak_ref<TValue>, std::hash<std::wstring_view>, std::equal_to<>> m_entries;
};

struct ReactPropertyName;

struct ReactPropertyNamespace : winrt::implements<ReactPropertyNamespace, IReactPropertyNamespace> {
  using NamespaceNametable = ReactNametable<ReactPropertyNamespace>;
  using LocalNameNametable = ReactNametable<ReactPropertyName>;

  ReactPropertyNamespace(hstring const &namespaceName) noexcept
      : m_namespaceName{namespaceName}, m_localNames{Mso::Make<LocalNameNametable>()} {}

  static com_ptr<ReactPropertyNamespace> GetNamespace(hstring const &namespaceName) noexcept {
    if (namespaceName.size() == 0) {
      return GlobalNamespace();
    }

    // Create the s_namespaces on demand.
    auto namespaces = s_namespaces.load(std::memory_order_relaxed);
    while (!namespaces) {
      Mso::CntPtr<NamespaceNametable> newNamespaces = Mso::Make<NamespaceNametable>();
      if (s_namespaces.compare_exchange_weak(namespaces, newNamespaces.Get())) {
        // We won the race: make sure that we store a pointer with ref count == 1.
        namespaces = newNamespaces.Detach();
      }
    }

    return namespaces->GetOrCreate(
        namespaceName, [&namespaceName]() noexcept { return make_self<ReactPropertyNamespace>(namespaceName); });
  }

  static com_ptr<ReactPropertyNamespace> GlobalNamespace() noexcept {
    com_ptr<ReactPropertyNamespace> result;

    {
      std::scoped_lock lock{s_mutex};
      result = s_globalNamespace.get();
    }

    if (!result) {
      auto newGlobalNamespace = make_self<ReactPropertyNamespace>(L"");
      std::scoped_lock lock{s_mutex};
      // Make sure that the value was not inserted while we were unlocked
      result = s_globalNamespace.get();
      if (!result) {
        result = std::move(newGlobalNamespace);
        s_globalNamespace = result;
      }
    }

    return result;
  }

  com_ptr<ReactPropertyName> GetLocalName(hstring const &localName) noexcept;

 public: // IReactPropertyNamespace
  hstring NamespaceName() noexcept {
    return m_namespaceName;
  }

 private:
  static std::atomic<NamespaceNametable *> s_namespaces;
  static std::mutex s_mutex;
  static weak_ref<ReactPropertyNamespace> s_globalNamespace;

  hstring m_namespaceName;
  Mso::CntPtr<LocalNameNametable> m_localNames;
};

/*static*/ std::atomic<ReactPropertyNamespace::NamespaceNametable *> ReactPropertyNamespace::s_namespaces{nullptr};
/*static*/ std::mutex ReactPropertyNamespace::s_mutex;
/*static*/ weak_ref<ReactPropertyNamespace> ReactPropertyNamespace::s_globalNamespace;

struct ReactPropertyName : winrt::implements<ReactPropertyName, IReactPropertyName> {
  ReactPropertyName(IReactPropertyNamespace const &ns, hstring const &localName) noexcept
      : m_namespace{ns}, m_localName{localName} {}

 public: // IReactPropertyName
  hstring LocalName() const noexcept {
    return m_localName;
  }

  IReactPropertyNamespace Namespace() const noexcept {
    return m_namespace;
  }

 private:
  IReactPropertyNamespace m_namespace;
  hstring m_localName;
};

com_ptr<ReactPropertyName> ReactPropertyNamespace::GetLocalName(hstring const &localName) noexcept {
  // Implement outside of class because we need the ReactPropertyName definition.
  return m_localNames->GetOrCreate(localName, [ this, &localName ]() noexcept {
    IReactPropertyNamespace x = *this;
    return make_self<ReactPropertyName>(x, localName);
  });
}

} // namespace winrt::Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

Microsoft::ReactNative::IReactPropertyNamespace ReactPropertyBag::GlobalNamespace() noexcept {
  return ReactPropertyNamespace::GlobalNamespace().as<Microsoft::ReactNative::IReactPropertyNamespace>();
}

Microsoft::ReactNative::IReactPropertyNamespace ReactPropertyBag::GetNamespace(hstring const &namespaceName) noexcept {
  return ReactPropertyNamespace::GetNamespace(namespaceName).as<Microsoft::ReactNative::IReactPropertyNamespace>();
}

Microsoft::ReactNative::IReactPropertyName ReactPropertyBag::GetName(
    Microsoft::ReactNative::IReactPropertyNamespace const &ns,
    hstring const &localName) noexcept {
  if (!ns) {
    return ReactPropertyNamespace::GlobalNamespace()
        ->GetLocalName(localName)
        .as<Microsoft::ReactNative::IReactPropertyName>();
  }

  return get_self<ReactPropertyNamespace>(ns)->GetLocalName(localName).as<Microsoft::ReactNative::IReactPropertyName>();
}

Windows::Foundation::IInspectable ReactPropertyBag::GetProperty(
    Microsoft::ReactNative::IReactPropertyName const &propertyName) noexcept {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(propertyName);
  if (it != m_entries.end()) {
    return it->second;
  }

  return {nullptr};
}

Windows::Foundation::IInspectable ReactPropertyBag::GetOrCreateProperty(
    Microsoft::ReactNative::IReactPropertyName const &propertyName,
    Microsoft::ReactNative::ReactCreatePropertyValue const &createValue) noexcept {
  Windows::Foundation::IInspectable result{GetProperty(propertyName)};
  if (!result) {
    Windows::Foundation::IInspectable newValue = createValue();
    std::scoped_lock lock{m_mutex};
    auto &entry = m_entries[propertyName];
    if (!entry) {
      entry = std::move(newValue);
    }

    result = entry;
  }

  return result;
}

Windows::Foundation::IInspectable ReactPropertyBag::SetProperty(
    Microsoft::ReactNative::IReactPropertyName const &propertyName,
    Windows::Foundation::IInspectable const &value) noexcept {
  if (!value) {
    return RemoveProperty(propertyName);
  }

  Windows::Foundation::IInspectable result{nullptr};
  std::scoped_lock lock{m_mutex};
  auto &entry = m_entries[propertyName];
  result = std::move(entry);
  entry = value;
  return result;
}

Windows::Foundation::IInspectable ReactPropertyBag::RemoveProperty(
    Microsoft::ReactNative::IReactPropertyName const &propertyName) noexcept {
  Windows::Foundation::IInspectable result{nullptr};
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(propertyName);
  if (it != m_entries.end()) {
    result = std::move(it->second);
    m_entries.erase(it);
  }

  return result;
}

} // namespace winrt::Microsoft::ReactNative::implementation
