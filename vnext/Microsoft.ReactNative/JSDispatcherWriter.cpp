// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JSDispatcherWriter.h"
#include <JSI/JSIDynamic.h>
#include <crash/verifyElseCrash.h>

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// JSDispatcherWriter implementation
//===========================================================================

JSDispatcherWriter::JSDispatcherWriter(
    IReactDispatcher const &jsDispatcher,
    facebook::jsi::Runtime &jsiRuntime) noexcept
    : m_jsDispatcher(jsDispatcher), m_jsiRuntime(jsiRuntime) {}

void JSDispatcherWriter::WithResultArgs(
    Mso::Functor<void(facebook::jsi::Runtime &rt, facebook::jsi::Value const *args, size_t argCount)>
        handler) noexcept {
  if (m_jsDispatcher.HasThreadAccess()) {
    VerifyElseCrash(!m_dynamicWriter);
    const facebook::jsi::Value *args{nullptr};
    size_t argCount{0};
    m_jsiWriter->AccessResultAsArgs(args, argCount);
    handler(m_jsiRuntime, args, argCount);
  } else {
    VerifyElseCrash(!m_jsiWriter);
    folly::dynamic dynValue = m_dynamicWriter->TakeValue();
    m_jsDispatcher.Post([handler, dynValue, &runtime = m_jsiRuntime]() {
      VerifyElseCrash(dynValue.isArray());
      std::vector<facebook::jsi::Value> args;
      args.reserve(dynValue.size());
      for (auto const &item : dynValue) {
        args.emplace_back(facebook::jsi::valueFromDynamic(runtime, item));
      }
      handler(runtime, args.data(), args.size());
    });
  }
}

void JSDispatcherWriter::WriteNull() noexcept {
  GetWriter().WriteNull();
}

void JSDispatcherWriter::WriteBoolean(bool value) noexcept {
  GetWriter().WriteBoolean(value);
}

void JSDispatcherWriter::WriteInt64(int64_t value) noexcept {
  GetWriter().WriteInt64(value);
}

void JSDispatcherWriter::WriteDouble(double value) noexcept {
  GetWriter().WriteDouble(value);
}

void JSDispatcherWriter::WriteString(const winrt::hstring &value) noexcept {
  GetWriter().WriteString(value);
}

void JSDispatcherWriter::WriteObjectBegin() noexcept {
  GetWriter().WriteObjectBegin();
}

void JSDispatcherWriter::WritePropertyName(const winrt::hstring &name) noexcept {
  GetWriter().WritePropertyName(name);
}

void JSDispatcherWriter::WriteObjectEnd() noexcept {
  GetWriter().WriteObjectEnd();
}

void JSDispatcherWriter::WriteArrayBegin() noexcept {
  GetWriter().WriteArrayBegin();
}

void JSDispatcherWriter::WriteArrayEnd() noexcept {
  GetWriter().WriteArrayEnd();
}

IJSValueWriter JSDispatcherWriter::GetWriter() noexcept {
  if (m_jsDispatcher.HasThreadAccess()) {
    VerifyElseCrash(!m_dynamicWriter);
    if (!m_jsiWriter) {
      m_jsiWriter = winrt::make_self<JsiWriter>(m_jsiRuntime);
      m_writer = m_jsiWriter.as<IJSValueWriter>();
    }
  } else {
    VerifyElseCrash(!m_jsiWriter);
    if (!m_dynamicWriter) {
      m_dynamicWriter = winrt::make_self<DynamicWriter>();
      m_writer = m_dynamicWriter.as<IJSValueWriter>();
    }
  }
  return m_writer;
}

} // namespace winrt::Microsoft::ReactNative
