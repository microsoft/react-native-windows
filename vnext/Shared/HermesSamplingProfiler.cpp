// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <chrono>
#include <future>

#include <ShlObj.h>
#include <Shlwapi.h>
#include <appmodel.h>
#include <hermes/hermes.h>
#include <winrt/Windows.Storage.h>
#include "Unicode.h"

#include "HermesSamplingProfiler.h"

namespace Microsoft::ReactNative {

namespace {
std::future<std::string> getTraceFilePath() noexcept {
  std::ostringstream os;
  uint32_t length{0};
  if (GetCurrentPackageFullName(&length, nullptr) != APPMODEL_ERROR_NO_PACKAGE) {
    auto hermesFolder = (co_await winrt::Windows::Storage::ApplicationData::Current().LocalFolder().CreateFolderAsync(
                             L"Hermes", winrt::Windows::Storage::CreationCollisionOption::OpenIfExists))
                            .Path();
    os << winrt::to_string(hermesFolder);
  } else {
    WCHAR wzAppData[MAX_PATH];
    if (SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, wzAppData) != S_OK)
      std::abort(); // We don't expect this to happen ever ..

    std::string appData = winrt::to_string(wzAppData);
    os << appData << "\\react-native\\Hermes";
  }

  auto now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())
                 .count();

  os << "\\cpu_" << now << ".cpuprofile";
  co_return os.str();
}
} // namespace

bool HermesSamplingProfiler::s_isStarted = false;
std::string HermesSamplingProfiler::s_lastTraceFilePath;

std::string HermesSamplingProfiler::GetLastTraceFilePath() noexcept {
  return s_lastTraceFilePath;
}

winrt::fire_and_forget HermesSamplingProfiler::Start() noexcept {
#ifdef INCLUDE_HERMES
  if (!s_isStarted) {
    s_isStarted = true;
    co_await winrt::resume_background();
    facebook::hermes::HermesRuntime::enableSamplingProfiler();
  }
#endif
  co_return;
}

std::future<std::string> HermesSamplingProfiler::Stop() noexcept {
#ifdef INCLUDE_HERMES
  if (s_isStarted) {
    s_isStarted = false;
    co_await winrt::resume_background();
    facebook::hermes::HermesRuntime::disableSamplingProfiler();
    s_lastTraceFilePath = co_await getTraceFilePath();
    facebook::hermes::HermesRuntime::dumpSampledTraceToFile(s_lastTraceFilePath);
  }
#endif
  co_return s_lastTraceFilePath;
}

bool HermesSamplingProfiler::IsStarted() noexcept {
  return s_isStarted;
}

} // namespace Microsoft::ReactNative
