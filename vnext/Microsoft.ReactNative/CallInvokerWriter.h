// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <JSI/LongLivedJsiValue.h>
#include <ReactCommon/CallInvoker.h>
#include <functional/functor.h>
#include "DynamicWriter.h"
#include "JsiWriter.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

// IJSValueWriter to ensure that JsiWriter is always used from a RuntimeExecutor.
// In case if writing is done outside of RuntimeExecutor, it uses DynamicWriter to create
// folly::dynamic which then is written to JsiWriter in RuntimeExecutor.
struct CallInvokerWriter : winrt::implements<CallInvokerWriter, IJSValueWriter> {
  ~CallInvokerWriter();
  CallInvokerWriter(
      const std::shared_ptr<facebook::react::CallInvoker> &jsInvoker,
      std::weak_ptr<LongLivedJsiRuntime> jsiRuntimeHolder) noexcept;
  void WithResultArgs(Mso::Functor<void(facebook::jsi::Runtime &rt, facebook::jsi::Value const *args, size_t argCount)>
                          handler) noexcept;

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

 private:
  IJSValueWriter GetWriter() noexcept;

 private:
  const std::shared_ptr<facebook::react::CallInvoker> m_callInvoker;
  std::weak_ptr<LongLivedJsiRuntime> m_jsiRuntimeHolder;
  winrt::com_ptr<DynamicWriter> m_dynamicWriter;
  winrt::com_ptr<JsiWriter> m_jsiWriter;
  IJSValueWriter m_writer;
};

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

} // namespace winrt::Microsoft::ReactNative