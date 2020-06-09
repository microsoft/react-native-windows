// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <cxxreact/CxxModule.h>
#include <functional/functor.h>

#include <NativeModules.h>
#include <ReactHost/React.h>

namespace Microsoft::ReactNative {

REACT_MODULE(DevSettings)
struct DevSettings {
  REACT_INIT(Initialize) void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(reload) void reload() noexcept;
  REACT_METHOD(reloadWithReason) void reloadWithReason(std::string reason) noexcept;
  REACT_METHOD(onFastRefresh) void onFastRefresh() noexcept;
  REACT_METHOD(setHotLoadingEnabled) void setHotLoadingEnabled(bool isHotLoadingEnabled) noexcept;
  REACT_METHOD(setIsDebuggingRemotely) void setIsDebuggingRemotely(bool isDebuggingRemotelyEnabled) noexcept;
  REACT_METHOD(setProfilingEnabled) void setProfilingEnabled(bool isProfilingEnabled) noexcept;
  REACT_METHOD(toggleElementInspector) void toggleElementInspector() noexcept;
  REACT_METHOD(addMenuItem) void addMenuItem(std::string title) noexcept;
  REACT_METHOD(setIsShakeToShowDevMenuEnabled) void setIsShakeToShowDevMenuEnabled(bool enabled) noexcept;

  REACT_METHOD(addListener)
  static void addListener(std::string eventName) noexcept;
  REACT_METHOD(removeListeners)
  static void removeListeners(double count) noexcept;

  static void SetReload(Mso::React::ReactOptions const &options, Mso::VoidFunctor &&func) noexcept;
  static void Reload(winrt::Microsoft::ReactNative::ReactPropertyBag const &properties) noexcept;

  //! Toggles the element inspector UI, allowing visual inspection of the react UI
  static void ToggleElementInspector(Mso::React::IReactContext &reactContext) noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
