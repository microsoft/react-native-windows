// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Collections.Generic;
using System.Diagnostics.ContractsLight;
using Microsoft.CodeAnalysis;

namespace Microsoft.ReactNative.Managed.CodeGen.Model
{
  public class ReactMethod
  {
    public IMethodSymbol Method { get; }

    public string Name { get; }

    public bool IsSynchronous { get; }

    public MethodReturnStyle ReturnStyle { get; }

    public ITypeSymbol EffectiveReturnType { get; }

    public IReadOnlyList<IParameterSymbol> EffectiveParameters { get; }

    public ReactMethod(IMethodSymbol method, string name, MethodReturnStyle returnStyle, ITypeSymbol effectiveReturnType, IReadOnlyList<IParameterSymbol> effectiveParameters, bool isSynchronous)
    {
      Contract.Requires(!(IsSynchronous && returnStyle == MethodReturnStyle.Task), "Task style methods are required to be asynchronous");
      Method = method;
      Name = name;
      ReturnStyle = returnStyle;
      EffectiveReturnType = effectiveReturnType;
      EffectiveParameters = effectiveParameters;
      IsSynchronous = isSynchronous;
    }

    public enum MethodReturnStyle
    {
      Void = 0,
      Callback,
      TwoCallbacks,
      Promise,
      ReturnValue,
      Task,
    }
  }
}
