// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <DevSettings.h>
#include <JSI/RuntimeHolder.h>
#include <JSI/ScriptStore.h>
#include <ReactPropertyBag.h>
#include <cxxreact/MessageQueueThread.h>
#include <hermes/hermes_api.h>

namespace Microsoft::ReactNative {

class HermesRuntimeHolder : public Microsoft::JSI::RuntimeHolderLazyInit {
 public: // RuntimeHolderLazyInit implementation.
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;
  facebook::react::JSIEngineOverride getRuntimeType() noexcept override;
  void crashHandler(int fileDescriptor) noexcept override;
  void teardown() noexcept override;

 public:
  HermesRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore) noexcept;
  ~HermesRuntimeHolder();

  static std::shared_ptr<HermesRuntimeHolder> loadFrom(
      winrt::Microsoft::ReactNative::ReactPropertyBag const &propertyBag) noexcept;

  static void storeTo(
      winrt::Microsoft::ReactNative::ReactPropertyBag const &propertyBag,
      std::shared_ptr<HermesRuntimeHolder> const &holder) noexcept;

  void addToProfiling() const noexcept;
  void removeFromProfiling() const noexcept;

  static void enableSamplingProfiler() noexcept;
  static void disableSamplingProfiler() noexcept;
  static void dumpSampledTraceToFile(const std::string &fileName) noexcept;

 private:
  void initRuntime() noexcept;

 private:
  jsr_runtime m_runtime{};
  std::shared_ptr<facebook::jsi::Runtime> m_jsiRuntime;
  std::once_flag m_onceFlag{};
  std::thread::id m_ownThreadId{};
  std::weak_ptr<facebook::react::DevSettings> m_weakDevSettings;
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsQueue;
  std::shared_ptr<facebook::jsi::PreparedScriptStore> m_preparedScriptStore;
};

} // namespace Microsoft::ReactNative
