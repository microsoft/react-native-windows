// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;

namespace Microsoft.ReactNative.Managed
{
  static class ReactContextExtensions
  {
    internal static void DispatchEvent<T>(this IReactContext reactContext, FrameworkElement view, string eventName, T eventData)
    {
      reactContext.DispatchEvent(view, eventName, (IJSValueWriter writer) => writer.WriteValue(eventData));
    }

    internal static void CallJsFunction<T>(this IReactContext reactContext, string moduleName, string method, T @params)
    {
      reactContext.CallJsFunction(moduleName, method, (IJSValueWriter writer) => writer.WriteValue(@params));
    }
  }
}
