// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "RuntimeHolder.h"
#include "ScriptStore.h"

#include <cxxreact/MessageQueueThread.h>

#include <Windows.h>

namespace Microsoft::ReactNative {

// Settings for a V8 runtime created through the V8 JSI ABI.
struct V8AbiRuntimeArgs {
  std::shared_ptr<facebook::react::MessageQueueThread> jsQueue;
  std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore;
  std::string debuggerRuntimeName;
  uint16_t inspectorPort{0}; // 0 keeps the v8jsi2.dll default port.
  bool enableInspector{false};
  bool inspectorBreakOnStart{false};
  bool enableMultiThreading{false};
};

// Creates a jsi::Runtime through the V8 JSI ABI (the v8_jsi_abi_* C functions) exported by v8jsi2.dll from the
// Microsoft.JavaScript.V8 package. Functions are resolved with GetProcAddress, leaving legacy v8jsi.dll imports
// unchanged. Returns nullptr and sets errorMessage on failure.
std::unique_ptr<facebook::jsi::Runtime>
MakeV8AbiRuntime(HMODULE v8jsiModule, V8AbiRuntimeArgs &&args, std::string &errorMessage) noexcept;

// Selected only by the ReactNative.ReactOptions/JSIEngineV8Abi property. The runtime type remains V8.
class V8AbiRuntimeHolder : public Microsoft::JSI::RuntimeHolderLazyInit {
 public: // RuntimeHolderLazyInit implementation.
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;
  facebook::react::JSIEngineOverride getRuntimeType() noexcept override;

  V8AbiRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore,
      bool enableMultiThreadingSupport) noexcept;

 private:
  void initRuntime() noexcept;

 private:
  std::shared_ptr<facebook::jsi::Runtime> m_jsiRuntime;
  std::once_flag m_onceFlag{};
  std::weak_ptr<facebook::react::DevSettings> m_weakDevSettings;
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsQueue;
  std::shared_ptr<facebook::jsi::PreparedScriptStore> m_preparedScriptStore;
  bool m_enableMultiThreadingSupport;
};

} // namespace Microsoft::ReactNative
