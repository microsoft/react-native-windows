// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <JSI/RuntimeHolder.h>

#include <jsi/jsi.h>
#include <thread>

#include <DevSettings.h>
#include <ReactPropertyBag.h>

namespace facebook::hermes {
class HermesRuntime;
}

namespace Microsoft::ReactNative {
class HermesShim;
}

namespace facebook::react {

class MessageQueueThread;

class HermesRuntimeHolder : public Microsoft::JSI::RuntimeHolderLazyInit {
 public: // RuntimeHolderLazyInit implementation.
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;
  facebook::react::JSIEngineOverride getRuntimeType() noexcept override;
  void crashHandler(int fileDescriptor) noexcept override;
  void teardown() noexcept override;

 public:
  HermesRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) noexcept;

  static std::shared_ptr<HermesRuntimeHolder> loadFrom(
      winrt::Microsoft::ReactNative::ReactPropertyBag const &propertyBag) noexcept;

  static void storeTo(
      winrt::Microsoft::ReactNative::ReactPropertyBag const &propertyBag,
      std::shared_ptr<HermesRuntimeHolder> const &holder) noexcept;

  void addToProfiling() const noexcept;
  void removeFromProfiling() const noexcept;

 private:
  void initRuntime() noexcept;

 private:
  std::shared_ptr<Microsoft::ReactNative::HermesShim> m_hermesShim;
  std::shared_ptr<facebook::hermes::HermesRuntime> m_hermesRuntime;
  std::once_flag m_onceFlag{};
  std::thread::id m_ownThreadId{};
  std::weak_ptr<facebook::react::DevSettings> m_weakDevSettings;
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsQueue;
};

} // namespace facebook::react
