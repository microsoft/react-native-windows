// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;

namespace Microsoft.ReactNative.Managed.CodeGen.Model
{
  public class ReactGetConstants
  {
    public IMethodSymbol Method { get; }

    public ReactGetConstants(IMethodSymbol method)
    {
      Method = method;
    }
  }
}
