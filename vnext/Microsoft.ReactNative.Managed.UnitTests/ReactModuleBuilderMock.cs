// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Microsoft.ReactNative.Managed.UnitTests
{
  class ReactModuleBuilderMock : IReactModuleBuilder
  {
    private string m_eventEmitterName;
    private Dictionary<string, Tuple<MethodReturnType, MethodDelegate>> m_methods =
        new Dictionary<string, Tuple<MethodReturnType, MethodDelegate>>();
    private Dictionary<string, SyncMethodDelegate> m_syncMethods =
        new Dictionary<string, SyncMethodDelegate>();
    private List<ConstantProvider> m_constProviders = new List<ConstantProvider>();
    private Dictionary<string, Action<IJSValueReader>> m_eventHandlers =
        new Dictionary<string, Action<IJSValueReader>>();

    public bool IsResolveCallbackCalled { get; private set; }
    public bool IsRejectCallbackCalled { get; private set; }

    public void SetEventEmitterName(string name)
    {
      m_eventEmitterName = name;
    }

    public void AddMethod(string name, MethodReturnType returnType, MethodDelegate method)
    {
      m_methods.Add(name, Tuple.Create(returnType, method));
    }

    public void AddSyncMethod(string name, SyncMethodDelegate method)
    {
      m_syncMethods.Add(name, method);
    }

    public void AddConstantProvider(ConstantProvider constantProvider)
    {
      m_constProviders.Add(constantProvider);
    }

    public void AddEventHandlerSetter(string name, ReactEventHandlerSetter eventHandlerSetter)
    {
      eventHandlerSetter((ReactArgWriter argWriter) =>
      {
        if (m_eventHandlers.TryGetValue(name, out var eventHandler))
        {
          var writer = new JSValueTreeWriter();
          writer.WriteArrayBegin();
          argWriter(writer);
          writer.WriteArrayEnd();

          eventHandler(new JSValueTreeReader(writer.TakeValue()));
        }
      });
    }

    public void Call0(string methodName) =>
        GetMethod0(methodName)?.Invoke(ArgReader(), ArgWriter(), null, null);

    public void Call0<T1>(string methodName, T1 arg1) =>
        GetMethod0(methodName)?.Invoke(ArgReader(arg1), ArgWriter(), null, null);

    public void Call0<T1, T2>(string methodName, T1 arg1, T2 arg2) =>
        GetMethod0(methodName)?.Invoke(ArgReader(arg1, arg2), ArgWriter(), null, null);

    public void Call0<T1, T2, T3>(string methodName, T1 arg1, T2 arg2, T3 arg3) =>
        GetMethod0(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), ArgWriter(), null, null);

    public Task<bool> Call1<TResult>(string methodName, Action<TResult> resolve)
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod1(methodName)?.Invoke(ArgReader(), ArgWriter(), ResolveCallback(resolve, promise), null);
      return promise.Task;
    }

    public Task<bool> Call1<T1, TResult>(string methodName, T1 arg1, Action<TResult> resolve)
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod1(methodName)?.Invoke(ArgReader(arg1), ArgWriter(), ResolveCallback(resolve, promise), null);
      return promise.Task;
    }

    public Task<bool> Call1<T1, T2, TResult>(string methodName, T1 arg1, T2 arg2, Action<TResult> resolve)
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod1(methodName)?.Invoke(ArgReader(arg1, arg2), ArgWriter(), ResolveCallback(resolve, promise), null);
      return promise.Task;
    }

    public Task<bool> Call1<T1, T2, T3, TResult>(string methodName, T1 arg1, T2 arg2, T3 arg3, Action<TResult> resolve)
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod1(methodName)?.Invoke(ArgReader(arg1, arg2, arg3), ArgWriter(), ResolveCallback(resolve, promise), null);
      return promise.Task;
    }

    public Task<bool> Call2<TResult, TError>(string methodName, Action<TResult> resolve, Action<TError> reject)
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod2(methodName)?.Invoke(ArgReader(), ArgWriter(),
          ResolveCallback(resolve, promise), RejectCallback(reject, promise));
      return promise.Task;
    }

    public Task<bool> Call2<T1, TResult, TError>(string methodName, T1 arg1,
        Action<TResult> resolve, Action<TError> reject)
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod2(methodName)?.Invoke(ArgReader(arg1), ArgWriter(),
          ResolveCallback(resolve, promise), RejectCallback(reject, promise));
      return promise.Task;
    }

    public Task<bool> Call2<T1, T2, TResult, TError>(string methodName, T1 arg1, T2 arg2,
        Action<TResult> resolve, Action<TError> reject)
    {
      var promise = new TaskCompletionSource<bool>();
      GetMethod2(methodName)?.Invoke(ArgReader(arg1, arg2), ArgWriter(),
          ResolveCallback(resolve, promise), RejectCallback(reject, promise));
      return promise.Task;
    }

    public Task<bool> Call2<T1, T2, T3, TResult, TError>(string methodName, T1 arg1, T2 arg2, T3 arg3,
        Action<TResult> resolve, Action<TError> reject)
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

    private MethodResultCallback ResolveCallback<T>(Action<T> resolve, TaskCompletionSource<bool> promise = null)
    {
      return (IJSValueWriter writer) =>
      {
        resolve(GetResult<T>(writer));
        IsResolveCallbackCalled = true;
        promise?.SetResult(true);
      };
    }

    private MethodResultCallback RejectCallback<T>(Action<T> reject, TaskCompletionSource<bool> promise = null)
    {
      return (IJSValueWriter writer) =>
      {
        reject(GetResult<T>(writer));
        IsRejectCallbackCalled = true;
        promise?.SetResult(false);
      };
    }

    private static T GetResult<T>(IJSValueWriter writer)
    {
      var resulReader = new JSValueTreeReader((writer as JSValueTreeWriter).TakeValue());
      resulReader.ReadArgs(out T result);
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
      foreach (var constantProvider in m_constProviders)
      {
        constantProvider(constantWriter);
      }

      constantWriter.WriteObjectEnd();
      return constantWriter.TakeValue().Object;
    }

    public void SetEventHandler<T>(string eventName, Action<T> eventHandler)
    {
      m_eventHandlers.Add(eventName, (IJSValueReader reader) =>
      {
        reader.ReadArgs(out T arg);
        eventHandler(arg);
      });
    }
  }
}
