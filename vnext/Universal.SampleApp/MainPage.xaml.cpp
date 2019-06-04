// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"

#include "MainPage.xaml.h"

#include "HostingPane.xaml.h"

#include <ReactUWP/ReactUwp.h>

#include <Windows.ApplicationModel.Core.h>
#include <Windows.UI.Xaml.h>
#include <Windows.UI.Xaml.Controls.h>

using namespace WindowsSampleApp;

using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

MainPage::MainPage()
{
  InitializeComponent();

  // Create Commands
  m_addPaneCommand = ref new RelayCommand(
    [this](Object^ parameter) { return x_PaneContainer->ColumnDefinitions->Size < 3; },
    [this](Object^ parameter) { this->AddPane(dynamic_cast<HostingPane^>(parameter)); });

  m_removePaneCommand = ref new RelayCommand(
    [this](Object^ parameter) { return x_PaneContainer->ColumnDefinitions->Size > 1; },
    [this](Object^ parameter) { this->RemovePane(dynamic_cast<HostingPane^>(parameter)); });

  AddPane(nullptr);
}

void WindowsSampleApp::MainPage::RemovePane(HostingPane^ pane)
{
  unsigned int index;

  if (x_PaneContainer->ColumnDefinitions->Size <= 1)
  {
    assert(false);
  }
  else if (x_PaneContainer->Children->IndexOf(pane, &index))
  {
    // Remove the Pane from the Grid
    x_PaneContainer->Children->RemoveAt(index);

    // Remove the Column that was hosting the Pane
    x_PaneContainer->ColumnDefinitions->RemoveAt(index);

    // Decrement Column values on Panes after this one
    unsigned int countColumns = x_PaneContainer->ColumnDefinitions->Size;
    for (;index < countColumns; ++index)
    {
      auto hostingPane  = dynamic_cast<HostingPane^>(x_PaneContainer->Children->GetAt(index));
      if (hostingPane != nullptr)
        hostingPane->SetValue(Grid::ColumnProperty, index);
      else
        assert(false);
    }

    // Ensure that any pane based commanding UI updates for these changes
    UpdatePaneCommandState();
  }
  else
  {
    assert(false);
  }
}

void WindowsSampleApp::MainPage::AddPane(HostingPane^ afterPane)
{
  unsigned int countColumns = x_PaneContainer->ColumnDefinitions->Size;
  if (countColumns < 3)
  {
    // Where to insert? After the pane passed in. At start if not passed or not found.
    unsigned int index = 0;
    if ((afterPane != nullptr) && x_PaneContainer->Children->IndexOf(afterPane, &index))
      ++index;

    // Add ColumnDefinition for Column that will host the Pane
    auto columnDefinition = ref new ColumnDefinition();
    columnDefinition->Width = GridLength(1.0, GridUnitType::Star);
    x_PaneContainer->ColumnDefinitions->InsertAt(index, columnDefinition);
    ++countColumns;

    // Create Pane and insert into the Grid
    HostingPane^ newPane = ref new HostingPane();
    newPane->AddPaneCommand = m_addPaneCommand;
    newPane->RemovePaneCommand = m_removePaneCommand;
    x_PaneContainer->Children->InsertAt(index, newPane);

    // Set the column indexes on all columns from this one to the end
    newPane->SetValue(Grid::ColumnProperty, index);
    for (unsigned int i = index + 1; i < countColumns; ++i)
    {
      auto currentPane = dynamic_cast<HostingPane^>(x_PaneContainer->Children->GetAt(i));
      currentPane->SetValue(Grid::ColumnProperty, i);
    }

    // Ensure that any pane based commanding UI updates for these changes
    UpdatePaneCommandState();
  }
  else
  {
    assert(false);
  }
}

void MainPage::UpdatePaneCommandState()
{
  // Ensure all pane commands update their CanExecute status
  for (unsigned int i = 0, c = x_PaneContainer->Children->Size; i < c; ++i)
  {
    auto currentPane = dynamic_cast<HostingPane^>(x_PaneContainer->Children->GetAt(i));

    auto addPaneCommand = dynamic_cast<RelayCommand^>(currentPane->AddPaneCommand);
    addPaneCommand->RaiseCanExecuteChanged();

    auto removePaneCommand = dynamic_cast<RelayCommand^>(currentPane->RemovePaneCommand);
    removePaneCommand->RaiseCanExecuteChanged();
  }
}
