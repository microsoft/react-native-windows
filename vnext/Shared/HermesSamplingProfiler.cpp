// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <chrono>

#include <hermes/hermes.h>
#include <winrt/Windows.Storage.h>
#include "Unicode.h"

#include "HermesSamplingProfiler.h"

namespace Microsoft::ReactNative {

namespace {
std::future<std::string> getTraceFilePath() noexcept {
  auto hermesFolder = (co_await winrt::Windows::Storage::ApplicationData::Current().LocalFolder().CreateFolderAsync(
                           L"Hermes", winrt::Windows::Storage::CreationCollisionOption::OpenIfExists))
                          .Path();
  auto now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())
                 .count();
  std::ostringstream os;
  os << Microsoft::Common::Unicode::Utf16ToUtf8(hermesFolder.c_str(), hermesFolder.size()) << "\\cpu_" << +now
     << ".cpuprofile";
  co_return os.str();
}
} // namespace

bool HermesSamplingProfiler::s_isStarted = false;
std::string HermesSamplingProfiler::s_lastTraceFilePath;

std::string HermesSamplingProfiler::GetLastTraceFilePath() noexcept {
  return s_lastTraceFilePath;
}

winrt::fire_and_forget HermesSamplingProfiler::Start() noexcept {
  s_isStarted = true;
  facebook::hermes::HermesRuntime::enableSamplingProfiler();
  co_return;
}

std::future<std::string> HermesSamplingProfiler::Stop() noexcept {
  s_isStarted = false;
  facebook::hermes::HermesRuntime::disableSamplingProfiler();
  s_lastTraceFilePath = co_await getTraceFilePath();
  facebook::hermes::HermesRuntime::dumpSampledTraceToFile(s_lastTraceFilePath);
  co_return s_lastTraceFilePath;
}

bool HermesSamplingProfiler::IsStarted() noexcept {
  return s_isStarted;
}

} // namespace Microsoft::ReactNative
