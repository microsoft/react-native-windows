// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reflection;
using System.Threading;
using static Microsoft.ReactNative.Managed.JSValueGenerator;
using static Microsoft.ReactNative.Managed.JSValueWriterGenerator;
using static System.Linq.Expressions.Expression;

namespace Microsoft.ReactNative.Managed
{
  class ReactConstantInfo
  {
    public ReactConstantInfo(PropertyInfo propertyInfo, ReactConstantAttribute constantAttribute)
    {
      ConstantName = constantAttribute.ConstantName ?? propertyInfo.Name;
      ConstantImpl = new Lazy<ReactConstantImpl>(() => MakeConstant(propertyInfo), LazyThreadSafetyMode.PublicationOnly);
    }

    public ReactConstantInfo(FieldInfo fieldInfo, ReactConstantAttribute constantAttribute)
    {
      ConstantName = constantAttribute.ConstantName ?? fieldInfo.Name;
      ConstantImpl = new Lazy<ReactConstantImpl>(() => MakeConstant(fieldInfo), LazyThreadSafetyMode.PublicationOnly);
    }

    private ReactConstantImpl MakeConstant(PropertyInfo propertyInfo)
    {
      // Generate code that looks like:
      //
      // (object module, IJSValueWriter outputWriter) =>
      // {
      //   outputWriter.WriteValue((module as MyModule).constantProperty);
      // };

      bool isStatic = propertyInfo.GetGetMethod().IsStatic;
      return CompileLambda<ReactConstantImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(IJSValueWriter), out var outputWriter),
          outputWriter.CallExt(WriteValueOf(propertyInfo.PropertyType),
            Property(isStatic ? null : module.CastTo(propertyInfo.DeclaringType), propertyInfo)));
    }

    private ReactConstantImpl MakeConstant(FieldInfo fieldInfo)
    {
      // Generate code that looks like:
      //
      // (object module, IJSValueWriter outputWriter) =>
      // {
      //   outputWriter.WriteValue((module as MyModule).constantField);
      // });

      bool isStatic = fieldInfo.IsStatic;
      return CompileLambda<ReactConstantImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(IJSValueWriter), out var outputWriter),
          outputWriter.CallExt(WriteValueOf(fieldInfo.FieldType),
            Field(isStatic ? null : module.CastTo(fieldInfo.DeclaringType), fieldInfo)));
    }

    public delegate void ReactConstantImpl(object module, IJSValueWriter writer);

    public string ConstantName { get; private set; }

    public Lazy<ReactConstantImpl> ConstantImpl { get; private set; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddConstantProvider((IJSValueWriter writer) =>
      {
        writer.WritePropertyName(ConstantName);
        ConstantImpl.Value(module, writer);
      });
    }
  }
}
