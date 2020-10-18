// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <activeObject/activeObject.h>
#include <cxxreact/CxxModule.h>
#include <eventWaitHandle/eventWaitHandle.h>

#include <winrt/Windows.UI.ViewManagement.h>

#include <React.h>
#include "IReactInstance.h"

namespace react::uwp {

// Listens for the current theme on the UI thread, storing the most recent. Will emit JS events on Appearance change.
class AppearanceChangeListener final : public Mso::ActiveObject<> {
  using ApplicationTheme = xaml::ApplicationTheme;
  using UISettings = winrt::Windows::UI::ViewManagement::UISettings;

 public:
  AppearanceChangeListener(const Mso::React::IReactContext &context, Mso::DispatchQueue const &uiQueue) noexcept;
  const char *GetColorScheme() const noexcept;

 private:
  static const char *ToString(ApplicationTheme theme) noexcept;
  void OnColorValuesChanged() noexcept;

  UISettings m_uiSettings;
  UISettings::ColorValuesChanged_revoker m_revoker;
  std::atomic<ApplicationTheme> m_currentTheme;
  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  std::weak_ptr<IReactInstance> m_weakReactInstance;
};

class AppearanceModule final : public facebook::xplat::module::CxxModule {
 public:
  static constexpr const char *Name = "Appearance";

  AppearanceModule(Mso::CntPtr<AppearanceChangeListener> &&appearanceListener) noexcept;
  std::string getName() override;
  std::vector<Method> getMethods() override;

 private:
  Mso::CntPtr<AppearanceChangeListener> m_changeListener;
};

} // namespace react::uwp
