// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Simple JsonReader

#include <cstdint>
#include <stack>
#include <string_view>

namespace winrt::Microsoft::ReactNative {

struct JsonSource {
  static_assert(sizeof(wchar_t) == 2, "This code expects 2-byte wchars");

  JsonSource(std::wstring_view buffer) noexcept : m_current{buffer.data()}, m_end{buffer.data() + buffer.size()} {}

  // Caller is responsible for checking IsEndOfData() before calling PeekChar()
  wchar_t PeekChar() const noexcept {
    return *m_current;
  }

  // Caller is responsible for checking IsEndOfData() before calling Inc()
  void Inc() noexcept {
    ++m_current;
  }

  bool IsEndOfData() const noexcept {
    return m_current == m_end;
  }

 private:
  const wchar_t *m_current = nullptr;
  const wchar_t *m_end = nullptr;
};

enum class JsonParseState {
  StartArray,
  StartObject,
  Name,
  Value,
  EndObject,
  EndArray,

  // Returned for invalid data
  ErrorInvalidData,

  // End of input reached before end of Json.
  ErrorEndOfData,
};

struct JsonReader {
 public:
  JsonReader(JsonSource source) : m_source{source} {
    m_isAllowed.All = 0;
    m_isAllowed.Option.StartContainer = 1;
  }

  JsonReader(const JsonReader &) = delete;
  JsonReader &operator=(const JsonReader &) = delete;

  JsonParseState ReadNext() noexcept;
  bool GetString(_Deref_post_count_(*valueLength) const wchar_t **value, _Out_ size_t *valueLength) noexcept;
  bool GetBool(_Out_ bool *value) noexcept;
  bool GetInt64(_Out_ std::int64_t *value) noexcept;
  bool GetDouble(_Out_ double *value) noexcept;
  bool IsNull() noexcept;
  bool IsString() noexcept {
    return m_isStringContent;
  }

 private:
  void ResetContainerState() noexcept;
  void OnValueExpected() noexcept;

  JsonParseState HandleStartContainer(JsonParseState state) noexcept;
  JsonParseState HandleEndContainer(JsonParseState oldState, JsonParseState newState) noexcept;

  bool HandleBeginString() noexcept;
  bool HandleNonStringChar(wchar_t wch) noexcept;
  bool HandleEscapeChar(wchar_t wch) noexcept;
  bool HandleEscapeCharHex(wchar_t wch) noexcept;
  bool HandleEndContainerOrValue(JsonParseState state) noexcept;
  bool HandleEndName() noexcept;

  JsonParseState HandleInvalidData() noexcept {
    m_isAllowed.All = 0;
    return JsonParseState::ErrorInvalidData;
  }

 private:
  JsonSource m_source;

  // Tracks the next allowed state(s)
  union {
    struct {
      // [ or {
      uint32_t StartContainer : 1;

      // "
      uint32_t BeginName : 1;

      // :
      uint32_t NameDelim : 1;

      // " or value char
      uint32_t BeginValue : 1;

      // valid name or value char
      uint32_t NonStringChar : 1;

      // valid string character
      uint32_t StringChar : 1;

      // valid escape char
      uint32_t EscapeChar : 1;

      // 4 hex digits
      uint32_t EscapeCharHex : 1;

      // ] or }
      uint32_t EndContainer : 1;

      // ,
      uint32_t EndComma : 1;
    } Option;

    uint32_t All;

  } m_isAllowed;

  // Stores the current name / value text
  std::wstring m_textBuffer;

  // Stack of parse states (Array, Object, Name, Value)
  std::stack<JsonParseState> m_states;

  // Used when decoding \uHHHH values
  size_t m_hexStartIndex = 0;

  // True when m_textBuffer contains a name or value string
  bool m_isStringContent = false;
};

} // namespace winrt::Microsoft::ReactNative
