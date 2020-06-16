// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DynamicWriter.h"
#include <crash/verifyElseCrash.h>

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// DynamicWriter implementation
//===========================================================================

folly::dynamic DynamicWriter::TakeValue() noexcept {
  return std::move(m_result);
}

void DynamicWriter::WriteNull() noexcept {
  WriteValue(folly::dynamic{});
}

void DynamicWriter::WriteBoolean(bool value) noexcept {
  WriteValue(folly::dynamic{value});
}

void DynamicWriter::WriteInt64(int64_t value) noexcept {
  WriteValue(folly::dynamic{value});
}

void DynamicWriter::WriteDouble(double value) noexcept {
  WriteValue(folly::dynamic{value});
}

void DynamicWriter::WriteString(const winrt::hstring &value) noexcept {
  WriteValue(folly::dynamic{winrt::to_string(value)});
}

void DynamicWriter::WriteObjectBegin() noexcept {
  if (m_state == State::PropertyValue) {
    m_stack.push_back(StackEntry{m_state, std::move(m_dynamic), std::move(m_propertyName)});
  } else if (m_state == State::Array) {
    m_stack.push_back(StackEntry{m_state, std::move(m_dynamic)});
  } else if (m_state != State::Start) {
    VerifyElseCrash(false);
  }

  m_dynamic = folly::dynamic::object();
  m_state = State::PropertyName;
}

void DynamicWriter::WritePropertyName(const winrt::hstring &name) noexcept {
  if (m_state == State::PropertyName) {
    m_propertyName = winrt::to_string(name);
    m_state = State::PropertyValue;
  } else {
    VerifyElseCrash(false);
  }
}

void DynamicWriter::WriteObjectEnd() noexcept {
  if (m_state == State::PropertyName) {
    if (m_stack.empty()) {
      m_result = std::move(m_dynamic);
      m_state = State::Finish;
      return;
    } else {
      StackEntry &entry = m_stack.back();
      if (entry.State == State::PropertyValue) {
        entry.Dynamic[std::move(entry.PropertyName)] = std::move(m_dynamic);
        m_dynamic = std::move(entry.Dynamic);
        m_state = State::PropertyName;
        m_stack.pop_back();
        return;
      } else if (entry.State == State::Array) {
        entry.Dynamic.push_back(std::move(m_dynamic));
        m_dynamic = std::move(entry.Dynamic);
        m_state = State::Array;
        m_stack.pop_back();
        return;
      }
    }
  }

  VerifyElseCrash(false);
}

void DynamicWriter::WriteArrayBegin() noexcept {
  if (m_state == State::PropertyValue) {
    m_stack.push_back(StackEntry{m_state, std::move(m_dynamic), std::move(m_propertyName)});
  } else if (m_state == State::Array) {
    m_stack.push_back(StackEntry{m_state, std::move(m_dynamic)});
  } else if (m_state != State::Start) {
    VerifyElseCrash(false);
  }

  m_dynamic = folly::dynamic::array();
  m_state = State::Array;
}

void DynamicWriter::WriteArrayEnd() noexcept {
  if (m_state == State::Array) {
    if (m_stack.empty()) {
      m_result = std::move(m_dynamic);
      m_state = State::Finish;
      return;
    } else {
      StackEntry &entry = m_stack.back();
      if (entry.State == State::PropertyValue) {
        entry.Dynamic[std::move(entry.PropertyName)] = std::move(m_dynamic);
        m_dynamic = std::move(entry.Dynamic);
        m_state = State::PropertyName;
        m_stack.pop_back();
        return;
      } else if (entry.State == State::Array) {
        entry.Dynamic.push_back(std::move(m_dynamic));
        m_dynamic = std::move(entry.Dynamic);
        m_state = State::Array;
        m_stack.pop_back();
        return;
      }
    }
  }

  VerifyElseCrash(false);
}

void DynamicWriter::WriteValue(folly::dynamic &&value) noexcept {
  if (m_state == State::PropertyValue) {
    m_dynamic[std::move(m_propertyName)] = std::move(value);
    m_state = State::PropertyName;
  } else if (m_state == State::Array) {
    m_dynamic.push_back(std::move(value));
  } else if (m_state == State::Start) {
    m_result = std::move(value);
    m_state = State::Finish;
  } else {
    VerifyElseCrash(false);
  }
}

/*static*/ folly::dynamic DynamicWriter::ToDynamic(JSValueArgWriter const &argWriter) noexcept {
  if (argWriter) {
    IJSValueWriter dynamicWriter = winrt::make<DynamicWriter>();
    argWriter(dynamicWriter);
    return dynamicWriter.as<DynamicWriter>()->TakeValue();
  }

  return {};
}

} // namespace winrt::Microsoft::ReactNative
