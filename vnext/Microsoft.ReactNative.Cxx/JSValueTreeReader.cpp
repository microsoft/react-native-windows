// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "JSValueTreeReader.h"

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// JSValueTreeReader implementation
//===========================================================================

JSValueTreeReader::StackEntry::StackEntry(const JSValue &value, const JSValueObject::const_iterator &property) noexcept
    : Value{value}, Property{property} {}

JSValueTreeReader::StackEntry::StackEntry(const JSValue &value, const JSValueArray::const_iterator &item) noexcept
    : Value{value}, Item{item} {}

JSValueTreeReader::JSValueTreeReader(const JSValue &value) noexcept : m_root{value}, m_current{&value} {}

JSValueTreeReader::JSValueTreeReader(JSValue &&value) noexcept
    : m_ownedValue{std::move(value)}, m_root{m_ownedValue}, m_current{&m_ownedValue} {}

JSValueType JSValueTreeReader::ValueType() noexcept {
  return m_current->Type();
}

bool JSValueTreeReader::GetNextObjectProperty(hstring &propertyName) noexcept {
  if (!m_isInContainer) {
    if (m_current->Type() == JSValueType::Object) {
      const auto &properties = m_current->Object();
      const auto &property = properties.begin();
      if (property != properties.end()) {
        m_stack.emplace_back(*m_current, property);
        SetCurrentValue(property->second);
        propertyName = to_hstring(property->first);
        return true;
      } else {
        m_isInContainer = !m_stack.empty();
      }
    }
  } else if (!m_stack.empty()) {
    auto &entry = m_stack.back();
    if (entry.Value.Type() == JSValueType::Object) {
      auto &property = entry.Property;
      if (++property != entry.Value.Object().end()) {
        SetCurrentValue(property->second);
        propertyName = to_hstring(property->first);
        return true;
      } else {
        m_current = &entry.Value;
        m_stack.pop_back();
        m_isInContainer = !m_stack.empty();
      }
    }
  }

  propertyName = to_hstring(L"");
  return false;
}

bool JSValueTreeReader::GetNextArrayItem() noexcept {
  if (!m_isInContainer) {
    if (m_current->Type() == JSValueType::Array) {
      const auto &item = m_current->Array().begin();
      if (item != m_current->Array().end()) {
        m_stack.emplace_back(*m_current, item);
        SetCurrentValue(*item);
        return true;
      } else {
        m_isInContainer = !m_stack.empty();
      }
    }
  } else if (!m_stack.empty()) {
    auto &entry = m_stack.back();
    if (entry.Value.Type() == JSValueType::Array) {
      if (++entry.Item != entry.Value.Array().end()) {
        SetCurrentValue(*entry.Item);
        return true;
      } else {
        m_current = &entry.Value;
        m_stack.pop_back();
        m_isInContainer = !m_stack.empty();
      }
    }
  }

  return false;
}

void JSValueTreeReader::SetCurrentValue(const JSValue &value) noexcept {
  m_current = &value;
  switch (value.Type()) {
    case JSValueType::Object:
    case JSValueType::Array:
      m_isInContainer = false;
      break;
    default:
      m_isInContainer = true;
      break;
  }
}

hstring JSValueTreeReader::GetString() noexcept {
  return to_hstring((ValueType() == JSValueType::String) ? m_current->String() : "");
}

bool JSValueTreeReader::GetBoolean() noexcept {
  return (ValueType() == JSValueType::Boolean) ? m_current->Boolean() : false;
}

int64_t JSValueTreeReader::GetInt64() noexcept {
  return (ValueType() == JSValueType::Int64) ? m_current->Int64() : 0;
}

double JSValueTreeReader::GetDouble() noexcept {
  return (ValueType() == JSValueType::Double) ? m_current->Double() : 0;
}

IJSValueReader MakeJSValueTreeReader(const JSValue &root) noexcept {
  return make<JSValueTreeReader>(root);
}

IJSValueReader MakeJSValueTreeReader(JSValue &&root) noexcept {
  return make<JSValueTreeReader>(std::move(root));
}

} // namespace winrt::Microsoft::ReactNative
