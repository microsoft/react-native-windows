// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;

namespace Microsoft.ReactNative.Managed.CodeGen.Model
{
  public class ReactConstantProvider
  {
    public IMethodSymbol Method { get; }

    public ReactConstantProvider(IMethodSymbol method)
    {
      Method = method;
    }
  }
}
