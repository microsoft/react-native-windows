// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

#include "HostingPane.xaml.h"
#include "Utilities/RelayCommand.h"
#include <unordered_map>
namespace Playground {

[Windows::Foundation::Metadata::WebHostHidden] public ref class MainPage sealed {
 public:
  MainPage();

 private:
  void RemovePane(HostingPane ^ pane);
  void AddPane(HostingPane ^ pane);
  void UpdatePaneCommandState();

 private:
  Platform::Collections::Vector<HostingPane ^> m_hostingPanes;

  RelayCommand ^ m_addPaneCommand;
  RelayCommand ^ m_removePaneCommand;

  protected:
  virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
};

} // namespace Playground
