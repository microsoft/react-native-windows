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
  class ReactMethodInfo
  {
    public ReactMethodInfo(ReflectionMethodInfo methodInfo, ReactMethodAttribute methodAttribute)
    {
      MethodName = methodAttribute.MethodName ?? methodInfo.Name;

      Type returnType = methodInfo.ReturnType;
      ParameterInfo[] parameters = methodInfo.GetParameters();
      if (returnType != typeof(void))
      {
        MethodReturnType = MethodReturnType.Callback;
        MethodImpl = new Lazy<ReactMethodImpl>(() => MakeReturningMethod(methodInfo, returnType, parameters), LazyThreadSafetyMode.PublicationOnly);
      }
      else
      {
        if (parameters.Length > 0 && typeof(Delegate).IsAssignableFrom(parameters[parameters.Length - 1].ParameterType))
        {
          if (parameters.Length > 1 && typeof(Delegate).IsAssignableFrom(parameters[parameters.Length - 2].ParameterType))
          {
            MethodReturnType = MethodReturnType.Promise;
            MethodImpl = new Lazy<ReactMethodImpl>(() => MakePromiseMethod(methodInfo, parameters), LazyThreadSafetyMode.PublicationOnly);
          }
          else
          {
            MethodReturnType = MethodReturnType.Callback;
            MethodImpl = new Lazy<ReactMethodImpl>(() => MakeCallbackMethod(methodInfo, parameters), LazyThreadSafetyMode.PublicationOnly);
          }
        }
        else
        {
          MethodReturnType = MethodReturnType.Void;
          MethodImpl = new Lazy<ReactMethodImpl>(() => MakeFireAndForgetMethod(methodInfo, parameters), LazyThreadSafetyMode.PublicationOnly);
        }
      }
    }

    private ReactMethodImpl MakeFireAndForgetMethod(ReflectionMethodInfo methodInfo, ParameterInfo[] parameters)
    {
      // Input parameters for generated lambda
      ParameterExpression moduleParameter = Expression.Parameter(typeof(object), "module");
      ParameterExpression inputReaderParameter = Expression.Parameter(typeof(IJSValueReader), "inputReader");
      ParameterExpression outputWriterParameter = Expression.Parameter(typeof(IJSValueWriter), "outputWriter");
      ParameterExpression resolveParameter = Expression.Parameter(typeof(MethodResultCallback), "resolve");
      ParameterExpression rejectParameter = Expression.Parameter(typeof(MethodResultCallback), "reject");

      // Input variables to read from inputReader
      ParameterExpression[] inputVariables = new ParameterExpression[parameters.Length];
      for (int i = 0; i < inputVariables.Length; ++i)
      {
        inputVariables[i] = Expression.Variable(parameters[i].ParameterType, parameters[i].Name);
      }

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
      statements.Add(Expression.Call(
          Expression.Convert(moduleParameter, methodInfo.DeclaringType),
          methodInfo,
          inputVariables));

      // Generate the lambda to return
      var lambda = Expression.Lambda<ReactMethodImpl>(
          Expression.Block(inputVariables, statements),
          moduleParameter, inputReaderParameter, outputWriterParameter, resolveParameter, rejectParameter);

      // Compile and return the lambda
      return lambda.Compile();
    }

    private ReactMethodImpl MakeCallbackMethod(ReflectionMethodInfo methodInfo, ParameterInfo[] parameters)
    {
      // The last variable is a delegate

      // Input parameters for generated lambda
      ParameterExpression moduleParameter = Expression.Parameter(typeof(object), "module");
      ParameterExpression inputReaderParameter = Expression.Parameter(typeof(IJSValueReader), "inputReader");
      ParameterExpression outputWriterParameter = Expression.Parameter(typeof(IJSValueWriter), "outputWriter");
      ParameterExpression resolveParameter = Expression.Parameter(typeof(MethodResultCallback), "resolve");
      ParameterExpression rejectParameter = Expression.Parameter(typeof(MethodResultCallback), "reject");

      // Input variables to read from inputReader
      ParameterExpression[] inputVariables = new ParameterExpression[parameters.Length - 1];
      for (int i = 0; i < inputVariables.Length; ++i)
      {
        inputVariables[i] = Expression.Variable(parameters[i].ParameterType, parameters[i].Name);
      }

      // Generate the resolve delegate
      var inputResolveParameter = parameters[parameters.Length - 1];
      var resolveMethodInfo = inputResolveParameter.ParameterType.GetMethod("Invoke");
      ParameterInfo[] resolveMethodParameters = resolveMethodInfo.GetParameters();
      if (resolveMethodParameters.Length != 1)
      {
        throw new ArgumentException($"Resolve callback type must have one parameter. Method: {MethodName}, Parameter: {inputResolveParameter.Name}");
      }
      Type resolveParameterType = resolveMethodParameters[0].ParameterType;
      ParameterExpression resolveLambdaParameter = Expression.Parameter(resolveParameterType, "value");

      List<Expression> resolveStatements = new List<Expression>();
      resolveStatements.Add(Expression.Call(outputWriterParameter, typeof(IJSValueWriter).GetMethod("WriteArrayBegin")));
      resolveStatements.Add(Expression.Call(null, JSValueWriter.GetWriteValueMethod(resolveParameterType), outputWriterParameter, resolveLambdaParameter));
      resolveStatements.Add(Expression.Call(outputWriterParameter, typeof(IJSValueWriter).GetMethod("WriteArrayEnd")));
      resolveStatements.Add(Expression.Invoke(resolveParameter, outputWriterParameter));
      var resolveLambda = Expression.Lambda(inputResolveParameter.ParameterType, Expression.Block(resolveStatements), resolveLambdaParameter);

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
      statements.Add(Expression.Call(
          Expression.Convert(moduleParameter, methodInfo.DeclaringType),
          methodInfo,
          (inputVariables as IEnumerable<Expression>).Append(resolveLambda)));

      // Generate the lambda to return
      var lambda = Expression.Lambda<ReactMethodImpl>(
          Expression.Block(inputVariables, statements),
          moduleParameter, inputReaderParameter, outputWriterParameter, resolveParameter, rejectParameter);

      // Compile and return the lambda
      return lambda.Compile();
    }

    private ReactMethodImpl MakePromiseMethod(ReflectionMethodInfo methodInfo, ParameterInfo[] parameters)
    {
      // The last two variables are delegates

      // Input parameters for generated lambda
      ParameterExpression moduleParameter = Expression.Parameter(typeof(object), "module");
      ParameterExpression inputReaderParameter = Expression.Parameter(typeof(IJSValueReader), "inputReader");
      ParameterExpression outputWriterParameter = Expression.Parameter(typeof(IJSValueWriter), "outputWriter");
      ParameterExpression resolveParameter = Expression.Parameter(typeof(MethodResultCallback), "resolve");
      ParameterExpression rejectParameter = Expression.Parameter(typeof(MethodResultCallback), "reject");

      // Input variables to read from inputReader
      ParameterExpression[] inputVariables = new ParameterExpression[parameters.Length - 2];
      for (int i = 0; i < inputVariables.Length; ++i)
      {
        inputVariables[i] = Expression.Variable(parameters[i].ParameterType, parameters[i].Name);
      }

      // Generate the resolve delegate
      var inputResolveParameter = parameters[parameters.Length - 2];
      var resolveMethodInfo = inputResolveParameter.ParameterType.GetMethod("Invoke");
      ParameterInfo[] resolveMethodParameters = resolveMethodInfo.GetParameters();
      if (resolveMethodParameters.Length != 1)
      {
        throw new ArgumentException($"Resolve callback type must have one parameter. Method: {MethodName}, Parameter: {inputResolveParameter.Name}");
      }
      Type resolveParameterType = resolveMethodParameters[0].ParameterType;
      ParameterExpression resolveLambdaParameter = Expression.Parameter(resolveParameterType, "value");

      List<Expression> resolveStatements = new List<Expression>();
      resolveStatements.Add(Expression.Call(outputWriterParameter, typeof(IJSValueWriter).GetMethod("WriteArrayBegin")));
      resolveStatements.Add(Expression.Call(null, JSValueWriter.GetWriteValueMethod(resolveParameterType), outputWriterParameter, resolveLambdaParameter));
      resolveStatements.Add(Expression.Call(outputWriterParameter, typeof(IJSValueWriter).GetMethod("WriteArrayEnd")));
      resolveStatements.Add(Expression.Invoke(resolveParameter, outputWriterParameter));
      var resolveLambda = Expression.Lambda(inputResolveParameter.ParameterType, Expression.Block(resolveStatements), resolveLambdaParameter);

      // Generate the reject delegate
      var inputRejectParameter = parameters[parameters.Length - 1];
      var rejectMethodInfo = inputRejectParameter.ParameterType.GetMethod("Invoke");
      ParameterInfo[] rejectMethodParameters = rejectMethodInfo.GetParameters();
      if (rejectMethodParameters.Length != 1)
      {
        throw new ArgumentException($"Reject callback type must have one parameter. Method: {MethodName}, Parameter: {inputRejectParameter.Name}");
      }
      Type rejectParameterType = rejectMethodParameters[0].ParameterType;
      ParameterExpression rejectLambdaParameter = Expression.Parameter(rejectParameterType, "error");

      List<Expression> rejectStatements = new List<Expression>();
      rejectStatements.Add(Expression.Call(outputWriterParameter, typeof(IJSValueWriter).GetMethod("WriteArrayBegin")));
      rejectStatements.Add(Expression.Call(null, JSValueWriter.GetWriteValueMethod(rejectParameterType), outputWriterParameter, rejectLambdaParameter));
      rejectStatements.Add(Expression.Call(outputWriterParameter, typeof(IJSValueWriter).GetMethod("WriteArrayEnd")));
      rejectStatements.Add(Expression.Invoke(rejectParameter, outputWriterParameter));
      var rejectLambda = Expression.Lambda(inputRejectParameter.ParameterType, Expression.Block(rejectStatements), rejectLambdaParameter);

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
      statements.Add(Expression.Call(
          Expression.Convert(moduleParameter, methodInfo.DeclaringType),
          methodInfo,
          (inputVariables as IEnumerable<Expression>).Append(resolveLambda).Append(rejectLambda)));

      // Generate the lambda to return
      var lambda = Expression.Lambda<ReactMethodImpl>(
          Expression.Block(inputVariables, statements),
          moduleParameter, inputReaderParameter, outputWriterParameter, resolveParameter, rejectParameter);

      // Compile and return the lambda
      return lambda.Compile();
    }

    private ReactMethodImpl MakeReturningMethod(ReflectionMethodInfo methodInfo, Type returnType, ParameterInfo[] parameters)
    {
      // It is like the MakeCallbackMethod but callback is not passed as a parameter.

      // Input parameters for generated lambda
      ParameterExpression moduleParameter = Expression.Parameter(typeof(object), "module");
      ParameterExpression inputReaderParameter = Expression.Parameter(typeof(IJSValueReader), "inputReader");
      ParameterExpression outputWriterParameter = Expression.Parameter(typeof(IJSValueWriter), "outputWriter");
      ParameterExpression resolveParameter = Expression.Parameter(typeof(MethodResultCallback), "resolve");
      ParameterExpression rejectParameter = Expression.Parameter(typeof(MethodResultCallback), "reject");

      // Input variables to read from inputReader
      ParameterExpression[] inputVariables = new ParameterExpression[parameters.Length];
      for (int i = 0; i < parameters.Length; ++i)
      {
        inputVariables[i] = Expression.Variable(parameters[i].ParameterType, parameters[i].Name);
      }

      // The result variable to store method call result
      var resultVariable = Expression.Variable(returnType);

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

      // Generate code to call resolve callback
      statements.Add(Expression.Invoke(resolveParameter, outputWriterParameter));

      // Generate the lambda to return
      var lambda = Expression.Lambda<ReactMethodImpl>(
          Expression.Block(inputVariables.Append(resultVariable), statements),
          moduleParameter, inputReaderParameter, outputWriterParameter, resolveParameter, rejectParameter);

      // Compile and return the lambda
      return lambda.Compile();
    }

    public delegate void ReactMethodImpl(
        object module,
        IJSValueReader inputReader,
        IJSValueWriter outputWriter,
        MethodResultCallback resolve,
        MethodResultCallback reject);

    public string MethodName { get; private set; }

    public MethodReturnType MethodReturnType { get; private set; }

    public Lazy<ReactMethodImpl> MethodImpl { get; private set; }

    public void AddToModuleBuilder(INativeModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddMethod(MethodName, MethodReturnType, (
          IJSValueReader inputReader,
          IJSValueWriter outputWriter,
          MethodResultCallback resolve,
          MethodResultCallback reject) =>
      {
        MethodImpl.Value(module, inputReader, outputWriter, resolve, reject);
      });
    }
  }
}
