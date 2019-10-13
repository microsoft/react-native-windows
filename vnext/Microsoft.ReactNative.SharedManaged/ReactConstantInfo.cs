// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;
using System;
using System.Linq.Expressions;
using System.Reflection;
using System.Threading;

namespace Microsoft.ReactNative.Managed
{
  class ReactConstantInfo
  {
    public ReactConstantInfo(PropertyInfo propertyInfo, ReactConstantAttribute constantAttribute)
    {
      ConstantName = constantAttribute.ConstantName ?? propertyInfo.Name;
      ConstantImpl = new Lazy<ReactConstantImpl>(() => MakePropertyConstant(propertyInfo), LazyThreadSafetyMode.PublicationOnly);
    }

    public ReactConstantInfo(FieldInfo fieldInfo, ReactConstantAttribute constantAttribute)
    {
      ConstantName = constantAttribute.ConstantName ?? fieldInfo.Name;
      ConstantImpl = new Lazy<ReactConstantImpl>(() => MakeFieldConstant(fieldInfo), LazyThreadSafetyMode.PublicationOnly);
    }

    private ReactConstantImpl MakePropertyConstant(PropertyInfo propertyInfo)
    {
      ParameterExpression moduleParameter = Expression.Parameter(typeof(object), "module");
      ParameterExpression writerParameter = Expression.Parameter(typeof(IJSValueWriter), "writer");

      bool isStatic = propertyInfo.GetGetMethod().IsStatic;
      Type propertyType = propertyInfo.PropertyType;
      var constantProperty = Expression.Property(isStatic ? null : Expression.Convert(moduleParameter, propertyInfo.DeclaringType), propertyInfo);
      var writeValueCall = Expression.Call(null, JSValueWriter.GetWriteValueMethod(propertyType), writerParameter, constantProperty);
      var lambda = Expression.Lambda<ReactConstantImpl>(writeValueCall, moduleParameter, writerParameter);

      return lambda.Compile();
    }

    private ReactConstantImpl MakeFieldConstant(FieldInfo fieldInfo)
    {
      ParameterExpression moduleParameter = Expression.Parameter(typeof(object), "module");
      ParameterExpression writerParameter = Expression.Parameter(typeof(IJSValueWriter), "writer");

      bool isStatic = fieldInfo.IsStatic;
      Type fieldType = fieldInfo.FieldType;
      var constantField = Expression.Field(isStatic ? null : Expression.Convert(moduleParameter, fieldInfo.DeclaringType), fieldInfo);
      var writeValueCall = Expression.Call(null, JSValueWriter.GetWriteValueMethod(fieldType), writerParameter, constantField);
      var lambda = Expression.Lambda<ReactConstantImpl>(writeValueCall, moduleParameter, writerParameter);

      return lambda.Compile();
    }

    public delegate void ReactConstantImpl(object module, IJSValueWriter writer);

    public string ConstantName { get; private set; }

    public Lazy<ReactConstantImpl> ConstantImpl { get; private set; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddConstantWriter((IJSValueWriter writer) =>
      {
        writer.WritePropertyName(ConstantName);
        ConstantImpl.Value(module, writer);
      });
    }
  }
}
