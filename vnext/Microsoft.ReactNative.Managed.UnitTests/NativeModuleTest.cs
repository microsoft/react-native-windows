// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Microsoft.ReactNative.Managed.UnitTests
{
  public struct Point
  {
    public int X;
    public int Y;
  }

  class SimpleNativeModule
  {
    [ReactMethod]
    public int Add(int x, int y)
    {
      return x + y;
    }

    [ReactMethod]
    public int Negate(int x)
    {
      return -x;
    }

    [ReactMethod]
    public string SayHello()
    {
      return "Hello";
    }

    [ReactMethod]
    public static int StaticAdd(int x, int y)
    {
      return x + y;
    }

    [ReactMethod]
    public static int StaticNegate(int x)
    {
      return -x;
    }

    [ReactMethod]
    public static string StaticSayHello()
    {
      return "Hello";
    }

    [ReactMethod]
    public void SayHello0()
    {
      Message = "Hello_0";
    }

    [ReactMethod]
    public void PrintPoint(Point pt)
    {
      Message = $"Point: ({pt.X}, {pt.Y})";
    }

    [ReactMethod]
    public void PrintLine(Point start, Point end)
    {
      Message = $"Line: ({start.X}, {start.Y})-({end.X}, {end.Y})";
    }

    [ReactMethod]
    public static void StaticSayHello1()
    {
      StaticMessage = "Hello_1";
    }

    [ReactMethod]
    public static void StaticPrintPoint(Point pt)
    {
      StaticMessage = $"Static Point: ({pt.X}, {pt.Y})";
    }

    [ReactMethod]
    public static void StaticPrintLine(Point start, Point end)
    {
      StaticMessage = $"Static Line: ({start.X}, {start.Y})-({end.X}, {end.Y})";
    }

    [ReactMethod]
    public void AddCallback(int x, int y, Action<int> resolve)
    {
      resolve(x + y);
    }

    [ReactMethod]
    public void NegateCallback(int x, Action<int> resolve)
    {
      resolve(-x);
    }

    [ReactMethod]
    public async void NegateAsyncCallback(int x, Action<int> resolve)
    {
      bool isPositive = await Task.Run(() => x >= 0);
      resolve(isPositive ? -x : x);
    }

    [ReactMethod]
    public void SayHelloCallback(Action<string> resolve)
    {
      resolve("Hello_2");
    }

    [ReactMethod]
    public static void StaticAddCallback(int x, int y, Action<int> resolve)
    {
      resolve(x + y);
    }

    [ReactMethod]
    public static void StaticNegateCallback(int x, Action<int> resolve)
    {
      resolve(-x);
    }

    [ReactMethod]
    public static async void StaticNegateAsyncCallback(int x, Action<int> resolve)
    {
      bool isPositive = await Task.Run(() => x >= 0);
      resolve(isPositive ? -x : x);
    }

    [ReactMethod]
    public static void StaticSayHelloCallback(Action<string> resolve)
    {
      resolve("Static Hello_2");
    }

    [ReactMethod]
    public void DivideCallbacks(int x, int y, Action<int> resolve, Action<string> reject)
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

    [ReactMethod]
    public void NegateCallbacks(int x, Action<int> resolve, Action<string> reject)
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


    [ReactMethod]
    public async void NegateAsyncCallbacks(int x, Action<int> resolve, Action<string> reject)
    {
      bool isPosititve = await Task.Run(() => x >= 0);
      if (isPosititve)
      {
        resolve(-x);
      }
      else
      {
        reject("Already negative");
      }
    }

    [ReactMethod]
    public void ResolveSayHelloCallbacks(Action<string> resolve, Action<string> reject)
    {
      resolve("Hello_3");
    }

    [ReactMethod]
    public void RejectSayHelloCallbacks(Action<string> resolve, Action<string> reject)
    {
      reject("Goodbye");
    }

    [ReactMethod]
    public static void StaticDivideCallbacks(int x, int y, Action<int> resolve, Action<string> reject)
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

    [ReactMethod]
    public static void StaticNegateCallbacks(int x, Action<int> resolve, Action<string> reject)
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

    [ReactMethod]
    public static async void StaticNegateAsyncCallbacks(int x, Action<int> resolve, Action<string> reject)
    {
      bool isPosititve = await Task.Run(() => x >= 0);
      if (isPosititve)
      {
        resolve(-x);
      }
      else
      {
        reject("Already negative");
      }
    }


    [ReactMethod]
    public static void StaticResolveSayHelloCallbacks(Action<string> resolve, Action<string> reject)
    {
      resolve("Hello_3");
    }

    [ReactMethod]
    public static void StaticRejectSayHelloCallbacks(Action<string> resolve, Action<string> reject)
    {
      reject("Goodbye");
    }

    [ReactMethod]
    public void DividePromise(int x, int y, IReactPromise<int> promise)
    {
      if (y != 0)
      {
        promise.Resolve(x / y);
      }
      else
      {
        promise.Reject(new ReactError { Message = "Division by 0" });
      }
    }

    [ReactMethod]
    public void NegatePromise(int x, IReactPromise<int> promise)
    {
      if (x >= 0)
      {
        promise.Resolve(-x);
      }
      else
      {
        promise.Reject(new ReactError { Message = "Already negative" });
      }
    }

    [ReactMethod]
    public async void NegateAsyncPromise(int x, IReactPromise<int> promise)
    {
      bool isPosititve = await Task.Run(() => x >= 0);
      if (isPosititve)
      {
        promise.Resolve(-x);
      }
      else
      {
        promise.Reject(new ReactError { Message = "Already negative" });
      }
    }

    // Each attribute has an optional parameter: JS name.
    [ReactMethod("voidPromise")]
    public void VoidPromise(int x, IReactPromise<JSValue.Void> promise)
    {
      if (x % 2 == 0)
      {
        promise.Resolve();
      }
      else
      {
        promise.Reject(new ReactError { Message = "Odd unexpected" });
      }
    }

    [ReactMethod]
    public void ResolveSayHelloPromise(IReactPromise<string> promise)
    {
      promise.Resolve("Hello_3");
    }

    [ReactMethod]
    public void RejectSayHelloPromise(IReactPromise<string> promise)
    {
      promise.Reject(new ReactError { Message = "Promise rejected" });
    }

    [ReactMethod]
    public static void StaticDividePromise(int x, int y, IReactPromise<int> promise)
    {
      if (y != 0)
      {
        promise.Resolve(x / y);
      }
      else
      {
        promise.Reject(new ReactError { Message = "Division by 0" });
      }
    }

    [ReactMethod]
    public static void StaticNegatePromise(int x, IReactPromise<int> promise)
    {
      if (x >= 0)
      {
        promise.Resolve(-x);
      }
      else
      {
        promise.Reject(new ReactError { Message = "Already negative" });
      }
    }

    [ReactMethod]
    public static async void StaticNegateAsyncPromise(int x, IReactPromise<int> promise)
    {
      bool isPosititve = await Task.Run(() => x >= 0);
      if (isPosititve)
      {
        promise.Resolve(-x);
      }
      else
      {
        promise.Reject(new ReactError { Message = "Already negative" });
      }
    }

    // Each attribute has an optional parameter: JS name.
    [ReactMethod("staticVoidPromise")]
    public static void StaticVoidPromise(int x, IReactPromise<JSValue.Void> promise)
    {
      if (x % 2 == 0)
      {
        promise.Resolve();
      }
      else
      {
        promise.Reject(new ReactError { Message = "Odd unexpected" });
      }
    }

    [ReactMethod]
    public static void StaticResolveSayHelloPromise(IReactPromise<string> promise)
    {
      promise.Resolve("Hello_3");
    }

    [ReactMethod]
    public static void StaticRejectSayHelloPromise(IReactPromise<string> promise)
    {
      promise.Reject(new ReactError { Message = "Promise rejected" });
    }

    [ReactSyncMethod]
    public int AddSync(int x, int y)
    {
      return x + y;
    }

    [ReactSyncMethod]
    public int NegateSync(int x)
    {
      return -x;
    }

    [ReactSyncMethod]
    public string SayHelloSync()
    {
      return "Hello";
    }

    [ReactSyncMethod]
    public static int StaticAddSync(int x, int y)
    {
      return x + y;
    }

    [ReactSyncMethod]
    public static int StaticNegateSync(int x)
    {
      return -x;
    }

    [ReactSyncMethod]
    public static string StaticSayHelloSync()
    {
      return "Hello";
    }

    [ReactConstant]
    public readonly string Constant1 = "MyConstant1";

    [ReactConstant("const2")]
    public static readonly string Constant2 = "MyConstant2";

    [ReactConstant("const3")]
    public Point Constant3 => new Point { X = 2, Y = 3 };

    [ReactConstant]
    public static Point Constant4 => new Point { X = 3, Y = 4 };

    [ReactConstantProvider]
    public void Constant5(ReactConstantProvider provider)
    {
      provider.Add("const51", new Point { X = 12, Y = 14 });
      provider.Add("const52", "MyConstant52");
    }

    [ReactConstantProvider]
    public static void Constant6(ReactConstantProvider provider)
    {
      provider.Add("const61", new Point { X = 15, Y = 17 });
      provider.Add("const62", "MyConstant62");
    }

    [ReactEvent("onIntResult1")]
    public Action<int> OnIntResult1 = null;

    [ReactEvent]
    public static Action<int> OnIntResult2 = null;

    [ReactEvent]
    public Action<Point> OnPointResult3 { get; set; }

    [ReactEvent("onPointResult4")]
    public static Action<Point> OnPointResult4 { get; set; }

    [ReactEvent]
    public Action<JSValue> OnObjectResult5 { get; set; }

    public string Message { get; set; }
    public static string StaticMessage { get; set; }
  }

  [TestClass]
  public class NativeModuleTest
  {
    private ReactModuleBuilderMock m_moduleBuilder;
    private ReactModuleInfo m_moduleInfo;
    private SimpleNativeModule m_module;

    [TestInitialize]
    public void Initialize()
    {
      m_moduleBuilder = new ReactModuleBuilderMock();
      m_moduleInfo = new ReactModuleInfo(typeof(SimpleNativeModule), "SimpleNativeModule", "SimpleNativeModule");
      m_module = (SimpleNativeModule)m_moduleInfo.ModuleProvider(m_moduleBuilder);
    }

    [TestMethod]
    public void TestMethodCall_Add()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.Add), 3, 5, (int result) => Assert.AreEqual(8, result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_Negate()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.Negate), 3, (int result) => Assert.AreEqual(-3, result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_SayHello()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.SayHello), (string result) => Assert.AreEqual("Hello", result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticAdd()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.StaticAdd), 3, 5, (int result) => Assert.AreEqual(8, result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegate()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.StaticNegate), 3, (int result) => Assert.AreEqual(-3, result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticSayHello()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.StaticSayHello), (string result) => Assert.AreEqual("Hello", result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_SayHello0()
    {
      m_moduleBuilder.Call0(nameof(SimpleNativeModule.SayHello0));
      Assert.AreEqual("Hello_0", m_module.Message);
    }

    [TestMethod]
    public void TestMethodCall_PrintPoint()
    {
      m_moduleBuilder.Call0(nameof(SimpleNativeModule.PrintPoint), new Point { X = 3, Y = 5 });
      Assert.AreEqual("Point: (3, 5)", m_module.Message);
    }

    [TestMethod]
    public void TestMethodCall_PrintLine()
    {
      m_moduleBuilder.Call0(nameof(SimpleNativeModule.PrintLine), new Point { X = 3, Y = 5 }, new Point { X = 6, Y = 8 });
      Assert.AreEqual("Line: (3, 5)-(6, 8)", m_module.Message);
    }

    [TestMethod]
    public void TestMethodCall_StaticSayHello1()
    {
      m_moduleBuilder.Call0(nameof(SimpleNativeModule.StaticSayHello1));
      Assert.AreEqual("Hello_1", SimpleNativeModule.StaticMessage);
    }

    [TestMethod]
    public void TestMethodCall_StaticPrintPoint()
    {
      m_moduleBuilder.Call0(nameof(SimpleNativeModule.StaticPrintPoint), new Point { X = 13, Y = 15 });
      Assert.AreEqual("Static Point: (13, 15)", SimpleNativeModule.StaticMessage);
    }

    [TestMethod]
    public void TestMethodCall_StaticPrintLine()
    {
      m_moduleBuilder.Call0(nameof(SimpleNativeModule.StaticPrintLine),
          new Point { X = 13, Y = 15 }, new Point { X = 16, Y = 18 });
      Assert.AreEqual("Static Line: (13, 15)-(16, 18)", SimpleNativeModule.StaticMessage);
    }

    [TestMethod]
    public void TestMethodCall_AddCallback()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.AddCallback), 7, -8, (int result) => Assert.AreEqual(-1, result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateCallback()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.NegateCallback), 3, (int result) => Assert.AreEqual(-3, result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAyncCallback()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.NegateAsyncCallback), 3, (int result) => Assert.AreEqual(-3, result)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_SayHelloCallback()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.SayHelloCallback), (string result) => Assert.AreEqual("Hello_2", result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticAddCallback()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.StaticAddCallback), 4, 56, (int result) => Assert.AreEqual(60, result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateCallback()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.StaticNegateCallback), 33, (int result) => Assert.AreEqual(-33, result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncCallback()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.StaticNegateAsyncCallback), 33, (int result) => Assert.AreEqual(-33, result)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticSayHelloCallback()
    {
      m_moduleBuilder.Call1(nameof(SimpleNativeModule.StaticSayHelloCallback), (string result) => Assert.AreEqual("Static Hello_2", result));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DivideCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.DivideCallbacks), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DivideCallbacksError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.DivideCallbacks), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.NegateCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateCallbacksError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.NegateCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.NegateAsyncCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncCallbacksError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.NegateAsyncCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_ResolveSayHelloCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.ResolveSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Goodbye", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_RejectSayHelloCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.RejectSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Goodbye", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDivideCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticDivideCallbacks), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDivideCallbacksError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticDivideCallbacks), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticNegateCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateCallbacksError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticNegateCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticNegateAsyncCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncCallbacksError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticNegateAsyncCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticResolveSayHelloCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticResolveSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Goodbye", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticRejectSayHelloCallbacks()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticRejectSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Goodbye", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DividePromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.DividePromise), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DividePromiseError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.DividePromise), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegatePromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.NegatePromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegatePromiseError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.NegatePromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncPromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.NegateAsyncPromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncPromiseError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.NegateAsyncPromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_VoidPromise()
    {
      m_moduleBuilder.Call2("voidPromise", 2,
          (JSValue.Void result) => { },
          (JSValue error) => Assert.AreEqual("Odd unexpected", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_VoidPromiseError()
    {
      m_moduleBuilder.Call2("voidPromise", 3,
          (JSValue.Void result) => { },
          (JSValue error) => Assert.AreEqual("Odd unexpected", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_ResolveSayHelloPromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.ResolveSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_RejectSayHelloPromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.RejectSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDividePromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticDividePromise), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDividePromiseError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticDividePromise), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegatePromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticNegatePromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegatePromiseError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticNegatePromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncPromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticNegateAsyncPromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncPromiseError()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticNegateAsyncPromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error.Object["message"].String)).Wait();
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticVoidPromise()
    {
      m_moduleBuilder.Call2("staticVoidPromise", 2,
          (JSValue.Void result) => { },
          (JSValue error) => Assert.AreEqual("Odd unexpected", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticVoidPromiseError()
    {
      m_moduleBuilder.Call2("staticVoidPromise", 3,
          (JSValue.Void result) => { },
          (JSValue error) => Assert.AreEqual("Odd unexpected", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }


    [TestMethod]
    public void TestMethodCall_StaticResolveSayHelloPromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticResolveSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticRejectSayHelloPromise()
    {
      m_moduleBuilder.Call2(nameof(SimpleNativeModule.StaticRejectSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error.Object["message"].String));
      Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodSyncCall_AddSync()
    {
      m_moduleBuilder.CallSync(nameof(SimpleNativeModule.AddSync), 3, 5, out int result);
      Assert.AreEqual(8, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_NegateSync()
    {
      m_moduleBuilder.CallSync(nameof(SimpleNativeModule.NegateSync), 3, out int result);
      Assert.AreEqual(-3, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_SayHelloSync()
    {
      m_moduleBuilder.CallSync(nameof(SimpleNativeModule.SayHelloSync), out string result);
      Assert.AreEqual("Hello", result);
    }

    [TestMethod]
    public void TestMethodSyncCall_StaticAddSync()
    {
      m_moduleBuilder.CallSync(nameof(SimpleNativeModule.StaticAddSync), 3, 5, out int result);
      Assert.AreEqual(8, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_StaticNegateSync()
    {
      m_moduleBuilder.CallSync(nameof(SimpleNativeModule.StaticNegateSync), 3, out int result);
      Assert.AreEqual(-3, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_StaticSayHelloSync()
    {
      m_moduleBuilder.CallSync(nameof(SimpleNativeModule.StaticSayHelloSync), out string result);
      Assert.AreEqual("Hello", result);
    }

    [TestMethod]
    public void TestConstants()
    {
      var constants = m_moduleBuilder.GetConstants();
      Assert.AreEqual("MyConstant1", constants[nameof(SimpleNativeModule.Constant1)].String);
      Assert.AreEqual("MyConstant2", constants["const2"].String);
      Assert.AreEqual(new Point { X = 2, Y = 3 }, constants["const3"].To<Point>());
      Assert.AreEqual(new Point { X = 3, Y = 4 }, constants[nameof(SimpleNativeModule.Constant4)].To<Point>());
      Assert.AreEqual(new Point { X = 12, Y = 14 }, constants["const51"].To<Point>());
      Assert.AreEqual("MyConstant52", constants["const52"].String);
      Assert.AreEqual(new Point { X = 15, Y = 17 }, constants["const61"].To<Point>());
      Assert.AreEqual("MyConstant62", constants["const62"].String);
    }

    [TestMethod]
    public void TestEvent_EventField()
    {
      bool eventRaised = false;
      m_moduleBuilder.SetEventHandler("onIntResult1", (int eventArg) =>
      {
        Assert.AreEqual(42, eventArg);
        eventRaised = true;
      });

      m_module.OnIntResult1(42);
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_StaticEventField()
    {
      bool eventRaised = false;
      m_moduleBuilder.SetEventHandler(nameof(SimpleNativeModule.OnIntResult2), (int eventArg) =>
      {
        Assert.AreEqual(64, eventArg);
        eventRaised = true;
      });

      SimpleNativeModule.OnIntResult2(64);
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_EventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilder.SetEventHandler(nameof(SimpleNativeModule.OnPointResult3), (Point eventArg) =>
      {
        Assert.AreEqual(12, eventArg.X);
        Assert.AreEqual(14, eventArg.Y);
        eventRaised = true;
      });

      m_module.OnPointResult3(new Point { X = 12, Y = 14 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_StaticEventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilder.SetEventHandler("onPointResult4", (Point eventArg) =>
      {
        Assert.AreEqual(32, eventArg.X);
        Assert.AreEqual(42, eventArg.Y);
        eventRaised = true;
      });

      SimpleNativeModule.OnPointResult4(new Point { X = 32, Y = 42 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_ObjectEventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilder.SetEventHandler(nameof(SimpleNativeModule.OnObjectResult5), (JSValue eventArg) =>
      {
        Assert.AreEqual(32, eventArg.Object["X"].Int64);
        Assert.AreEqual(42, eventArg.Object["Y"].Int64);
        eventRaised = true;
      });

      JSValue data = new JSValue(new Dictionary<string, JSValue>() { { "X", new JSValue(32) }, { "Y", new JSValue(42) }, });

      m_module.OnObjectResult5(data);
      Assert.IsTrue(eventRaised);
    }
  }
}
