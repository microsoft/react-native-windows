// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Modules/ThemingModule.h>
#include <IReactInstance.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>

namespace react {
  namespace uwp {

    class Theming : public facebook::react::Theming
    {
    public:
      Theming(const std::shared_ptr<IReactInstance>& reactInstance);
      virtual ~Theming();

      const char* getTheme() override;
      const char* getHighContrast() override;
      const char* highContrastChanged() override;

    private:
      void EnteredLightMode(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Windows::UI::Xaml::VisualStateChangedEventArgs const& /*e*/);
      void EnteredDarkMode(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Windows::UI::Xaml::VisualStateChangedEventArgs const& /*e*/);
      void HighContrastChanged(winrt::Windows::Foundation::IInspectable const& /*sender*/ /*e*/);

      void fireThemeEvent(const char* newTheme);
      void fireHighContrastEvent(const char* newHighContrastState);

      const char* m_lastTheme;
      const char* m_lastHighContrastState;
      std::weak_ptr<IReactInstance> m_wkReactInstance;
      winrt::Windows::UI::ViewManagement::AccessibilitySettings::HighContrastChanged_revoker m_highContrastChangedRevoker;
    };

  }
} // namespace react::uwp
