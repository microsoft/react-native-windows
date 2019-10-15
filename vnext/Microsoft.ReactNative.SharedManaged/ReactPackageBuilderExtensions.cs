using Microsoft.ReactNative.Bridge;
using Microsoft.ReactNative.Managed;
using System.Reflection;

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
