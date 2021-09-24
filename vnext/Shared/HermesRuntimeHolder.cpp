// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <memory>
#include <mutex>

#include <JSI/decorator.h>
#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/SystraceSection.h>
#include <hermes/hermes.h>
#ifndef NDEBUG
#include <hermes/hermes_dbg.h>
#endif
#include "HermesRuntimeHolder.h"

#include <tracing/tracing.h>

#if defined(HERMES_ENABLE_DEBUGGER)
#include <hermes/inspector/chrome/Registration.h>
#endif

using namespace facebook;

namespace facebook {
namespace react {

namespace {

std::unique_ptr<facebook::hermes::HermesRuntime> makeHermesRuntimeSystraced(
    const ::hermes::vm::RuntimeConfig &runtimeConfig) {
  SystraceSection s("HermesExecutorFactory::makeHermesRuntimeSystraced");

  auto res = LoadLibrary(L"hermes.dll");
  if (!res) {
    DWORD lastError = GetLastError();
    std::stringstream ostr;
    ostr << "Failure loading hermes.dll:" << std::hex << lastError;
    facebook::react::tracing::error(ostr.str().c_str());
  } else {
    facebook::react::tracing::error("hermes.dll loaded successfully");
  }

  auto res2 = LoadLibraryEx(L"hermes.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
  if (!res2) {
    DWORD lastError = GetLastError();
    std::stringstream ostr;
    ostr << "Failure loading hermes.dll even without resolving references:" << std::hex << lastError;
    facebook::react::tracing::error(ostr.str().c_str());
  } else {
    facebook::react::tracing::error("hermes.dll loaded successfully without resolving references");
  }

auto res3 = LoadLibrary(L"icu.dll");
  if (!res3) {
    DWORD lastError = GetLastError();
    std::stringstream ostr;
    ostr << "Failure loading icu.dll:" << std::hex << lastError;
    facebook::react::tracing::error(ostr.str().c_str());
  } else {
    facebook::react::tracing::error("icu.dll loaded successfully");
  }

  auto res4 = LoadLibrary(L"c:\\windows\\system32\\icu.dll");
  if (!res4) {
    DWORD lastError = GetLastError();
    std::stringstream ostr;
    ostr << "Failure loading c:\\windows\\system32\\icu.dll:" << std::hex << lastError;
    facebook::react::tracing::error(ostr.str().c_str());
  } else {
    facebook::react::tracing::error("c:\\windows\\system32\\icu.dll loaded successfully");
  }


  auto res5 = LoadLibrary(L"icuuc.dll");
  if (!res5) {
    DWORD lastError = GetLastError();
    std::stringstream ostr;
    ostr << "Failure loading icuuc.dll:" << std::hex << lastError;
    facebook::react::tracing::error(ostr.str().c_str());
  }

  return hermes::makeHermesRuntime(runtimeConfig);
}

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

} // namespace

std::shared_ptr<jsi::Runtime> HermesRuntimeHolder::getRuntime() noexcept {
  std::call_once(m_once_flag, [this]() { initRuntime(); });

  if (!m_runtime)
    std::terminate();

  // Make sure that the runtime instance is not consumed from multiple threads.
  if (m_own_thread_id != std::this_thread::get_id())
    std::terminate();

  return m_runtime;
}

HermesRuntimeHolder::HermesRuntimeHolder(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) noexcept
    : m_devSettings(std::move(devSettings)), m_jsQueue(std::move(jsQueue)) {}

void HermesRuntimeHolder::initRuntime() noexcept {
  auto runtimeConfig = ::hermes::vm::RuntimeConfig();
  auto hermesRuntime = makeHermesRuntimeSystraced(runtimeConfig);
  facebook::hermes::HermesRuntime &hermesRuntimeRef = *hermesRuntime;

#ifndef NDEBUG
  m_runtime = std::make_shared<hermes::RuntimeDebugFlavorProxy>(std::move(hermesRuntime));
#else
  m_runtime = std::move(hermesRuntime);
#endif
  m_own_thread_id = std::this_thread::get_id();

#ifdef HERMES_ENABLE_DEBUGGER
  if (m_devSettings->useDirectDebugger) {
    auto adapter = std::make_unique<HermesExecutorRuntimeAdapter>(m_runtime, hermesRuntimeRef, m_jsQueue);
    facebook::hermes::inspector::chrome::enableDebugging(
        std::move(adapter),
        m_devSettings->debuggerRuntimeName.empty() ? "Hermes React Native" : m_devSettings->debuggerRuntimeName);
  }
#endif

  // Add js engine information to Error.prototype so in error reporting we
  // can send this information.
  auto errorPrototype =
      m_runtime->global().getPropertyAsObject(*m_runtime, "Error").getPropertyAsObject(*m_runtime, "prototype");
  errorPrototype.setProperty(*m_runtime, "jsEngine", "hermes");
}

} // namespace react
} // namespace facebook
