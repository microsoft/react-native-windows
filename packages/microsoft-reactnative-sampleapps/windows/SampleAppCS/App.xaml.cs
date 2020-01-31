// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative;

namespace SampleAppCS
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default
    /// Application class.
    /// </summary>
    sealed partial class App : ReactApplication
    {
        /// <summary>
        /// Initializes the singleton application object.  This is the first line
        /// of authored code executed, and as such is the logical equivalent of
        /// main() or WinMain().
        /// </summary>
        public App()
        {
            InstanceSettings.MainComponentName = "SampleApp";

#if BUNDLE
            InstanceSettings.JavaScriptBundleFile = "index.windows";
            InstanceSettings.UseWebDebugger = false;
            InstanceSettings.UseLiveReload = false;
#else
            InstanceSettings.JavaScriptMainModuleName = "index";
            InstanceSettings.UseWebDebugger = true;
            InstanceSettings.UseLiveReload = true;
#endif

#if DEBUG
            InstanceSettings.EnableDeveloperMenu = true;
#else
            InstanceSettings.EnableDeveloperMenu = false;
#endif

            PackageProviders.Add(new Microsoft.ReactNative.Managed.ReactPackageProvider()); // Includes any modules in this project
            PackageProviders.Add(new SampleLibraryCS.ReactPackageProvider());
            PackageProviders.Add(new SampleLibraryCpp.ReactPackageProvider());

            InitializeComponent();
        }
    }
}
