// {{ name }}.cs : Defines the entry point for the application.
//

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;

using Microsoft.ReactNative;

namespace {{ namespace }}
{
  class Program
  {
    static void Main(string[] args)
    {
      AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;

      var appDirectory = System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

      var reactInstanceSettingsBuilder = new ReactInstanceSettingsBuilder()
        .DebugBundlePath("index")
        .JavaScriptBundleFile("index.windows")
        .BundleRootPath(appDirectory)
#if BUNDLE
        .UseFastRefresh(false)
#else
        .UseFastRefresh(true)
#endif
#if _DEBUG
        .UseDirectDebugger(true)
        .UseDeveloperSupport(true);
#else
        .UseDirectDebugger(false)
        .UseDeveloperSupport(false);
#endif

      var reactPackageProviders = new List<IReactPackageProvider>();
      AutolinkedNativeModules.RegisterAutolinkedNativeModulePackages(reactPackageProviders);

      var viewOptions = new ReactViewOptions();
      viewOptions.ComponentName = "{{ mainComponentName }}";

      // Initialize and Manage the ReactNativeHost
      var reactNativeAppBuilder = new ReactNativeAppBuilder()
        .AddPackageProviders(reactPackageProviders)
        .SetReactInstanceSettings(reactInstanceSettingsBuilder.ReactInstanceSettings)
        .SetReactViewOptions(viewOptions);

      // Start the react-native instance by creating a javascript runtime and load the bundle.
      var reactNativeWin32App = reactNativeAppBuilder.Build();

      reactNativeWin32App.Start();
    }

    private static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
    {
      Trace.TraceError($"Unhandled Exception: { (e.ExceptionObject as Exception)?.ToString() }");
    }
  }

}