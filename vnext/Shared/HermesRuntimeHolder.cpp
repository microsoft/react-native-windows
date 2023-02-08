// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HermesRuntimeHolder.h"

#include <JSI/decorator.h>
#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/SystraceSection.h>
#include <hermes/hermes.h>
#include "HermesShim.h"

#if defined(HERMES_ENABLE_DEBUGGER)
#include <hermes/inspector/chrome/Registration.h>
#endif

#include <memory>
#include <mutex>

using namespace facebook;
using namespace Microsoft::ReactNative;

namespace React {
using namespace winrt::Microsoft::ReactNative;
}

namespace facebook::react {

React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<HermesRuntimeHolder>>>
HermesRuntimeHolderProperty() noexcept {
  static React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<HermesRuntimeHolder>>> propId{
      L"ReactNative.HermesRuntimeHolder", L"HermesRuntimeHolder"};
  return propId;
}

namespace {

#ifdef HERMES_ENABLE_DEBUGGER
class HermesExecutorRuntimeAdapter final : public facebook::hermes::inspector::RuntimeAdapter {
 public:
  HermesExecutorRuntimeAdapter(
      std::shared_ptr<jsi::Runtime> runtime,
      facebook::hermes::HermesRuntime &hermesRuntime,
      std::shared_ptr<MessageQueueThread> thread)
      : m_runtime(runtime), m_hermesRuntime(hermesRuntime), m_thread(std::move(thread)) {}

  virtual ~HermesExecutorRuntimeAdapter() = default;

  jsi::Runtime &getRuntime() override {
    return *m_runtime;
  }

  facebook::hermes::debugger::Debugger &getDebugger() override {
    return m_hermesRuntime.getDebugger();
  }

  void tickleJs() override {
    // The queue will ensure that runtime_ is still valid when this
    // gets invoked.
    m_thread->runOnQueue([&runtime = m_runtime]() {
      auto func = runtime->global().getPropertyAsFunction(*runtime, "__tickleJs");
      func.call(*runtime);
    });
  }

 private:
  std::shared_ptr<jsi::Runtime> m_runtime;
  facebook::hermes::HermesRuntime &m_hermesRuntime;

  std::shared_ptr<MessageQueueThread> m_thread;
};
#endif

std::shared_ptr<HermesShim> makeHermesShimSystraced(bool enableDefaultCrashHandler) {
  SystraceSection s("HermesExecutorFactory::makeHermesRuntimeSystraced");
  if (enableDefaultCrashHandler) {
    return HermesShim::makeWithWER();
  } else {
    return HermesShim::make();
  }
}

} // namespace

void HermesRuntimeHolder::crashHandler(int fileDescriptor) noexcept {
  m_hermesShim->dumpCrashData(fileDescriptor);
}

void HermesRuntimeHolder::teardown() noexcept {
#ifdef HERMES_ENABLE_DEBUGGER
  if (auto devSettings = m_weakDevSettings.lock(); devSettings && devSettings->useDirectDebugger) {
    facebook::hermes::inspector::chrome::disableDebugging(*m_hermesRuntime);
  }
#endif
}

facebook::react::JSIEngineOverride HermesRuntimeHolder::getRuntimeType() noexcept {
  return facebook::react::JSIEngineOverride::Hermes;
}

std::shared_ptr<jsi::Runtime> HermesRuntimeHolder::getRuntime() noexcept {
  std::call_once(m_onceFlag, [this]() { initRuntime(); });
  VerifyElseCrash(m_hermesRuntime);
  VerifyElseCrashSz(m_ownThreadId == std::this_thread::get_id(), "Must be accessed from JS thread.");
  return m_hermesRuntime;
}

HermesRuntimeHolder::HermesRuntimeHolder(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) noexcept
    : m_weakDevSettings(devSettings), m_jsQueue(std::move(jsQueue)) {}

void HermesRuntimeHolder::initRuntime() noexcept {
  auto devSettings = m_weakDevSettings.lock();
  VerifyElseCrash(devSettings);

  m_hermesShim = makeHermesShimSystraced(devSettings->enableDefaultCrashHandler);
  m_hermesRuntime = m_hermesShim->getRuntime();
  m_ownThreadId = std::this_thread::get_id();

#ifdef HERMES_ENABLE_DEBUGGER
  if (devSettings->useDirectDebugger) {
    auto adapter = std::make_unique<HermesExecutorRuntimeAdapter>(m_hermesRuntime, *m_hermesRuntime, m_jsQueue);
    facebook::hermes::inspector::chrome::enableDebugging(
        std::move(adapter),
        devSettings->debuggerRuntimeName.empty() ? "Hermes React Native" : devSettings->debuggerRuntimeName);
  }
#endif

  // Add js engine information to Error.prototype so in error reporting we
  // can send this information.
  auto errorPrototype = m_hermesRuntime->global()
                            .getPropertyAsObject(*m_hermesRuntime, "Error")
                            .getPropertyAsObject(*m_hermesRuntime, "prototype");
  errorPrototype.setProperty(*m_hermesRuntime, "jsEngine", "hermes");
}

std::shared_ptr<HermesRuntimeHolder> HermesRuntimeHolder::loadFrom(
    React::ReactPropertyBag const &propertyBag) noexcept {
  return *(propertyBag.Get(HermesRuntimeHolderProperty()));
}

void HermesRuntimeHolder::storeTo(
    React::ReactPropertyBag const &propertyBag,
    std::shared_ptr<HermesRuntimeHolder> const &holder) noexcept {
  propertyBag.Set(HermesRuntimeHolderProperty(), holder);
}

void HermesRuntimeHolder::addToProfiling() const noexcept {
  m_hermesShim->addToProfiling();
}

void HermesRuntimeHolder::removeFromProfiling() const noexcept {
  m_hermesShim->removeFromProfiling();
}

} // namespace facebook::react
