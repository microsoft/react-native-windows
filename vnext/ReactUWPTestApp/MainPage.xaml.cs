// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;

namespace ReactUWPTestApp
{
  public sealed partial class MainPage : Page
  {
    public MainPage()
    {
      this.InitializeComponent();

      AutomationProperties.SetAutomationId(this, "MainPage");
      Loaded += OnLoaded;
    }

    protected override AutomationPeer OnCreateAutomationPeer()
    {
      return new FrameworkElementAutomationPeer(this);
    }

    private void OnLoaded(object sender, RoutedEventArgs e)
    {
      var automationPeer = new FrameworkElementAutomationPeer(this);
      automationPeer.RaiseAutomationEvent(AutomationEvents.AsyncContentLoaded);
      itemsControl.ItemsSource = Tests;
      NavigateToTestCommand.Frame = this.Frame;
    }

    public List<TestDeclaration> Tests
    {
      get { return TestInventory.Tests; }
    }
  }
}
