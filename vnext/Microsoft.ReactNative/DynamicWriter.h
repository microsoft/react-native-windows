#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "folly/dynamic.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct DynamicWriter : winrt::implements<DynamicWriter, IJSValueWriter> {
  folly::dynamic TakeValue() noexcept;

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
  static folly::dynamic ToDynamic(JSValueArgWriter const &argWriter) noexcept;

 private:
  enum struct State { Start, PropertyName, PropertyValue, Array, Finish };

  struct StackEntry {
    StackEntry(State state, folly::dynamic &&object, std::string &&propertyName) noexcept
        : State{state}, Dynamic{std::move(object)}, PropertyName{std::move(propertyName)} {}

    StackEntry(State state, folly::dynamic &&array) noexcept : State{state}, Dynamic(std::move(array)) {}

    State State;
    folly::dynamic Dynamic;
    std::string PropertyName;
  };

 private:
  void WriteValue(folly::dynamic &&value) noexcept;

 private:
  State m_state{State::Start};
  std::vector<StackEntry> m_stack;
  folly::dynamic m_dynamic;
  std::string m_propertyName;
  folly::dynamic m_result;
};

} // namespace winrt::Microsoft::ReactNative
