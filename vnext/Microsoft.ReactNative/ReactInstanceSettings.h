// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactInstanceSettings.g.h"

#if _DEBUG
#define REACT_DEFAULT_USE_DEVELOPER_SUPPORT true
#define REACT_DEFAULT_USE_WEB_DEBUGGER true
#define REACT_DEFAULT_USE_FAST_REFRESH true
#define REACT_DEFAULT_USE_LIVE_RELOAD false
#define REACT_DEFAULT_ENABLE_DEVELOPER_MENU true
#else
#define REACT_DEFAULT_USE_DEVELOPER_SUPPORT false
#define REACT_DEFAULT_USE_WEB_DEBUGGER false
#define REACT_DEFAULT_USE_FAST_REFRESH false
#define REACT_DEFAULT_USE_LIVE_RELOAD false
#define REACT_DEFAULT_ENABLE_DEVELOPER_MENU false
#endif // _DEBUG

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactInstanceSettings : ReactInstanceSettingsT<ReactInstanceSettings> {
  ReactInstanceSettings() noexcept;

  IReactPropertyBag Properties() noexcept;

  hstring MainComponentName() noexcept;
  void MainComponentName(hstring const &value) noexcept;

  bool UseDeveloperSupport() noexcept;
  void UseDeveloperSupport(bool value) noexcept;

  hstring JavaScriptMainModuleName() noexcept;
  void JavaScriptMainModuleName(hstring const &value) noexcept;

  hstring JavaScriptBundleFile() noexcept;
  void JavaScriptBundleFile(hstring const &value) noexcept;

  bool UseWebDebugger() noexcept;
  void UseWebDebugger(bool value) noexcept;

  bool UseFastRefresh() noexcept;
  void UseFastRefresh(bool value) noexcept;

  bool UseLiveReload() noexcept;
  void UseLiveReload(bool value) noexcept;

  bool UseDirectDebugger() noexcept;
  void UseDirectDebugger(bool value) noexcept;

  bool DebuggerBreakOnNextLine() noexcept;
  void DebuggerBreakOnNextLine(bool value) noexcept;

  bool UseJsi() noexcept;
  void UseJsi(bool value) noexcept;

  bool EnableJITCompilation() noexcept;
  void EnableJITCompilation(bool value) noexcept;

  bool EnableByteCodeCaching() noexcept;
  void EnableByteCodeCaching(bool value) noexcept;

  bool EnableDeveloperMenu() noexcept;
  void EnableDeveloperMenu(bool value) noexcept;

  hstring ByteCodeFileUri() noexcept;
  void ByteCodeFileUri(hstring const &value) noexcept;

  hstring DebugHost() noexcept;
  void DebugHost(hstring const &value) noexcept;

  hstring DebugBundlePath() noexcept;
  void DebugBundlePath(hstring const &value) noexcept;

  hstring BundleRootPath() noexcept;
  void BundleRootPath(hstring const &value) noexcept;

  uint16_t DebuggerPort() noexcept;
  void DebuggerPort(uint16_t value) noexcept;

  IRedBoxHandler RedBoxHandler() noexcept;
  void RedBoxHandler(IRedBoxHandler const &value) noexcept;

  IReactDispatcher UIDispatcher() noexcept;
  void UIDispatcher(IReactDispatcher const &value) noexcept;

  JSIEngine JSIEngineOverride() noexcept;
  void JSIEngineOverride(JSIEngine value) noexcept;

 private:
  IReactPropertyBag m_properties{ReactPropertyBagHelper::CreatePropertyBag()};
  hstring m_mainComponentName{};
  bool m_useDeveloperSupport{REACT_DEFAULT_USE_DEVELOPER_SUPPORT};
  hstring m_javaScriptMainModuleName{};
  hstring m_javaScriptBundleFile{};
  bool m_useWebDebugger{REACT_DEFAULT_USE_WEB_DEBUGGER};
  bool m_useFastRefresh{REACT_DEFAULT_USE_FAST_REFRESH};
  bool m_useLiveReload{REACT_DEFAULT_USE_LIVE_RELOAD};
  bool m_useDirectDebugger{false};
  bool m_debuggerBreakOnNextLine{false};
  bool m_useJsi{true};
  bool m_enableJITCompilation{true};
  bool m_enableByteCodeCaching{false};
  bool m_enableDeveloperMenu{REACT_DEFAULT_ENABLE_DEVELOPER_MENU};
  hstring m_byteCodeFileUri{};
  hstring m_debugHost{};
  hstring m_debugBundlePath{};
  hstring m_bundleRootPath{};
  uint16_t m_debuggerPort{9229};
  IRedBoxHandler m_redBoxHandler{nullptr};
  JSIEngine m_jSIEngineOverride{JSIEngine::Chakra};
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactInstanceSettings : ReactInstanceSettingsT<ReactInstanceSettings, implementation::ReactInstanceSettings> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation

namespace winrt::Microsoft::ReactNative::implementation {

//=============================================================================================
// ReactInstanceSettings inline implementation
//=============================================================================================

inline IReactPropertyBag ReactInstanceSettings::Properties() noexcept {
  return m_properties;
}

inline hstring ReactInstanceSettings::MainComponentName() noexcept {
  return m_mainComponentName;
}

inline void ReactInstanceSettings::MainComponentName(hstring const &value) noexcept {
  m_mainComponentName = value;
}

inline bool ReactInstanceSettings::UseDeveloperSupport() noexcept {
  return m_useDeveloperSupport;
}

inline void ReactInstanceSettings::UseDeveloperSupport(bool value) noexcept {
  m_useDeveloperSupport = value;
}

inline hstring ReactInstanceSettings::JavaScriptMainModuleName() noexcept {
  return m_javaScriptMainModuleName;
}

inline void ReactInstanceSettings::JavaScriptMainModuleName(hstring const &value) noexcept {
  m_javaScriptMainModuleName = value;
}

inline hstring ReactInstanceSettings::JavaScriptBundleFile() noexcept {
  return m_javaScriptBundleFile;
}

inline void ReactInstanceSettings::JavaScriptBundleFile(hstring const &value) noexcept {
  m_javaScriptBundleFile = value;
}

inline bool ReactInstanceSettings::UseWebDebugger() noexcept {
  return m_useWebDebugger;
}

inline void ReactInstanceSettings::UseWebDebugger(bool value) noexcept {
  m_useWebDebugger = value;
}

inline bool ReactInstanceSettings::UseFastRefresh() noexcept {
  return m_useFastRefresh;
}

inline void ReactInstanceSettings::UseFastRefresh(bool value) noexcept {
  m_useFastRefresh = value;
}

inline bool ReactInstanceSettings::UseLiveReload() noexcept {
  return m_useLiveReload;
}

inline void ReactInstanceSettings::UseLiveReload(bool value) noexcept {
  m_useLiveReload = value;
}

inline bool ReactInstanceSettings::UseDirectDebugger() noexcept {
  return m_useDirectDebugger;
}

inline void ReactInstanceSettings::UseDirectDebugger(bool value) noexcept {
  m_useDirectDebugger = value;
}

inline bool ReactInstanceSettings::DebuggerBreakOnNextLine() noexcept {
  return m_debuggerBreakOnNextLine;
}

inline void ReactInstanceSettings::DebuggerBreakOnNextLine(bool value) noexcept {
  m_debuggerBreakOnNextLine = value;
}

inline bool ReactInstanceSettings::UseJsi() noexcept {
  return m_useJsi;
}

inline void ReactInstanceSettings::UseJsi(bool value) noexcept {
  m_useJsi = value;
}

inline bool ReactInstanceSettings::EnableJITCompilation() noexcept {
  return m_enableJITCompilation;
}

inline void ReactInstanceSettings::EnableJITCompilation(bool value) noexcept {
  m_enableJITCompilation = value;
}

inline bool ReactInstanceSettings::EnableByteCodeCaching() noexcept {
  return m_enableByteCodeCaching;
}

inline void ReactInstanceSettings::EnableByteCodeCaching(bool value) noexcept {
  m_enableByteCodeCaching = value;
}

inline bool ReactInstanceSettings::EnableDeveloperMenu() noexcept {
  return m_enableDeveloperMenu;
}

inline void ReactInstanceSettings::EnableDeveloperMenu(bool value) noexcept {
  m_enableDeveloperMenu = value;
}

inline hstring ReactInstanceSettings::ByteCodeFileUri() noexcept {
  return m_byteCodeFileUri;
}

inline void ReactInstanceSettings::ByteCodeFileUri(hstring const &value) noexcept {
  m_byteCodeFileUri = value;
}

inline hstring ReactInstanceSettings::DebugHost() noexcept {
  return m_debugHost;
}

inline void ReactInstanceSettings::DebugHost(hstring const &value) noexcept {
  m_debugHost = value;
}

inline hstring ReactInstanceSettings::DebugBundlePath() noexcept {
  return m_debugBundlePath;
}

inline void ReactInstanceSettings::DebugBundlePath(hstring const &value) noexcept {
  m_debugBundlePath = value;
}

inline hstring ReactInstanceSettings::BundleRootPath() noexcept {
  return m_bundleRootPath;
}

inline void ReactInstanceSettings::BundleRootPath(hstring const &value) noexcept {
  m_bundleRootPath = value;
}

inline uint16_t ReactInstanceSettings::DebuggerPort() noexcept {
  return m_debuggerPort;
}

inline void ReactInstanceSettings::DebuggerPort(uint16_t value) noexcept {
  m_debuggerPort = value;
}

inline IRedBoxHandler ReactInstanceSettings::RedBoxHandler() noexcept {
  return m_redBoxHandler;
}

inline void ReactInstanceSettings::RedBoxHandler(IRedBoxHandler const &value) noexcept {
  m_redBoxHandler = value;
}

inline JSIEngine ReactInstanceSettings::JSIEngineOverride() noexcept {
  return m_jSIEngineOverride;
}

inline void ReactInstanceSettings::JSIEngineOverride(JSIEngine value) noexcept {
  m_jSIEngineOverride = value;
}

} // namespace winrt::Microsoft::ReactNative::implementation
