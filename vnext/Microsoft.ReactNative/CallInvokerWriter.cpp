// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CallInvokerWriter.h"
#include <JSI/JSIDynamic.h>
#include <crash/verifyElseCrash.h>

namespace winrt::Microsoft::ReactNative {

// Special IJSValueWriter that does nothing.
// We use it instead of JsiWriter when JSI runtime is not available anymore.
struct JSNoopWriter : winrt::implements<JSNoopWriter, IJSValueWriter> {
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
};

//===========================================================================
// CallInvokerWriter implementation
//===========================================================================

CallInvokerWriter::CallInvokerWriter(
    const std::shared_ptr<facebook::react::CallInvoker> &jsInvoker,
    std::weak_ptr<LongLivedJsiRuntime> jsiRuntimeHolder) noexcept
    : m_callInvoker(jsInvoker), m_jsiRuntimeHolder(std::move(jsiRuntimeHolder)) {}

    CallInvokerWriter::~CallInvokerWriter() {
  if (auto jsiRuntimeHolder = m_jsiRuntimeHolder.lock()) {
    jsiRuntimeHolder->allowRelease();
  }
}

void CallInvokerWriter::WithResultArgs(
    Mso::Functor<void(facebook::jsi::Runtime &rt, facebook::jsi::Value const *args, size_t argCount)>
        handler) noexcept {
/*
  if (m_jsDispatcher.HasThreadAccess()) {
    VerifyElseCrash(!m_dynamicWriter);
    if (auto jsiRuntimeHolder = m_jsiRuntimeHolder.lock()) {
      const facebook::jsi::Value *args{nullptr};
      size_t argCount{0};
      m_jsiWriter->AccessResultAsArgs(args, argCount);
      handler(jsiRuntimeHolder->Runtime(), args, argCount);
      m_jsiWriter = nullptr;
    }
  } else {
*/
    VerifyElseCrash(!m_jsiWriter);
    folly::dynamic dynValue = m_dynamicWriter->TakeValue();
    VerifyElseCrash(dynValue.isArray());
    m_callInvoker->invokeAsync([handler, dynValue = std::move(dynValue), weakJsiRuntimeHolder = m_jsiRuntimeHolder, self = get_strong()](facebook::jsi::Runtime& runtime) {
      std::vector<facebook::jsi::Value> args;
      args.reserve(dynValue.size());
      for (auto const &item : dynValue) {
        args.emplace_back(facebook::jsi::valueFromDynamic(runtime, item));
      }
      handler(runtime, args.data(), args.size());
    });
  //}
}

void CallInvokerWriter::WriteNull() noexcept {
  GetWriter().WriteNull();
}

void CallInvokerWriter::WriteBoolean(bool value) noexcept {
  GetWriter().WriteBoolean(value);
}

void CallInvokerWriter::WriteInt64(int64_t value) noexcept {
  GetWriter().WriteInt64(value);
}

void CallInvokerWriter::WriteDouble(double value) noexcept {
  GetWriter().WriteDouble(value);
}

void CallInvokerWriter::WriteString(const winrt::hstring &value) noexcept {
  GetWriter().WriteString(value);
}

void CallInvokerWriter::WriteObjectBegin() noexcept {
  GetWriter().WriteObjectBegin();
}

void CallInvokerWriter::WritePropertyName(const winrt::hstring &name) noexcept {
  GetWriter().WritePropertyName(name);
}

void CallInvokerWriter::WriteObjectEnd() noexcept {
  GetWriter().WriteObjectEnd();
}

void CallInvokerWriter::WriteArrayBegin() noexcept {
  GetWriter().WriteArrayBegin();
}

void CallInvokerWriter::WriteArrayEnd() noexcept {
  GetWriter().WriteArrayEnd();
}

IJSValueWriter CallInvokerWriter::GetWriter() noexcept {
  if (!m_writer) {
/*
    if (m_jsDispatcher.HasThreadAccess()) {
      if (auto jsiRuntimeHolder = m_jsiRuntimeHolder.lock()) {
        m_jsiWriter = winrt::make_self<JsiWriter>(jsiRuntimeHolder->Runtime());
        m_writer = m_jsiWriter.as<IJSValueWriter>();
      } else {
        m_writer = winrt::make<JSNoopWriter>();
      }
    } else {
     */
      m_dynamicWriter = winrt::make_self<DynamicWriter>();
      m_writer = m_dynamicWriter.as<IJSValueWriter>();
    //}
  }
  Debug(VerifyElseCrash(m_dynamicWriter != nullptr/* || m_jsDispatcher.HasThreadAccess()*/));
  return m_writer;
}

//===========================================================================
// JSNoopWriter implementation
//===========================================================================

void JSNoopWriter::WriteNull() noexcept {}
void JSNoopWriter::WriteBoolean(bool /*value*/) noexcept {}
void JSNoopWriter::WriteInt64(int64_t /*value*/) noexcept {}
void JSNoopWriter::WriteDouble(double /*value*/) noexcept {}
void JSNoopWriter::WriteString(const winrt::hstring & /*value*/) noexcept {}
void JSNoopWriter::WriteObjectBegin() noexcept {}
void JSNoopWriter::WritePropertyName(const winrt::hstring & /*name*/) noexcept {}
void JSNoopWriter::WriteObjectEnd() noexcept {}
void JSNoopWriter::WriteArrayBegin() noexcept {}
void JSNoopWriter::WriteArrayEnd() noexcept {}

} // namespace winrt::Microsoft::ReactNative
