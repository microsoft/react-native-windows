// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <cxxreact/CxxModule.h>
#include <cxxreact/CxxNativeModule.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/JsBundleType.h>
#include <cxxreact/ReactMarker.h>
#include <folly/Bits.h>
#include <folly/json.h>
#include <jsi/jsi.h>
#include <jsireact/JSIExecutor.h>
#include <filesystem>
#include "OInstance.h"
#include "Unicode.h"

#include "JSI/RuntimeHolder.h"

#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/ModuleRegistry.h>

#if (defined(_MSC_VER) && (defined(WINRT)))
#include <Utils/LocalBundleReader.h>
#endif

#include <werapi.h>
#include <BatchingMessageQueueThread.h>
#include <CppRuntimeOptions.h>
#include <CreateModules.h>
#include <DevSettings.h>
#include <DevSupportManager.h>
#include <IReactRootView.h>
#include <ReactPropertyBag.h>
#include <SchedulerSettings.h>
#include <Shlwapi.h>
#include <safeint.h>
#include "Inspector/ReactInspectorThread.h"
#include "PackagerConnection.h"
#include "Threading/MessageDispatchQueue.h"

#if defined(ENABLE_DEVSERVER_HBCBUNDLES)
#include <hermes/BytecodeVersion.h>
#endif
#include "HermesRuntimeHolder.h"

#if defined(USE_V8)
#include <JSI/V8RuntimeHolder.h>
#include "V8JSIRuntimeHolder.h"
#endif
#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/TurboModuleBinding.h>
#include <react/renderer/runtimescheduler/RuntimeScheduler.h>
#include <react/renderer/runtimescheduler/RuntimeSchedulerBinding.h>
#include <react/renderer/runtimescheduler/RuntimeSchedulerCallInvoker.h>
#include "BaseScriptStoreImpl.h"

#include <tracing/tracing.h>
namespace fs = std::filesystem;

using namespace facebook;
using namespace Microsoft::JSI;

using std::make_shared;
using winrt::Microsoft::ReactNative::ReactPropertyBagHelper;


namespace {

uint32_t getPageSize() noexcept {
  SYSTEM_INFO systemInfo;
  GetSystemInfo(&systemInfo);
  return systemInfo.dwPageSize;
}

} // namespace


namespace Microsoft::ReactNative {

// Note: Based on
// https://github.com/facebook/react-native/blob/24d91268b64c7abbd4b26547ffcc663dc90ec5e7/ReactCommon/cxxreact/Instance.cpp#L112
bool isHBCBundle(const std::string &bundle) {
  static uint32_t constexpr HBCBundleMagicNumber = 0xffe7c3c3;

  // Note:: Directly access the pointer to avoid copy/length-check. It matters as this string contains the bundle which
  // can be potentially huge.
  // https://herbsutter.com/2008/04/07/cringe-not-vectors-are-guaranteed-to-be-contiguous/#comment-483
  auto header = reinterpret_cast<const facebook::react::BundleHeader *>(&bundle[0]);
  if (HBCBundleMagicNumber == header->magic32.value) {
    return true;
  } else {
    return false;
  }
}

void LoadRemoteUrlScript(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    std::shared_ptr<facebook::react::IDevSupportManager> devManager,
    std::string &&jsBundleRelativePath,
    std::function<void(std::unique_ptr<const facebook::react::JSBigStdString> script, const std::string &sourceURL)>
        fnLoadScriptCallback) noexcept {
  // First attempt to get download the Js locally, to catch any bundling
  // errors before attempting to load the actual script.

  uint32_t hermesBytecodeVersion = 0;
#if defined(ENABLE_DEVSERVER_HBCBUNDLES)
  hermesBytecodeVersion = ::hermes::hbc::BYTECODE_VERSION;
#endif

  auto [jsBundleString, success] = GetJavaScriptFromServer(
      devSettings->sourceBundleHost,
      devSettings->sourceBundlePort,
      devSettings->debugBundlePath.empty() ? jsBundleRelativePath : devSettings->debugBundlePath,
      devSettings->platformName,
      devSettings->bundleAppId,
      devSettings->devBundle,
      devSettings->useFastRefresh,
      devSettings->inlineSourceMap,
      hermesBytecodeVersion);

  if (!success) {
    devSettings->errorCallback(jsBundleString);
    return;
  }

  int64_t currentTimeInMilliSeconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count();

  auto bundleUrl = facebook::react::DevServerHelper::get_BundleUrl(
      devSettings->sourceBundleHost,
      devSettings->sourceBundlePort,
      devSettings->debugBundlePath.empty() ? jsBundleRelativePath : devSettings->debugBundlePath,
      devSettings->platformName,
      devSettings->bundleAppId,
      devSettings->devBundle,
      devSettings->useFastRefresh,
      devSettings->inlineSourceMap,
      hermesBytecodeVersion);

  // This code is based on the HBC Bundle integration on Android
  // Ref:
  // https://github.com/facebook/react-native/blob/24d91268b64c7abbd4b26547ffcc663dc90ec5e7/ReactAndroid/src/main/jni/react/jni/CatalystInstanceImpl.cpp#L231
  if (isHBCBundle(jsBundleString)) {
    auto script = std::make_unique<facebook::react::JSBigStdString>(jsBundleString, false);
    const char *buffer = script->c_str();
    uint32_t bufferLength = (uint32_t)script->size();

    // Please refer the code here for details on the file format:
    // https://github.com/facebook/metro/blob/b1bacf52070be62872d6bd3420f37a4405ed34e6/packages/metro/src/lib/bundleToBytecode.js#L29
    // Essentially, there is an 8 byte long file header with 4 bytes of a magic number followed by 4 bytes to encode
    // the number of modules.The module buffers follows, each one starts with 4 byte header which encodes module
    // length.A properly formatted HBCB should have at least 8 bytes..
    uint32_t offset = 8;
#define __SAFEADD__(s1, s2, t)             \
  if (!msl::utilities::SafeAdd(s1, s2, t)) \
    break;
    while (offset < bufferLength) {
      uint32_t segment;
      __SAFEADD__(offset, 4, segment)
      uint32_t moduleLength = (bufferLength < segment) ? 0 : *(((uint32_t *)buffer) + offset / 4);

      // Early break if the module length is computed as 0.. as the segment start may be overflowing the buffer.
      if (moduleLength == 0)
        break;

      uint32_t segmentEnd;
      __SAFEADD__(moduleLength, segment, segmentEnd)
      // Early break if the segment overflows beyond the buffer. This is unlikely for a properly formatted
      // HBCB though.
      if (segmentEnd > bufferLength)
        break;

      fnLoadScriptCallback(
          std::make_unique<const facebook::react::JSBigStdString>(std::string(buffer + segment, buffer + segmentEnd)),
          bundleUrl);

      // Aligned at 4 byte boundary.
      offset += ((moduleLength + 3) & ~3) + 4;
    }
#undef __SAFEADD__
  } else {
    // Remote debug executor loads script from a Uri, rather than taking the actual bundle string
    fnLoadScriptCallback(std::make_unique<const facebook::react::JSBigStdString>(jsBundleString), bundleUrl);
  }
}


// We only support files whose size can fit within an uint32_t. Memory
// mapping an empty or a larger file fails. If the file contents are not
// null-terminated, file is copied into a dynamically  allocated,
// page-file backed,  null-terminated buffer
class FileMappingBigString : public facebook::react::JSBigString {
 public:
  FileMappingBigString(const std::wstring &filename, uint32_t offset = 0);

  bool isAscii() const override {
    return false;
  }

  const char *c_str() const override {
    return m_data.get();
  }

  size_t size() const override {
    return m_size;
  }

  // Returns nullptr when memory mapping failed
  void *file_data() const {
    return m_fileData.get();
  }

  // Returns zero when memory mapping failed
  uint32_t file_size() const {
    return m_fileSize;
  }

  static std::unique_ptr<const FileMappingBigString> fromPath(const std::wstring &filename);

 private:
  static void fileDataDeleter(void *p) {
    WerUnregisterMemoryBlock(p);
    UnmapViewOfFile(p);
  }

  std::unique_ptr<void, decltype(&CloseHandle)> m_fileMapping;
  std::unique_ptr<void, decltype(&fileDataDeleter)> m_fileData;
  std::unique_ptr<char, void (*)(void *)> m_data;
  uint32_t m_fileSize;
  uint32_t m_size;
};


FileMappingBigString::FileMappingBigString(const std::wstring &filename, uint32_t offset)
    : JSBigString{},
      m_fileMapping{nullptr, &CloseHandle},
      m_fileData{nullptr, &fileDataDeleter},
      m_data{nullptr, nullptr},
      m_size{0},
      m_fileSize{0} {
#if (defined(WINRT))
  std::unique_ptr<void, decltype(&CloseHandle)> fileHandle{
      CreateFile2(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr /* pCreateExParams */),
      &CloseHandle};
#else
  std::unique_ptr<void, decltype(&CloseHandle)> fileHandle{
      CreateFileW(
          filename.c_str(),
          GENERIC_READ,
          FILE_SHARE_READ,
          nullptr /* lpSecurityAttributes */,
          OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL,
          nullptr /* hTemplateFile */),
      &CloseHandle};
#endif

  if (fileHandle.get() == INVALID_HANDLE_VALUE) {
    return;
  }

  LARGE_INTEGER size;
  if (!GetFileSizeEx(fileHandle.get(), &size)) {
    return;
  }

  if (size.LowPart == 0 || size.HighPart != 0) {
    return;
  }

  m_fileSize = size.LowPart;
  if (offset > m_fileSize) {
    offset = m_fileSize;
  }
  m_size = m_fileSize - offset;

#if (defined(WINRT))
  m_fileMapping.reset(CreateFileMappingFromApp(
      fileHandle.get(), nullptr /* SecurityAttributes */, PAGE_READONLY, m_fileSize, nullptr /* Name */));
#else
  m_fileMapping.reset(CreateFileMapping(
      fileHandle.get(),
      nullptr /* lpAttributes */,
      PAGE_READONLY,
      0 /* dwMaximumSizeHigh */,
      m_fileSize,
      nullptr /* lpName */));
#endif

  if (!m_fileMapping) {
    m_size = 0;
    return;
  }

#if (defined(WINRT))
  m_fileData.reset(
      MapViewOfFileFromApp(m_fileMapping.get(), FILE_MAP_READ, 0 /* FileOffset */, 0 /* NumberOfBytesToMap */));
#else
  m_fileData.reset(MapViewOfFile(
      m_fileMapping.get(),
      FILE_MAP_READ,
      0 /* dwFileOffsetHigh */,
      0 /* dwFileOffsetLow */,
      0 /* dwNumberOfBytesToMap */));
#endif

  if (!m_fileData) {
    m_size = 0;
    return;
  }

  WerRegisterMemoryBlock(m_fileData.get(), m_fileSize);

  static const uint32_t s_pageSize = getPageSize();
  if (m_fileSize % s_pageSize != 0) {
    // Data are owned by m_fileData, deleter is no-op
    m_data = decltype(m_data){static_cast<char *>(m_fileData.get()) + offset, [](void *) {}};
  } else {
    // Ensure m_data is null-terminated
    m_data = decltype(m_data){
        new char[m_size + 1],
        [](void *pv) // Can't just say &operator delete[] because of calling
                     // convention mismatches
        { delete[] static_cast<char *>(pv); }};
    memcpy(m_data.get(), static_cast<char *>(m_fileData.get()) + offset, m_size);
    m_data.get()[m_size] = '\0';
  }
}

std::unique_ptr<const FileMappingBigString> FileMappingBigString::fromPath(const std::wstring &filename) {
  return std::make_unique<FileMappingBigString>(filename);
}

std::unique_ptr<const facebook::react::JSBigString> JsBigStringFromPath(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    const std::string &jsBundleRelativePath) noexcept {
#if (defined(_MSC_VER) && !defined(WINRT))
  std::wstring bundlePath = (fs::u8path(devSettings->bundleRootPath) / jsBundleRelativePath).wstring();
  return FileMappingBigString::fromPath(bundlePath);
#else
  std::wstring bundlePath;
  if (devSettings->bundleRootPath.starts_with("resource://")) {
    auto uri = winrt::Windows::Foundation::Uri(
        winrt::to_hstring(devSettings->bundleRootPath), winrt::to_hstring(jsBundleRelativePath));
    bundlePath = uri.ToString();
  } else {
    bundlePath = (fs::u8path(devSettings->bundleRootPath) / (jsBundleRelativePath + ".bundle")).wstring();
  }

  return std::make_unique<::Microsoft::ReactNative::StorageFileBigString>(bundlePath);
#endif
}

} // namespace Microsoft::ReactNative

namespace facebook::react {

void logMarker(const facebook::react::ReactMarker::ReactMarkerId /*id*/, const char * /*tag*/) {}

} // namespace facebook::react
