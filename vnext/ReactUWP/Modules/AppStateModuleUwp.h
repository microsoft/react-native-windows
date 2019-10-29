// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>
#include <Modules/AppStateModule.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>

namespace react {
namespace uwp {

class AppState : public facebook::react::AppState {
 public:
  AppState(const std::shared_ptr<IReactInstance> &reactInstance);
  virtual ~AppState();

  const char *getState() override;

 private:
  void EnteredBackground(
      winrt::Windows::Foundation::IInspectable const & /*sender*/,
      winrt::Windows::ApplicationModel::EnteredBackgroundEventArgs const & /*e*/);
  void LeavingBackground(
      winrt::Windows::Foundation::IInspectable const & /*sender*/,
      winrt::Windows::ApplicationModel::LeavingBackgroundEventArgs const & /*e*/);

  void fireEvent(const char *newState);

  const char *m_lastState;
  std::weak_ptr<IReactInstance> m_wkReactInstance;
  winrt::Windows::UI::Xaml::Application::EnteredBackground_revoker m_enteredBackgroundRevoker;
  winrt::Windows::UI::Xaml::Application::LeavingBackground_revoker m_leavingBackgroundRevoker;
};

} // namespace uwp
} // namespace react
