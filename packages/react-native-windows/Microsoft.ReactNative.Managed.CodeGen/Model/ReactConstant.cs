// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using System.Diagnostics.ContractsLight;

namespace Microsoft.ReactNative.Managed.CodeGen.Model
{
  public class ReactConstant
  {
    public ISymbol Symbol { get; }

    public string Name { get; }

    public ReactConstant(ISymbol symbol, string name)
    {
      Contract.Assert(symbol is IPropertySymbol || symbol is IFieldSymbol, "Should only be called for properties and fields");

      Symbol = symbol;
      Name = name;
    }
  }
}
