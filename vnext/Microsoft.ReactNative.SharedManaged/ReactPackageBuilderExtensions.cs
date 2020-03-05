// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading;

namespace Microsoft.ReactNative.Managed
{
  static class ReactPackageBuilderExtensions
  {
    struct AttributedModule
    {
      public Type ModuleType;
      public ReactModuleAttribute ModuleAttribute;

      public void AddToPackageBuilder(IReactPackageBuilder packageBuilder)
      {
        ReactModuleInfo moduleInfo = ReactModuleInfo.GetOrAddModuleInfo(ModuleType, ModuleAttribute);
        packageBuilder.AddModule(moduleInfo.ModuleName, moduleInfo.ModuleProvider);
      }
    }

    private static readonly Lazy<Dictionary<string, AttributedModule>> m_attributedModules =
      new Lazy<Dictionary<string, AttributedModule>>(InitAttributedModules, LazyThreadSafetyMode.PublicationOnly);

    private static Dictionary<string, AttributedModule> InitAttributedModules()
    {
      var initializers =
        from typeInfo in typeof(ReactPackageBuilderExtensions).GetTypeInfo().Assembly.DefinedTypes
        let moduleAttribute = typeInfo.GetCustomAttribute<ReactModuleAttribute>()
        where moduleAttribute != null
        select new AttributedModule { ModuleType = typeInfo.AsType(), ModuleAttribute = moduleAttribute };
      return initializers.ToDictionary(m => ReactModuleInfo.GetModuleName(m.ModuleType, m.ModuleAttribute));
    }

    public static void AddAttributedModules(this IReactPackageBuilder packageBuilder)
    {
      foreach (var attributedModule in m_attributedModules.Value)
      {
        attributedModule.Value.AddToPackageBuilder(packageBuilder);
      }
    }

    public static bool TryAddAttributedModule(this IReactPackageBuilder packageBuilder, string moduleName)
    {
      if (m_attributedModules.Value.TryGetValue(moduleName, out var attributedModule))
      {
        attributedModule.AddToPackageBuilder(packageBuilder);
        return true;
      }

      return false;
    }

    internal static void AddViewManagers(this IReactPackageBuilder packageBuilder)
    {
      foreach (var typeInfo in typeof(ReactPackageBuilderExtensions).GetTypeInfo().Assembly.DefinedTypes)
      {
        if (!typeInfo.IsAbstract && typeInfo.ImplementedInterfaces.Contains(typeof(IViewManager)))
        {
          packageBuilder.AddViewManager(typeInfo.Name, () => (IViewManager)Activator.CreateInstance(typeInfo.AsType()));
        }
      }
    }
  }
}
