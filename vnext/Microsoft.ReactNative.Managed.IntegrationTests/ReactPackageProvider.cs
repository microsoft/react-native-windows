// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed.IntegrationTests
{
  public partial class ReactPackageProvider : IReactPackageProvider
  {
    public void CreatePackage(IReactPackageBuilder packageBuilder)
    {
      CreatePackageImplementation(packageBuilder);
    }

    /// <summary>
    /// This method is implemented by the C# code generator
    /// </summary>
    partial void CreatePackageImplementation(IReactPackageBuilder packageBuilder);
  }
}
