// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.CodeAnalysis;
using System.Collections.Generic;

namespace Microsoft.ReactNative.Managed.CodeGen.Model
{
  public class ReactAssembly
  {
    public ICollection<INamedTypeSymbol> ViewManagers { get; } = new List<INamedTypeSymbol>();

    public ICollection<ReactModule> Modules { get; } = new List<ReactModule>();

    public IDictionary<ITypeSymbol, IMethodSymbol> JSReaderFunctions { get; } = new Dictionary<ITypeSymbol, IMethodSymbol>();

    public IDictionary<ITypeSymbol, IMethodSymbol> JSWriterFunctions { get; } = new Dictionary<ITypeSymbol, IMethodSymbol>();

    public IDictionary<INamedTypeSymbol, TypeKind> SerializableTypes { get; } = new Dictionary<INamedTypeSymbol, TypeKind>();
  }

}
