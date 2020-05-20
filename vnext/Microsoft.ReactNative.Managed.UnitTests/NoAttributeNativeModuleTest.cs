// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Microsoft.ReactNative.Managed.UnitTests
{
  class SimpleNativeModule2
  {
    public void Initialize(IReactContext context)
    {
      IsInitialized = true;
      Assert.IsNotNull(context);

      // Event and Function fields are initialized before ReactInitializer method call.
      Assert.IsNotNull(OnIntEvent);
      Assert.IsNotNull(JSIntFunction);
    }

    public int Add(int x, int y)
    {
      return x + y;
    }

    public int Negate(int x)
    {
      return -x;
    }

    public string SayHello()
    {
      return "Hello";
    }

    public static int StaticAdd(int x, int y)
    {
      return x + y;
    }

    public static int StaticNegate(int x)
    {
      return -x;
    }

    public static string StaticSayHello()
    {
      return "Hello";
    }

    public void SayHello0()
    {
      Message = "Hello_0";
    }

    public void PrintPoint(Point pt)
    {
      Message = $"Point: ({pt.X}, {pt.Y})";
    }

    public void PrintLine(Point start, Point end)
    {
      Message = $"Line: ({start.X}, {start.Y})-({end.X}, {end.Y})";
    }

    public static void StaticSayHello1()
    {
      StaticMessage = "Hello_1";
    }

    public static void StaticPrintPoint(Point pt)
    {
      StaticMessage = $"Static Point: ({pt.X}, {pt.Y})";
    }

    public static void StaticPrintLine(Point start, Point end)
    {
      StaticMessage = $"Static Line: ({start.X}, {start.Y})-({end.X}, {end.Y})";
    }

    public void AddCallback(int x, int y, Action<int> resolve)
    {
      resolve(x + y);
    }

    public void NegateCallback(int x, Action<int> resolve)
    {
      resolve(-x);
    }

    public async void NegateAsyncCallback(int x, Action<int> resolve)
    {
      bool isPositive = await Task.Run(() => x >= 0);
      resolve(isPositive ? -x : x);
    }

    public void SayHelloCallback(Action<string> resolve)
    {
      resolve("Hello_2");
    }

    public static void StaticAddCallback(int x, int y, Action<int> resolve)
    {
      resolve(x + y);
    }

    public static void StaticNegateCallback(int x, Action<int> resolve)
    {
      resolve(-x);
    }

    public static async void StaticNegateAsyncCallback(int x, Action<int> resolve)
    {
      bool isPositive = await Task.Run(() => x >= 0);
      resolve(isPositive ? -x : x);
    }

    public static void StaticSayHelloCallback(Action<string> resolve)
    {
      resolve("Static Hello_2");
    }

    public void CallbackZeroArgs(Action resolve)
    {
      resolve();
    }

    public void CallbackTwoArgs(Action<int, int> resolve)
    {
      resolve(1, 2);
    }

    public void CallbackThreeArgs(Action<int, int, string> resolve)
    {
      resolve(1, 2, "Hello");
    }

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

    public void ResolveSayHelloCallbacks(Action<string> resolve, Action<string> _)
    {
      resolve("Hello_3");
    }

    public void RejectSayHelloCallbacks(Action<string> _, Action<string> reject)
    {
      reject("Goodbye");
    }

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

    public static void StaticResolveSayHelloCallbacks(Action<string> resolve, Action<string> _)
    {
      resolve("Hello_3");
    }

    public static void StaticRejectSayHelloCallbacks(Action<string> _, Action<string> reject)
    {
      reject("Goodbye");
    }

    public void TwoCallbacksZeroArgs1(Action resolve, Action _)
    {
      resolve();
    }

    public void TwoCallbacksZeroArgs2(Action _, Action reject)
    {
      reject();
    }

    public void TwoCallbacksTwoArgs1(Action<int, int> resolve, Action<int, int> _)
    {
      resolve(1, 2);
    }

    public void TwoCallbacksTwoArgs2(Action<int, int> _, Action<int, int> reject)
    {
      reject(1, 2);
    }

    public void TwoCallbacksThreeArgs1(Action<int, int, string> resolve, Action<int, int, string> _)
    {
      resolve(1, 2, "Hello");
    }

    public void TwoCallbacksThreeArgs2(Action<int, int, string> _, Action<int, int, string> reject)
    {
      reject(1, 2, "Hello");
    }

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

    public void ResolveSayHelloPromise(IReactPromise<string> promise)
    {
      promise.Resolve("Hello_3");
    }

    public void RejectSayHelloPromise(IReactPromise<string> promise)
    {
      promise.Reject(new ReactError { Message = "Promise rejected" });
    }

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

    public static void StaticResolveSayHelloPromise(IReactPromise<string> promise)
    {
      promise.Resolve("Hello_3");
    }

    public static void StaticRejectSayHelloPromise(IReactPromise<string> promise)
    {
      promise.Reject(new ReactError { Message = "Promise rejected" });
    }

    public int AddSync(int x, int y)
    {
      return x + y;
    }

    public int NegateSync(int x)
    {
      return -x;
    }

    public string SayHelloSync()
    {
      return "Hello";
    }

    public static int StaticAddSync(int x, int y)
    {
      return x + y;
    }

    public static int StaticNegateSync(int x)
    {
      return -x;
    }

    public static string StaticSayHelloSync()
    {
      return "Hello";
    }

    public readonly string Constant1 = "MyConstant1";

    public static readonly string Constant2 = "MyConstant2";

    public Point Constant3 => new Point { X = 2, Y = 3 };

    public static Point Constant4 => new Point { X = 3, Y = 4 };

    public void Constant5(ReactConstantProvider provider)
    {
      provider.Add("const51", new Point { X = 12, Y = 14 });
      provider.Add("const52", "MyConstant52");
    }

    public static void Constant6(ReactConstantProvider provider)
    {
      provider.Add("const61", new Point { X = 15, Y = 17 });
      provider.Add("const62", "MyConstant62");
    }

    // Field that allows to emit native module events.
    public Action<int> OnIntEvent = null;

    // An event without arguments
    public Action OnNoArgEvent = null;

    // An event with two arguments
    public Action<Point, Point> OnTwoArgsEvent = null;

    public Action<Point> OnPointEvent = null;

    public Action<string> OnStringEvent = null;

    // Use JSValue which is an immutable JSON-like data representation.
    public Action<JSValue> OnJSValueEvent = null;

    // Property that allows to emit native module events.
    public Action<int> OnIntEventProp { get; set; }

    // An event without arguments
    public Action OnNoArgEventProp { get; set; }

    // An event with two arguments
    public Action<Point, Point> OnTwoArgsEventProp { get; set; }

    public Action<Point> OnPointEventProp { get; set; }

    public Action<string> OnStringEventProp { get; set; }

    // Use JSValue which is an immutable JSON-like data representation.
    public Action<JSValue> OnJSValueEventProp { get; set; }

    // Field that allows to call JS functions.
    public Action<int> JSIntFunction = null;

    public Action<Point> JSPointFunction = null;

    // Use two arguments. Specify JS function name different from the field name.
    public Action<Point, Point> JSLineFunction = null;

    // Use no arguments.
    public Action NoArgFunction = null;

    public Action<string> JSStringFunction = null;

    // Use JSValue which is an immutable JSON-like data representation.
    public Action<JSValue> JSValueFunction = null;

    // Property that allows to call JS functions.
    public Action<int> JSIntFunctionProp { get; set; }

    public Action<Point> JSPointFunctionProp { get; set; }

    // Use two arguments. Specify JS function name different from the property name.
    public Action<Point, Point> JSLineFunctionProp { get; set; }

    // Use no arguments.
    public Action NoArgFunctionProp { get; set; }

    public Action<string> JSStringFunctionProp { get; set; }

    // Use JSValue which is an immutable JSON-like data representation.
    public Action<JSValue> JSValueFunctionProp { get; set; }

    // Properties below are not exposed to JS and only used by tests to observe results.
    public bool IsInitialized { get; set; }
    public string Message { get; set; }
    public static string StaticMessage { get; set; }
  }

  class ModuleMetadataBuilder<TModule>
  {
    private TModule m_module;
    private IReactModuleBuilder m_moduleBuilder;
    private List<InitializerDelegate> m_initializers = new List<InitializerDelegate>();

    public ModuleMetadataBuilder(TModule module, IReactModuleBuilder moduleBuilder)
    {
      m_module = module;
      m_moduleBuilder = moduleBuilder;
    }

    public string ModuleName { get; set; }

    public string EventEmitterName { get; set; }

    public void AddInitializer(Func<TModule, InitializerDelegate> getInitializer)
    {
      m_initializers.Add(getInitializer(m_module));
    }

    public void AddMethod<TResult>(string name, Func<TModule, Func<TResult>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Callback,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          var result = method();
          outputWriter.WriteArgs(result);
          resolve(outputWriter);
        });
    }

    public void AddMethod<T1, TResult>(string name, Func<TModule, Func<T1, TResult>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Callback,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1);
          var result = method(arg1);
          outputWriter.WriteArgs(result);
          resolve(outputWriter);
        });
    }

    public void AddMethod<T1, T2, TResult>(string name, Func<TModule, Func<T1, T2, TResult>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Callback,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1, out T2 arg2);
          var result = method(arg1, arg2);
          outputWriter.WriteArgs(result);
          resolve(outputWriter);
        });
    }

    public void AddMethod(string name, Func<TModule, Action> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Void,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method();
        });
    }

    public void AddMethod<T1>(string name, Func<TModule, Action<T1>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Void,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1);
          method(arg1);
        });
    }

    public void AddMethod<T1, T2>(string name, Func<TModule, Action<T1, T2>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Void,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1, out T2 arg2);
          method(arg1, arg2);
        });
    }

    public void AddMethod(string name, Func<TModule, Action<Action>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Callback,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method(() => resolve(outputWriter.WriteArgs()));
        });
    }

    public void AddMethod<TResult1>(string name, Func<TModule, Action<Action<TResult1>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Callback,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method(result1 => resolve(outputWriter.WriteArgs(result1)));
        });
    }

    public void AddMethod<TResult1, TResult2>(string name, Func<TModule, Action<Action<TResult1, TResult2>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Callback,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method((result1, result2) => resolve(outputWriter.WriteArgs(result1, result2)));
        });
    }

    public void AddMethod<TResult1, TResult2, TResult3>(string name, Func<TModule, Action<Action<TResult1, TResult2, TResult3>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Callback,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method((result1, result2, result3) => resolve(outputWriter.WriteArgs(result1, result2, result3)));
        });
    }

    public void AddMethod<T1, TResult>(string name, Func<TModule, Action<T1, Action<TResult>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Callback,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1);
          method(arg1, result => resolve(outputWriter.WriteArgs(result)));
        });
    }

    public void AddMethod<T1, T2, TResult>(string name, Func<TModule, Action<T1, T2, Action<TResult>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Callback,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1, out T2 arg2);
          method(arg1, arg2, result => resolve(outputWriter.WriteArgs(result)));
        });
    }

    public void AddMethod(string name, Func<TModule, Action<Action, Action>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.TwoCallbacks,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method(() => resolve(outputWriter.WriteArgs()), () => reject(outputWriter.WriteArgs()));
        });
    }

    public void AddMethod<T1, T2>(string name, Func<TModule, Action<Action<T1>, Action<T2>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.TwoCallbacks,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method(result => resolve(outputWriter.WriteArgs(result)), result => reject(outputWriter.WriteArgs(result)));
        });
    }

    public void AddMethod<T1, T2, T3, T4>(string name, Func<TModule, Action<Action<T1, T2>, Action<T3, T4>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.TwoCallbacks,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method(
            (result1, result2) => resolve(outputWriter.WriteArgs(result1, result2)),
            (result1, result2) => reject(outputWriter.WriteArgs(result1, result2)));
        });
    }

    public void AddMethod<T1, T2, T3, T4, T5, T6>(string name, Func<TModule, Action<Action<T1, T2, T3>, Action<T4, T5, T6>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.TwoCallbacks,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method(
            (result1, result2, result3) => resolve(outputWriter.WriteArgs(result1, result2, result3)),
            (result1, result2, result3) => reject(outputWriter.WriteArgs(result1, result2, result3)));
        });
    }

    public void AddMethod<T1, T2, T3>(string name, Func<TModule, Action<T1, Action<T2>, Action<T3>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.TwoCallbacks,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1);
          method(arg1, result => resolve(outputWriter.WriteArgs(result)), result => reject(outputWriter.WriteArgs(result)));
        });
    }

    public void AddMethod<T1, T2, T3, T4>(string name, Func<TModule, Action<T1, T2, Action<T3>, Action<T4>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.TwoCallbacks,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1, out T2 arg2);
          method(arg1, arg2, result => resolve(outputWriter.WriteArgs(result)), result => reject(outputWriter.WriteArgs(result)));
        });
    }

    public void AddMethod<T1>(string name, Func<TModule, Action<IReactPromise<T1>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Promise,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          method(new ReactPromise<T1>(outputWriter, resolve, reject));
        });
    }

    public void AddMethod<T1, T2>(string name, Func<TModule, Action<T1, IReactPromise<T2>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Promise,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1);
          method(arg1, new ReactPromise<T2>(outputWriter, resolve, reject));
        });
    }

    public void AddMethod<T1, T2, T3>(string name, Func<TModule, Action<T1, T2, IReactPromise<T3>>> getMethod)
    {
      m_moduleBuilder.AddMethod(name, MethodReturnType.Promise,
        (IJSValueReader inputReader, IJSValueWriter outputWriter, MethodResultCallback resolve, MethodResultCallback reject) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1, out T2 arg2);
          method(arg1, arg2, new ReactPromise<T3>(outputWriter, resolve, reject));
        });
    }

    public void AddSyncMethod<TResult>(string name, Func<TModule, Func<TResult>> getMethod)
    {
      m_moduleBuilder.AddSyncMethod(name, (IJSValueReader inputReader, IJSValueWriter outputWriter) =>
      {
        var method = getMethod(m_module);
        TResult result = method();
        outputWriter.WriteArgs(result);
      });
    }

    public void AddSyncMethod<T1, TResult>(string name, Func<TModule, Func<T1, TResult>> getMethod)
    {
      m_moduleBuilder.AddSyncMethod(name, (IJSValueReader inputReader, IJSValueWriter outputWriter) =>
      {
        var method = getMethod(m_module);
        inputReader.ReadArgs(out T1 arg1);
        TResult result = method(arg1);
        outputWriter.WriteArgs(result);
      });
    }

    public void AddSyncMethod<T1, T2, TResult>(string name, Func<TModule, Func<T1, T2, TResult>> getMethod)
    {
      m_moduleBuilder.AddSyncMethod(name, (IJSValueReader inputReader, IJSValueWriter outputWriter) =>
        {
          var method = getMethod(m_module);
          inputReader.ReadArgs(out T1 arg1, out T2 arg2);
          TResult result = method(arg1, arg2);
          outputWriter.WriteArgs(result);
        });
    }

    public void AddConstant<T>(string name, Func<TModule, T> getConstant)
    {
      m_moduleBuilder.AddConstantProvider(constantWriter => constantWriter.WriteObjectProperty(name, getConstant(m_module)));
    }

    public void AddConstantProvider(Func<TModule, Action<ReactConstantProvider>> getConstantProvider)
    {
      m_moduleBuilder.AddConstantProvider(constantWriter => getConstantProvider(m_module)(new ReactConstantProvider(constantWriter)));
    }

    public void AddJSEvent(string eventEmitterName, string name, Action<TModule, Action> setEventHandler)
    {
      m_moduleBuilder.AddInitializer(reactContext =>
      {
        setEventHandler(m_module, () => new ReactContext(reactContext).EmitJSEvent(
          eventEmitterName ?? EventEmitterName ?? "RCTDeviceEventEmitter", name));
      });
    }

    public void AddJSEvent<T1>(string eventEmitterName, string name, Action<TModule, Action<T1>> setEventHandler)
    {
      m_moduleBuilder.AddInitializer(reactContext =>
      {
        setEventHandler(m_module, (T1 arg1) => new ReactContext(reactContext).EmitJSEvent(
          eventEmitterName ?? EventEmitterName ?? "RCTDeviceEventEmitter", name, arg1));
      });
    }

    public void AddJSEvent<T1, T2>(string eventEmitterName, string name, Action<TModule, Action<T1, T2>> setEventHandler)
    {
      m_moduleBuilder.AddInitializer(reactContext =>
      {
        setEventHandler(m_module, (T1 arg1, T2 arg2) => new ReactContext(reactContext).EmitJSEvent(
          eventEmitterName ?? EventEmitterName ?? "RCTDeviceEventEmitter", name, arg1, arg2));
      });
    }

    public void AddJSFunction(string moduleName, string name, Action<TModule, Action> setFunctionHandler)
    {
      m_moduleBuilder.AddInitializer(reactContext =>
      {
        setFunctionHandler(m_module, () => new ReactContext(reactContext).CallJSFunction(
          moduleName ?? ModuleName, name));
      });
    }

    public void AddJSFunction<T1>(string moduleName, string name, Action<TModule, Action<T1>> setFunctionHandler)
    {
      m_moduleBuilder.AddInitializer(reactContext =>
      {
        setFunctionHandler(m_module, (T1 arg1) => new ReactContext(reactContext).CallJSFunction(
          moduleName ?? ModuleName, name, arg1));
      });
    }

    public void AddJSFunction<T1, T2>(string moduleName, string name, Action<TModule, Action<T1, T2>> setFunctionHandler)
    {
      m_moduleBuilder.AddInitializer(reactContext =>
      {
        setFunctionHandler(m_module, (T1 arg1, T2 arg2) => new ReactContext(reactContext).CallJSFunction(
          moduleName ?? ModuleName, name, arg1, arg2));
      });
    }

    public void Complete()
    {
      // Add initializers in the end to have them invoked after event and function fields are initialized.
      foreach (var initializer in m_initializers)
      {
        m_moduleBuilder.AddInitializer(initializer);
      }
    }
  }

  [TestClass]
  public class NoAttributeNativeModuleTest
  {
    private ReactModuleBuilderMock m_moduleBuilderMock;
    private SimpleNativeModule2 m_module;

    [TestInitialize]
    public void Initialize()
    {
      m_moduleBuilderMock = new ReactModuleBuilderMock();
      m_module = new SimpleNativeModule2(); // m_moduleBuilderMock.CreateModule<SimpleNativeModule2>(m_moduleInfo);

      var mb = new ModuleMetadataBuilder<SimpleNativeModule2>(m_module, m_moduleBuilderMock);
      mb.ModuleName = "SimpleNativeModule2";

      mb.AddInitializer(m => m.Initialize);

      mb.AddMethod("Add", m => new Func<int, int, int>(m.Add));
      mb.AddMethod("Negate", m => new Func<int, int>(m.Negate));
      mb.AddMethod("SayHello", m => new Func<string>(m.SayHello));
      mb.AddMethod("StaticAdd", m => new Func<int, int, int>(SimpleNativeModule2.StaticAdd));
      mb.AddMethod("StaticNegate", m => new Func<int, int>(SimpleNativeModule2.StaticNegate));
      mb.AddMethod("StaticSayHello", m => new Func<string>(SimpleNativeModule2.StaticSayHello));

      mb.AddMethod("SayHello0", m => new Action(m.SayHello0));
      mb.AddMethod("PrintPoint", m => new Action<Point>(m.PrintPoint));
      mb.AddMethod("PrintLine", m => new Action<Point, Point>(m.PrintLine));
      mb.AddMethod("StaticSayHello1", m => new Action(SimpleNativeModule2.StaticSayHello1));
      mb.AddMethod("StaticPrintPoint", m => new Action<Point>(SimpleNativeModule2.StaticPrintPoint));
      mb.AddMethod("StaticPrintLine", m => new Action<Point, Point>(SimpleNativeModule2.StaticPrintLine));

      mb.AddMethod("AddCallback", m => new Action<int, int, Action<int>>(m.AddCallback));
      mb.AddMethod("NegateCallback", m => new Action<int, Action<int>>(m.NegateCallback));
      mb.AddMethod("NegateAsyncCallback", m => new Action<int, Action<int>>(m.NegateAsyncCallback));
      mb.AddMethod("SayHelloCallback", m => new Action<Action<string>>(m.SayHelloCallback));
      mb.AddMethod("StaticAddCallback", m => new Action<int, int, Action<int>>(SimpleNativeModule2.StaticAddCallback));
      mb.AddMethod("StaticNegateCallback", m => new Action<int, Action<int>>(SimpleNativeModule2.StaticNegateCallback));
      mb.AddMethod("StaticNegateAsyncCallback", m => new Action<int, Action<int>>(SimpleNativeModule2.StaticNegateAsyncCallback));
      mb.AddMethod("StaticSayHelloCallback", m => new Action<Action<string>>(SimpleNativeModule2.StaticSayHelloCallback));

      mb.AddMethod("CallbackZeroArgs", m => new Action<Action>(m.CallbackZeroArgs));
      mb.AddMethod("CallbackTwoArgs", m => new Action<Action<int, int>>(m.CallbackTwoArgs));
      mb.AddMethod("CallbackThreeArgs", m => new Action<Action<int, int, string>>(m.CallbackThreeArgs));

      mb.AddMethod("DivideCallbacks", m => new Action<int, int, Action<int>, Action<string>>(m.DivideCallbacks));
      mb.AddMethod("NegateCallbacks", m => new Action<int, Action<int>, Action<string>>(m.NegateCallbacks));
      mb.AddMethod("NegateAsyncCallbacks", m => new Action<int, Action<int>, Action<string>>(m.NegateAsyncCallbacks));
      mb.AddMethod("ResolveSayHelloCallbacks", m => new Action<Action<string>, Action<string>>(m.ResolveSayHelloCallbacks));
      mb.AddMethod("RejectSayHelloCallbacks", m => new Action<Action<string>, Action<string>>(m.RejectSayHelloCallbacks));
      mb.AddMethod("StaticDivideCallbacks", m => new Action<int, int, Action<int>, Action<string>>(SimpleNativeModule2.StaticDivideCallbacks));
      mb.AddMethod("StaticNegateCallbacks", m => new Action<int, Action<int>, Action<string>>(SimpleNativeModule2.StaticNegateCallbacks));
      mb.AddMethod("StaticNegateAsyncCallbacks", m => new Action<int, Action<int>, Action<string>>(SimpleNativeModule2.StaticNegateAsyncCallbacks));
      mb.AddMethod("StaticResolveSayHelloCallbacks", m => new Action<Action<string>, Action<string>>(SimpleNativeModule2.StaticResolveSayHelloCallbacks));
      mb.AddMethod("StaticRejectSayHelloCallbacks", m => new Action<Action<string>, Action<string>>(SimpleNativeModule2.StaticRejectSayHelloCallbacks));

      mb.AddMethod("TwoCallbacksZeroArgs1", m => new Action<Action, Action>(m.TwoCallbacksZeroArgs1));
      mb.AddMethod("TwoCallbacksZeroArgs2", m => new Action<Action, Action>(m.TwoCallbacksZeroArgs2));
      mb.AddMethod("TwoCallbacksTwoArgs1", m => new Action<Action<int, int>, Action<int, int>>(m.TwoCallbacksTwoArgs1));
      mb.AddMethod("TwoCallbacksTwoArgs2", m => new Action<Action<int, int>, Action<int, int>>(m.TwoCallbacksTwoArgs2));
      mb.AddMethod("TwoCallbacksThreeArgs1", m => new Action<Action<int, int, string>, Action<int, int, string>>(m.TwoCallbacksThreeArgs1));
      mb.AddMethod("TwoCallbacksThreeArgs2", m => new Action<Action<int, int, string>, Action<int, int, string>>(m.TwoCallbacksThreeArgs2));

      mb.AddMethod("DividePromise", m => new Action<int, int, IReactPromise<int>>(m.DividePromise));
      mb.AddMethod("NegatePromise", m => new Action<int, IReactPromise<int>>(m.NegatePromise));
      mb.AddMethod("NegateAsyncPromise", m => new Action<int, IReactPromise<int>>(m.NegateAsyncPromise));
      mb.AddMethod("voidPromise", m => new Action<int, IReactPromise<JSValue.Void>>(m.VoidPromise));
      mb.AddMethod("ResolveSayHelloPromise", m => new Action<IReactPromise<string>>(m.ResolveSayHelloPromise));
      mb.AddMethod("RejectSayHelloPromise", m => new Action<IReactPromise<string>>(m.RejectSayHelloPromise));
      mb.AddMethod("StaticDividePromise", m => new Action<int, int, IReactPromise<int>>(SimpleNativeModule2.StaticDividePromise));
      mb.AddMethod("StaticNegatePromise", m => new Action<int, IReactPromise<int>>(SimpleNativeModule2.StaticNegatePromise));
      mb.AddMethod("StaticNegateAsyncPromise", m => new Action<int, IReactPromise<int>>(SimpleNativeModule2.StaticNegateAsyncPromise));
      mb.AddMethod("staticVoidPromise", m => new Action<int, IReactPromise<JSValue.Void>>(SimpleNativeModule2.StaticVoidPromise));
      mb.AddMethod("StaticResolveSayHelloPromise", m => new Action<IReactPromise<string>>(SimpleNativeModule2.StaticResolveSayHelloPromise));
      mb.AddMethod("StaticRejectSayHelloPromise", m => new Action<IReactPromise<string>>(SimpleNativeModule2.StaticRejectSayHelloPromise));

      mb.AddSyncMethod("AddSync", m => new Func<int, int, int>(m.AddSync));
      mb.AddSyncMethod("NegateSync", m => new Func<int, int>(m.NegateSync));
      mb.AddSyncMethod("SayHelloSync", m => new Func<string>(m.SayHelloSync));
      mb.AddSyncMethod("StaticAddSync", m => new Func<int, int, int>(SimpleNativeModule2.StaticAddSync));
      mb.AddSyncMethod("StaticNegateSync", m => new Func<int, int>(SimpleNativeModule2.StaticNegateSync));
      mb.AddSyncMethod<string>("StaticSayHelloSync", m => SimpleNativeModule2.StaticSayHelloSync);

      mb.AddConstant("Constant1", m => m.Constant1);
      mb.AddConstant("const2", m => SimpleNativeModule2.Constant2);
      mb.AddConstant("const3", m => m.Constant3);
      mb.AddConstant("Constant4", m => SimpleNativeModule2.Constant4);

      mb.AddConstantProvider(m => m.Constant5);
      mb.AddConstantProvider(m => SimpleNativeModule2.Constant6);

      mb.AddJSEvent<int>(null, "OnIntEvent", (m, handler) => { m.OnIntEvent = handler; });
      mb.AddJSEvent(null, "OnNoArgEvent", (m, handler) => { m.OnNoArgEvent = handler; });
      mb.AddJSEvent<Point, Point>(null, "OnTwoArgsEvent", (m, handler) => { m.OnTwoArgsEvent = handler; });
      mb.AddJSEvent<Point>(null, "onPointEvent", (m, handler) => { m.OnPointEvent = handler; });
      mb.AddJSEvent<string>("MyEventEmitter", "onStringEvent", (m, handler) => { m.OnStringEvent = handler; });
      mb.AddJSEvent<JSValue>(null, "OnJSValueEvent", (m, handler) => { m.OnJSValueEvent = handler; });

      mb.AddJSEvent<int>(null, "OnIntEventProp", (m, handler) => { m.OnIntEventProp = handler; });
      mb.AddJSEvent(null, "OnNoArgEventProp", (m, handler) => { m.OnNoArgEventProp = handler; });
      mb.AddJSEvent<Point, Point>(null, "OnTwoArgsEventProp", (m, handler) => { m.OnTwoArgsEventProp = handler; });
      mb.AddJSEvent<Point>(null, "onPointEventProp", (m, handler) => { m.OnPointEventProp = handler; });
      mb.AddJSEvent<string>("MyEventEmitter", "onStringEventProp", (m, handler) => { m.OnStringEventProp = handler; });
      mb.AddJSEvent<JSValue>(null, "OnJSValueEventProp", (m, handler) => { m.OnJSValueEventProp = handler; });

      mb.AddJSFunction<int>(null, "JSIntFunction", (m, handler) => { m.JSIntFunction = handler; });
      mb.AddJSFunction<Point>(null, "pointFunc", (m, handler) => { m.JSPointFunction = handler; });
      mb.AddJSFunction<Point, Point>(null, "lineFunc", (m, handler) => { m.JSLineFunction = handler; });
      mb.AddJSFunction(null, "NoArgFunction", (m, handler) => { m.NoArgFunction = handler; });
      mb.AddJSFunction<string>("MyModule", "stringFunc", (m, handler) => { m.JSStringFunction = handler; });
      mb.AddJSFunction<JSValue>(null, "JSValueFunction", (m, handler) => { m.JSValueFunction = handler; });

      mb.AddJSFunction<int>(null, "JSIntFunctionProp", (m, handler) => { m.JSIntFunctionProp = handler; });
      mb.AddJSFunction<Point>(null, "pointFuncProp", (m, handler) => { m.JSPointFunctionProp = handler; });
      mb.AddJSFunction<Point, Point>(null, "lineFuncProp", (m, handler) => { m.JSLineFunctionProp = handler; });
      mb.AddJSFunction(null, "NoArgFunctionProp", (m, handler) => { m.NoArgFunctionProp = handler; });
      mb.AddJSFunction<string>("MyModule", "stringFuncProp", (m, handler) => { m.JSStringFunctionProp = handler; });
      mb.AddJSFunction<JSValue>(null, "JSValueFunctionProp", (m, handler) => { m.JSValueFunctionProp = handler; });

      mb.Complete();

      m_moduleBuilderMock.Initialize();
    }

    [TestMethod]
    public void TestMethodCall_Add()
    {
      m_moduleBuilderMock.Call1<int, int, Action<int>>(nameof(SimpleNativeModule2.Add), 3, 5, (int result) => Assert.AreEqual(8, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_Negate()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule2.Negate), 3, (int result) => Assert.AreEqual(-3, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_SayHello()
    {
      m_moduleBuilderMock.Call1<Action<string>>(nameof(SimpleNativeModule2.SayHello), (string result) => Assert.AreEqual("Hello", result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticAdd()
    {
      m_moduleBuilderMock.Call1<int, int, Action<int>>(nameof(SimpleNativeModule2.StaticAdd), 3, 5, (int result) => Assert.AreEqual(8, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegate()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule2.StaticNegate), 3, (int result) => Assert.AreEqual(-3, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticSayHello()
    {
      m_moduleBuilderMock.Call1<Action<string>>(nameof(SimpleNativeModule2.StaticSayHello), (string result) => Assert.AreEqual("Hello", result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_SayHello0()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule2.SayHello0));
      Assert.AreEqual("Hello_0", m_module.Message);
    }

    [TestMethod]
    public void TestMethodCall_PrintPoint()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule2.PrintPoint), new Point { X = 3, Y = 5 });
      Assert.AreEqual("Point: (3, 5)", m_module.Message);
    }

    [TestMethod]
    public void TestMethodCall_PrintLine()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule2.PrintLine), new Point { X = 3, Y = 5 }, new Point { X = 6, Y = 8 });
      Assert.AreEqual("Line: (3, 5)-(6, 8)", m_module.Message);
    }

    [TestMethod]
    public void TestMethodCall_StaticSayHello1()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule2.StaticSayHello1));
      Assert.AreEqual("Hello_1", SimpleNativeModule2.StaticMessage);
    }

    [TestMethod]
    public void TestMethodCall_StaticPrintPoint()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule2.StaticPrintPoint), new Point { X = 13, Y = 15 });
      Assert.AreEqual("Static Point: (13, 15)", SimpleNativeModule2.StaticMessage);
    }

    [TestMethod]
    public void TestMethodCall_StaticPrintLine()
    {
      m_moduleBuilderMock.Call0(nameof(SimpleNativeModule2.StaticPrintLine),
          new Point { X = 13, Y = 15 }, new Point { X = 16, Y = 18 });
      Assert.AreEqual("Static Line: (13, 15)-(16, 18)", SimpleNativeModule2.StaticMessage);
    }

    [TestMethod]
    public void TestMethodCall_AddCallback()
    {
      m_moduleBuilderMock.Call1<int, int, Action<int>>(nameof(SimpleNativeModule2.AddCallback), 7, -8, (int result) => Assert.AreEqual(-1, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateCallback()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule2.NegateCallback), 3, (int result) => Assert.AreEqual(-3, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncCallback()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule2.NegateAsyncCallback), 3, (int result) => Assert.AreEqual(-3, result)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_SayHelloCallback()
    {
      m_moduleBuilderMock.Call1<Action<string>>(nameof(SimpleNativeModule2.SayHelloCallback), (string result) => Assert.AreEqual("Hello_2", result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticAddCallback()
    {
      m_moduleBuilderMock.Call1<int, int, Action<int>>(nameof(SimpleNativeModule2.StaticAddCallback), 4, 56, (int result) => Assert.AreEqual(60, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateCallback()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule2.StaticNegateCallback), 33, (int result) => Assert.AreEqual(-33, result));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncCallback()
    {
      m_moduleBuilderMock.Call1<int, Action<int>>(nameof(SimpleNativeModule2.StaticNegateAsyncCallback), 33, (int result) => Assert.AreEqual(-33, result)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticSayHelloCallback()
    {
      m_moduleBuilderMock.Call1<Action<string>>(nameof(SimpleNativeModule2.StaticSayHelloCallback), (string result) => Assert.AreEqual("Static Hello_2", result));
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
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.DivideCallbacks), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (string error) => Assert.AreEqual("Division by 0", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DivideCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.DivideCallbacks), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (string error) => Assert.AreEqual("Division by 0", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateCallbacks()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.NegateCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (string error) => Assert.AreEqual("Already negative", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.NegateCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (string error) => Assert.AreEqual("Already negative", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncCallbacks()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.NegateAsyncCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (string error) => Assert.AreEqual("Already negative", error)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.NegateAsyncCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (string error) => Assert.AreEqual("Already negative", error)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_ResolveSayHelloCallbacks()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<string>>(nameof(SimpleNativeModule2.ResolveSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (string error) => Assert.AreEqual("Goodbye", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_RejectSayHelloCallbacks()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<string>>(nameof(SimpleNativeModule2.RejectSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (string error) => Assert.AreEqual("Goodbye", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDivideCallbacks()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.StaticDivideCallbacks), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (string error) => Assert.AreEqual("Division by 0", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDivideCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.StaticDivideCallbacks), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (string error) => Assert.AreEqual("Division by 0", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateCallbacks()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.StaticNegateCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (string error) => Assert.AreEqual("Already negative", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.StaticNegateCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (string error) => Assert.AreEqual("Already negative", error));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncCallbacks()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.StaticNegateAsyncCallbacks), 5,
          (int result) => Assert.AreEqual(-5, result),
          (string error) => Assert.AreEqual("Already negative", error)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncCallbacksError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<string>>(nameof(SimpleNativeModule2.StaticNegateAsyncCallbacks), -5,
          (int result) => Assert.AreEqual(5, result),
          (string error) => Assert.AreEqual("Already negative", error)).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticResolveSayHelloCallbacks()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<string>>(nameof(SimpleNativeModule2.StaticResolveSayHelloCallbacks),
          (string result) => Assert.AreEqual("Hello_3", result),
          (string error) => Assert.AreEqual("Goodbye", error));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticRejectSayHelloCallbacks()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<string>>(nameof(SimpleNativeModule2.StaticRejectSayHelloCallbacks),
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
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.DividePromise), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_DividePromiseError()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.DividePromise), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegatePromise()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.NegatePromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegatePromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.NegatePromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncPromise()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.NegateAsyncPromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"])).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_NegateAsyncPromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.NegateAsyncPromise), -5,
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
      m_moduleBuilderMock.Call2<Action<string>, Action<JSValue>>(nameof(SimpleNativeModule2.ResolveSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_RejectSayHelloPromise()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<JSValue>>(nameof(SimpleNativeModule2.RejectSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDividePromise()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.StaticDividePromise), 6, 2,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticDividePromiseError()
    {
      m_moduleBuilderMock.Call2<int, int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.StaticDividePromise), 6, 0,
          (int result) => Assert.AreEqual(3, result),
          (JSValue error) => Assert.AreEqual("Division by 0", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegatePromise()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.StaticNegatePromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegatePromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.StaticNegatePromise), -5,
          (int result) => Assert.AreEqual(5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncPromise()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.StaticNegateAsyncPromise), 5,
          (int result) => Assert.AreEqual(-5, result),
          (JSValue error) => Assert.AreEqual("Already negative", error["message"])).Wait();
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticNegateAsyncPromiseError()
    {
      m_moduleBuilderMock.Call2<int, Action<int>, Action<JSValue>>(nameof(SimpleNativeModule2.StaticNegateAsyncPromise), -5,
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
      m_moduleBuilderMock.Call2<Action<string>, Action<JSValue>>(nameof(SimpleNativeModule2.StaticResolveSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsResolveCallbackCalled);
    }

    [TestMethod]
    public void TestMethodCall_StaticRejectSayHelloPromise()
    {
      m_moduleBuilderMock.Call2<Action<string>, Action<JSValue>>(nameof(SimpleNativeModule2.StaticRejectSayHelloPromise),
          (string result) => Assert.AreEqual("Hello_3", result),
          (JSValue error) => Assert.AreEqual("Promise rejected", error["message"]));
      Assert.IsTrue(m_moduleBuilderMock.IsRejectCallbackCalled);
    }

    [TestMethod]
    public void TestMethodSyncCall_AddSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule2.AddSync), 3, 5, out int result);
      Assert.AreEqual(8, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_NegateSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule2.NegateSync), 3, out int result);
      Assert.AreEqual(-3, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_SayHelloSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule2.SayHelloSync), out string result);
      Assert.AreEqual("Hello", result);
    }

    [TestMethod]
    public void TestMethodSyncCall_StaticAddSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule2.StaticAddSync), 3, 5, out int result);
      Assert.AreEqual(8, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_StaticNegateSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule2.StaticNegateSync), 3, out int result);
      Assert.AreEqual(-3, result);
    }

    [TestMethod]
    public void TestMethodSyncCall_StaticSayHelloSync()
    {
      m_moduleBuilderMock.CallSync(nameof(SimpleNativeModule2.StaticSayHelloSync), out string result);
      Assert.AreEqual("Hello", result);
    }

    [TestMethod]
    public void TestConstants()
    {
      var constants = m_moduleBuilderMock.GetConstants();
      Assert.AreEqual("MyConstant1", constants[nameof(SimpleNativeModule2.Constant1)]);
      Assert.AreEqual("MyConstant2", constants["const2"]);
      Assert.AreEqual(new Point { X = 2, Y = 3 }, constants["const3"].To<Point>());
      Assert.AreEqual(new Point { X = 3, Y = 4 }, constants[nameof(SimpleNativeModule2.Constant4)].To<Point>());
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "JSIntFunction", (IReadOnlyList<JSValue> args) =>
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "pointFunc", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        functionCalled = true;
      });

      m_module.JSPointFunction(new Point { X = 4, Y = 2 });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_TwoArgFunctionField()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "lineFunc", (IReadOnlyList<JSValue> args) =>
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "NoArgFunction", (IReadOnlyList<JSValue> args) =>
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "JSValueFunction", (IReadOnlyList<JSValue> args) =>
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "JSValueFunction", (IReadOnlyList<JSValue> args) =>
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "JSIntFunctionProp", (IReadOnlyList<JSValue> args) =>
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "pointFuncProp", (IReadOnlyList<JSValue> args) =>
      {
        Assert.AreEqual(4, args[0]["X"]);
        Assert.AreEqual(2, args[0]["Y"]);
        functionCalled = true;
      });

      m_module.JSPointFunctionProp(new Point { X = 4, Y = 2 });
      Assert.IsTrue(functionCalled);
    }

    [TestMethod]
    public void TestFunction_TwoArgFunctionProperty()
    {
      bool functionCalled = false;
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "lineFuncProp", (IReadOnlyList<JSValue> args) =>
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "NoArgFunctionProp", (IReadOnlyList<JSValue> args) =>
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "JSValueFunctionProp", (IReadOnlyList<JSValue> args) =>
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
      m_moduleBuilderMock.ExpectFunction("SimpleNativeModule2", "JSValueFunctionProp", (IReadOnlyList<JSValue> args) =>
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
