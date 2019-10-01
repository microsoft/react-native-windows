// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DynamicWriter.h"

namespace winrt::Microsoft::ReactNative::Bridge {

//===========================================================================
// DynamicWriter implementation
//===========================================================================

folly::dynamic DynamicWriter::TakeValue() noexcept {
  return std::move(m_result);
}

bool DynamicWriter::WriteNull() noexcept {
  return WriteValue(folly::dynamic{});
}

bool DynamicWriter::WriteBoolean(bool value) noexcept {
  return WriteValue(folly::dynamic{value});
}

bool DynamicWriter::WriteInt64(int64_t value) noexcept {
  return WriteValue(folly::dynamic{value});
}

bool DynamicWriter::WriteDouble(double value) noexcept {
  return WriteValue(folly::dynamic{value});
}

bool DynamicWriter::WriteString(const winrt::hstring &value) noexcept {
  return WriteValue(folly::dynamic{winrt::to_string(value)});
}

bool DynamicWriter::WriteObjectBegin() noexcept {
  if (m_state == State::PropertyValue) {
    m_stack.push_back(
        StackEntry{m_state, std::move(m_dynamic), std::move(m_propertyName)});
  } else if (m_state == State::Array) {
    m_stack.push_back(StackEntry{m_state, std::move(m_dynamic)});
  } else if (m_state != State::Start) {
    return false;
  }

  m_dynamic = folly::dynamic::object();
  m_state = State::PropertyName;

  return true;
}

bool DynamicWriter::WritePropertyName(const winrt::hstring &name) noexcept {
  if (m_state == State::PropertyName) {
    m_propertyName = winrt::to_string(name);
    m_state = State::PropertyValue;
    return true;
  }

  return false;
}

bool DynamicWriter::WriteObjectEnd() noexcept {
  if (m_state == State::PropertyName) {
    if (m_stack.empty()) {
      m_result = std::move(m_dynamic);
      m_state = State::Finish;
    } else {
      StackEntry &entry = m_stack.back();
      if (entry.State == State::PropertyValue) {
        entry.Dynamic[std::move(entry.PropertyName)] = std::move(m_dynamic);
        m_dynamic = std::move(entry.Dynamic);
        m_state = State::PropertyName;
        m_stack.pop_back();
        return true;
      } else if (entry.State == State::Array) {
        entry.Dynamic.push_back(std::move(m_dynamic));
        m_dynamic = std::move(entry.Dynamic);
        m_state = State::Array;
        m_stack.pop_back();
        return true;
      }
    }
  }

  return false;
}

bool DynamicWriter::WriteArrayBegin() noexcept {
  if (m_state == State::PropertyValue) {
    m_stack.push_back(
        StackEntry{m_state, std::move(m_dynamic), std::move(m_propertyName)});
  } else if (m_state == State::Array) {
    m_stack.push_back(StackEntry{m_state, std::move(m_dynamic)});
  } else if (m_state != State::Start) {
    return false;
  }

  m_dynamic = folly::dynamic::array();
  m_state = State::Array;
  return true;
}

bool DynamicWriter::WriteArrayEnd() noexcept {
  if (m_state == State::Array) {
    if (m_stack.empty()) {
      m_result = std::move(m_dynamic);
      m_state = State::Finish;
    } else {
      StackEntry &entry = m_stack.back();
      if (entry.State == State::PropertyValue) {
        entry.Dynamic[std::move(entry.PropertyName)] = std::move(m_dynamic);
        m_dynamic = std::move(entry.Dynamic);
        m_state = State::PropertyName;
        m_stack.pop_back();
        return true;
      } else if (entry.State == State::Array) {
        entry.Dynamic.push_back(std::move(m_dynamic));
        m_dynamic = std::move(entry.Dynamic);
        m_state = State::Array;
        m_stack.pop_back();
        return true;
      }
    }
  }

  return false;
}

bool DynamicWriter::WriteValue(folly::dynamic &&value) noexcept {
  if (m_state == State::PropertyValue) {
    m_dynamic[std::move(m_propertyName)] = std::move(value);
    m_state = State::PropertyName;
    return true;
  } else if (m_state == State::Array) {
    m_dynamic.push_back(std::move(value));
    return true;
  }

  return false;
}

} // namespace winrt::Microsoft::ReactNative::Bridge
