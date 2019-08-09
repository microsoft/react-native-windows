// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative;
using Microsoft.ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Playground
{
  sealed class SamplePackage : IReactPackage
  {
    public IReadOnlyList<NativeModuleBase> CreateNativeModules(ReactContext reactContext)
    {
      return new List<NativeModuleBase>() { new SampleModule() };
    }
  }
}
