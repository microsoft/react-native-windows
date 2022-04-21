// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <JSI/RuntimeHolder.h>

#include <jsi/jsi.h>
#include <thread>

#include <DevSettings.h>

namespace facebook::hermes {
class HermesRuntime;
}

namespace facebook {
namespace react {

class HermesRuntimeHolder : public Microsoft::JSI::RuntimeHolderLazyInit {
 public:
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;
  facebook::react::JSIEngineOverride getRuntimeType() noexcept override;

  void crashHandler(int fileDescriptor) noexcept override;

  void teardown() noexcept override;

  HermesRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) noexcept;

 private:
  void initRuntime() noexcept;
  std::shared_ptr<facebook::hermes::HermesRuntime> m_hermesRuntime;

  std::once_flag m_once_flag;
  std::thread::id m_own_thread_id;

  std::weak_ptr<facebook::react::DevSettings> m_weakDevSettings;
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsQueue;
};

} // namespace react
} // namespace facebook
