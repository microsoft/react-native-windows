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

  // To specify JS module and default event emitter name:
  // [ReactModule("simpleNativeModule", EventEmitterName = "simpleNativeModuleEmitter")]
  [ReactModule]
  class SimpleNativeModule
  {
    [ReactInitializer]
    public void Initialize(ReactContext context)
    {
      IsInitialized = true;
      Assert.IsNotNull(context);

      // Event and Function fields are initialized before ReactInitializer method call.
      Assert.IsNotNull(OnIntEvent);
      Assert.IsNotNull(JSIntFunction);
    }

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
    public void CallbackZeroArgs(Action resolve)
    {
      resolve();
    }

    [ReactMethod]
    public void CallbackTwoArgs(Action<int, int> resolve)
    {
      resolve(1, 2);
    }

    [ReactMethod]
    public void CallbackThreeArgs(Action<int, int, string> resolve)
    {
      resolve(1, 2, "Hello");
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
    public void ResolveSayHelloCallbacks(Action<string> resolve, Action<string> _)
    {
      resolve("Hello_3");
    }

    [ReactMethod]
    public void RejectSayHelloCallbacks(Action<string> _, Action<string> reject)
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
    public static void StaticResolveSayHelloCallbacks(Action<string> resolve, Action<string> _)
    {
      resolve("Hello_3");
    }

    [ReactMethod]
    public static void StaticRejectSayHelloCallbacks(Action<string> _, Action<string> reject)
    {
      reject("Goodbye");
    }

    [ReactMethod]
    public void TwoCallbacksZeroArgs1(Action resolve, Action _)
    {
      resolve();
    }

    [ReactMethod]
    public void TwoCallbacksZeroArgs2(Action _, Action reject)
    {
      reject();
    }

    [ReactMethod]
    public void TwoCallbacksTwoArgs1(Action<int, int> resolve, Action<int, int> _)
    {
      resolve(1, 2);
    }

    [ReactMethod]
    public void TwoCallbacksTwoArgs2(Action<int, int> _, Action<int, int> reject)
    {
      reject(1, 2);
    }

    [ReactMethod]
    public void TwoCallbacksThreeArgs1(Action<int, int, string> resolve, Action<int, int, string> _)
    {
      resolve(1, 2, "Hello");
    }

    [ReactMethod]
    public void TwoCallbacksThreeArgs2(Action<int, int, string> _, Action<int, int, string> reject)
    {
      reject(1, 2, "Hello");
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

    // Field that allows to emit native module events.
    [ReactEvent]
    public Action<int> OnIntEvent = null;

    // An event without arguments
    [ReactEvent]
    public Action OnNoArgEvent = null;

    // An event with two arguments
    [ReactEvent]
    public Action<Point, Point> OnTwoArgsEvent = null;

    // Specify event name different from the field name.
    [ReactEvent("onPointEvent")]
    public Action<Point> OnPointEvent = null;

    // By default we use the event emitter name from ReactModule which is by default 'RCTDeviceEventEmitter'.
    // Here we specify event emitter name local for this event.
    [ReactEvent("onStringEvent", EventEmitterName = "MyEventEmitter")]
    public Action<string> OnStringEvent = null;

    // Use JSValue which is an immutable JSON-like data representation.
    [ReactEvent]
    public Action<JSValue> OnJSValueEvent = null;

    // Property that allows to emit native module events.
    [ReactEvent]
    public Action<int> OnIntEventProp { get; set; }

    // An event without arguments
    [ReactEvent]
    public Action OnNoArgEventProp { get; set; }

    // An event with two arguments
    [ReactEvent]
    public Action<Point, Point> OnTwoArgsEventProp { get; set; }

    // Specify event name different from the property name.
    [ReactEvent("onPointEventProp")]
    public Action<Point> OnPointEventProp { get; set; }

    // By default we use the event emitter name from ReactModuleAttribute which is by default 'RCTDeviceEventEmitter'.
    // Here we specify event emitter name local for this event.
    [ReactEvent("onStringEventProp", EventEmitterName = "MyEventEmitter")]
    public Action<string> OnStringEventProp { get; set; }

    // Use JSValue which is an immutable JSON-like data representation.
    [ReactEvent]
    public Action<JSValue> OnJSValueEventProp { get; set; }

    // Field that allows to call JS functions.
    [ReactFunction]
    public Action<int> JSIntFunction = null;

    // Specify JS function name different from the field name.
    [ReactFunction("pointFunc")]
    public Action<Point> JSPointFunction = null;

    // Use two arguments. Specify JS function name different from the field name.
    [ReactFunction("lineFunc")]
    public Action<Point, Point> JSLineFunction = null;

    // Use no arguments.
    [ReactFunction]
    public Action NoArgFunction = null;

    // By default we use the module name from ReactModuleAttribute which is by default the class name.
    // Here we specify module name local for this function.
    [ReactFunction("stringFunc", ModuleName = "MyModule")]
    public Action<string> JSStringFunction = null;

    // Use JSValue which is an immutable JSON-like data representation.
    [ReactFunction]
    public Action<JSValue> JSValueFunction = null;

    // Property that allows to call JS functions.
    [ReactFunction]
    public Action<int> JSIntFunctionProp { get; set; }

    // Specify JS function name different from the property name.
    [ReactFunction("pointFuncProp")]
    public Action<Point> JSPointFunctionProp { get; set; }

    // Use two arguments. Specify JS function name different from the property name.
    [ReactFunction("lineFuncProp")]
    public Action<Point, Point> JSLineFunctionProp { get; set; }

    // Use no arguments.
    [ReactFunction]
    public Action NoArgFunctionProp { get; set; }

    // By default we use the module name from ReactModuleAttribute which is by default the class name.
    // Here we specify module name local for this function.
    [ReactFunction("stringFuncProp", ModuleName = "MyModule")]
    public Action<string> JSStringFunctionProp { get; set; }

    // Use JSValue which is an immutable JSON-like data representation.
    [ReactFunction]
    public Action<JSValue> JSValueFunctionProp { get; set; }

    // Properties below are not exposed to JS and only used by tests to observe results.
    public bool IsInitialized { get; set; }
    public string Message { get; set; }
    public static string StaticMessage { get; set; }
  }

  [TestClass]
  public class NativeModuleTest
  {
    private ReactModuleBuilderMock m_moduleBuilderMock;
    private ReactModuleInfo m_moduleInfo;
    private SimpleNativeModule m_module;

    [TestInitialize]
    public void Initialize()
    {
      m_moduleBuilderMock = new ReactModuleBuilderMock();
      m_moduleInfo = new ReactModuleInfo(typeof(SimpleNativeModule));
      m_module = m_moduleBuilderMock.CreateModule<SimpleNativeModule>(m_moduleInfo);
    }

    [TestMethod]
    public void TestMethodCall_Add()
    {
      m_moduleBuilderMock.Call1<int, int, Action<int>>(nameof(SimpleNativeModule.Add), 3, 5, (int result) => Assert.AreEqual(8, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_Negate()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule.Negate), 3, (int result) => Assert.AreEqual(-3, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_SayHello()
    {
      m_moduleBuilderMock.Call1<Action<string>>(nameof(SimpleNativeModule.SayHello), (string result) => Assert.AreEqual("Hello", result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticAdd()
    {
      m_moduleBuilderMock.Call1<int, int, Action<int>>(nameof(SimpleNativeModule.StaticAdd), 3, 5, (int result) => Assert.AreEqual(8, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegate()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule.StaticNegate), 3, (int result) => Assert.AreEqual(-3, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticSayHello()
    {
      m_moduleBuilderMock.Call1<Action<string>>(nameof(SimpleNativeModule.StaticSayHello), (string result) => Assert.AreEqual("Hello", result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_SayHello0()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule.SayHello0));
      Assert.AreEqual("Hello_0", m_module.Message);
    }

    [TestMethod]
    public void TestMethodCall_PrintPoint()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule.PrintPoint), new Point { X = 3, Y = 5 });
      Assert.AreEqual("Point: (3, 5)", m_module.Message);
    }

    [TestMethod]
    public void TestMethodCall_PrintLine()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule.PrintLine), new Point { X = 3, Y = 5 }, new Point { X = 6, Y = 8 });
      Assert.AreEqual("Line: (3, 5)-(6, 8)", m_module.Message);
    }

    [TestMethod]
    public void TestMethodCall_StaticSayHello1()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule.StaticSayHello1));
      Assert.AreEqual("Hello_1", SimpleNativeModule.StaticMessage);
    }

    [TestMethod]
    public void TestMethodCall_StaticPrintPoint()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule.StaticPrintPoint), new Point { X = 13, Y = 15 });
      Assert.AreEqual("Static Point: (13, 15)", SimpleNativeModule.StaticMessage);
    }

    [TestMethod]
    public void TestMethodCall_StaticPrintLine()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule.StaticPrintLine),
          new Point { X = 13, Y = 15 }, new Point { X = 16, Y = 18 });
      Assert.AreEqual("Static Line: (13, 15)-(16, 18)", SimpleNativeModule.StaticMessage);
    }

    [TestMethod]
    public void TestMethodCall_AddCallback()
    {
      m_moduleBuilderMock.Call1<int, int, Action<int>>(nameof(SimpleNativeModule.AddCallback), 7, -8, (int result) => Assert.AreEqual(-1, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateCallback()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule.NegateCallback), 3, (int result) => Assert.AreEqual(-3, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAyncCallback()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule.NegateAsyncCallback), 3, (int result) => Assert.AreEqual(-3, result)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_SayHelloCallback()
    {
      m_moduleBuilderMock.Call1<Action<string>>(nameof(SimpleNativeModule.SayHelloCallback), (string result) => Assert.AreEqual("Hello_2", result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticAddCallback()
    {
      m_moduleBuilderMock.Call1<int, int, Action<int>>(nameof(SimpleNativeModule.StaticAddCallback), 4, 56, (int result) => Assert.AreEqual(60, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateCallback()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule.StaticNegateCallback), 33, (int result) => Assert.AreEqual(-33, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncCallback()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule.StaticNegateAsyncCallback), 33, (int result) => Assert.AreEqual(-33, result)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticSayHelloCallback()
    {
      m_moduleBuilderMock.Call1<Action<string>>(nameof(SimpleNativeModule.StaticSayHelloCallback), (string result) => Assert.AreEqual("Static Hello_2", result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_CallbackZeroArgs()
    {
      m_moduleBuilderMock.Call1<Action>(nameof(SimpleNativeModule.CallbackZeroArgs), () => { });
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_CallbackTwoArgs()
    {
      m_moduleBuilderMock.Call1<Action<int, int>>(nameof(SimpleNativeModule.CallbackTwoArgs), (int p1, int p2) =>
      {
        Assert.AreEqual(1, p1);
        Assert.AreEqual(2, p2);
      });
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_CallbackThreeArgs()
    {
      m_moduleBuilderMock.Call1<Action<int, int, string>>(nameof(SimpleNativeModule.CallbackThreeArgs), (int p1, int p2, string p3) =>
      {
        Assert.AreEqual(1, p1);
        Assert.AreEqual(2, p2);
        Assert.AreEqual("Hello", p3);
      });
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DivideCallbacks()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<string>>(nameof(SimpleNativeModule.DivideCallbacks), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (string error) => Assert.AreEqual("Division by 0", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DivideCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<string>>(nameof(SimpleNativeModule.DivideCallbacks), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (string error) => Assert.AreEqual("Division by 0", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateCallbacks()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule.NegateCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (string error) => Assert.AreEqual("Already negative", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule.NegateCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (string error) => Assert.AreEqual("Already negative", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncCallbacks()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule.NegateAsyncCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (string error) => Assert.AreEqual("Already negative", error)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule.NegateAsyncCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (string error) => Assert.AreEqual("Already negative", error)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_ResolveSayHelloCallbacks()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<string>>(nameof(SimpleNativeModule.ResolveSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (string error) => Assert.AreEqual("Goodbye", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_RejectSayHelloCallbacks()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<string>>(nameof(SimpleNativeModule.RejectSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (string error) => Assert.AreEqual("Goodbye", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDivideCallbacks()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<string>>(nameof(SimpleNativeModule.StaticDivideCallbacks), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (string error) => Assert.AreEqual("Division by 0", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDivideCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<string>>(nameof(SimpleNativeModule.StaticDivideCallbacks), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (string error) => Assert.AreEqual("Division by 0", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateCallbacks()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule.StaticNegateCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (string error) => Assert.AreEqual("Already negative", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule.StaticNegateCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (string error) => Assert.AreEqual("Already negative", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncCallbacks()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule.StaticNegateAsyncCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (string error) => Assert.AreEqual("Already negative", error)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule.StaticNegateAsyncCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (string error) => Assert.AreEqual("Already negative", error)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticResolveSayHelloCallbacks()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<string>>(nameof(SimpleNativeModule.StaticResolveSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (string error) => Assert.AreEqual("Goodbye", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticRejectSayHelloCallbacks()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<string>>(nameof(SimpleNativeModule.StaticRejectSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (string error) => Assert.AreEqual("Goodbye", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }


    [TestMethod]
    public void TestMethodCall_TwoCallbacksZeroArgs1()
    {
      m_moduleBuilderMock.Call2<Action, Action>(nameof(SimpleNativeModule.TwoCallbacksZeroArgs1),
        () => { }, () => { Assert.Fail(); });
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_TwoCallbacksZeroArgs2()
    {
      m_moduleBuilderMock.Call2<Action, Action>(nameof(SimpleNativeModule.TwoCallbacksZeroArgs2),
        () => { Assert.Fail(); }, () => { });
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_TwoCallbacksTwoArgs1()
    {
      m_moduleBuilderMock.Call2<Action<int, int>, Action<int, int>>(
        nameof(SimpleNativeModule.TwoCallbacksTwoArgs1),
        (int p1, int p2) =>
        {
          Assert.AreEqual(1, p1);
          Assert.AreEqual(2, p2);
        },
        (int p1, int p2) => { Assert.Fail(); });
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_TwoCallbacksTwoArgs2()
    {
      m_moduleBuilderMock.Call2<Action<int, int>, Action<int, int>>(
        nameof(SimpleNativeModule.TwoCallbacksTwoArgs2),
        (int p1, int p2) => { Assert.Fail(); },
        (int p1, int p2) =>
        {
          Assert.AreEqual(1, p1);
          Assert.AreEqual(2, p2);
        });
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_TwoCallbacksThreeArgs1()
    {
      m_moduleBuilderMock.Call2<Action<int, int, string>, Action<int, int, string>>(
        nameof(SimpleNativeModule.TwoCallbacksThreeArgs1),
        (int p1, int p2, string p3) =>
        {
          Assert.AreEqual(1, p1);
          Assert.AreEqual(2, p2);
          Assert.AreEqual("Hello", p3);
        },
        (int p1, int p2, string p3) => { Assert.Fail(); });
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_TwoCallbacksThreeArgs2()
    {
      m_moduleBuilderMock.Call2<Action<int, int, string>, Action<int, int, string>>(
        nameof(SimpleNativeModule.TwoCallbacksThreeArgs2),
        (int p1, int p2, string p3) => { Assert.Fail(); },
        (int p1, int p2, string p3) =>
        {
          Assert.AreEqual(1, p1);
          Assert.AreEqual(2, p2);
          Assert.AreEqual("Hello", p3);
        });
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DividePromise()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.DividePromise), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DividePromiseError()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.DividePromise), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegatePromise()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.NegatePromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegatePromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.NegatePromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncPromise()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.NegateAsyncPromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"])).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncPromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.NegateAsyncPromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"])).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_VoidPromise()
    {
      m_moduleBuilderMock.Call2<int, Action<JSValue.Void>, Action<JSValue>>("voidPromise", 2,
          (JSValue.Void result) => { },
          (JSValue error) => Assert.AreEqual("Odd unexpected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_VoidPromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<JSValue.Void>, Action<JSValue>>("voidPromise", 3,
          (JSValue.Void result) => { },
          (JSValue error) => Assert.AreEqual("Odd unexpected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_ResolveSayHelloPromise()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<JSValue>>(nameof(SimpleNativeModule.ResolveSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_RejectSayHelloPromise()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<JSValue>>(nameof(SimpleNativeModule.RejectSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDividePromise()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.StaticDividePromise), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDividePromiseError()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.StaticDividePromise), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegatePromise()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.StaticNegatePromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegatePromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.StaticNegatePromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncPromise()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.StaticNegateAsyncPromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"])).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncPromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule.StaticNegateAsyncPromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"])).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticVoidPromise()
    {
      m_moduleBuilderMock.Call2<int, Action<JSValue.Void>, Action<JSValue>>("staticVoidPromise", 2,
          (JSValue.Void result) => { },
          (JSValue error) => Assert.AreEqual("Odd unexpected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticVoidPromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<JSValue.Void>, Action<JSValue>>("staticVoidPromise", 3,
          (JSValue.Void result) => { },
          (JSValue error) => Assert.AreEqual("Odd unexpected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }


    [TestMethod]
    public void TestMethodCall_StaticResolveSayHelloPromise()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<JSValue>>(nameof(SimpleNativeModule.StaticResolveSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticRejectSayHelloPromise()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<JSValue>>(nameof(SimpleNativeModule.StaticRejectSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodSyncCall_AddSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule.AddSync), 3, 5, out int result);
      Assert.AreEqual(8, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_NegateSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule.NegateSync), 3, out int result);
      Assert.AreEqual(-3, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_SayHelloSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule.SayHelloSync), out string result);
      Assert.AreEqual("Hello", result);
    }

    [TestMethod]
    public void TestMethodSyncCall_StaticAddSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule.StaticAddSync), 3, 5, out int result);
      Assert.AreEqual(8, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_StaticNegateSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule.StaticNegateSync), 3, out int result);
      Assert.AreEqual(-3, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_StaticSayHelloSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule.StaticSayHelloSync), out string result);
      Assert.AreEqual("Hello", result);
    }

    [TestMethod]
    public void TestConstants()
    {
      var constants = m_moduleBuilderMock.GetConstants();
      Assert.AreEqual("MyConstant1", constants[nameof(SimpleNativeModule.Constant1)]);
      Assert.AreEqual("MyConstant2", constants["const2"]);
      Assert.AreEqual(new Point { X = 2, Y = 3 }, constants["const3"].To<Point>());
      Assert.AreEqual(new Point { X = 3, Y = 4 }, constants[nameof(SimpleNativeModule.Constant4)].To<Point>());
      Assert.AreEqual(new Point { X = 12, Y = 14 }, constants["const51"].To<Point>());
      Assert.AreEqual("MyConstant52", constants["const52"]);
      Assert.AreEqual(new Point { X = 15, Y = 17 }, constants["const61"].To<Point>());
      Assert.AreEqual("MyConstant62", constants["const62"]);
    }

    [TestMethod]
    public void TestEvent_IntEventField()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnIntEvent", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(42, args[0]);
        eventRaised = true;
      });

      m_module.OnIntEvent(42);
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_NoArgEventField()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnNoArgEvent", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(0, args.Count);
        eventRaised = true;
      });

      m_module.OnNoArgEvent();
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_TwoArgsEventField()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnTwoArgsEvent", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        Assert.AreEqual(12, args[1]["X"]);
        Assert.AreEqual(18, args[1]["Y"]);
        eventRaised = true;
      });

      m_module.OnTwoArgsEvent(new Point { X = 4, Y = 2 }, new Point { X = 12, Y = 18 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSNameEventField()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "onPointEvent", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        eventRaised = true;
      });

      m_module.OnPointEvent(new Point { X = 4, Y = 2 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSEventEmitterEventField()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("MyEventEmitter", "onStringEvent", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual("Hello World!", args[0]);
        eventRaised = true;
      });

      m_module.OnStringEvent("Hello World!");
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSValueObjectEventField()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnJSValueEvent", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        eventRaised = true;
      });

      m_module.OnJSValueEvent(new JSValueObject { ["X"] = 4, ["Y"] = 2 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSValueArrayEventField()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnJSValueEvent", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual("X", args[0][0]);
        Assert.AreEqual(4, args[0][1]);
        Assert.AreEqual(true, args[0][2]);
        Assert.AreEqual(42, args[0][3]["Id"]);
        eventRaised = true;
      });

      m_module.OnJSValueEvent(new JSValueArray { "X", 4, true, new JSValueObject { ["Id"] = 42 } });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSValueArray1EventField()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnJSValueEvent", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(42, args[0][0]);
        eventRaised = true;
      });

      m_module.OnJSValueEvent(new JSValueArray { 42 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_IntEventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnIntEventProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(42, args[0]);
        eventRaised = true;
      });

      m_module.OnIntEventProp(42);
      Assert.IsTrue(eventRaised);
    }


    [TestMethod]
    public void TestEvent_NoArgEventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnNoArgEventProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(0, args.Count);
        eventRaised = true;
      });

      m_module.OnNoArgEventProp();
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_TwoArgsEventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnTwoArgsEventProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        Assert.AreEqual(12, args[1]["X"]);
        Assert.AreEqual(18, args[1]["Y"]);
        eventRaised = true;
      });

      m_module.OnTwoArgsEventProp(new Point { X = 4, Y = 2 }, new Point { X = 12, Y = 18 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSNameEventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "onPointEventProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        eventRaised = true;
      });

      m_module.OnPointEventProp(new Point { X = 4, Y = 2 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSEventEmitterEventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("MyEventEmitter", "onStringEventProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual("Hello World!", args[0]);
        eventRaised = true;
      });

      m_module.OnStringEventProp("Hello World!");
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSValueObjectEventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnJSValueEventProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        eventRaised = true;
      });

      m_module.OnJSValueEventProp(new JSValueObject { ["X"] = 4, ["Y"] = 2 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSValueArrayEventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnJSValueEventProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual("X", args[0][0]);
        Assert.AreEqual(4, args[0][1]);
        Assert.AreEqual(true, args[0][2]);
        Assert.AreEqual(42, args[0][3]["Id"]);
        eventRaised = true;
      });

      m_module.OnJSValueEventProp(new JSValueArray { "X", 4, true, new JSValueObject { ["Id"] = 42 } });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestEvent_JSValueArray1EventProperty()
    {
      bool eventRaised = false;
      m_moduleBuilderMock.ExpectEvent("RCTDeviceEventEmitter", "OnJSValueEventProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(42, args[0][0]);
        eventRaised = true;
      });

      m_module.OnJSValueEventProp(new JSValueArray { 42 });
      Assert.IsTrue(eventRaised);
    }

    [TestMethod]
    public void TestFunction_JSIntFunctionField()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "JSIntFunction", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(42, args[0]);
        functionCalled = true;
      });

      m_module.JSIntFunction(42);
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_JSNameFunctionField()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "pointFunc", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        functionCalled = true;
      });

      m_module.JSPointFunction(new Point { X = 4, Y = 2 });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_TwoArgsFunctionField()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "lineFunc", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        Assert.AreEqual(12, args[1]["X"]);
        Assert.AreEqual(18, args[1]["Y"]);
        functionCalled = true;
      });

      m_module.JSLineFunction(new Point { X = 4, Y = 2 }, new Point { X = 12, Y = 18 });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_NoArgFunctionField()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "NoArgFunction", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(0, args.Count);
        functionCalled = true;
      });

      m_module.NoArgFunction();
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_JSModuleNameFunctionField()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("MyModule", "stringFunc", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual("Hello World!", args[0]);
        functionCalled = true;
      });

      m_module.JSStringFunction("Hello World!");
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_JSValueObjectFunctionField()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "JSValueFunction", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        functionCalled = true;
      });

      m_module.JSValueFunction(new JSValueObject { { "X", 4 }, { "Y", 2 } });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_JSValueArrayFunctionField()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "JSValueFunction", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual("X", args[0][0]);
        Assert.AreEqual(4, args[0][1]);
        Assert.AreEqual(true, args[0][2]);
        Assert.AreEqual(42, args[0][3]["Id"]);
        functionCalled = true;
      });

      m_module.JSValueFunction(new JSValueArray { "X", 4, true, new JSValueObject { { "Id", 42 } } });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_JSIntFunctionProperty()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "JSIntFunctionProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(42, args[0]);
        functionCalled = true;
      });

      m_module.JSIntFunctionProp(42);
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_JSNameFunctionProperty()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "pointFuncProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        functionCalled = true;
      });

      m_module.JSPointFunctionProp(new Point { X = 4, Y = 2 });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_TwoArgsFunctionProperty()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "lineFuncProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        Assert.AreEqual(12, args[1]["X"]);
        Assert.AreEqual(18, args[1]["Y"]);
        functionCalled = true;
      });

      m_module.JSLineFunctionProp(new Point { X = 4, Y = 2 }, new Point { X = 12, Y = 18 });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_NoArgFunctionProperty()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "NoArgFunctionProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(0, args.Count);
        functionCalled = true;
      });

      m_module.NoArgFunctionProp();
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_JSModuleNameFunctionProperty()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("MyModule", "stringFuncProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual("Hello World!", args[0]);
        functionCalled = true;
      });

      m_module.JSStringFunctionProp("Hello World!");
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_JSValueObjectFunctionProperty()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "JSValueFunctionProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        functionCalled = true;
      });

      m_module.JSValueFunctionProp(new JSValueObject { { "X", 4 }, { "Y", 2 } });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_JSValueArrayFunctionProperty()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule", "JSValueFunctionProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual("X", args[0][0]);
        Assert.AreEqual(4, args[0][1]);
        Assert.AreEqual(true, args[0][2]);
        Assert.AreEqual(42, args[0][3]["Id"]);
        functionCalled = true;
      });

      m_module.JSValueFunctionProp(new JSValueArray { "X", 4, true, new JSValueObject { { "Id", 42 } } });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestInitialized()
    {
      Assert.IsTrue(m_module.IsInitialized);
    }
  }
}
