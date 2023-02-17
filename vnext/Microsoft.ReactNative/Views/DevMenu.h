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
  static void Show(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept;

 private:
  void Init() noexcept;
  const Mso::CntPtr<Mso::React::IReactContext> m_context;
#ifndef CORE_ABI
  winrt::CoreDispatcher::AcceleratorKeyActivated_revoker m_coreDispatcherAKARevoker{};
  xaml::UIElement::KeyDown_revoker m_keyDownRevoker;
#endif
};

} // namespace Microsoft::ReactNative
