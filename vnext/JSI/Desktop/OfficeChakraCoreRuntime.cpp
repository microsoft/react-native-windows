// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "OfficeChakraCoreRuntime.h"

#include "ChakraCoreRuntimeFactory.h"
#include "jsi/decorator.h"
#include "utilities.h"

#include <future>
#include <thread>

namespace {

struct FileVersionInfoResource {
  uint16_t len;
  uint16_t valLen;
  uint16_t type;
  wchar_t key[_countof(L"VS_VERSION_INFO")];
  uint16_t padding1;
  VS_FIXEDFILEINFO fixedFileInfo;
  uint32_t padding2;
};

uint64_t getDllFileVersion(const wchar_t *const dllName) noexcept {
  auto freeLibraryWrapper = [](void *p) { FreeLibrary((HMODULE)p); };

  HMODULE moduleHandle;
  if (!GetModuleHandleExW(0, dllName, &moduleHandle)) {
    std::terminate();
  }

  std::unique_ptr<void, decltype(freeLibraryWrapper)> moduleHandleWrapper(
      moduleHandle, std::move(freeLibraryWrapper));

  HRSRC versionResourceHandle =
      FindResourceW(moduleHandle, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);

  if (!versionResourceHandle ||
      SizeofResource(
          static_cast<HMODULE>(moduleHandleWrapper.get()),
          versionResourceHandle) < sizeof(FileVersionInfoResource)) {
    std::terminate();
  }

  HGLOBAL versionResourcePtrHandle =
      LoadResource(moduleHandle, versionResourceHandle);

  if (!versionResourcePtrHandle) {
    std::terminate();
  }

  FileVersionInfoResource *versionInfo = static_cast<FileVersionInfoResource *>(
      LockResource(versionResourcePtrHandle));

  if (!versionInfo) {
    std::terminate();
  }

  uint64_t dllFileVersion = versionInfo->fixedFileInfo.dwFileVersionMS;
  dllFileVersion <<= 32;
  dllFileVersion |= versionInfo->fixedFileInfo.dwFileVersionLS;

  return dllFileVersion;
}

} // anonymous namespace

namespace facebook {
namespace jsi {
namespace chakra {

namespace {

void persistPreparedScriptHelper(
    const std::shared_ptr<PreparedScriptStore> &preparedScriptStore,
    const std::shared_ptr<const Buffer> &script,
    const ScriptSignature &scriptSignature,
    const JSRuntimeSignature &runtimeSignature) {
  auto runtime =
      makeChakraCoreRuntime({true /* disableBackgroundWork */,
                             true /* disableExecutablePageAllocation */});

  std::shared_ptr<const ChakraCoreRuntime::ChakraCorePreparedJavaScript>
      preparedJS = std::static_pointer_cast<
          const ChakraCoreRuntime::ChakraCorePreparedJavaScript>(
          runtime->prepareJavaScript(script, scriptSignature.url));

  // Since preparedJS keeps a raw reference to runtime, runtime must stay alive
  // until persistPreparedScript is done with preparedJS->bytecodeBuffer().
  // Hence persistPreparedScript must be sychronous.
  preparedScriptStore->persistPreparedScript(
      preparedJS->bytecodeBuffer(), scriptSignature, runtimeSignature, nullptr);
}

} // namespace

OfficeChakraCoreRuntime::OfficeChakraCoreRuntime(
    ChakraRuntimeArgs &&args,
    OfficeChakraCoreRuntimeArgs &&officeArgs) noexcept
    // Since the constructor and destructor of RuntimeDecorator do not access
    // its cached m_runtime, using m_runtime before it's initialized here is
    // safe. See comments on the RuntimeDecorator constructor for more
    // information.
    : RuntimeDecorator(m_runtime),
      m_runtime(std::move(args)),
      m_args(std::move(officeArgs)) {
  std::call_once(s_versionInitFlag, []() {
    s_version = getDllFileVersion(L"ChakraCore.dll");
  });

  // setupMemoryTracker();
  // setupNativePromiseContinuation();
}

Value OfficeChakraCoreRuntime::evaluateJavaScript(
    const std::shared_ptr<const Buffer> &unused,
    const std::string &sourceURL) {
  assert(unused == nullptr);

  VersionedBuffer script = m_args.m_scriptStore->getVersionedScript(sourceURL);

  // We make a copy of sourceURL here.
  ScriptSignature scriptSignature{sourceURL, script.version};
  JSRuntimeSignature runtimeSignature{"ChakraCore", s_version};

  std::shared_ptr<const Buffer> preparedScript =
      m_args.m_preparedScriptStore->tryGetPreparedScript(
          scriptSignature, runtimeSignature, nullptr);

  if (!preparedScript) {
    std::future<void> persistPreparedScriptFuture = std::async(
        std::launch::async,
        persistPreparedScriptHelper,
        m_args.m_preparedScriptStore,
        script.buffer,
        scriptSignature,
        runtimeSignature);

    if (!m_args.m_asyncPreparedScriptGeneration) {
      persistPreparedScriptFuture.wait();
    }

    return plain().evaluateJavaScript(script.buffer, sourceURL);
  }

  return evaluatePreparedJavaScript(std::static_pointer_cast<
                                    const PreparedJavaScript>(
      std::make_shared<const ChakraCoreRuntime::ChakraCorePreparedJavaScript>(
          m_runtime, sourceURL, script.buffer, preparedScript)));
}

uint64_t OfficeChakraCoreRuntime::s_version = 0;
std::once_flag OfficeChakraCoreRuntime::s_versionInitFlag;

} // namespace chakra
} // namespace jsi
} // namespace facebook
