// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using System.Reflection;
using react.uwp;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace WindowsSampleCSharpApp
{
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

  /// <summary>
  /// An empty page that can be used on its own or navigated to within a Frame.
  /// </summary>
  public sealed partial class MainPage : Page
  {
    public MainPage()
    {
      this.InitializeComponent();

      LoadFilenameSettings();
    }

    private void OnReloadClicked(object sender, RoutedEventArgs args)
    {
      LoadReact();
    }

    // TODO: Update this app to have same UI and loading code as WindowsSampleApp
    private void LoadReact()
    {
      StoreFilenameSettings();

      InstanceSettings settings;
      settings.UseWebDebugger = x_UseWebDebuggerCheckBox.IsChecked.Value;
      settings.UseLiveReload = x_UseLiveReloadCheckBox.IsChecked.Value;
      var instance = Instance.Create(x_JavaScriptFilename.Text /*, modules*/);
      instance.RegisterModule(new SampleModule());
      instance.Start(settings);

      x_RootElement.Instance = instance;

      string initialProps = "{ "
                + "\"accountUpn\":\"user@contoso.com\""
                + ", \"accountType\":\"OrgId\""
                + "}";

      x_RootElement.InitialProps = initialProps;

      x_RootElement.JsComponentName = x_ReactAppName.Text;

      x_RootElement.StartRender();
    }

    const string c_containerName = "js";
    const string c_filenameSetting = "filename";
    const string c_appnameSetting = "appname";
    const string c_filenameSettingDefault = "Universal.SampleApp\\index.uwp";
    const string c_appnameSettingDefault = "Bootstrap";

    static public Windows.Storage.ApplicationDataContainer GetJsSettings()
    {
      var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
      var container = localSettings.CreateContainer(
        c_containerName,
        Windows.Storage.ApplicationDataCreateDisposition.Always);

      return container;
    }

    public void LoadFilenameSettings()
    {
      var container = GetJsSettings();

      string filename = c_filenameSettingDefault;
      if (container.Values.ContainsKey(c_filenameSetting))
      {
        filename = container.Values[c_filenameSetting].ToString();
      }

      string appname = c_appnameSettingDefault;
      if (container.Values.ContainsKey(c_appnameSetting))
      {
        appname = container.Values[c_appnameSetting].ToString();
      }

      x_JavaScriptFilename.Text = filename;
      x_ReactAppName.Text = appname;
    }

    public void StoreFilenameSettings()
    {
      var container = GetJsSettings();

      container.Values[c_filenameSetting] = x_JavaScriptFilename.Text;
      container.Values[c_appnameSetting] = x_ReactAppName.Text;
    }
  }
}
