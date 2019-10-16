// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System.Collections.Generic;

using Microsoft.ReactNative;
using Microsoft.ReactNative.Bridge;

namespace SampleApp
{
    sealed class MainReactNativeHost : ReactNativeHost
    {
        public MainReactNativeHost()
        {
            this.InstanceSettings.UseWebDebugger = false; // Disabled temporarily because of issue #2877
            this.InstanceSettings.UseLiveReload = true; // true by default in debug builds already
            this.InstanceSettings.UseJsi = true;

#if DEBUG
            this.InstanceSettings.EnableDeveloperMenu = true;
#else
            this.InstanceSettings.EnableDeveloperMenu = false;
#endif

        }

        protected override string MainComponentName => "SampleApp";
        protected override string JavaScriptMainModuleName => "index.windows";

        protected override IReadOnlyList<IReactPackageProvider> PackageProviders
        {
            get
            {
                return new IReactPackageProvider[] {
                    new Microsoft.ReactNative.Managed.ReactPackageProvider(), // Includes any modules in this project
                    new SampleLibraryCS.ReactPackageProvider(),
                    new SampleLibraryCPP.ReactPackageProvider(),
                };
            }
        }
    }
}
