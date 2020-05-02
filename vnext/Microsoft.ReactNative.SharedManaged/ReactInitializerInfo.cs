// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reflection;
using System.Threading;
using static Microsoft.ReactNative.Managed.JSValueGenerator;
using static Microsoft.ReactNative.Managed.ReactContextGenerator;
using static System.Linq.Expressions.Expression;

namespace Microsoft.ReactNative.Managed
{
  class ReactInitializerInfo
  {
    public ReactInitializerInfo(MethodInfo methodInfo)
    {
      ParameterInfo[] parameters = methodInfo.GetParameters();
      if (parameters.Length != 1 || parameters[0].ParameterType != typeof(ReactContext))
      {
        throw new ArgumentException($"Initializer method must have one parameter of IReactContext type."
          + $" Module: {methodInfo.DeclaringType.FullName} Method: {methodInfo.Name}");
      }

      InitializerImpl = new Lazy<ReactInitializerImpl>(() => MakeInitializer(methodInfo), LazyThreadSafetyMode.PublicationOnly);
    }

    private ReactInitializerImpl MakeInitializer(MethodInfo methodInfo)
    {
      // Generate code that looks like:
      //
      // (object module, IReactContext reactContext) =>
      // {
      //   (module as MyModule).initializerMethod(new ReactContext(reactContext));
      // });

      return CompileLambda<ReactInitializerImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(IReactContext), out var reactContext),
        Call(module.CastTo(methodInfo.DeclaringType), methodInfo, New(ReactContextConstructor(), reactContext)));
    }

    public delegate void ReactInitializerImpl(object module, IReactContext reactConext);

    public Lazy<ReactInitializerImpl> InitializerImpl { get; private set; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddInitializer((IReactContext reactContext) => InitializerImpl.Value(module, reactContext));
    }
  }
}
