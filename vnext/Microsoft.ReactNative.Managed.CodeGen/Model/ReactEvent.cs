// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using System.Collections.Immutable;

namespace Microsoft.ReactNative.Managed.CodeGen.Model
{
  public class ReactEvent : ReactCallback
  {
    public ReactEvent(ISymbol symbol, ImmutableArray<IParameterSymbol> callbackParameters, string name, string emitterName)
    : base(symbol, callbackParameters, name, emitterName)
    {
    }
  }
}
