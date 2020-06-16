// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Collections.Generic;
using Microsoft.CodeAnalysis;

namespace Microsoft.ReactNative.Managed.CodeGen.Model
{
  public class ReactModule
  {
    public INamedTypeSymbol ModuleType { get; }

    public string ModuleName { get; }

    public string EventEmitterName { get; }

    public ICollection<ReactInitializer> Initializers { get; } = new List<ReactInitializer>();

    public ICollection<ReactConstant> Constants { get; } = new List<ReactConstant>();

    public ICollection<ReactConstantProvider> ConstantProviders { get; } = new List<ReactConstantProvider>();

    public ICollection<ReactMethod> Methods { get; } = new List<ReactMethod>();

    public ICollection<ReactFunction> Functions { get; } = new List<ReactFunction>();

    public ICollection<ReactEvent> Events { get; } = new List<ReactEvent>();

    public ReactModule(INamedTypeSymbol moduleType, string moduleName, string eventEmitterName)
    {
      ModuleType = moduleType;
      ModuleName = moduleName;
      EventEmitterName = eventEmitterName;
    }
  }
}
