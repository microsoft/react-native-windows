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
    public ReactTestPage()
    {
      this.InitializeComponent();
      AutomationProperties.SetAutomationId(this, "ReactTestPage");
    }

    protected override void OnNavigatedTo(NavigationEventArgs e)
    {
      base.OnNavigatedTo(e);
      LoadReact(e.Parameter as TestDeclaration);
    }

    private void LoadReact(TestDeclaration testDeclaration)
    {
      InstanceSettings settings;
      settings.UseWebDebugger = true;
      settings.UseLiveReload = true;

      var instance = Instance.Create(testDeclaration.JSFileName);

      //instantiate sample module for registering callbacks for live reload, JS error handling etc.,
      instance.RegisterModule(new SampleModule());
      instance.Start(settings);

      RootElement.Instance = instance;

      //Setup sample initial properties for initializing UI thread
      string initialProps = "{ "
          + "\"one\":\"1\""
          + ", \"two\":\"2\""
          + "}";

      RootElement.InitialProps = initialProps;
      RootElement.JsComponentName = testDeclaration.JSComponentName;
      RootElement.StartRender();
    }
  }

  public class SampleModule : react.uwp.IModule
  {
    Dictionary<string, string> _constants;
    Dictionary<string, MethodDelegate> m_delegates = new Dictionary<string, MethodDelegate>();
    Dictionary<String, MethodWithCallbackDelegate> m_delegateWithCallback = new Dictionary<string, MethodWithCallbackDelegate>();

    public SampleModule()
    {
      _constants = new Dictionary<string, string>();
      _constants.Add("a", "\"b\"");

      m_delegates.Add("method1", new MethodDelegate(
       (string para) =>
       {

       }));

      m_delegateWithCallback.Add("method2", new MethodWithCallbackDelegate(
       (string para, MethodCallback callback) =>
       {
         string[] results = new string[] { "{ \"result\":true }" };
         callback(results);
       }));
    }

    public IReadOnlyDictionary<string, string> Constants
    {
      get
      {
        return _constants;
      }
    }

    public IReadOnlyDictionary<string, MethodDelegate> Methods
    {
      get
      {
        return m_delegates;
      }
    }

    public IReadOnlyDictionary<string, MethodWithCallbackDelegate> MethodsWithCallback
    {
      get
      {
        return m_delegateWithCallback;
      }
    }

    public string Name
    {
      get
      {
        return "SampleModule";
      }
    }
  }
}
