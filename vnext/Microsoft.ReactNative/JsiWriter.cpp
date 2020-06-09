// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiWriter.h"
#include <crash/verifyElseCrash.h>

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// JsiWriter implementation
//===========================================================================

JsiWriter::JsiWriter(facebook::jsi::Runtime &runtime) noexcept : m_runtime(runtime) {
  Push({ContainerState::AcceptValueAndFinish});
}

facebook::jsi::Value JsiWriter::MoveResult() noexcept {
  VerifyElseCrash(m_containers.size() == 0);
  return std::move(m_result);
}

void JsiWriter::WriteNull() noexcept {
  WriteValue(facebook::jsi::Value::null());
}

void JsiWriter::WriteBoolean(bool value) noexcept {
  WriteValue({value});
}

void JsiWriter::WriteInt64(int64_t value) noexcept {
  WriteValue({static_cast<double>(value)});
}

void JsiWriter::WriteDouble(double value) noexcept {
  WriteValue({value});
}

void JsiWriter::WriteString(const winrt::hstring &value) noexcept {
  WriteValue({m_runtime, facebook::jsi::String::createFromUtf8(m_runtime, winrt::to_string(value))});
}

void JsiWriter::WriteObjectBegin() noexcept {
  // legal to create an object when it is accepting a value
  VerifyElseCrash(Top().State != ContainerState::AcceptPropertyName);
  Push({ContainerState::AcceptPropertyName, facebook::jsi::Object(m_runtime)});
}

void JsiWriter::WritePropertyName(const winrt::hstring &name) noexcept {
  // legal to set a property name only when AcceptPropertyName
  auto &top = Top();
  VerifyElseCrash(top.State == ContainerState::AcceptPropertyName);
  top.State = ContainerState::AcceptPropertyValue;
  top.PropertyName = winrt::to_string(name);
}

void JsiWriter::WriteObjectEnd() noexcept {
  // legal to finish an object only when AcceptPropertyName
  VerifyElseCrash(Top().State == ContainerState::AcceptPropertyName);
  WriteValue(Pop().CurrentObject.value());
}

void JsiWriter::WriteArrayBegin() noexcept {
  // legal to create an array only when it is accepting a value
  VerifyElseCrash(Top().State != ContainerState::AcceptPropertyName);
  Push({ContainerState::AcceptArrayElement});
}

void JsiWriter::WriteArrayEnd() noexcept {
  // legal to finish an array only when AcceptArrayElement
  auto &top = Top();
  VerifyElseCrash(top.State == ContainerState::AcceptArrayElement);

  facebook::jsi::Array createdArray(m_runtime, top.CurrentArrayElements.size());
  for (size_t i = 0; i < top.CurrentArrayElements.size(); i++) {
    createdArray.setValueAtIndex(m_runtime, i, std::move(top.CurrentArrayElements.at(i)));
  }
  Pop();
  WriteValue({m_runtime, createdArray});
}

void JsiWriter::WriteValue(facebook::jsi::Value &&value) noexcept {
  auto &top = Top();
  switch (top.State) {
    case ContainerState::AcceptValueAndFinish: {
      m_result = std::move(value);
      Pop();
      VerifyElseCrash(m_containers.size() == 0);
      break;
    }
    case ContainerState::AcceptArrayElement: {
      top.CurrentArrayElements.push_back(std::move(value));
      break;
    }
    case ContainerState::AcceptPropertyValue: {
      auto &createdObject = top.CurrentObject.value();
      createdObject.setProperty(m_runtime, top.PropertyName.c_str(), std::move(value));
      top.State = ContainerState::AcceptPropertyName;
      top.PropertyName = {};
      break;
    }
    default:
      VerifyElseCrash(false);
  }
}

JsiWriter::Container &JsiWriter::Top() noexcept {
  VerifyElseCrash(m_containers.size() > 0);
  return m_containers[m_containers.size() - 1];
}

JsiWriter::Container JsiWriter::Pop() noexcept {
  auto top = std::move(Top());
  m_containers.pop_back();
  return top;
}

void JsiWriter::Push(Container &&container) noexcept {
  m_containers.push_back(std::move(container));
}

/*static*/ facebook::jsi::Value JsiWriter::ToJsiValue(
    facebook::jsi::Runtime &runtime,
    JSValueArgWriter const &argWriter) noexcept {
  if (argWriter) {
    IJSValueWriter jsiWriter = winrt::make<JsiWriter>(runtime);
    argWriter(jsiWriter);
    return jsiWriter.as<JsiWriter>()->MoveResult();
  }

  return {};
}

} // namespace winrt::Microsoft::ReactNative
