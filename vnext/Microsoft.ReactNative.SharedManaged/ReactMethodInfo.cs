// Copyright (c) Microsoft Corporation.
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
      //     result => resolve(outputWriter.WriteArgs(result)));
      // }

      // The last parameter in parameters is a 'resolve' delegate
      return CompileLambda<ReactMethodImpl>(
        MethodParameters(out var module, out var inputReader, out var outputWriter, out var resolve, out _),
        MethodArgs(parameters, out var argTypes, out var args,
          out var resolveCallbackType, out var resolveArgType),
        inputReader.CallExt(ReadArgsOf(argTypes), args),
        module.CastTo(methodInfo.DeclaringType).Call(methodInfo, args,
          AutoLambda(resolveCallbackType, Parameter(resolveArgType, out var result),
            resolve.Invoke(outputWriter.CallExt(WriteArgsOf(resolveArgType), result)))));
    }

    private ReactMethodImpl MakeTwoCallbacksMethod(MethodInfo methodInfo, ParameterInfo[] parameters)
    {
      // Generate code that looks like:
      //
      // (object module, IJSValueReader inputReader, IJSValueWriter outputWriter,
      //    MethodResultCallback resolve, MethodResultCallback reject) =>
      // {
      //   inputReader.ReadArgs(out ArgType0 arg0, out ArgType1 arg1);
      //   (module as ModuleType).Method(arg0, arg1,
      //     result => resolve(outputWriter.WriteArgs(result)),
      //     error => reject(outputWriter.WriteError(error)));
      // }

      // The last two parameters in parameters are resolve and reject delegates
      return CompileLambda<ReactMethodImpl>(
        MethodParameters(out var module, out var inputReader, out var outputWriter, out var resolve, out var reject),
        MethodArgs(parameters, out var argTypes, out var args,
          out var resolveCallbackType, out var resolveArgType,
          out var rejectCallbackType, out var rejectArgType),
        inputReader.CallExt(ReadArgsOf(argTypes), args),
        module.CastTo(methodInfo.DeclaringType).Call(methodInfo, args,
          AutoLambda(resolveCallbackType, Parameter(resolveArgType, out var result),
            resolve.Invoke(outputWriter.CallExt(WriteArgsOf(resolveArgType), result))),
          AutoLambda(rejectCallbackType, Parameter(rejectArgType, out var error),
            reject.Invoke(outputWriter.CallExt(WriteErrorOf(rejectArgType), error)))));
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
