#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "jsi/jsi.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct JsiWriter : winrt::implements<JsiWriter, IJSValueWriter> {
  JsiWriter(facebook::jsi::Runtime &runtime) noexcept;
  facebook::jsi::Value MoveResult() noexcept;
  facebook::jsi::Value CopyResult() noexcept;

 public: // IJSValueWriter
  void WriteNull() noexcept;
  void WriteBoolean(bool value) noexcept;
  void WriteInt64(int64_t value) noexcept;
  void WriteDouble(double value) noexcept;
  void WriteString(const winrt::hstring &value) noexcept;
  void WriteObjectBegin() noexcept;
  void WritePropertyName(const winrt::hstring &name) noexcept;
  void WriteObjectEnd() noexcept;
  void WriteArrayBegin() noexcept;
  void WriteArrayEnd() noexcept;

 public:
  static facebook::jsi::Value ToJsiValue(facebook::jsi::Runtime &runtime, JSValueArgWriter const &argWriter) noexcept;

 private:
  enum class ContinuationAction {
    AcceptValueAndFinish,
    AcceptArrayElement,
    AcceptPropertyName,
    AcceptPropertyValue,
  };

  struct Continuation {
    ContinuationAction Action;
    std::vector<facebook::jsi::Value> Values;
    std::string PropertyName;

    Continuation(ContinuationAction action) noexcept : Action(action) {}
    Continuation(ContinuationAction action, facebook::jsi::Value &&value) noexcept : Action(action) {
      Values.push_back(std::move(value));
    }

    Continuation(const Continuation &) = delete;
    Continuation(Continuation &&) = default;
  };

  // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Number/MIN_SAFE_INTEGER
  static const int64_t MinSafeInteger = -9007199254740991L;
  static const int64_t MaxSafeInteger = 9007199254740991L;

 private:
  void WriteValue(facebook::jsi::Value &&value) noexcept;
  Continuation &Top() noexcept;
  Continuation Pop() noexcept;
  void Push(Continuation &&continuation) noexcept;

 private:
  facebook::jsi::Runtime &m_runtime;
  facebook::jsi::Value m_result;
  std::vector<Continuation> m_continuations;
};

} // namespace winrt::Microsoft::ReactNative
