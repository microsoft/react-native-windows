#pragma once

#include "folly/dynamic.h"
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace Microsoft::ReactNative {

struct DynamicWriter
    : winrt::implements<
          DynamicWriter,
          winrt::Microsoft::ReactNative::Bridge::IJSValueWriter> {

  folly::dynamic TakeValue() noexcept;

 public: // IJSValueWriter
  bool WriteNull() noexcept;
  bool WriteBoolean(bool value) noexcept;
  bool WriteInt64(int64_t value) noexcept;
  bool WriteDouble(double value) noexcept;
  bool WriteString(const winrt::hstring &value) noexcept;
  bool WriteObjectBegin() noexcept;
  bool WritePropertyName(const winrt::hstring &name) noexcept;
  bool WriteObjectEnd() noexcept;
  bool WriteArrayBegin() noexcept;
  bool WriteArrayEnd() noexcept;

 private:
  enum struct State { Start, PropertyName, PropertyValue, Array, Finish };

  struct StackEntry {
    StackEntry(
        State state,
        folly::dynamic &&object,
        std::string &&propertyName) noexcept
        : State{state},
          Dynamic{std::move(object)},
          PropertyName{std::move(propertyName)} {}

    StackEntry(State state, folly::dynamic &&array) noexcept
        : State{state}, Dynamic(std::move(array)) {}

    State State;
    folly::dynamic Dynamic;
    std::string PropertyName;
  };

 private:
  bool WriteValue(folly::dynamic &&value) noexcept;

 private:
  State m_state{State::Start};
  std::vector<StackEntry> m_stack;
  folly::dynamic m_dynamic;
  std::string m_propertyName;
  folly::dynamic m_result;
};

} // namespace Microsoft::ReactNative
