#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "jsi/jsi.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct JsiWriter : winrt::implements<JsiWriter, IJSValueWriter> {
  JsiWriter(facebook::jsi::Runtime &runtime) noexcept;
  facebook::jsi::Value MoveResult() noexcept;

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
  enum class ContainerState {
    AcceptValueAndFinish,
    AcceptArrayElement,
    AcceptPropertyName,
    AcceptPropertyValue,
  };

  struct Container {
    ContainerState State;
    std::optional<facebook::jsi::Object> CurrentObject;
    std::vector<facebook::jsi::Value> CurrentArrayElements;
    std::string PropertyName;

    Container(ContainerState state) noexcept : State(state) {}
    Container(ContainerState state, facebook::jsi::Object &&value) noexcept
        : State(state), CurrentObject(std::move(value)) {}

    Container(const Container &) = delete;
    Container(Container &&) = default;
  };

 private:
  void WriteValue(facebook::jsi::Value &&value) noexcept;
  Container &Top() noexcept;
  Container Pop() noexcept;
  void Push(Container &&container) noexcept;

 private:
  facebook::jsi::Runtime &m_runtime;
  facebook::jsi::Value m_result;
  std::vector<Container> m_containers;
};

} // namespace winrt::Microsoft::ReactNative
