// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

using Microsoft.ReactNative.Bridge;
using Microsoft.ReactNative.Managed;

namespace SampleLibraryCS
{
    public sealed class SampleLibraryPackage : IReactPackage
    {
        public IReadOnlyList<INativeModule> CreateNativeModules(ReactContext reactContext)
        {
            return _nativeModules ?? (_nativeModules = new List<INativeModule>() {
                new ManagedNativeModule(new FancyMath()),
                new FancyMathABI(),
                new ManagedNativeModule(new SampleModule()),
                new SampleModuleABI(),
            });
        }
        private IReadOnlyList<INativeModule> _nativeModules;

        public IReadOnlyList<IViewManager> CreateViewManagers(ReactContext reactContext)
        {
            return _viewManagers ?? (_viewManagers = new List<IViewManager>() {
                new ManagedViewManager(new CircleViewManager()),
                new CircleViewManagerABI(),
            });
        }
        private IReadOnlyList<IViewManager> _viewManagers;
    }
}
