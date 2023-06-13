// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <hermes/hermes_api.h>
#include <chrono>
#include <future>

#include "HermesRuntimeHolder.h"
#include "HermesSamplingProfiler.h"
#include "IReactDispatcher.h"
#include "ReactPropertyBag.h"
#include "Utils.h"

using namespace winrt::Microsoft::ReactNative;
using namespace facebook::react;

namespace Microsoft::ReactNative {

namespace {

// Implements an awaiter for Mso::DispatchQueue
auto resume_in_dispatcher(const IReactDispatcher &dispatcher) noexcept {
  struct awaitable {
    awaitable(const IReactDispatcher &dispatcher) noexcept : dispatcher_(dispatcher) {}

    bool await_ready() const noexcept {
      return false;
    }

    void await_resume() const noexcept {}

    void await_suspend(std::experimental::coroutine_handle<> resume) noexcept {
      callback_ = [context = resume.address()]() noexcept {
        std::experimental::coroutine_handle<>::from_address(context)();
      };
      dispatcher_.Post(std::move(callback_));
    }

   private:
    IReactDispatcher dispatcher_;
    ReactDispatcherCallback callback_;
  };

  return awaitable{dispatcher};
}

std::future<std::string> getTraceFilePath() noexcept {
  std::string hermesDataPath = co_await Microsoft::React::getApplicationDataPath(L"Hermes");
  std::ostringstream os;
  auto now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())
                 .count();

  os << hermesDataPath << "\\cpu_" << now << ".cpuprofile";
  co_return os.str();
}

} // namespace

std::atomic_bool HermesSamplingProfiler::s_isStarted{false};
std::string HermesSamplingProfiler::s_lastTraceFilePath;

std::string HermesSamplingProfiler::GetLastTraceFilePath() noexcept {
  return s_lastTraceFilePath;
}

winrt::fire_and_forget HermesSamplingProfiler::Start(
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  bool expectedIsStarted = false;
  if (s_isStarted.compare_exchange_strong(expectedIsStarted, true)) {
    IReactDispatcher jsDispatcher = implementation::ReactDispatcher::GetJSDispatcher(reactContext->Properties());
    ReactPropertyBag propertyBag = ReactPropertyBag(reactContext->Properties());

    co_await resume_in_dispatcher(jsDispatcher);
    std::shared_ptr<HermesRuntimeHolder> hermesRuntimeHolder = HermesRuntimeHolder::loadFrom(propertyBag);
    hermesRuntimeHolder->addToProfiling();

    co_await winrt::resume_background();
    HermesRuntimeHolder::enableSamplingProfiler();
  }

  co_return;
}

std::future<std::string> HermesSamplingProfiler::Stop(
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  bool expectedIsStarted = true;
  if (s_isStarted.compare_exchange_strong(expectedIsStarted, false)) {
    IReactDispatcher jsDispatcher = implementation::ReactDispatcher::GetJSDispatcher(reactContext->Properties());
    ReactPropertyBag propertyBag = ReactPropertyBag(reactContext->Properties());

    co_await winrt::resume_background();
    HermesRuntimeHolder::disableSamplingProfiler();

    s_lastTraceFilePath = co_await getTraceFilePath();
    HermesRuntimeHolder::dumpSampledTraceToFile(s_lastTraceFilePath);

    co_await resume_in_dispatcher(jsDispatcher);
    std::shared_ptr<HermesRuntimeHolder> hermesRuntimeHolder = HermesRuntimeHolder::loadFrom(propertyBag);
    hermesRuntimeHolder->removeFromProfiling();

    co_await winrt::resume_background();
  }

  co_return s_lastTraceFilePath;
}

bool HermesSamplingProfiler::IsStarted() noexcept {
  return s_isStarted.load();
}

} // namespace Microsoft::ReactNative
