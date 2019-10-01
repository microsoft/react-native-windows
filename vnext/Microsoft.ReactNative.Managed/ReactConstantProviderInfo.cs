// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;
using System;
using System.Linq.Expressions;
using System.Reflection;
using System.Threading;
using ReflectionMethodInfo = System.Reflection.MethodInfo;

namespace Microsoft.ReactNative.Managed
{
  class ReactConstantProviderInfo
  {
    public ReactConstantProviderInfo(ReflectionMethodInfo methodInfo)
    {
      ConstantProviderImpl = new Lazy<ReactConstantProviderImpl>(() => MakeConstantProvider(methodInfo), LazyThreadSafetyMode.PublicationOnly);
    }

    private ReactConstantProviderImpl MakeConstantProvider(ReflectionMethodInfo methodInfo)
    {
      ParameterExpression moduleParameter = Expression.Parameter(typeof(object), "module");
      ParameterExpression constantProviderParameter = Expression.Parameter(typeof(ReactConstantProvider), "constantProvider");

      ParameterInfo[] parameters = methodInfo.GetParameters();
      if (parameters.Length != 1 || parameters[0].ParameterType != typeof(ReactConstantProvider))
      {
        throw new ArgumentException($"Constant provider method must have one parameter of ReactConstantProvider type."
            + $" Module: {methodInfo.DeclaringType.FullName} {methodInfo.Name}");
      }

      bool isStatic = methodInfo.IsStatic;

      var constantProviderCall = Expression.Call(isStatic ? null : Expression.Convert(moduleParameter, methodInfo.DeclaringType), methodInfo, constantProviderParameter);
      var lambda = Expression.Lambda<ReactConstantProviderImpl>(constantProviderCall, moduleParameter, constantProviderParameter);

      return lambda.Compile();
    }

    public delegate void ReactConstantProviderImpl(object module, ReactConstantProvider constantProvider);

    public Lazy<ReactConstantProviderImpl> ConstantProviderImpl { get; private set; }

    public void AddToModuleBuilder(INativeModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddConstantWriter((IJSValueWriter writer) =>
      {
        ReactConstantProvider constantProvider = new ReactConstantProvider(writer);
        ConstantProviderImpl.Value(module, constantProvider);
      });
    }
  }
}
