// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "CrashManager.h"
#include "ReactInstanceWin.h"

#include <winrt/Windows.Storage.h>
#include <csignal>

#include <WerApi.h>

namespace Mso::React {

// When calling SetUnhandledExceptionFilter the previous filter is returned.
// That should be saved so that if needed it can be called after the new unhandled exception has finished executing.
// This allows the Windows Error Reporting system to process the error and upload the data for processing.
static LPTOP_LEVEL_EXCEPTION_FILTER g_previousExceptionFilter = nullptr;
static _crt_signal_t g_previousSignalHandler = SIG_ERR;
static std::wstring g_logFileName;
static bool g_WERExceptionFilterWasCalled{false}; // Prevent recursion in the custom handler

/*static*/ std::atomic_uint CrashManager::_regCount{0};

extern "C" LONG WINAPI CustomWERExceptionFilter(LPEXCEPTION_POINTERS const exceptionPointers) {
  if (g_WERExceptionFilterWasCalled) {
    return EXCEPTION_EXECUTE_HANDLER;
  }

  g_WERExceptionFilterWasCalled = true;

  CrashManager::OnUnhandledException();

  if (exceptionPointers == nullptr) {
    return EXCEPTION_EXECUTE_HANDLER;
  }

  return g_previousExceptionFilter(exceptionPointers);
}

void __cdecl on_sigabrt(int signum) {
  CustomWERExceptionFilter(nullptr);
  signal(signum, SIG_DFL);
}

void InternalRegisterCustomHandler() noexcept {
  // Do this now because by the time we catch the exception we may be in OOM
#ifndef CORE_ABI // win32 vs uwp file permissions
  wchar_t currentDirectory[MAX_PATH]{};
  VerifyElseCrash(!!GetTempPath(MAX_PATH, currentDirectory));
  g_logFileName = currentDirectory;
#else
  g_logFileName = winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path() + L"\\";
#endif

  g_logFileName += L"ReactNativeCrashDetails_" + std::to_wstring(GetCurrentProcessId()) + L".txt";

  VerifySucceededElseCrash(::WerRegisterFile(g_logFileName.c_str(), WerRegFileTypeOther, WER_FILE_ANONYMOUS_DATA));

  g_previousExceptionFilter = ::SetUnhandledExceptionFilter(CustomWERExceptionFilter);

  g_previousSignalHandler = signal(SIGABRT, &on_sigabrt);
}

void InternalUnregisterCustomHandler() noexcept {
  VerifySucceededElseCrash(::WerUnregisterFile(g_logFileName.c_str()));

  if (g_previousExceptionFilter) {
    ::SetUnhandledExceptionFilter(g_previousExceptionFilter);
    g_previousExceptionFilter = nullptr;
  }

  if (g_previousSignalHandler != SIG_ERR) {
    signal(SIGABRT, g_previousSignalHandler);
  }
}

/*static*/ void CrashManager::OnUnhandledException() noexcept {
  FILE *f{nullptr};
  auto err = _wfopen_s(&f, g_logFileName.c_str(), L"w");
  if (err == 0) {
    CrashHandler(_fileno(f));
    fclose(f);
  }
}

/*static*/ void CrashManager::CrashHandler(int fileDescriptor) noexcept {
  ReactInstanceWin::CrashHandler(fileDescriptor);

  // call any other component's static crash handlers here
}

/*static*/ void CrashManager::RegisterCustomHandler() noexcept {
  if (_regCount++ == 0) {
    InternalRegisterCustomHandler();
  }
}

/*static*/ void CrashManager::UnregisterCustomHandler() noexcept {
  if (--_regCount == 0) {
    InternalUnregisterCustomHandler();
  }
}

#if defined(DEBUG)
/*static*/ std::string CrashManager::TestCollectCrashData() {
  FILE *f{nullptr};
  if (tmpfile_s(&f) != 0) {
    return std::string{};
  }

  CrashHandler(_fileno(f));
  const auto sz = (size_t)ftell(f);
  rewind(f);

  std::string out;
  out.resize(sz);
  const auto readsz = fread(&out[0], 1, sz, f);
  assert(readsz == sz);
  fclose(f);

  return out;
}
#endif

} // namespace Mso::React
