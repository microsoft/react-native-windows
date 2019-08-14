#pragma once

#include "ReactInstanceSettings.g.h"

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactInstanceSettings : ReactInstanceSettingsT<ReactInstanceSettings> {
  ReactInstanceSettings() = default;

  bool UseWebDebugger() {
    return m_useWebDebugger;
  }
  void UseWebDebugger(bool value) {
    m_useWebDebugger = value;
  }

  bool UseLiveReload() {
    return m_useLiveReload;
  }
  void UseLiveReload(bool value) {
    m_useLiveReload = value;
  }

  bool UseDirectDebugger() {
    return m_useDirectDebugger;
  }
  void UseDirectDebugger(bool value) {
    m_useDirectDebugger = value;
  }

  bool UseJsi() {
    return m_useJsi;
  }
  void UseJsi(bool value) {
    m_useJsi = value;
  }

  bool EnableJITCompilation() {
    return m_enableJITCompilation;
  }
  void EnableJITCompilation(bool value) {
    m_enableJITCompilation = value;
  }

  bool EnableByteCodeCaching() {
    return m_enableByteCodeCaching;
  }
  void EnableByteCodeCaching(bool value) {
    m_enableByteCodeCaching = value;
  }

  hstring ByteCodeFileUri() {
    return m_byteCodeFileUri;
  }
  void ByteCodeFileUri(hstring const &value) {
    m_byteCodeFileUri = value;
  }

  hstring DebugHost() {
    return m_debugHost;
  }
  void DebugHost(hstring const &value) {
    m_debugHost = value;
  }

  hstring DebugBundlePath() {
    return m_debugBundlePath;
  }
  void DebugBundlePath(hstring const &value) {
    m_debugBundlePath = value;
  }

  hstring BundleRootPath() {
    return m_bundleRootPath;
  }
  void BundleRootPath(hstring const &value) {
    m_bundleRootPath = value;
  }

 private:
#if _DEBUG
  // TODO: Discuss whether this is where we should have these set by default
  //       versus in the new project template.  This makes it easier for a
  //       debug build to work out of the box.
  bool m_useWebDebugger{TRUE};
  bool m_useLiveReload{TRUE};
#else
  bool m_useWebDebugger{FALSE};
  bool m_useLiveReload{FALSE};
#endif
  bool m_useDirectDebugger{FALSE};
  bool m_useJsi{TRUE};
  bool m_enableJITCompilation{TRUE};
  bool m_enableByteCodeCaching{FALSE};

  hstring m_byteCodeFileUri{};
  hstring m_debugHost{};
  hstring m_debugBundlePath{};
  hstring m_bundleRootPath{};
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactInstanceSettings : ReactInstanceSettingsT<
                                   ReactInstanceSettings,
                                   implementation::ReactInstanceSettings> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
