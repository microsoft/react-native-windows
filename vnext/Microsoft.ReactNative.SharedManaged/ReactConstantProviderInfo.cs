// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reflection;
using System.Threading;
using static Microsoft.ReactNative.Managed.JSValueGenerator;
using static System.Linq.Expressions.Expression;

namespace Microsoft.ReactNative.Managed
{
  class ReactConstantProviderInfo
  {
    public ReactConstantProviderInfo(MethodInfo methodInfo)
    {
      ParameterInfo[] parameters = methodInfo.GetParameters();
      if (parameters.Length != 1 || parameters[0].ParameterType != typeof(ReactConstantProvider))
      {
        throw new ArgumentException($"Constant provider method must have one parameter of ReactConstantProvider type."
          + $" Module: {methodInfo.DeclaringType.FullName} Method: {methodInfo.Name}");
      }

      ConstantProviderImpl = new Lazy<ReactConstantProviderImpl>(() => MakeConstantProvider(methodInfo), LazyThreadSafetyMode.PublicationOnly);
    }

    private ReactConstantProviderImpl MakeConstantProvider(MethodInfo methodInfo)
    {
      // Generate code that looks like:
      //
      // (object module, ReactConstantProvider constantProvider) =>
      // {
      //   (module as MyModule).constantProviderMethod(constantProvider);
      // });

      bool isStatic = methodInfo.IsStatic;
      return CompileLambda<ReactConstantProviderImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(ReactConstantProvider), out var constantProvider),
        Call(isStatic ? null : module.CastTo(methodInfo.DeclaringType), methodInfo, constantProvider));
    }

    public delegate void ReactConstantProviderImpl(object module, ReactConstantProvider constantProvider);

    public Lazy<ReactConstantProviderImpl> ConstantProviderImpl { get; private set; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddConstantProvider((IJSValueWriter writer) =>
      {
        ReactConstantProvider constantProvider = new ReactConstantProvider(writer);
        ConstantProviderImpl.Value(module, constantProvider);
      });
    }
  }
}
