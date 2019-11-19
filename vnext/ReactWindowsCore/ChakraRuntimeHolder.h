// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <DevSettings.h>

#include <JSI/Shared/RuntimeHolder.h>
#include <jsi/Shared/ScriptStore.h>

#include <mutex>
#include <thread>

namespace Microsoft::JSI {

struct ChakraRuntimeArgs;

class ChakraRuntimeHolder : public facebook::jsi::RuntimeHolderLazyInit {
 public:
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;

  ChakraRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::unique_ptr<facebook::jsi::ScriptStore> &&scriptStore,
      std::unique_ptr<facebook::jsi::PreparedScriptStore> &&preparedScriptStore) noexcept;

 private:
  void InitRuntime() noexcept;

  std::shared_ptr<ChakraRuntimeArgs> m_args;
  std::shared_ptr<facebook::jsi::Runtime> m_runtime;

  std::once_flag m_init_runtime_flag;
  std::thread::id m_own_thread_id;
};

} // namespace Microsoft::JSI
