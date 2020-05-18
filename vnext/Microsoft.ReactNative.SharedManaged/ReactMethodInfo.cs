// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Reflection;
using System.Threading;
using static Microsoft.ReactNative.Managed.JSValueGenerator;
using static Microsoft.ReactNative.Managed.JSValueReaderGenerator;
using static Microsoft.ReactNative.Managed.JSValueWriterGenerator;
using static System.Linq.Expressions.Expression;

namespace Microsoft.ReactNative.Managed
{
  class ReactMethodInfo
  {
    public ReactMethodInfo(MethodInfo methodInfo, ReactMethodAttribute methodAttribute)
    {
      MethodName = methodAttribute.MethodName ?? methodInfo.Name;

      Type returnType = methodInfo.ReturnType;
      ParameterInfo[] parameters = methodInfo.GetParameters();
      Type lastParameterType = parameters.Length > 0 ? parameters[parameters.Length - 1].ParameterType : null;
      Type secondToLastParameterType = parameters.Length > 1 ? parameters[parameters.Length - 2].ParameterType : null;
      Func<ReactMethodImpl> createMethod;

      bool isPromise(Type type)
      {
        var typeInfo = type?.GetTypeInfo();
        if (typeInfo != null && typeInfo.IsGenericType)
        {
          var genericArgs = type.GetGenericArguments();
          if (genericArgs.Length == 1)
          {
            return typeof(IReactPromise<>).MakeGenericType(genericArgs).IsAssignableFrom(type);
          }
        }
        return false;
      }

      bool isCallback(Type type) => type != null && typeof(Delegate).IsAssignableFrom(type);

      if (returnType != typeof(void))
      {
        MethodReturnType = MethodReturnType.Callback;
        createMethod = () => MakeReturningMethod(methodInfo, returnType, parameters);
      }
      else if (isPromise(lastParameterType))
      {
        MethodReturnType = MethodReturnType.Promise;
        createMethod = () => MakePromiseMethod(methodInfo, parameters);
      }
      else if (isCallback(lastParameterType))
      {
        if (isCallback(secondToLastParameterType))
        {
          MethodReturnType = MethodReturnType.TwoCallbacks;
          createMethod = () => MakeTwoCallbacksMethod(methodInfo, parameters);
        }
        else
        {
          MethodReturnType = MethodReturnType.Callback;
          createMethod = () => MakeCallbackMethod(methodInfo, parameters);
        }
      }
      else
      {
        MethodReturnType = MethodReturnType.Void;
        createMethod = () => MakeFireAndForgetMethod(methodInfo, parameters);
      }

      MethodImpl = new Lazy<ReactMethodImpl>(createMethod, LazyThreadSafetyMode.PublicationOnly);
    }

    static VariableWrapper[] MethodParameters(
      out VariableWrapper module,
      out VariableWrapper inputReader,
      out VariableWrapper outputWriter,
      out VariableWrapper resolve,
      out VariableWrapper reject)
    {
      var result = new VariableWrapper[5];
      result[0] = Parameter(typeof(object), out module);
      result[1] = Parameter(typeof(IJSValueReader), out inputReader);
      result[2] = Parameter(typeof(IJSValueWriter), out outputWriter);
      result[3] = Parameter(typeof(MethodResultCallback), out resolve);
      result[4] = Parameter(typeof(MethodResultCallback), out reject);
      return result;
    }

    static ConstructorInfo ReactPromiseCtorOf(Type type) =>
      typeof(ReactPromise<>).MakeGenericType(type).GetConstructor(new Type[] {
        typeof(IJSValueWriter), typeof(MethodResultCallback), typeof(MethodResultCallback) });

    private ReactMethodImpl MakeFireAndForgetMethod(MethodInfo methodInfo, ParameterInfo[] parameters)
    {
      // Generate code that looks like:
      //
      // (object module, IJSValueReader inputReader, IJSValueWriter outputWriter,
      //    MethodResultCallback resolve, MethodResultCallback reject) =>
      // {
      //   inputReader.ReadArgs(out ArgType0 arg0, out ArgType1 arg1);
      //   (module as ModuleType).Method(arg0, arg1);
      // }

      return CompileLambda<ReactMethodImpl>(
        MethodParameters(out var module, out var inputReader, out _, out _, out _),
        MethodArgs(parameters, out var argTypes, out var args),
        inputReader.CallExt(ReadArgsOf(argTypes), args),
        module.CastTo(methodInfo.DeclaringType).Call(methodInfo, args));
    }

    private ReactMethodImpl MakeCallbackMethod(MethodInfo methodInfo, ParameterInfo[] parameters)
    {
      // Generate code that looks like:
      //
      // (object module, IJSValueReader inputReader, IJSValueWriter outputWriter,
      //    MethodResultCallback resolve, MethodResultCallback reject) =>
      // {
      //   inputReader.ReadArgs(out ArgType0 arg0, out ArgType1 arg1);
      //   (module as ModuleType).Method(arg0, arg1,
      //     (result1, result2) => resolve(outputWriter.WriteArgs(result1, result2)));
      // }

      // The last parameter in parameters is a 'resolve' delegate
      return CompileLambda<ReactMethodImpl>(
        MethodParameters(out var module, out var inputReader, out var outputWriter, out var resolve, out _),
        MethodArgs(parameters, out var argTypes, out var args,
          out var resolveCallbackType, out var resolveArgTypes),
        inputReader.CallExt(ReadArgsOf(argTypes), args),
        module.CastTo(methodInfo.DeclaringType).Call(methodInfo, args,
          AutoLambda(resolveCallbackType, Parameters(resolveArgTypes, out var results),
            resolve.Invoke(outputWriter.CallExt(WriteArgsOf(resolveArgTypes), results)))));
    }

    private ReactMethodImpl MakeTwoCallbacksMethod(MethodInfo methodInfo, ParameterInfo[] parameters)
    {
      // Some native modules use first callback as failure, others use second. We make them both to
      // behave the same way and let developers to assign meaning to the first and second callbacks.
      //
      // Generate code that looks like:
      //
      // (object module, IJSValueReader inputReader, IJSValueWriter outputWriter,
      //    MethodResultCallback resolve, MethodResultCallback reject) =>
      // {
      //   inputReader.ReadArgs(out ArgType0 arg0, out ArgType1 arg1);
      //   (module as ModuleType).Method(arg0, arg1,
      //     (result1, result2) => resolve(outputWriter.WriteArgs(result1, result2)),
      //     (error1, error2) => reject(outputWriter.WriteArgs(error1, error2)));
      // }

      // The last two parameters in parameters are resolve and reject delegates
      return CompileLambda<ReactMethodImpl>(
        MethodParameters(out var module, out var inputReader, out var outputWriter, out var resolve, out var reject),
        MethodArgs(parameters, out var argTypes, out var args,
          out var resolveCallbackType, out var resolveArgTypes,
          out var rejectCallbackType, out var rejectArgTypes),
        inputReader.CallExt(ReadArgsOf(argTypes), args),
        module.CastTo(methodInfo.DeclaringType).Call(methodInfo, args,
          AutoLambda(resolveCallbackType, Parameters(resolveArgTypes, out var results),
            resolve.Invoke(outputWriter.CallExt(WriteArgsOf(resolveArgTypes), results))),
          AutoLambda(rejectCallbackType, Parameters(rejectArgTypes, out var errors),
            reject.Invoke(outputWriter.CallExt(WriteArgsOf(rejectArgTypes), errors)))));
    }

    private ReactMethodImpl MakePromiseMethod(MethodInfo methodInfo, ParameterInfo[] parameters)
    {
      // Generate code that looks like:
      //
      // (object module, IJSValueReader inputReader, IJSValueWriter outputWriter,
      //    MethodResultCallback resolve, MethodResultCallback reject) =>
      // {
      //   inputReader.ReadArgs(out ArgType0 arg0, out ArgType1 arg1);
      //   (module as ModuleType).Method(arg0, arg1,
      //     new ReactPromise<TValue>(outputWriter, resolve, reject));
      // }

      // The last two parameters in parameters are resolve and reject delegates
      return CompileLambda<ReactMethodImpl>(
        MethodParameters(out var module, out var inputReader, out var outputWriter, out var resolve, out var reject),
        MethodArgs(parameters, out var argTypes, out var args, out var promiseResultType),
        inputReader.CallExt(ReadArgsOf(argTypes), args),
        module.CastTo(methodInfo.DeclaringType).Call(methodInfo, args,
          New(ReactPromiseCtorOf(promiseResultType), outputWriter, resolve, reject)));
    }

    private ReactMethodImpl MakeReturningMethod(MethodInfo methodInfo, Type returnType, ParameterInfo[] parameters)
    {
      // Generate code that looks like:
      //
      // (object module, IJSValueReader inputReader, IJSValueWriter outputWriter,
      //    MethodResultCallback resolve, MethodResultCallback reject) =>
      // {
      //   inputReader.ReadArgs(out ArgType0 arg0, out ArgType1 arg1);
      //   resolve(outputWriter.WriteArgs((module as ModuleType).Method(arg0, arg1)));
      // }

      // It is like the MakeCallbackMethod but callback is not passed as a parameter.
      return CompileLambda<ReactMethodImpl>(
        MethodParameters(out var module, out var inputReader, out var outputWriter, out var resolve, out _),
        MethodArgs(parameters, out var argTypes, out var args),
        inputReader.CallExt(ReadArgsOf(argTypes), args),
        resolve.Invoke(outputWriter.CallExt(WriteArgsOf(returnType),
          module.CastTo(methodInfo.DeclaringType).Call(methodInfo, args))));
    }

    public delegate void ReactMethodImpl(
        object module,
        IJSValueReader inputReader,
        IJSValueWriter outputWriter,
        MethodResultCallback resolve,
        MethodResultCallback reject);

    public string MethodName { get; }

    public MethodReturnType MethodReturnType { get; }

    public Lazy<ReactMethodImpl> MethodImpl { get; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
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
