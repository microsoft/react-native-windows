// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "RuntimeHolder.h"
#include "ScriptStore.h"

#include <cxxreact/MessageQueueThread.h>

namespace Microsoft::ReactNative {

class V8RuntimeHolder : public Microsoft::JSI::RuntimeHolderLazyInit {
 public: // RuntimeHolderLazyInit implementation.
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;
  facebook::react::JSIEngineOverride getRuntimeType() noexcept override;

  V8RuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore,
      bool enableMultiThreadingSupport) noexcept;

 private:
  void initRuntime() noexcept;

 private:
  std::shared_ptr<facebook::jsi::Runtime> m_jsiRuntime;
  std::once_flag m_onceFlag{};
  std::thread::id m_ownThreadId{};
  std::weak_ptr<facebook::react::DevSettings> m_weakDevSettings;
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsQueue;
  std::shared_ptr<facebook::jsi::PreparedScriptStore> m_preparedScriptStore;
  bool m_enableMultiThreadingSupport;
};

} // namespace Microsoft::ReactNative
