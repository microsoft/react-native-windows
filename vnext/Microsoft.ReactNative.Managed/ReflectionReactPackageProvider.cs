// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.ReactNative;

namespace Microsoft.ReactNative.Managed
{
  /// <summary>
  /// A default implementation that uses reflection to extract the Modules and ViewManagers
  /// to be registered on the package builder.
  /// </summary>
  /// <typeparam name="T">This must be thea type defined in the assembly where the items for the react package should be extracted.</typeparam>
  /// <remarks>
  /// The common use case of this type is adding this to the PackageProviders from the constructor of your application (say: in <c>App()</c> of App.g.cs) like <c>PackageProviders.Add(new ReflectionReactPackageProvider&lt;App&gt;());</c>.
  /// </remarks>
  public class ReflectionReactPackageProvider<T> : IReactPackageProvider
  {
    public virtual void CreatePackage(IReactPackageBuilder packageBuilder)
    {
      ReflectionReactPackageProviderExtensions.AddReflectionReactPackageProvider<T>(packageBuilder);
    }
  }

  /// <summary>
  /// Extensions for registring a reflection based package provider
  /// </summary>
  public static class ReflectionReactPackageProviderExtensions
  {
    /// <summary>
    /// Helper extension method to register a reflection
    /// </summary>
    /// <typeparam name="T">This must be thea type defined in the assembly where the items for the react package should be extracted.</typeparam>
    /// <remarks>
    /// The common use case of this type is when you have a react library. Then you would call this from the <see cref="IReactPackageProvider.CreatePackage"/> function in ReactPackageProvider class that implements <see cref="IReactPackageProvider"/> like:
    /// <c>
    /// public sealed class ReactPackageProvider : IReactPackageProvider
    /// {
    ///     public void CreatePackage(IReactPackageBuilder packageBuilder)
    ///     {
    ///         packageBuilder.AddReflectionReactPackageProvider&lt;ReactPackageProvider&gt;();
    ///     }
    /// }
    /// </c>.
    /// </remarks>
    public static void AddReflectionReactPackageProvider<T>(this IReactPackageBuilder packageBuilder)
    {
      var assembly = typeof(T).Assembly;

      packageBuilder.AddAttributedModules(assembly);
      packageBuilder.AddViewManagers(assembly);
      JSValueReaderGenerator.RegisterAssembly(assembly);
      JSValueWriterGenerator.RegisterAssembly(assembly);
    }
  }
}
