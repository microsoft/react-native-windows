// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// HostingPane.xaml.h
// Declaration of the HostingPane class
//

#pragma once

#include "HostingPane.g.h"

#include <ReactUWP/IXamlRootView.h>

#include <memory>
#include <string>

namespace Playground {

[Windows::Foundation::Metadata::WebHostHidden] public ref class HostingPane sealed {
 public:
  HostingPane();

  property uint32_t DebuggerPort {
    uint32_t get();
    void set(uint32_t value);
  }

  property Windows::UI::Xaml::Input::ICommand ^
      AddPaneCommand {
        void set(Windows::UI::Xaml::Input::ICommand ^ value);
        Windows::UI::Xaml::Input::ICommand ^ get();
      }

      property Windows::UI::Xaml::Input::ICommand ^
      RemovePaneCommand {
        void set(Windows::UI::Xaml::Input::ICommand ^ value);
        Windows::UI::Xaml::Input::ICommand ^ get();
      }

      private : void OnSelectionChanged_JavaScriptFilename(
                    Platform::Object ^ sender,
                    Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^ args);

  void OnSelectionChanged_ReactAppName(
      Platform::Object ^ sender,
      Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^ args);
  void OnLoadClicked(Platform::Object ^ sender, Platform::Object ^ args);
  void OnUnloadClicked(Platform::Object ^ sender, Platform::Object ^ args);
  void OnReloadClicked(Platform::Object ^ sender, Platform::Object ^ args);
  void OnBeforeTextChanging_CheckAllDigits(
      Windows::UI::Xaml::Controls::TextBox ^ sender,
      Windows::UI::Xaml::Controls::TextBoxBeforeTextChangingEventArgs ^ args);

  void DetachAndClearRoot();
  void LoadReactNative();

  void LoadFilenameSettings();
  void StoreFilenameSettings();

  void UpdateUI();

  internal : std::shared_ptr<react::uwp::IReactInstance> getInstance();
  void markAsNeedsReload();
  void persistUseWebDebugger(bool useWebDebugger);
  void persistUseLiveReload(bool useLiveReload);
  void persistUseDirectDebugger(bool useDirectDebugger);
  void persistBreakOnNextLine(bool breakOnNextLine);

  void InitComboBoxes();
  void LoadKnownApps();

 private:
  std::wstring m_loadedJSComponentName;
  std::wstring m_loadedBundleFileName;
  static constexpr uint16_t defaultDebuggerPort = 9229;
  uint16_t m_debuggerPort{defaultDebuggerPort};

  react::uwp::ReactInstanceCreator m_instanceCreator;

  std::shared_ptr<react::uwp::IXamlRootView> m_rootView;
  std::shared_ptr<react::uwp::IReactInstance> m_instance;

  Platform::Collections::Vector<Platform::String ^> ^ m_jsFileNames;
  Platform::Collections::Vector<Platform::String ^> ^ m_ReactAppNames;
};

} // namespace Playground
