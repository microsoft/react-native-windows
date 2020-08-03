// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <ReactHost/React.h>

namespace Microsoft::ReactNative {

struct ConfigureBundlerDlg : winrt::implements<ConfigureBundlerDlg, winrt::IInspectable> {
  ConfigureBundlerDlg(Mso::WeakPtr<Mso::React::IReactHost> weakReactHost);

 private:
  void CreateAndShowUI() noexcept;
  void Hide() noexcept;

  const Mso::WeakPtr<Mso::React::IReactHost> m_weakReactHost;

  xaml::Controls::Flyout m_flyout{nullptr};
  xaml::Controls::Button::Click_revoker m_applyRevoker{};
  xaml::Controls::Button::Click_revoker m_cancelRevoker{};
  xaml::Controls::Button::Click_revoker m_resetRevoker{};
};

} // namespace Microsoft::ReactNative
