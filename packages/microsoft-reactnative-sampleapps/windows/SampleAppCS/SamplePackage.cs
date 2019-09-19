// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;
using System.Collections.Generic;

namespace SampleApp
{
  sealed class SamplePackage : IReactPackage
  {
    public IReadOnlyList<INativeModule> CreateNativeModules(
      ReactContext reactContext)
    {
      return new List<INativeModule>() { new SampleModule() };
    }
  }
}
