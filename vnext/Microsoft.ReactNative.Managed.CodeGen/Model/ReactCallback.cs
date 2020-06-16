// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using System.Collections.Immutable;
using System.Diagnostics.ContractsLight;

namespace Microsoft.ReactNative.Managed.CodeGen.Model
{
  public abstract class ReactCallback
  {
    public ISymbol Symbol { get; }

    public ImmutableArray<IParameterSymbol> CallbackParameters { get; }

    public string Name { get; }

    public string CallbackContextName { get; }

    public ReactCallback(ISymbol symbol, ImmutableArray<IParameterSymbol> callbackParameters, string name, string callbackContextName)
    {
      Contract.Requires(symbol is IPropertySymbol || symbol is IFieldSymbol, "Should only be called for properties and fields");

      Symbol = symbol;
      CallbackParameters = callbackParameters;
      Name = name;
      CallbackContextName = callbackContextName;
    }
  }
}
