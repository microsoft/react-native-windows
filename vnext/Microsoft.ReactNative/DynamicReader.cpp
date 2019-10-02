// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DynamicReader.h"
#include "Crash.h"

namespace winrt::Microsoft::ReactNative::Bridge {

//===========================================================================
// DynamicReader implementation
//===========================================================================

DynamicReader::DynamicReader(const folly::dynamic &root) noexcept
    : m_root{&root} {}

JSValueReaderState DynamicReader::ReadNext() noexcept {
  switch (m_state) {
    case JSValueReaderState::Error: {
      // The Error state is the initial state if m_root is not null.
      if (!m_stack.empty() && !m_root) {
        return JSValueReaderState::Error;
      }

      folly::dynamic::Type type = m_root->type();
      if (type == folly::dynamic::Type::OBJECT) {
        m_stack.emplace_back(m_root);
        return m_state = JSValueReaderState::ObjectBegin;
      } else if (type == folly::dynamic::Type::ARRAY) {
        m_stack.emplace_back(m_root);
        return m_state = JSValueReaderState::ArrayBegin;
      } else {
        return JSValueReaderState::Error;
      }
    }
    case JSValueReaderState::ObjectBegin: {
      auto &entry = m_stack.back();
      const auto &obj = entry.Value->items();
      entry.Property = obj.begin();
      if (entry.Property != obj.end()) {
        return m_state = JSValueReaderState::PropertyName;
      } else {
        return m_state = JSValueReaderState::ObjectEnd;
      }
    }
    case JSValueReaderState::ArrayBegin: {
      VerifyElseCrash(!m_stack.empty());
      auto &entry = m_stack.back();
      const auto &arr = *entry.Value;
      entry.Item = arr.begin();
      if (entry.Item != arr.end()) {
        return ReadValue(&*entry.Item);
      } else {
        return m_state = JSValueReaderState::ArrayEnd;
      }
    }
    case JSValueReaderState::PropertyName: {
      VerifyElseCrash(!m_stack.empty());
      auto &entry = m_stack.back();
      const auto &obj = entry.Value->items();
      if (entry.Property != obj.end()) {
        return ReadValue(&(entry.Property.value()->second));
      } else {
        return m_state = JSValueReaderState::Error;
      }
    }
    case JSValueReaderState::ObjectEnd:
    case JSValueReaderState::ArrayEnd:
    case JSValueReaderState::NullValue:
    case JSValueReaderState::BooleanValue:
    case JSValueReaderState::Int64Value:
    case JSValueReaderState::DoubleValue:
    case JSValueReaderState::StringValue: {
      VerifyElseCrash(!m_stack.empty());
      m_stack.pop_back();
      if (m_stack.empty()) {
        return m_state = JSValueReaderState::Error;
      }

      auto &entry = m_stack.back();
      folly::dynamic::Type type = entry.Value->type();
      if (type == folly::dynamic::OBJECT) {
        auto objIter = entry.Property.value();
        entry.Property = ++objIter;
        if (objIter != entry.Value->items().end()) {
          return m_state = JSValueReaderState::PropertyName;
        } else {
          return m_state = JSValueReaderState::ObjectEnd;
        }
      } else if (type == folly::dynamic::ARRAY) {
        if (++entry.Item != entry.Value->end()) {
          return ReadValue(&*entry.Item);
        } else {
          return m_state = JSValueReaderState::ArrayEnd;
        }
      }
    }
    default:
      return m_state = JSValueReaderState::Error;
  }
}

JSValueReaderState DynamicReader::ReadValue(
    const folly::dynamic *value) noexcept {
  m_stack.emplace_back(value);
  switch (value->type()) {
    case folly::dynamic::Type::NULLT:
      return m_state = JSValueReaderState::NullValue;
    case folly::dynamic::Type::BOOL:
      return m_state = JSValueReaderState::BooleanValue;
    case folly::dynamic::Type::INT64:
      return m_state = JSValueReaderState::Int64Value;
    case folly::dynamic::Type::DOUBLE:
      return m_state = JSValueReaderState::DoubleValue;
    case folly::dynamic::Type::STRING:
      return m_state = JSValueReaderState::StringValue;
    case folly::dynamic::Type::OBJECT:
      return m_state = JSValueReaderState::ObjectBegin;
    case folly::dynamic::Type::ARRAY:
      return m_state = JSValueReaderState::ArrayBegin;
    default:
      return m_state = JSValueReaderState::Error;
  }
}

_Success_(return ) bool DynamicReader::TryGetBoolen(
    _Out_ bool &value) noexcept {
  if (m_state == JSValueReaderState::BooleanValue) {
    VerifyElseCrash(!m_stack.empty());
    value = m_stack.back().Value->asBool();
    return true;
  }

  value = false;
  return false;
}

_Success_(return ) bool DynamicReader::TryGetInt64(
    _Out_ int64_t &value) noexcept {
  if (m_state == JSValueReaderState::Int64Value) {
    VerifyElseCrash(!m_stack.empty());
    value = m_stack.back().Value->asInt();
    return true;
  }
  else if (m_state == JSValueReaderState::DoubleValue) {
    VerifyElseCrash(!m_stack.empty());
    value = static_cast<int64_t>(m_stack.back().Value->asDouble());
    return true;
  }

  value = 0;
  return false;
}

_Success_(return ) bool DynamicReader::TryGetDouble(
    _Out_ double &value) noexcept {
  if (m_state == JSValueReaderState::DoubleValue) {
    VerifyElseCrash(!m_stack.empty());
    value = m_stack.back().Value->asDouble();
    return true;
  }
  else if (m_state == JSValueReaderState::Int64Value) {
    VerifyElseCrash(!m_stack.empty());
    value = static_cast<double>(m_stack.back().Value->asInt());
    return true;
  }


  value = 0;
  return false;
}

_Success_(return ) bool DynamicReader::TryGetString(
    winrt::hstring &value) noexcept {
  if (m_state == JSValueReaderState::StringValue) {
    VerifyElseCrash(!m_stack.empty());
    const std::string &str = m_stack.back().Value->asString();
    value = winrt::to_hstring(str);
    return true;
  } else if (m_state == JSValueReaderState::PropertyName) {
    VerifyElseCrash(!m_stack.empty());
    const std::string &str = m_stack.back().Property.value()->first.getString();
    value = winrt::to_hstring(str);
    return true;
  }

  value = L"";
  return false;
}

} // namespace winrt::Microsoft::ReactNative::Bridge
