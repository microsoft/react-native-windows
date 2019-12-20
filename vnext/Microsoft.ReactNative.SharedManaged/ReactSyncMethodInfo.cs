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
  class ReactSyncMethodInfo
  {
    public ReactSyncMethodInfo(MethodInfo methodInfo, ReactSyncMethodAttribute methodAttribute)
    {
      MethodName = methodAttribute.MethodName ?? methodInfo.Name;
      MethodImpl = new Lazy<ReactSyncMethodImpl>(() => MakeMethod(methodInfo), LazyThreadSafetyMode.PublicationOnly);
    }

    private ReactSyncMethodImpl MakeMethod(MethodInfo methodInfo)
    {
      // Generate code that looks like:
      //
      // (object module, IJSValueReader inputReader, IJSValueWriter outputWriter) =>
      // {
      //   inputReader.ReadArgs(out ArgType0 arg0, out ArgType1 arg1)
      //   outputWriter.WriteArgs((module as ModuleType).SyncMethod(arg0, arg1));
      // }

      return CompileLambda<ReactSyncMethodImpl>(
        Parameter(typeof(object), out var module),
        Parameter(typeof(IJSValueReader), out var inputReader),
        Parameter(typeof(IJSValueWriter), out var outputWriter),
        MethodArgs(methodInfo.GetParameters(), out var argTypes, out var args),
        inputReader.CallExt(ReadArgsOf(argTypes), args),
        outputWriter.CallExt(WriteArgsOf(methodInfo.ReturnType),
          module.CastTo(methodInfo.DeclaringType).Call(methodInfo, args)));
    }

    public delegate void ReactSyncMethodImpl(object module, IJSValueReader inputReader, IJSValueWriter outputWriter);

    public string MethodName { get; private set; }

    public Lazy<ReactSyncMethodImpl> MethodImpl { get; private set; }

    public void AddToModuleBuilder(IReactModuleBuilder moduleBuilder, object module)
    {
      moduleBuilder.AddSyncMethod(MethodName, (IJSValueReader inputReader, IJSValueWriter outputWriter) =>
      {
        MethodImpl.Value(module, inputReader, outputWriter);
      });
    }
  }
}
