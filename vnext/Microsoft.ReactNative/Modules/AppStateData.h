// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Modules/AppStateModule.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include "ReactHost/React.h"
#include "activeObject/activeObject.h"

namespace react::uwp {

// AppStateData ensures that subscription and un-subscription to Application events is happening in Main UI thread.
struct AppStateData : Mso::ActiveObject<> {
  using Super = ActiveObjectType;

  AppStateData(Mso::React::IReactContext &reactContext, Mso::DispatchQueue const &uiQueue) noexcept;
  ~AppStateData() override;
  void Initialize() noexcept override;
  void Finalize() noexcept override;

  char const *GetState() noexcept;

 private:
  void RaiseEvent(char const *newState) noexcept;

 private:
  std::mutex m_stateMutex;
  char const *m_lastState{nullptr};
  Mso::CntPtr<Mso::React::IReactContext> m_reactContext;
  winrt::Windows::UI::Xaml::Application::EnteredBackground_revoker m_enteredBackgroundRevoker;
  winrt::Windows::UI::Xaml::Application::LeavingBackground_revoker m_leavingBackgroundRevoker;
};

// It is a temporary class that we need to keep until we remove ReactUWP
class AppState2 : public facebook::react::AppState {
 public:
  AppState2(Mso::React::IReactContext &reactContext, Mso::DispatchQueue const &uiQueue) noexcept;

 public: // facebook::react::AppState
  ~AppState2() override;
  char const *getState() override;

 private:
  Mso::CntPtr<AppStateData> m_data;
};

} // namespace react::uwp
