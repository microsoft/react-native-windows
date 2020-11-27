// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

#if !USE_WINUI3
using Windows.UI.Xaml;
#else
using Microsoft.UI.Xaml;
#endif

namespace Microsoft.ReactNative.Managed.UnitTests
{
  class ReactModuleBuilderMock : IReactModuleBuilder
  {
    private readonly List<InitializerDelegate> m_initializers = new List<InitializerDelegate>();
    private readonly Dictionary<string, Tuple<MethodReturnType, MethodDelegate>> m_methods =
        new Dictionary<string, Tuple<MethodReturnType, MethodDelegate>>();
    private readonly Dictionary<string, SyncMethodDelegate> m_syncMethods =
        new Dictionary<string, SyncMethodDelegate>();
    private readonly List<ConstantProviderDelegate> m_constantProviders = new List<ConstantProviderDelegate>();
    private Action<string, string, JSValue> m_jsEventHandler;
    private Action<string, string, JSValue> m_jsFunctionHandler;

    public bool IsResolveCallbackCalled { get; private set; }
    public bool IsRejectCallbackCalled { get; private set; }

    public T CreateModule<T>(ReactModuleInfo moduleInfo) where T : class
    {
      var reactContext = new ReactContextMock(this);
      var module = (T)moduleInfo.ModuleProvider(this);
      foreach (var initializer in m_initializers)
      {
        initializer(reactContext);
      }

      return module;
    }

    public void Initialize()
    {
      var reactContext = new ReactContextMock(this);
      foreach (var initializer in m_initializers)
      {
        initializer(reactContext);
      }
    }

    public void AddInitializer(InitializerDelegate initializer)
    {
      m_initializers.Add(initializer);
    }

    public void AddConstantProvider(ConstantProviderDelegate constantProvider)
    {
      m_constantProviders.Add(constantProvider);
    }

    public void AddMethod(string name, MethodReturnType returnType, MethodDelegate method)
    {
      m_methods.Add(name, Tuple.Create(returnType, method));
    }

    public void AddSyncMethod(string name, SyncMethodDelegate method)
    {
      m_syncMethods.Add(name, method);
    }

    public void Call0(string methodName) =>
        GetMethod0(methodName)?.Invoke(ArgReader(), ArgWriter(), null, null);

    public void Call0<T1>(string methodName, T1 arg1) =>
        GetMethod0(methodName)?.Invoke(ArgReader(arg1), ArgWriter(), null, null);

    public void Call0<T1, T2>(string methodName, T1 arg1, T2 arg2) =>
        GetMethod0(methodName)?.Invoke(ArgReader(arg1, arg2), ArgWriter(), null, null);

    public void Call0<T1, T2, T3>(string methodName, T1 arg1, T2 arg2, T3 arg3) =>
        GetMethod0(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), ArgWriter(), null, null);

    public Task<bool> Call1<TResolve>(string methodName, TResolve resolve) where TResolve : Delegate
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod1(methodName)?.Invoke(ArgReader(), ArgWriter(), ResolveCallback(resolve, promise), null);
      return promise.Task;
    }

    public Task<bool> Call1<T1, TResolve>(string methodName, T1 arg1, TResolve resolve) where TResolve : Delegate
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod1(methodName)?.Invoke(ArgReader(arg1), ArgWriter(), ResolveCallback(resolve, promise), null);
      return promise.Task;
    }

    public Task<bool> Call1<T1, T2, TResolve>(string methodName, T1 arg1, T2 arg2, TResolve resolve) where TResolve : Delegate
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod1(methodName)?.Invoke(ArgReader(arg1, arg2), ArgWriter(), ResolveCallback(resolve, promise), null);
      return promise.Task;
    }

    public Task<bool> Call1<T1, T2, T3, TResolve>(string methodName, T1 arg1, T2 arg2, T3 arg3, TResolve resolve) where TResolve : Delegate
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod1(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), ArgWriter(), ResolveCallback(resolve, promise), null);
      return promise.Task;
    }

    public Task<bool> Call2<TResolve, TReject>(string methodName, TResolve resolve, TReject reject)
      where TResolve : Delegate where TReject : Delegate
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod2(methodName)?.Invoke(ArgReader(), ArgWriter(),
          ResolveCallback(resolve, promise), RejectCallback(reject, promise));
      return promise.Task;
    }

    public Task<bool> Call2<T1, TResolve, TReject>(string methodName, T1 arg1,
        TResolve resolve, TReject reject) where TResolve : Delegate where TReject : Delegate
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod2(methodName)?.Invoke(ArgReader(arg1), ArgWriter(),
          ResolveCallback(resolve, promise), RejectCallback(reject, promise));
      return promise.Task;
    }

    public Task<bool> Call2<T1, T2, TResolve, TReject>(string methodName, T1 arg1, T2 arg2,
        TResolve resolve, TReject reject) where TResolve : Delegate where TReject : Delegate
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod2(methodName)?.Invoke(ArgReader(arg1, arg2), ArgWriter(),
          ResolveCallback(resolve, promise), RejectCallback(reject, promise));
      return promise.Task;
    }

    public Task<bool> Call2<T1, T2, T3, TResolve, TReject>(string methodName, T1 arg1, T2 arg2, T3 arg3,
        TResolve resolve, TReject reject) where TResolve : Delegate where TReject : Delegate
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod2(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), ArgWriter(),
          ResolveCallback(resolve, promise), RejectCallback(reject, promise));
      return promise.Task;
    }

    public void CallSync<TResult>(string methodName, out TResult result)
    {
      var writer = ArgWriter();
      GetSyncMethod(methodName)?.Invoke(ArgReader(), writer);
      result = GetResult<TResult>(writer);
    }

    public void CallSync<T1, TResult>(string methodName, T1 arg1, out TResult result)
    {
      var writer = ArgWriter();
      GetSyncMethod(methodName)?.Invoke(ArgReader(arg1), writer);
      result = GetResult<TResult>(writer);
    }

    public void CallSync<T1, T2, TResult>(string methodName, T1 arg1, T2 arg2, out TResult result)
    {
      var writer = ArgWriter();
      GetSyncMethod(methodName)?.Invoke(ArgReader(arg1, arg2), writer);
      result = GetResult<TResult>(writer);
    }

    public void CallSync<T1, T2, T3, TResult>(string methodName, T1 arg1, T2 arg2, T3 arg3, out TResult result)
    {
      var writer = ArgWriter();
      GetSyncMethod(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), writer);
      result = GetResult<TResult>(writer);
    }

    private MethodDelegate GetMethod0(string methodName) =>
        (m_methods.TryGetValue(methodName, out var tuple) && tuple.Item1 == MethodReturnType.Void) ? tuple.Item2 : null;

    private MethodDelegate GetMethod1(string methodName) =>
        (m_methods.TryGetValue(methodName, out var tuple) && tuple.Item1 == MethodReturnType.Callback) ? tuple.Item2 : null;

    private MethodDelegate GetMethod2(string methodName) =>
        (m_methods.TryGetValue(methodName, out var tuple)
        && (tuple.Item1 == MethodReturnType.TwoCallbacks) || tuple.Item1 == MethodReturnType.Promise) ? tuple.Item2 : null;

    private SyncMethodDelegate GetSyncMethod(string methodName) =>
        m_syncMethods.TryGetValue(methodName, out var syncMethod) ? syncMethod : null;

    private MethodResultCallback ResolveCallback<T>(T resolve, TaskCompletionSource<bool> promise = null) where T : Delegate
    {
      return (IJSValueWriter writer) =>
      {
        CallCallback(resolve, writer);
        IsResolveCallbackCalled = true;
        promise?.SetResult(true);
      };
    }

    private MethodResultCallback RejectCallback<T>(T reject, TaskCompletionSource<bool> promise = null) where T : Delegate
    {
      return (IJSValueWriter writer) =>
      {
        CallCallback(reject, writer);
        IsRejectCallbackCalled = true;
        promise?.SetResult(false);
      };
    }

    private static void CallCallback<T>(T callback, IJSValueWriter writer) where T : Delegate
    {
      var resulReader = new JSValueTreeReader((writer as JSValueTreeWriter).TakeValue());
      var delegateInvoke = typeof(T).GetMethod("Invoke");
      var parameters = delegateInvoke.GetParameters();
      var paramTypes = parameters.Select(p => p.ParameterType).ToArray();
      if (paramTypes.Length == 0)
      {
        delegateInvoke.Invoke(callback, new object[] { });
      }
      else
      {
        object[] args = new object[paramTypes.Length + 1];
        args[0] = resulReader;
        var readArgsMethod = JSValueReaderGenerator.ReadArgsOf(paramTypes);
        readArgsMethod.Invoke(null, args);
        delegateInvoke.Invoke(callback, args.Skip(1).ToArray());
      }
    }

    private static T GetResult<T>(IJSValueWriter writer)
    {
      var resulReader = new JSValueTreeReader((writer as JSValueTreeWriter).TakeValue());
      resulReader.ReadValue(out T result);
      return result;
    }

    private static IJSValueWriter ArgWriter() => new JSValueTreeWriter();

    private static IJSValueReader ArgReader() => CreateArgReader(w => w.WriteArgs());

    private static IJSValueReader ArgReader<T1>(T1 arg1) => CreateArgReader(w => w.WriteArgs(arg1));

    private static IJSValueReader ArgReader<T1, T2>(T1 arg1, T2 arg2) =>
        CreateArgReader(w => w.WriteArgs(arg1, arg2));

    private static IJSValueReader ArgReader<T1, T2, T3>(T1 arg1, T2 arg2, T3 arg3) =>
        CreateArgReader(w => w.WriteArgs(arg1, arg2, arg3));

    private static IJSValueReader CreateArgReader(Func<IJSValueWriter, IJSValueWriter> argWriter) =>
        new JSValueTreeReader((argWriter(new JSValueTreeWriter()) as JSValueTreeWriter).TakeValue());

    public IReadOnlyDictionary<string, JSValue> GetConstants()
    {
      var constantWriter = new JSValueTreeWriter();
      constantWriter.WriteObjectBegin();
      foreach (var constantProvider in m_constantProviders)
      {
        constantProvider(constantWriter);
      }

      constantWriter.WriteObjectEnd();
      return constantWriter.TakeValue().AsObject();
    }

    public void ExpectEvent(string eventEmitterName, string eventName, Action<IReadOnlyList<JSValue>> checkValues)
    {
      m_jsEventHandler = (string actualEventEmitterName, string actualEventName, JSValue value) =>
      {
        Assert.AreEqual(eventEmitterName, actualEventEmitterName);
        Assert.AreEqual(eventName, actualEventName);
        Assert.AreEqual(JSValueType.Array, value.Type);
        checkValues(value.AsArray());
      };
    }

    public void ExpectFunction(string moduleName, string functionName, Action<IReadOnlyList<JSValue>> checkValues)
    {
      m_jsFunctionHandler = (string actualModuleName, string actualFunctionName, JSValue value) =>
      {
        Assert.AreEqual(moduleName, actualModuleName);
        Assert.AreEqual(functionName, actualFunctionName);
        Assert.AreEqual(JSValueType.Array, value.Type);
        checkValues(value.AsArray());
      };
    }

    public void CallJSFunction(string moduleName, string functionName, JSValueArgWriter paramsArgWriter)
    {
      var writer = new JSValueTreeWriter();
      paramsArgWriter(writer);
      m_jsFunctionHandler(moduleName, functionName, writer.TakeValue());
    }

    public void EmitJSEvent(string eventEmitterName, string eventName, JSValueArgWriter paramsArgWriter)
    {
      var writer = new JSValueTreeWriter();
      writer.WriteArrayBegin();
      paramsArgWriter(writer);
      writer.WriteArrayEnd();
      m_jsEventHandler(eventEmitterName, eventName, writer.TakeValue());
    }
  }

  class ReactSettingsSnapshot : IReactSettingsSnapshot
  {
    public bool DebuggerBreakOnNextLine => throw new NotImplementedException();

    public ushort DebuggerPort => throw new NotImplementedException();

    public bool UseDirectDebugger => throw new NotImplementedException();

    public bool UseFastRefresh => throw new NotImplementedException();

    public bool UseWebDebugger => throw new NotImplementedException();

    public string BundleRootPath => throw new NotImplementedException();

    public string DebugBundlePath => throw new NotImplementedException();

    public string JavaScriptBundleFile => throw new NotImplementedException();

    public string SourceBundleHost => throw new NotImplementedException();

    public ushort SourceBundlePort => throw new NotImplementedException();
  }

  class ReactContextMock : IReactContext
  {
    private readonly ReactModuleBuilderMock m_builder;

    public ReactContextMock(ReactModuleBuilderMock builder)
    {
      m_builder = builder;
    }

    public IReactSettingsSnapshot SettingsSnapshot => throw new NotImplementedException();

    public IReactPropertyBag Properties { get; } = ReactPropertyBagHelper.CreatePropertyBag();

    public IReactNotificationService Notifications { get; } = ReactNotificationServiceHelper.CreateNotificationService();

    public IReactDispatcher UIDispatcher => Properties.Get(ReactDispatcherHelper.UIDispatcherProperty) as IReactDispatcher;

    public IReactDispatcher JSDispatcher => Properties.Get(ReactDispatcherHelper.JSDispatcherProperty) as IReactDispatcher;

    public Object JSRuntime => throw new NotImplementedException();

    public void DispatchEvent(FrameworkElement view, string eventName, JSValueArgWriter eventDataArgWriter)
    {
      throw new NotImplementedException();
    }

    public void CallJSFunction(string moduleName, string functionName, JSValueArgWriter paramsArgWriter)
    {
      m_builder.CallJSFunction(moduleName, functionName, paramsArgWriter);
    }

    public void EmitJSEvent(string eventEmitterName, string eventName, JSValueArgWriter paramsArgWriter)
    {
      m_builder.EmitJSEvent(eventEmitterName, eventName, paramsArgWriter);
    }
  }
}
