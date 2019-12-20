// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Terms used in here:
//   Container: represents a [] or {}
//   isStringContent: is the current value a string? Either a name or quoted value

#include "pch.h"
#include "JsonReader.h"
#include "Crash.h"

namespace winrt::Microsoft::ReactNative {

static bool IsSimpleWhitespace(wchar_t c) noexcept {
  return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

// Main processing loop: reads tokens until end of stream, error, or
// a valid state transition is reached.
JsonParseState JsonReader::ReadNext() noexcept {
  for (;; m_source.Inc()) {
    if (m_source.IsEndOfData())
      return JsonParseState::ErrorEndOfData;

    wchar_t ch = m_source.PeekChar();

    if (m_isAllowed.Option.StringChar) {
      if (m_isAllowed.Option.EscapeChar) {
        if (HandleEscapeChar(ch))
          continue;
        return HandleInvalidData();
      }

      if (m_isAllowed.Option.EscapeCharHex) {
        if (HandleEscapeCharHex(ch))
          continue;
        return HandleInvalidData();
      }

      if (ch == '\\') {
        m_isAllowed.Option.EscapeChar = 1;
        continue;
      }

      if (ch == '\"') {
        if (HandleEndName()) {
          m_source.Inc();
          return JsonParseState::Name;
        }

        if (HandleEndContainerOrValue(JsonParseState::Value)) {
          m_source.Inc();
          return JsonParseState::Value;
        }

        VerifyElseCrashSz(false, "This should be impossible to hit");
      }

      // Regular string character
      m_textBuffer += ch;
      continue;
    }

    // All whitespace outside of strings is ignored
    if (IsSimpleWhitespace(ch))
      continue;

    switch (ch) {
      case '[':
        return HandleStartContainer(JsonParseState::StartArray);
      case ']':
        return HandleEndContainer(JsonParseState::StartArray, JsonParseState::EndArray);
      case '{':
        return HandleStartContainer(JsonParseState::StartObject);
      case '}':
        return HandleEndContainer(JsonParseState::StartObject, JsonParseState::EndObject);

      case ',': {
        if (m_isAllowed.Option.EndComma) {
          if (HandleEndContainerOrValue(JsonParseState::Value))
            return JsonParseState::Value;

          ResetContainerState();
          continue;
        }
        break;
      }

      case '"': {
        if (HandleBeginString())
          continue;
        break;
      }

      case ':': {
        if (m_isAllowed.Option.NameDelim) {
          m_isAllowed.All = 0;
          OnValueExpected();
          continue;
        }
        break;
      }

      default: {
        // Regular non-string character
        if (HandleNonStringChar(ch))
          continue;

        break;
      }
    }

    return HandleInvalidData();
  }
}

// Called when a new Container is started
// Called after each item in a Container
void JsonReader::ResetContainerState() noexcept {
  m_isAllowed.All = 0;

  if (m_states.top() == JsonParseState::StartArray) {
    OnValueExpected();
  } else {
    m_isAllowed.Option.BeginName = 1;
    m_isStringContent = false;
    m_textBuffer.clear();
  }
}

// Called after '[', ':', ','
void JsonReader::OnValueExpected() noexcept {
  m_isAllowed.Option.StartContainer = 1;
  m_isAllowed.Option.BeginValue = 1;
  m_isStringContent = false;
  m_textBuffer.clear();
}

// Handle the start of a new Container: [ {
JsonParseState JsonReader::HandleStartContainer(JsonParseState state) noexcept {
  if (!m_isAllowed.Option.StartContainer)
    return HandleInvalidData();

  m_states.push(state);
  ResetContainerState();
  m_isAllowed.Option.EndContainer = 1;

  m_source.Inc();
  return state;
}

// Handle the end of a Container: } ]
JsonParseState JsonReader::HandleEndContainer(JsonParseState oldState, JsonParseState newState) noexcept {
  if (!m_isAllowed.Option.EndContainer)
    return HandleInvalidData();

  // Check for previously ended value first
  if (HandleEndContainerOrValue(JsonParseState::Value))
    return JsonParseState::Value;

  if (HandleEndContainerOrValue(oldState)) {
    m_isStringContent = false;
    m_textBuffer.clear();

    m_source.Inc();
    return newState;
  }

  return HandleInvalidData();
}

// Handle starting a new string: "
bool JsonReader::HandleBeginString() noexcept {
  if (m_isAllowed.Option.BeginName || m_isAllowed.Option.BeginValue) {
    m_states.push(m_isAllowed.Option.BeginName ? JsonParseState::Name : JsonParseState::Value);

    m_isAllowed.All = 0;
    m_isAllowed.Option.StringChar = 1;
    m_isStringContent = true;
    return true;
  }

  return false;
}

// Handle value characters: numbers, true, false, null
bool JsonReader::HandleNonStringChar(wchar_t ch) noexcept {
  // FUTURE: could add more validation here

  if (m_isAllowed.Option.NonStringChar) {
    m_textBuffer += ch;
    return true;
  }

  if (m_isAllowed.Option.BeginValue) {
    m_isAllowed.All = 0;
    m_isAllowed.Option.NonStringChar = 1;
    m_isAllowed.Option.EndComma = 1;
    m_isAllowed.Option.EndContainer = 1;
    m_states.push(JsonParseState::Value);
    m_textBuffer.clear();

    m_textBuffer += ch;
    return true;
  }

  return false;
}

// Handle an escaped string character
bool JsonReader::HandleEscapeChar(wchar_t ch) noexcept {
  switch (ch) {
    case '\"':
    case '\\':
    case '/':
      break;
    case 'b':
      ch = '\b';
      break;
    case 'f':
      ch = '\f';
      break;
    case 'n':
      ch = '\n';
      break;
    case 'r':
      ch = '\r';
      break;
    case 't':
      ch = '\t';
      break;
    case 'v':
      ch = '\v';
      break;

    case 'u':
      // Switch to hex-mode
      m_isAllowed.Option.EscapeChar = 0;
      m_isAllowed.Option.EscapeCharHex = 1;
      m_hexStartIndex = m_textBuffer.length();
      return true;

    default:
      return false;
  }

  m_textBuffer += ch;

  m_isAllowed.Option.EscapeChar = 0;
  return true;
}

// Appends the current char into the buffer
// If all hex characters have been collected, try converting to wchar
bool JsonReader::HandleEscapeCharHex(wchar_t ch) noexcept {
  constexpr size_t HexCharCount = 4;

  m_textBuffer += ch;
  if (m_textBuffer.length() < m_hexStartIndex + HexCharCount)
    return true;

  const wchar_t *hexStart = m_textBuffer.c_str() + m_hexStartIndex;
  wchar_t *hexEnd = nullptr;
  ch = static_cast<wchar_t>(wcstoul(hexStart, &hexEnd, 16 /*base*/));

  if (hexStart + HexCharCount != hexEnd)
    return false;

  m_textBuffer.resize(m_hexStartIndex);
  m_textBuffer += ch;

  m_isAllowed.Option.EscapeCharHex = 0;
  return true;
}

// Helper for handling the end of a Container or value: , ] }
bool JsonReader::HandleEndContainerOrValue(JsonParseState state) noexcept {
  if (m_states.top() == state) {
    m_states.pop();

    m_isAllowed.All = 0;
    if (m_states.size() > 0) {
      m_isAllowed.Option.EndComma = 1;
      m_isAllowed.Option.EndContainer = 1;
    }

    return true;
  }

  return false;
}

// Handle the end of a name: "
bool JsonReader::HandleEndName() noexcept {
  if (m_states.top() == JsonParseState::Name) {
    m_states.pop();
    m_isAllowed.All = 0;
    m_isAllowed.Option.NameDelim = 1;
    return true;
  }

  return false;
}

// Return the current string literal
bool JsonReader::GetString(const wchar_t **value, size_t *valueLength) noexcept {
  if (m_isStringContent) {
    *value = m_textBuffer.c_str();
    *valueLength = m_textBuffer.length();
    return true;
  }

  *value = nullptr;
  *valueLength = 0;
  return false;
}

// Return the current value as a bool
bool JsonReader::GetBool(bool *value) noexcept {
  if (!m_isStringContent) {
    if (m_textBuffer.compare(L"true") == 0) {
      *value = true;
      return true;
    }

    if (m_textBuffer.compare(L"false") == 0) {
      *value = false;
      return true;
    }
  }

  *value = false;
  return false;
}

// Return the current value as an Int64
bool JsonReader::GetInt64(int64_t *value) noexcept {
  if (!m_isStringContent && m_textBuffer.length()) {
    wchar_t *end = nullptr;
    auto iValue = _wcstoi64(m_textBuffer.c_str(), &end, 10 /*base*/);
    if (end == m_textBuffer.c_str() + m_textBuffer.size()) {
      *value = iValue;
      return true;
    }

    // Didn't read enough characters, fall through and fail
  }

  *value = 0;
  return false;
}

// Return the current value as a Double
bool JsonReader::GetDouble(double *value) noexcept {
  if (!m_isStringContent && m_textBuffer.length()) {
    wchar_t *end = nullptr;
    auto dvalue = wcstod(m_textBuffer.c_str(), &end);
    if (end == m_textBuffer.c_str() + m_textBuffer.size()) {
      *value = dvalue;
      return true;
    }

    // Didn't read enough characters, fall through and fail
  }

  *value = 0;
  return false;
}

// Return true if the current value is the null value
bool JsonReader::IsNull() noexcept {
  if (!m_isStringContent) {
    return (m_textBuffer.compare(L"null") == 0);
  }

  return false;
}

} // namespace winrt::Microsoft::ReactNative
