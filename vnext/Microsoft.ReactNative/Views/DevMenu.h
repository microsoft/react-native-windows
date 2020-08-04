// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <cxxreact/CxxModule.h>
#include <functional/functor.h>

#include <NativeModules.h>
#include <ReactHost/React.h>

namespace Microsoft::ReactNative {

struct DevMenuManager : public std::enable_shared_from_this<DevMenuManager> {
  DevMenuManager(Mso::CntPtr<Mso::React::IReactContext> const &reactContext);

  static void InitDevMenu(
      Mso::CntPtr<Mso::React::IReactContext> const &reactContext,
      Mso::VoidFunctor &&configureBundler) noexcept;
  static void Show(React::IReactPropertyBag const &properties) noexcept;
  static void Hide(React::IReactPropertyBag const &properties) noexcept;

 private:
  void CreateAndShowUI() noexcept;
  void Hide() noexcept;
  void Init() noexcept;

  const Mso::CntPtr<Mso::React::IReactContext> m_context;
  xaml::Controls::Flyout m_flyout{nullptr};
  xaml::Controls::Button::Click_revoker m_remoteDebugJSRevoker{};
  xaml::Controls::Button::Click_revoker m_cancelRevoker{};
  xaml::Controls::Button::Click_revoker m_toggleInspectorRevoker{};
  xaml::Controls::Button::Click_revoker m_configBundlerRevoker{};
  xaml::Controls::Button::Click_revoker m_reloadJSRevoker{};
  xaml::Controls::Button::Click_revoker m_fastRefreshRevoker{};
  xaml::Controls::Button::Click_revoker m_directDebuggingRevoker{};
  xaml::Controls::Button::Click_revoker m_breakOnNextLineRevoker{};
  winrt::CoreDispatcher::AcceleratorKeyActivated_revoker m_coreDispatcherAKARevoker{};
  xaml::UIElement::KeyDown_revoker m_keyDownRevoker;
};

} // namespace Microsoft::ReactNative
