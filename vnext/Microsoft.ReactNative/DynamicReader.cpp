// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DynamicReader.h"

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// DynamicReader implementation
//===========================================================================

/*static*/ DynamicReader::StackEntry DynamicReader::StackEntry::ObjectProperty(
    const folly::dynamic *value,
    const folly::dynamic::const_item_iterator &property) noexcept {
  StackEntry entry;
  entry.Value = value;
  entry.Property = property;
  return entry;
}

/*static*/ DynamicReader::StackEntry DynamicReader::StackEntry::ArrayItem(
    const folly::dynamic *value,
    const folly::dynamic::const_iterator &item) noexcept {
  StackEntry entry;
  entry.Value = value;
  entry.Item = item;
  return entry;
}

DynamicReader::DynamicReader(const folly::dynamic &root) noexcept : m_current{&root} {}

JSValueType DynamicReader::ValueType() noexcept {
  switch (m_current->type()) {
    case folly::dynamic::Type::NULLT:
      return JSValueType::Null;
    case folly::dynamic::Type::ARRAY:
      return JSValueType::Array;
    case folly::dynamic::Type::BOOL:
      return JSValueType::Boolean;
    case folly::dynamic::Type::DOUBLE:
      return JSValueType::Double;
    case folly::dynamic::Type::INT64:
      return JSValueType::Int64;
    case folly::dynamic::Type::OBJECT:
      return JSValueType::Object;
    case folly::dynamic::Type::STRING:
      return JSValueType::String;
    default:
      return JSValueType::Null;
  }
}

bool DynamicReader::GetNextObjectProperty(hstring &propertyName) noexcept {
  if (!m_isIterating) {
    if (m_current->type() == folly::dynamic::Type::OBJECT) {
      const auto &properties = m_current->items();
      const auto &property = properties.begin();
      if (property != properties.end()) {
        m_stack.push_back(StackEntry::ObjectProperty(m_current, property));
        SetCurrentValue(&(property->second));
        propertyName = to_hstring(property->first.asString());
        return true;
      } else {
        m_isIterating = !m_stack.empty();
      }
    }
  } else if (!m_stack.empty()) {
    auto &entry = m_stack.back();
    if (entry.Value->type() == folly::dynamic::Type::OBJECT) {
      auto &property = entry.Property.value();
      if (++property != entry.Value->items().end()) {
        SetCurrentValue(&(property->second));
        propertyName = to_hstring(property->first.asString());
        return true;
      } else {
        m_current = entry.Value;
        m_stack.pop_back();
        m_isIterating = !m_stack.empty();
      }
    }
  }

  propertyName = to_hstring(L"");
  return false;
}

bool DynamicReader::GetNextArrayItem() noexcept {
  if (!m_isIterating) {
    if (m_current->type() == folly::dynamic::Type::ARRAY) {
      const auto &item = m_current->begin();
      if (item != m_current->end()) {
        m_stack.push_back(StackEntry::ArrayItem(m_current, item));
        SetCurrentValue(&*item);
        return true;
      } else {
        m_isIterating = !m_stack.empty();
      }
    }
  } else if (!m_stack.empty()) {
    auto &entry = m_stack.back();
    if (entry.Value->type() == folly::dynamic::Type::ARRAY) {
      if (++entry.Item != entry.Value->end()) {
        SetCurrentValue(&*entry.Item);
        return true;
      } else {
        m_current = entry.Value;
        m_stack.pop_back();
        m_isIterating = !m_stack.empty();
      }
    }
  }

  return false;
}

void DynamicReader::SetCurrentValue(const folly::dynamic *value) noexcept {
  m_current = value;
  switch (value->type()) {
    case folly::dynamic::Type::OBJECT:
    case folly::dynamic::Type::ARRAY:
      m_isIterating = false;
      break;
    default:
      m_isIterating = true;
      break;
  }
}

hstring DynamicReader::GetString() noexcept {
  return to_hstring((m_current->type() == folly::dynamic::Type::STRING) ? m_current->getString() : "");
}

bool DynamicReader::GetBoolean() noexcept {
  return (m_current->type() == folly::dynamic::Type::BOOL) ? m_current->getBool() : false;
}

int64_t DynamicReader::GetInt64() noexcept {
  return (m_current->type() == folly::dynamic::Type::INT64) ? m_current->getInt() : 0;
}

double DynamicReader::GetDouble() noexcept {
  return (m_current->type() == folly::dynamic::Type::DOUBLE) ? m_current->getDouble() : 0;
}

} // namespace winrt::Microsoft::ReactNative
