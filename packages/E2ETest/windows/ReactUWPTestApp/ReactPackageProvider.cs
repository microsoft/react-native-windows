// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;

namespace TreeDumpLibrary
{
    public sealed class ReactPackageProvider : IReactPackageProvider
    {
        public void CreatePackage(IReactPackageBuilder packageBuilder)
        {
            packageBuilder.AddReflectionReactPackageProvider<ReactPackageProvider>();
        }
    }
}
