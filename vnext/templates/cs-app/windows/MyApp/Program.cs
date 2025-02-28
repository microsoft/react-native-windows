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

      // Create a ReactNativeWin32App with the ReactNativeAppBuilder
      var reactNativeWin32App = new ReactNativeAppBuilder().Build();

      // Configure the initial InstanceSettings for the app's ReactNativeHost
      var settings = reactNativeWin32App.ReactNativeHost.InstanceSettings;
      // Register any autolinked native modules
      Microsoft.ReactNative.Managed.AutolinkedNativeModules.RegisterAutolinkedNativeModulePackages(settings.PackageProviders);

#if BUNDLE
      // Load the JS bundle from a file (not Metro):
      // Set the path (on disk) where the .bundle file is located
      settings.BundleRootPath = "file://" + appDirectory + "\\Bundle\\";
      // Set the name of the bundle file (without the .bundle extension)
      settings.JavaScriptBundleFile = "index.windows";
      // Disable hot reload
      settings.UseFastRefresh = false;
#else
      // Load the JS bundle from Metro
      settings.JavaScriptBundleFile = "index";
      // Enable hot reload
      settings.UseFastRefresh = true;
#endif
#if _DEBUG
      // For Debug builds
      // Enable Direct Debugging of JS
      settings.UseDirectDebugger = true;
      // Enable the Developer Menu
      settings.UseDeveloperSupport = true;
#else
      // For Release builds:
      // Disable Direct Debugging of JS
      settings.UseDirectDebugger = false;
      // Disable the Developer Menu
      settings.UseDeveloperSupport = false;
#endif

      // Get the AppWindow so we can configure its initial title and size
      var appWindow = reactNativeWin32App.AppWindow;
      appWindow.Title = "{{ mainComponentName }}";
      appWindow.Resize(new Windows.Graphics.SizeInt32(1000, 1000));

      // Get the ReactViewOptions so we can set the initial RN component to load
      var viewOptions = reactNativeWin32App.ReactViewOptions;
      viewOptions.ComponentName = "{{ mainComponentName }}";

      // Start the app
      reactNativeWin32App.Start();
    }

    private static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
    {
      Trace.TraceError($"Unhandled Exception: { (e.ExceptionObject as Exception)?.ToString() }");
    }
  }

}