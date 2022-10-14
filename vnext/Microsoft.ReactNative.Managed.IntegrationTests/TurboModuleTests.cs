// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Threading.Tasks;

// TODO: Add tests for methods that throw exceptions
// TODO: Remove reflection-based code gen for reader/writer

namespace Microsoft.ReactNative.Managed.IntegrationTests
{
  public struct MyTurboModuleConstants
  {
    [ReactProperty("constantString3")]
    public string ConstantString3;
    // It must be passed as ConstantInt3
    public int ConstantInt3;
  }

  public struct Point
  {
    [ReactProperty("x")]
    public int X;
    [ReactProperty("y")]
    public int Y;
  }

  [ReactModule]
  public class MyTurboModule
  {
    private ReactContext m_reactContext;

    public static ReactPropertyId<string> TestName = new ReactPropertyId<string>(nameof(MyTurboModule), "TestName");
    public static ReactPropertyId<IReactDispatcher> TestDispatcher = new ReactPropertyId<IReactDispatcher>(nameof(MyTurboModule), "TestDispatcher");

    [ReactInitializer]
    public void Initialize(ReactContext reactContext)
    {
      m_reactContext = reactContext;
    }

    [ReactSyncMethod("getTestName")]
    public string GetTestName()
    {
      return m_reactContext.Properties.Get(TestName);
    }

    [ReactSyncMethod("logAction")]
    public int LogAction(string actionName, JSValue value)
    {
      TestEventService.LogEvent(actionName, value);
      return 0;
    }

    [ReactConstant("constantString")]
    public string ConstantString => "myConstantString";

    [ReactConstant("constantInt")]
    public int ConstantInt => 3;

    [ReactConstantProvider]
    public void GetConstants(ReactConstantProvider provider)
    {
      provider.Add("constantString2", "Hello");
      provider.Add("constantInt2", 10);
    }

    [ReactGetConstants]
    public MyTurboModuleConstants GetConstants2()
    {
      return new MyTurboModuleConstants
      {
        ConstantString3 = "strong-typed-constants!",
        ConstantInt3 = 20
      };
    }

    [ReactMethod("add")]
    public int Add(int x, int y)
    {
      return x + y;
    }

    [ReactMethod("negate")]
    public int Negate(int x)
    {
      return -x;
    }

    [ReactMethod("sayHello")]
    public string SayHello()
    {
      return "Hello";
    }

    [ReactMethod("sayHello0")]
    public void SayHello0()
    {
      TestEventService.LogEvent("sayHello0", "Hello");
    }

    [ReactMethod("printPoint")]
    public void PrintPoint(Point pt)
    {
      TestEventService.LogEvent("printPoint", $"Point: ({pt.X}, {pt.Y})");
    }

    [ReactMethod("printLine")]
    public void PrintLine(Point start, Point end)
    {
      TestEventService.LogEvent("printLine", $"Line: ({start.X}, {start.Y})-({end.X}, {end.Y})");
    }

    [ReactMethod("addCallback")]
    public void AddCallback(int x, int y, Action<int> resolve)
    {
      resolve(x + y);
    }

    [ReactMethod("negateCallback")]
    public void NegateCallback(int x, Action<int> resolve)
    {
      resolve(-x);
    }

    [ReactMethod("negateAsyncCallback")]
    public async void NegateAsyncCallback(int x, Action<int> resolve)
    {
      await Task.Run(() => resolve(-x));
    }

    [ReactMethod("negateTaskRunCallback")]
    public void NegateTaskRunCallback(int x, Action<int> resolve)
    {
      Task.Run(() => resolve(-x));
    }

    [ReactMethod("sayHelloCallback")]
    public void SayHelloCallback(Action<string> resolve)
    {
      resolve("Hello_2");
    }

    [ReactMethod("callbackZeroArgs")]
    public void CallbackZeroArgs(Action resolve)
    {
      resolve();
    }

    [ReactMethod("callbackTwoArgs")]
    public void CallbackTwoArgs(Action<int, int> resolve)
    {
      resolve(1, 2);
    }

    [ReactMethod("callbackThreeArgs")]
    public void CallbackThreeArgs(Action<int, int, string> resolve)
    {
      resolve(1, 2, "Hello");
    }

    [ReactMethod("divideCallbacks")]
    public void DivideCallbacks(
      int x,
      int y,
      Action<int> resolve,
      Action<string> reject)
    {
      if (y != 0)
      {
        resolve(x / y);
      }
      else
      {
        reject("Division by 0");
      }
    }

    [ReactMethod("negateCallbacks")]
    public void NegateCallbacks(
      int x,
      Action<int> resolve,
      Action<string> reject)
    {
      if (x >= 0)
      {
        resolve(-x);
      }
      else
      {
        reject("Already negative");
      }
    }

    [ReactMethod("negateAsyncCallbacks")]
    public async void NegateAsyncCallbacks(
      int x,
      Action<int> resolve,
      Action<string> reject)
    {
      await Task.Run(() =>
      {
        if (x >= 0)
        {
          resolve(-x);
        }
        else
        {
          reject("Already negative");
        }
      });
    }

    [ReactMethod("negateTaskRunCallbacks")]
    public void NegateTaskRunCallbacks(
      int x,
      Action<int> resolve,
      Action<string> reject)
    {
      Task.Run(() =>
      {
        if (x >= 0)
        {
          resolve(-x);
        }
        else
        {
          reject("Already negative");
        }
      });
    }


    [ReactMethod("resolveSayHelloCallbacks")]
    public void ResolveSayHelloCallbacks(
      Action<string> resolve,
      Action<string> _)
    {
      resolve("Hello_3");
    }

    [ReactMethod("rejectSayHelloCallbacks")]
    public void RejectSayHelloCallbacks(
      Action<string> _,
      Action<string> reject)
    {
      reject("Goodbye");
    }

    [ReactMethod("twoCallbacksZeroArgs")]
    public void TwoCallbacksZeroArgs(
      bool useFirst,
      Action callback1,
      Action callback2)
    {
      if (useFirst)
      {
        callback1();
      }
      else
      {
        callback2();
      }
    }

    [ReactMethod("twoCallbacksTwoArgs")]
    public void TwoCallbacksTwoArgs(
      bool useFirst,
      Action<int, int> callback1,
      Action<int, int> callback2)
    {
      if (useFirst)
      {
        callback1(1, 2);
      }
      else
      {
        callback2(3, 4);
      }
    }

    [ReactMethod("twoCallbacksThreeArgs")]
    public void TwoCallbacksThreeArgs(
      bool useFirst,
      Action<int, int, string> callback1,
      Action<int, int, string> callback2)
    {
      if (useFirst)
      {
        callback1(1, 2, "Hello");
      }
      else
      {
        callback2(3, 4, "World");
      }
    }

    [ReactMethod("dividePromise")]
    public void DividePromise(int x, int y, ReactPromise<int> result)
    {
      if (y != 0)
      {
        result.Resolve(x / y);
      }
      else
      {
        result.Reject("Division by 0");
      }
    }

    [ReactMethod("negatePromise")]
    public void NegatePromise(int x, ReactPromise<int> result)
    {
      if (x >= 0)
      {
        result.Resolve(-x);
      }
      else
      {
        result.Reject("Already negative");
      }
    }

    [ReactMethod("negateAsyncPromise")]
    public async void NegateAsyncPromise(int x, ReactPromise<int> result)
    {
      await Task.Run(() =>
      {
        if (x >= 0)
        {
          result.Resolve(-x);
        }
        else
        {
          result.Reject("Already negative");
        }
      });
    }

    [ReactMethod("negateTaskRunPromise")]
    public void NegateTaskRunPromise(int x, ReactPromise<int> result)
    {
      Task.Run(() =>
      {
        if (x >= 0)
        {
          result.Resolve(-x);
        }
        else
        {
          result.Reject("Already negative");
        }
      });
    }

    [ReactMethod("voidPromise")]
    public void VoidPromise(int x, ReactPromise result)
    {
      if (x % 2 == 0)
      {
        result.Resolve();
      }
      else
      {
        result.Reject("Odd unexpected");
      }
    }

    [ReactMethod("resolveSayHelloPromise")]
    public void ResolveSayHelloPromise(ReactPromise<string> result)
    {
      result.Resolve("Hello_4");
    }

    [ReactMethod("rejectSayHelloPromise")]
    public void RejectSayHelloPromise(ReactPromise<string> result)
    {
      result.Reject("Promise rejected");
    }

    [ReactMethod("divideTask")]
    public Task<int> DivideTask(int x, int y)
    {
      if (y != 0)
      {
        return Task.FromResult(x / y);
      }
      else
      {
        return Task.FromException<int>(new InvalidOperationException("Division by 0"));
      }
    }

    [ReactMethod("divideAsyncTask")]
    public async Task<int> DivideAsyncTask(int x, int y)
    {
      return await Task.Run(() =>
      {
        if (y != 0)
        {
          return x / y;
        }
        else
        {
          throw new InvalidOperationException("Async Division by 0");
        }
      });
    }

    [ReactMethod("voidTask")]
    public Task VoidTask(int x)
    {
      if (x % 2 == 0)
      {
        return Task.CompletedTask;
      }
      else
      {
        return Task.FromException(new InvalidOperationException("Odd unexpected"));
      }
    }

    [ReactMethod("voidAsyncTask")]
    public async Task VoidAsyncTask(int x)
    {
      await Task.Run(() =>
      {
        if (x % 2 != 0)
        {
          throw new InvalidOperationException("Async Odd unexpected");
        }
      });
    }

    [ReactSyncMethod("addSync")]
    public int AddSync(int x, int y)
    {
      return x + y;
    }

    [ReactSyncMethod("negateSync")]
    public int NegateSync(int x)
    {
      return -x;
    }

    [ReactSyncMethod("sayHelloSync")]
    public string SayHelloSync()
    {
      return "Hello";
    }
  }

  [TestClass]
  public class TurboModuleTests
  {
    [TestInitialize]
    public void Initialize()
    {
      TestEventService.Initialize();
      TestNotificationService.Initialize();
    }

    [TestMethod]
    public void ExecuteSampleTurboModule()
    {
      var hostHolder = new TestReactNativeHostHolder("TurboModuleTests", host =>
      {
        host.PackageProviders.Add(new ReactPackageProvider());
        var properties = new ReactPropertyBag(host.InstanceSettings.Properties);
        properties.Set(MyTurboModule.TestName, "ExecuteSampleTurboModule");
      });

      try
      {
        TestEventService.ObserveEvents(new[]
        {
          new TestEvent("constantString", "myConstantString"),
          new TestEvent("constantInt", 3),
          new TestEvent("constantString2", "Hello"),
          new TestEvent("constantInt2", 10),
          new TestEvent("constantString3", "strong-typed-constants!"),
          new TestEvent("constantInt3", 20),

          new TestEvent("add", 10),
          new TestEvent("negate", -10),
          new TestEvent("sayHello", "Hello"),

          new TestEvent("sayHello0", "Hello"),
          new TestEvent("printPoint", "Point: (1, 2)"),
          new TestEvent("printLine", "Line: (1, 2)-(3, 4)"),

          new TestEvent("addCallback", 15),
          new TestEvent("negateCallback", -15),
          new TestEvent("negateAsyncCallback", -16),
          new TestEvent("negateTaskRunCallback", -17),
          new TestEvent("sayHelloCallback", "Hello_2"),

          new TestEvent("callbackZeroArgs"),
          new TestEvent("callbackTwoArgs", new JSValueObject{ { "x", 1}, { "y", 2} }),
          new TestEvent("callbackThreeArgs", new JSValueObject{ { "x", 1 }, { "y", 2 }, { "msg", "Hello" } }),

          new TestEvent("divideCallbacks", 2),
          new TestEvent("divideCallbacks.error", "Division by 0"),
          new TestEvent("negateCallbacks", -10),
          new TestEvent("negateCallbacks.error", "Already negative"),
          new TestEvent("negateAsyncCallbacks", -10),
          new TestEvent("negateAsyncCallbacks.error", "Already negative"),
          new TestEvent("negateTaskRunCallbacks", -10),
          new TestEvent("negateTaskRunCallbacks.error", "Already negative"),
          new TestEvent("resolveSayHelloCallbacks", "Hello_3"),
          new TestEvent("rejectSayHelloCallbacks.error", "Goodbye"),

          new TestEvent("twoCallbacksZeroArgs1", "success"),
          new TestEvent("twoCallbacksZeroArgs2", "failure"),
          new TestEvent("twoCallbacksTwoArgs1", new JSValueObject{ { "x", 1 }, { "y", 2 } }),
          new TestEvent("twoCallbacksTwoArgs2", new JSValueObject{ { "x", 3 }, { "y", 4 } }),
          new TestEvent("twoCallbacksThreeArgs1", new JSValueObject{ { "x", 1 }, { "y", 2 }, { "msg", "Hello" } }),
          new TestEvent("twoCallbacksThreeArgs2", new JSValueObject{ { "x", 3 }, { "y", 4 }, { "msg", "World" } }),

          new TestEvent("dividePromise", 5),
          new TestEvent("dividePromise.error", "Division by 0"),
          new TestEvent("negatePromise", -10),
          new TestEvent("negatePromise.error", "Already negative"),
          new TestEvent("negateAsyncPromise", -10),
          new TestEvent("negateAsyncPromise.error", "Already negative"),
          new TestEvent("negateTaskRunPromise", -10),
          new TestEvent("negateTaskRunPromise.error", "Already negative"),
          new TestEvent("voidPromise", "success"),
          new TestEvent("voidPromise.error", "Odd unexpected"),
          new TestEvent("resolveSayHelloPromise", "Hello_4"),
          new TestEvent("rejectSayHelloPromise", "Promise rejected"),

          new TestEvent("divideTask", 5),
          new TestEvent("divideTask.error", "Division by 0"),
          new TestEvent("divideAsyncTask", 2),
          new TestEvent("divideAsyncTask.error", "Async Division by 0"),
          new TestEvent("voidTask", "success"),
          new TestEvent("voidTask.error", "Odd unexpected"),
          new TestEvent("voidAsyncTask", "success"),
          new TestEvent("voidAsyncTask.error", "Async Odd unexpected"),

          new TestEvent("addSync", 42),
          new TestEvent("negateSync", -12),
          new TestEvent("sayHelloSync", "Hello"),
        });
      }
      finally
      {
        hostHolder.Dispose();
      }
    }
  }
}
