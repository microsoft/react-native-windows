// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using Windows.UI.Xaml;

namespace Microsoft.ReactNative.Managed
{
  static class ReactContextGenerator
  {
    private static MethodInfo GetExtensionMethod(string methodName, Type[] typeArgs, params Type[] requiredArgTypes)
    {
      var extMethod =
        from member in typeof(ReactContextExtensions).GetMember(methodName, BindingFlags.Static | BindingFlags.Public)
        let method = member as MethodInfo
        let isGeneric = method.IsGenericMethod
        where method != null
          && method.IsDefined(typeof(ExtensionAttribute), inherit: false)
        let parameters = method.GetParameters()
        where parameters.Length == typeArgs.Length + requiredArgTypes.Length
          && Enumerable.Range(0, requiredArgTypes.Length).All(i => parameters[i].ParameterType == requiredArgTypes[i])
        select isGeneric ? method.MakeGenericMethod(typeArgs) : method;
      return extMethod.First();
    }

    public static MethodInfo DispatchEventOf(params Type[] typeArgs)
    {
      return GetExtensionMethod(nameof(ReactContextExtensions.DispatchEvent), typeArgs,
        typeof(IReactContext), typeof(FrameworkElement), typeof(string));
    }

    public static MethodInfo CallJSFunctionOf(params Type[] typeArgs)
    {
      return GetExtensionMethod(nameof(ReactContextExtensions.CallJSFunction), typeArgs,
        typeof(IReactContext), typeof(string), typeof(string));
    }

    public static MethodInfo EmitJSEventOf(params Type[] typeArgs)
    {
      return GetExtensionMethod(nameof(ReactContextExtensions.EmitJSEvent), typeArgs,
        typeof(IReactContext), typeof(string), typeof(string));
    }
  }
}
