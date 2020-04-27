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
    private static MethodInfo GetMethod(string methodName, Type[] typeArgs, params Type[] requiredArgTypes)
    {
      var extMethod =
        from member in typeof(ReactContext).GetMember(methodName, BindingFlags.Instance | BindingFlags.Public)
        let method = member as MethodInfo
        let isGeneric = method.IsGenericMethod
        where method != null
        let parameters = method.GetParameters()
        where parameters.Length == typeArgs.Length + requiredArgTypes.Length
          && Enumerable.Range(0, requiredArgTypes.Length).All(i => parameters[i].ParameterType == requiredArgTypes[i])
        select isGeneric ? method.MakeGenericMethod(typeArgs) : method;
      return extMethod.First();
    }

    public static ConstructorInfo ReactContextConstructor() =>
      typeof(ReactContext).GetConstructor(new Type[] { typeof(IReactContext) });

    public static MethodInfo DispatchEventOf(params Type[] typeArgs)
    {
      return GetMethod(nameof(ReactContext.DispatchEvent), typeArgs, typeof(FrameworkElement), typeof(string));
    }

    public static MethodInfo CallJSFunctionOf(params Type[] typeArgs)
    {
      return GetMethod(nameof(ReactContext.CallJSFunction), typeArgs, typeof(string), typeof(string));
    }

    public static MethodInfo EmitJSEventOf(params Type[] typeArgs)
    {
      return GetMethod(nameof(ReactContext.EmitJSEvent), typeArgs, typeof(string), typeof(string));
    }
  }
}
