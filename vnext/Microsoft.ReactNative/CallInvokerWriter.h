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

  // This should be called before the code flow exits the scope of the CallInvoker,
  // thus requiring the CallInokerWriter to call m_callInvoker->invokeAsync to call back into JS.
  void ExitCurrentCallInvokeScope() noexcept;

 private:
  IJSValueWriter GetWriter() noexcept;

 private:
  const std::shared_ptr<facebook::react::CallInvoker> m_callInvoker;
  std::weak_ptr<LongLivedJsiRuntime> m_jsiRuntimeHolder;
  winrt::com_ptr<DynamicWriter> m_dynamicWriter;
  winrt::com_ptr<JsiWriter> m_jsiWriter;
  IJSValueWriter m_writer;

  // If a callback is invoked synchronously we can call the JS callback directly.
  // However, if we post to another thread, or call the callback on the same thread but after we exit the current
  // RuntimeExecutor callback, then we need to save the callback args in a dynamic and post it back to the CallInvoker
  bool m_fastPath{true};
  const std::thread::id m_threadId;
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
