// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading;

namespace Microsoft.ReactNative.Managed
{
  public static class ReactPackageBuilderExtensions
  {
    private static readonly Assembly s_currentAssembly = typeof(ReactPackageBuilderExtensions).Assembly;

    /// <summary>
    /// This caches for each assembly (keyed on location
    /// </summary>
    /// <remarks>
    /// We use concurrentDictionary instead of ConcurrentBag due to it's performance charateristics.
    /// </remarks>
    private static readonly ConcurrentDictionary<Assembly, Lazy<Dictionary<string, AttributedModule>>> s_registeredAssemblies =
      new ConcurrentDictionary<Assembly, Lazy<Dictionary<string, AttributedModule>>>();

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

    private static Dictionary<string, AttributedModule> InitAttributedModules(Assembly assembly)
    {
      var initializers =
        from typeInfo in assembly.DefinedTypes
        let moduleAttribute = typeInfo.GetCustomAttribute<ReactModuleAttribute>()
        where moduleAttribute != null
        select new AttributedModule { ModuleType = typeInfo.AsType(), ModuleAttribute = moduleAttribute };
      return initializers.ToDictionary(m => ReactModuleInfo.GetModuleName(m.ModuleType, m.ModuleAttribute));
    }

    /// <summary>
    /// This method registered the attributed models for the given assembly via reflection to the package builder.
    /// </summary>
    public static void AddAttributedModules(this IReactPackageBuilder packageBuilder, Assembly assembly)
    {
      var attributedModules = s_registeredAssemblies.GetOrAdd(assembly, asm => new Lazy<Dictionary<string, AttributedModule>>(() => InitAttributedModules(asm)));
      foreach (var attributedModule in attributedModules.Value.Values)
      {
        attributedModule.AddToPackageBuilder(packageBuilder);
      }
    }

    /// <summary>
    /// Adds the attributed models for the current assembly via reflection to the package builder.
    /// </summary>
    /// <remarks>
    /// This method will be obsolete soon. It should only be called by existing generated project that still use the `Microsoft.ReactNative.SharedManaged` project
    /// where s_currentAssembly will reflect the users assembly that compiles this code as well.
    /// </remarks>
    public static void AddAttributedModules(this IReactPackageBuilder packageBuilder)
    {
      AddAttributedModules(packageBuilder, s_currentAssembly);
    }

    public static bool TryAddAttributedModule(this IReactPackageBuilder packageBuilder, string moduleName)
    {
      if (s_registeredAssemblies.TryGetValue(s_currentAssembly, out var attributedModules))
      {
        if (attributedModules.Value.TryGetValue(moduleName, out var attributedModule))
        {
          attributedModule.AddToPackageBuilder(packageBuilder);
          return true;
        }
      }

      return false;
    }

    /// <summary>
    /// Adds the view managers defined in the given assembly via reflection to the package builder.
    /// </summary>
    public static void AddViewManagers(this IReactPackageBuilder packageBuilder, Assembly assembly)
    {
      foreach (var typeInfo in assembly.DefinedTypes)
      {
        if (!typeInfo.IsAbstract && typeInfo.ImplementedInterfaces.Contains(typeof(IViewManager)))
        {
          packageBuilder.AddViewManager(typeInfo.Name, () => (IViewManager)Activator.CreateInstance(typeInfo.AsType()));
        }
      }
    }

    /// <summary>
    /// Adds the view manages in the given assembly via reflection to the package builder.
    /// </summary>
    /// <remarks>
    /// This method will be obsolete soon. It should only be called by existing generated project that still use the `Microsoft.ReactNative.SharedManaged` project
    /// where s_currentAssembly will reflect the users assembly that compiles this code as well.
    /// </remarks>
    public static void AddViewManagers(this IReactPackageBuilder packageBuilder)
    {
      AddViewManagers(packageBuilder, s_currentAssembly);
    }
  }
}
