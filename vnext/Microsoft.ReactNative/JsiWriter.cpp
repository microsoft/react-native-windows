// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiWriter.h"
#include <crash/verifyElseCrash.h>

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// JsiWriter implementation
//===========================================================================

JsiWriter::JsiWriter(facebook::jsi::Runtime &runtime) noexcept : m_runtime(runtime) {
  Push({ContinuationAction::AcceptValueAndFinish});
}

facebook::jsi::Value JsiWriter::MoveResult() noexcept {
  VerifyElseCrash(m_continuations.size() == 0);
  return std::move(m_result);
}

facebook::jsi::Value JsiWriter::CopyResult() noexcept {
  VerifyElseCrash(m_continuations.size() == 0);
  return {m_runtime, m_result};
}

void JsiWriter::WriteNull() noexcept {
  WriteValue(facebook::jsi::Value::null());
}

void JsiWriter::WriteBoolean(bool value) noexcept {
  WriteValue({value});
}

void JsiWriter::WriteInt64(int64_t value) noexcept {
  // JavaScript's integer is not int64_t, need to ensure that the value is in range
  VerifyElseCrash(MinSafeInteger <= value && value <= MaxSafeInteger);

  // unfortunately JSI only supports int and double for number, choose double here
  // make sure that doing type conversion in C++ makes no data loss
  double d = static_cast<double>(value);
  VerifyElseCrash(floor(d) == d);
  WriteValue(d);
}

void JsiWriter::WriteDouble(double value) noexcept {
  WriteValue({value});
}

void JsiWriter::WriteString(const winrt::hstring &value) noexcept {
  WriteValue({m_runtime, facebook::jsi::String::createFromUtf8(m_runtime, winrt::to_string(value))});
}

void JsiWriter::WriteObjectBegin() noexcept {
  // legal to create an object when it is accepting a value
  VerifyElseCrash(Top().Action != ContinuationAction::AcceptPropertyName);
  Push({ContinuationAction::AcceptPropertyName, {m_runtime, facebook::jsi::Object(m_runtime)}});
}

void JsiWriter::WritePropertyName(const winrt::hstring &name) noexcept {
  // legal to set a property name only when AcceptPropertyName
  auto &top = Top();
  VerifyElseCrash(top.Action == ContinuationAction::AcceptPropertyName);
  top.Action = ContinuationAction::AcceptPropertyValue;
  top.PropertyName = winrt::to_string(name);
}

void JsiWriter::WriteObjectEnd() noexcept {
  // legal to finish an object only when AcceptPropertyName
  VerifyElseCrash(Top().Action == ContinuationAction::AcceptPropertyName);
  auto createdObject = std::move(Pop().Values.at(0));
  WriteValue(std::move(createdObject));
}

void JsiWriter::WriteArrayBegin() noexcept {
  // legal to create an array only when it is accepting a value
  VerifyElseCrash(Top().Action != ContinuationAction::AcceptPropertyName);
  Push({ContinuationAction::AcceptArrayElement});
}

void JsiWriter::WriteArrayEnd() noexcept {
  // legal to finish an array only when AcceptArrayElement
  auto &top = Top();
  VerifyElseCrash(top.Action == ContinuationAction::AcceptArrayElement);
  facebook::jsi::Array createdArray(m_runtime, top.Values.size());
  for (size_t i = 0; i < top.Values.size(); i++) {
    createdArray.setValueAtIndex(m_runtime, i, std::move(top.Values.at(i)));
  }
  Pop();
  WriteValue({m_runtime, createdArray});
}

void JsiWriter::WriteValue(facebook::jsi::Value &&value) noexcept {
  auto &top = Top();
  switch (top.Action) {
    case ContinuationAction::AcceptValueAndFinish: {
      m_result = std::move(value);
      Pop();
      VerifyElseCrash(m_continuations.size() == 0);
      break;
    }
    case ContinuationAction::AcceptArrayElement: {
      top.Values.push_back(std::move(value));
      break;
    }
    case ContinuationAction::AcceptPropertyValue: {
      auto createdObject = top.Values.at(0).getObject(m_runtime);
      createdObject.setProperty(m_runtime, top.PropertyName.c_str(), std::move(value));
      top.Action = ContinuationAction::AcceptPropertyName;
      top.PropertyName = {};
      break;
    }
    default:
      VerifyElseCrash(false);
  }
}

JsiWriter::Continuation &JsiWriter::Top() noexcept {
  VerifyElseCrash(m_continuations.size() > 0);
  return m_continuations[m_continuations.size() - 1];
}

JsiWriter::Continuation JsiWriter::Pop() noexcept {
  auto top = std::move(Top());
  m_continuations.pop_back();
  return top;
}

void JsiWriter::Push(Continuation &&continuation) noexcept {
  m_continuations.push_back(std::move(continuation));
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
