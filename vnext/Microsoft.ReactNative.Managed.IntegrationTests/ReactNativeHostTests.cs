// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Windows.Foundation;

namespace Microsoft.ReactNative.Managed.IntegrationTests
{
  [ReactModule]
  class TestHostModule
  {
    [ReactInitializer]
    public void Initialize(ReactContext context)
    {
      TestEventService.LogEvent("TestHostModule.Initialize", JSValue.Null);
    }

    [ReactFunction("addValues", ModuleName = "TestHostModuleFunctions")]
    public Action<int, int> AddValues { get; set; }

    [ReactMethod("startTests")]
    public void StartTests()
    {
      TestEventService.LogEvent("TestHostModule.StartTests", JSValue.Null);

      TestEventService.LogEvent("call addValues", new JSValueArray { 4, 7 });
      AddValues(4, 7);
    }

    [ReactMethod("returnResult")]
    public void ReturnResult(JSValue value)
    {
      TestEventService.LogEvent("TestHostModule.ReturnResult", value);
    }
  }

  [TestClass]
  public class ReactNativeHostTests
  {
    [TestInitialize]
    public void Initialize()
    {
      TestEventService.Initialize();
    }

    [TestMethod]
    public void Activation_Succeeds()
    {
      Assert.IsNotNull(new ReactNativeHost());
    }

    [TestMethod]
    public void PackageProviders_AsConstructed_IsEmpty()
    {
      var host = new ReactNativeHost();
      Assert.AreEqual(0, host.PackageProviders.Count);
    }

    [TestMethod]
    public void PackageProviders_Append_ReflectsAddition()
    {
      var host = new ReactNativeHost();
      IReactPackageProvider packageProvider = new ReactPackageProvider();
      host.PackageProviders.Add(packageProvider);
      Assert.AreEqual(1, host.PackageProviders.Count);
    }

    [TestMethod]
    public void InstanceSettings_BundleRootPathAsConstructed_IsEmpty()
    {
      var host = new ReactNativeHost();
      Assert.AreEqual(string.Empty, host.InstanceSettings.BundleRootPath);
    }

    [TestMethod]
    public void InstanceSettings_BundleRootPathAsAssigned_MatchesAssignedValue()
    {
      const string path = "a/b/c";
      var host = new ReactNativeHost();
      host.InstanceSettings.BundleRootPath = path;
      Assert.AreEqual(path, host.InstanceSettings.BundleRootPath);
    }

    [TestMethod]
    public void Run_JSDrivenTests()
    {
      var hostHolder = new TestReactNativeHostHolder("ReactNativeHostTests", host =>
      {
        host.PackageProviders.Add(new ReactPackageProvider());
      });

      try
      {
        TestEventService.ObserveEvents(new[]
        {
          new TestEvent("TestHostModule.Initialize"),
          new TestEvent("TestHostModule.StartTests"),
          new TestEvent("call addValues", new JSValueArray{ 4, 7 }),
          new TestEvent("TestHostModule.ReturnResult", 11)
        });
      }
      finally
      {
        hostHolder.Dispose();
      }
    }

    [TestMethod]
    public void HostReloadedAsyncActionCompletedEvent()
    {
      var hostHolder = new TestReactNativeHostHolder(
        "ReactNativeHostTests",
        host =>
        {
          host.ReloadInstance().Completed = (IAsyncAction _, AsyncStatus status) =>
          {
            switch (status)
            {
              case AsyncStatus.Completed: TestEventService.LogEvent("InstanceLoaded.Completed"); break;
              case AsyncStatus.Canceled: TestEventService.LogEvent("InstanceLoaded.Canceled"); break;
              default: TestEventService.LogEvent("InstanceLoaded.Failed"); break;
            }
          };
        },
        new TestReactNativeHostHolder.Options(noInstanceLoad: true));

      try
      {
        TestEventService.ObserveEvents(new[] { new TestEvent("InstanceLoaded.Completed") });
      }
      finally
      {
        hostHolder.Dispose();
      }
    }
    [TestMethod]
    public void LoadInstance_FiresInstanceLoaded_Success()
    {
      var hostHolder = new TestReactNativeHostHolder("ReactNativeHostTests", host =>
      {
        host.InstanceSettings.InstanceLoaded += (object _, InstanceLoadedEventArgs args) =>
          TestEventService.LogEvent($"InstanceLoaded.{(args.Failed ? "Failed" : "Success")}");
      });

      try
      {
        TestEventService.ObserveEvents(new[] { new TestEvent("InstanceLoaded.Success") });
      }
      finally
      {
        hostHolder.Dispose();
      }
    }

    [TestMethod]
    public void LoadBundleWithError_FiresInstanceLoaded_Failed()
    {
      var hostHolder = new TestReactNativeHostHolder("SyntaxError", host =>
      {
        host.InstanceSettings.InstanceLoaded += (object _, InstanceLoadedEventArgs args) =>
          TestEventService.LogEvent($"InstanceLoaded.{(args.Failed ? "Failed" : "Success")}");
      });

      try
      {
        TestEventService.ObserveEvents(new[] { new TestEvent("InstanceLoaded.Failed") });
      }
      finally
      {
        hostHolder.Dispose();
      }
    }

    [TestMethod]
    public void LoadBundleWithError_ReloadInstance_Fails()
    {
      var hostHolder = new TestReactNativeHostHolder(
        "SyntaxError",
        host =>
        {
          host.ReloadInstance().Completed = (IAsyncAction _, AsyncStatus status) =>
          {
            switch (status)
            {
              case AsyncStatus.Completed: TestEventService.LogEvent("InstanceLoaded.Completed"); break;
              case AsyncStatus.Canceled: TestEventService.LogEvent("InstanceLoaded.Canceled"); break;
              default: TestEventService.LogEvent("InstanceLoaded.Failed"); break;
            }
          };
        },
        new TestReactNativeHostHolder.Options(noInstanceLoad: true));

      try
      {
        TestEventService.ObserveEvents(new[] { new TestEvent("InstanceLoaded.Canceled") });
      }
      finally
      {
        hostHolder.Dispose();
      }
    }
  }
}
