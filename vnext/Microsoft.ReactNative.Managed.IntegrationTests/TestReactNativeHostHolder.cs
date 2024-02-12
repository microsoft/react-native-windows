// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Reflection;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace Microsoft.ReactNative.Managed.IntegrationTests
{
  internal class TestReactNativeHostHolder : IDisposable
  {
    public ReactNativeHost Host { get; }
    //private DispatcherQueueController m_queueController = null;
    private bool m_isDisposed;

    public struct Options
    {
      public bool NoInstanceLoad { get; }

      public Options(bool noInstanceLoad) { NoInstanceLoad = noInstanceLoad; }
    }

    public TestReactNativeHostHolder(string jsBundle, Action<ReactNativeHost> hostInitializer, Options options = default)
    {
      Host = new ReactNativeHost();
      _ = CoreApplication.MainView.CoreWindow.DispatcherQueue.TryEnqueue(() =>
      {
        // bundle is assumed to be co-located with the test binary
        var testBinaryPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

        Host.InstanceSettings.BundleRootPath = Path.GetFullPath(testBinaryPath + "\\..\\JSBundles");
        Host.InstanceSettings.JavaScriptBundleFile = jsBundle;
        Host.InstanceSettings.UseDeveloperSupport = false;
        Host.InstanceSettings.UseFastRefresh = false;
        Host.InstanceSettings.UseDeveloperSupport = false;

        hostInitializer(Host);

        if (!options.NoInstanceLoad)
        {
          _ = Host.LoadInstance();
        }
      });
    }

    public void Dispose()
    {
      // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
      Dispose(disposing: true);
      GC.SuppressFinalize(this);
    }

    protected virtual void Dispose(bool disposing)
    {
      if (!m_isDisposed)
      {
        if (disposing)
        {
          Host.UnloadInstance().AsTask().Wait();
         // m_queueController.ShutdownQueueAsync().AsTask().Wait();
        }

        m_isDisposed = true;
      }
    }

  }
}
