// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;
using System.Collections.Generic;
using Windows.Networking.Sockets;

using Windows.Web.UI;

namespace SampleApp
{
  sealed class SamplePackage : IReactPackage
  {
    public IReadOnlyList<INativeModule> CreateNativeModules(
      ReactContext reactContext)
    {
      return new List<INativeModule>() {
        new SampleModule(),
      };
    }

    public IReadOnlyList<IViewManager> CreateViewManagers(ReactContext reactContext)
    {
      return new List<IViewManager>();
    }
  }
}
