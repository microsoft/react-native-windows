// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;

namespace Microsoft.ReactNative.Managed
{
  static class ReactContextExtensions
  {
    public static void DispatchEvent<T>(this IReactContext reactContext, FrameworkElement view, string eventName, T arg)
    {
      reactContext.DispatchEvent(view, eventName, (IJSValueWriter writer) => writer.WriteValue(arg));
    }

    public static void CallJSFunction(this IReactContext reactContext, string moduleName, string methodName)
    {
      reactContext.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs());
    }

    public static void CallJSFunction<T1>(this IReactContext reactContext, string moduleName, string methodName, T1 arg1)
    {
      reactContext.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1));
    }

    public static void CallJSFunction<T1, T2>(this IReactContext reactContext, string moduleName, string methodName, T1 arg1, T2 arg2)
    {
      reactContext.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2));
    }

    public static void CallJSFunction<T1, T2, T3>(this IReactContext reactContext, string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3)
    {
      reactContext.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3));
    }

    public static void CallJSFunction<T1, T2, T3, T4>(this IReactContext reactContext,
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
    {
      reactContext.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4));
    }

    public static void CallJSFunction<T1, T2, T3, T4, T5>(this IReactContext reactContext,
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
    {
      reactContext.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4, arg5));
    }

    public static void CallJSFunction<T1, T2, T3, T4, T5, T6>(this IReactContext reactContext,
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
    {
      reactContext.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4, arg5, arg6));
    }

    public static void CallJSFunction<T1, T2, T3, T4, T5, T6, T7>(this IReactContext reactContext,
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
    {
      reactContext.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4, arg5, arg6, arg7));
    }

    public static void EmitJSEvent<T>(this IReactContext reactContext, string eventEmitterName, string eventName, T arg)
    {
      reactContext.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) => writer.WriteValue(arg));
    }
  }
}
