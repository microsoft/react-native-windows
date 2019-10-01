// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;

namespace Microsoft.ReactNative.Managed
{
  class ReactModulePackage : INativeModulePackage
  {
    public void CreateModuleProviders(ModuleProviderAdder addModuleProvider)
    {
      ReactModuleInfo.AddAttributedModules(addModuleProvider);
    }
  }
}
