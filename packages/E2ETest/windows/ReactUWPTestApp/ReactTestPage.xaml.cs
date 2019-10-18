// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using react.uwp;
using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace ReactUWPTestApp
{
  public sealed partial class ReactTestPage : Page
  {
#if BUNDLE
    private static readonly string JSFileName = "index.windows";
#else   
    private static readonly string JSFileName = "app/index";
#endif
    private static readonly string JSComponentName = "ReactUWPTestApp";
    public ReactTestPage()
    {
      this.InitializeComponent();
      AutomationProperties.SetAutomationId(this, "ReactTestPage");
      LoadReact();
    }

    private void LoadReact()
    {
      InstanceSettings settings;
#if BUNDLE
      settings.UseWebDebugger = false;
      settings.UseLiveReload = false;
#else
      settings.UseWebDebugger = true;
      settings.UseLiveReload = true;
#endif
      settings.EnableDeveloperMenu = false;
      var instance = Instance.Create(JSFileName);

      instance.Start(settings);

      RootElement.Instance = instance;

      RootElement.InitialProps = "{}";
      RootElement.JsComponentName = JSComponentName;
      RootElement.StartRender();
    }

    private void Reload_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
    {
      RootElement.StopRender();
      LoadReact();
    }
  }
}
