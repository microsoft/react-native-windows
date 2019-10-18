// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Reflection;

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{
  static class ReactPackageBuilderExtensions
  {
    internal static void AddAttributedModules(this IReactPackageBuilder packageBuilder)
    {
      foreach (var type in typeof(ReactPackageBuilderExtensions).GetTypeInfo().Assembly.GetTypes())
      {
        var moduleAttribute = type.GetTypeInfo().GetCustomAttribute<ReactModuleAttribute>();
        if (moduleAttribute != null)
        {
          ReactModuleInfo moduleInfo = ReactModuleInfo.GetOrAddModuleInfo(type, moduleAttribute);
          packageBuilder.AddModule(moduleInfo.ModuleName, moduleInfo.ModuleProvider);
        }
      }
    }
  }
}
