// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative;

namespace SampleApp
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
            MainComponentName = "SampleApp";
            JavaScriptMainModuleName = "index.windows";

            PackageProviders.Add(new Microsoft.ReactNative.Managed.ReactPackageProvider()); // Includes any modules in this project
            PackageProviders.Add(new SampleLibraryCS.ReactPackageProvider());
            PackageProviders.Add(new SampleLibraryCPP.ReactPackageProvider());

            InitializeComponent();
        }
    }
}
