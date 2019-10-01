// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;
using Microsoft.ReactNative.Managed.Linq;
using System;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Reflection;
using System.Threading;
using ReflectionMethodInfo = System.Reflection.MethodInfo;

namespace Microsoft.ReactNative.Managed
{
  class ReactSyncMethodInfo
  {
    public ReactSyncMethodInfo(ReflectionMethodInfo methodInfo, ReactSyncMethodAttribute methodAttribute)
    {
      MethodName = methodAttribute.MethodName ?? methodInfo.Name;
      MethodImpl = new Lazy<ReactSyncMethodImpl>(() => MakeMethod(methodInfo), LazyThreadSafetyMode.PublicationOnly);
    }

    private ReactSyncMethodImpl MakeMethod(ReflectionMethodInfo methodInfo)
    {
      // Input parameters for generated lambda
      ParameterExpression moduleParameter = Expression.Parameter(typeof(object), "module");
      ParameterExpression inputReaderParameter = Expression.Parameter(typeof(IJSValueReader), "inputReader");
      ParameterExpression outputWriterParameter = Expression.Parameter(typeof(IJSValueWriter), "outputWriter");

      // Input variables to read from inputReader
      ParameterInfo[] parameters = methodInfo.GetParameters();
      ParameterExpression[] inputVariables = new ParameterExpression[parameters.Length];
      for (int i = 0; i < parameters.Length; ++i)
      {
        inputVariables[i] = Expression.Variable(parameters[i].ParameterType, parameters[i].Name);
      }

      // The result variable to store method call result
      var resultVariable = Expression.Variable(methodInfo.ReturnType);

      // Statements of the generated lambda
      List<Expression> statements = new List<Expression>();

      // Generate code to read input variables from the inputReader
      var callReadNext = Expression.Call(inputReaderParameter, typeof(IJSValueReader).GetMethod("ReadNext"));
      statements.Add(callReadNext);
      foreach (ParameterExpression variable in inputVariables)
      {
        statements.Add(callReadNext);
        statements.Add(Expression.Call(null, JSValueReader.GetReadValueMethod(variable.Type), inputReaderParameter, variable));
      }

      // Generate code to call the method
      statements.Add(Expression.Assign(resultVariable,
          Expression.Call(
              Expression.Convert(moduleParameter, methodInfo.DeclaringType),
              methodInfo,
              inputVariables)));

      // Generate code to write result to outputWriter
      statements.Add(Expression.Call(outputWriterParameter, typeof(IJSValueWriter).GetMethod("WriteArrayBegin")));
      statements.Add(Expression.Call(null, JSValueWriter.GetWriteValueMethod(resultVariable.Type), outputWriterParameter, resultVariable));
      statements.Add(Expression.Call(outputWriterParameter, typeof(IJSValueWriter).GetMethod("WriteArrayEnd")));

      // Generate the lambda to return
      var lambda = Expression.Lambda<ReactSyncMethodImpl>(
          Expression.Block(inputVariables.Append(resultVariable), statements),
          moduleParameter, inputReaderParameter, outputWriterParameter);

      // Compile and return the lambda
      return lambda.Compile();
    }

    public delegate void ReactSyncMethodImpl(
        object module,
        IJSValueReader inputReader,
        IJSValueWriter outputWriter);

    public string MethodName { get; private set; }

    public Lazy<ReactSyncMethodImpl> MethodImpl { get; private set; }

    public void AddToModuleBuilder(INativeModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddSyncMethod(MethodName, (IJSValueReader inputReader, IJSValueWriter outputWriter) =>
      {
        MethodImpl.Value(module, inputReader, outputWriter);
      });
    }
  }
}
