// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "JsonJSValueReader.h"

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// JsonJSValueReader implementation
//===========================================================================

JsonJSValueReader::JsonJSValueReader(std::wstring &&jsonText) noexcept
    : m_jsonText{std::move(jsonText)}, m_jsonReader{JsonSource{m_jsonText}} {
  SetCurrentValue(m_jsonReader.ReadNext());
}

JSValueType JsonJSValueReader::ValueType() noexcept {
  return m_valueType;
}

bool JsonJSValueReader::GetNextObjectProperty(hstring &propertyName) noexcept {
  if (!m_isIterating) {
    if (m_valueType == JSValueType::Object) {
      if (m_jsonReader.ReadNext() == JsonParseState::Name) {
        m_stack.push_back(JSValueType::Object);
        propertyName = GetHString();
        SetCurrentValue(m_jsonReader.ReadNext());
        return true;
      } else {
        m_isIterating = !m_stack.empty();
      }
    }
  } else if (!m_stack.empty()) {
    JSValueType valueType = m_stack.back();
    if (valueType == JSValueType::Object) {
      if (m_jsonReader.ReadNext() == JsonParseState::Name) {
        propertyName = GetHString();
        SetCurrentValue(m_jsonReader.ReadNext());
        return true;
      } else {
        m_valueType = valueType;
        m_stack.pop_back();
        m_isIterating = !m_stack.empty();
      }
    }
  }

  propertyName = to_hstring(L"");
  return false;
}

bool JsonJSValueReader::GetNextArrayItem() noexcept {
  if (!m_isIterating) {
    if (m_valueType == JSValueType::Array) {
      JsonParseState parseState = m_jsonReader.ReadNext();
      if (IsContainerOrValue(parseState)) {
        m_stack.push_back(JSValueType::Array);
        SetCurrentValue(parseState);
        return true;
      } else {
        m_isIterating = !m_stack.empty();
      }
    }
  } else if (!m_stack.empty()) {
    JSValueType valueType = m_stack.back();
    if (valueType == JSValueType::Array) {
      JsonParseState parseState = m_jsonReader.ReadNext();
      if (IsContainerOrValue(parseState)) {
        SetCurrentValue(parseState);
        return true;
      } else {
        m_valueType = valueType;
        m_stack.pop_back();
        m_isIterating = !m_stack.empty();
      }
    }
  }

  return false;
}

void JsonJSValueReader::SetCurrentValue(JsonParseState parseState) noexcept {
  switch (parseState) {
    case JsonParseState::StartObject:
      m_valueType = JSValueType::Object;
      m_isIterating = false;
      break;

    case JsonParseState::StartArray:
      m_valueType = JSValueType::Array;
      m_isIterating = false;
      break;

    case JsonParseState::Value: {
      m_isIterating = true;

      if (m_jsonReader.IsString()) {
        m_valueType = JSValueType::String;
        m_stringValue = GetHString();
        break;
      }

      if (m_jsonReader.GetInt64(&m_int64Value)) {
        m_valueType = JSValueType::Int64;
        break;
      }

      if (m_jsonReader.GetDouble(&m_doubleValue)) {
        m_valueType = JSValueType::Double;
        break;
      }

      if (m_jsonReader.GetBool(&m_boolValue)) {
        m_valueType = JSValueType::Boolean;
        break;
      }

      m_valueType = JSValueType::Null;
      break;
    }

    default:
      m_valueType = JSValueType::Null;
      break;
  }
}

hstring JsonJSValueReader::GetString() noexcept {
  return (m_valueType == JSValueType::String) ? m_stringValue : hstring(L"");
}

bool JsonJSValueReader::GetBoolean() noexcept {
  return (m_valueType == JSValueType::Boolean) ? m_boolValue : false;
}

int64_t JsonJSValueReader::GetInt64() noexcept {
  return (m_valueType == JSValueType::Int64) ? m_int64Value : 0;
}

double JsonJSValueReader::GetDouble() noexcept {
  return (m_valueType == JSValueType::Double) ? m_doubleValue : 0;
}

hstring JsonJSValueReader::GetHString() noexcept {
  const wchar_t *data;
  size_t length;
  if (m_jsonReader.GetString(&data, &length)) {
    return hstring(data, static_cast<hstring::size_type>(length));
  } else {
    return hstring(L"");
  }
}

bool JsonJSValueReader::IsContainerOrValue(JsonParseState parseState) noexcept {
  return parseState == JsonParseState::StartArray || parseState == JsonParseState::StartObject ||
      parseState == JsonParseState::Value;
}

} // namespace winrt::Microsoft::ReactNative
