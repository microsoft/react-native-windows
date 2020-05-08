// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;

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
            MainComponentName = "SampleApp";

#if BUNDLE
            JavaScriptBundleFile = "index.windows";
            InstanceSettings.UseWebDebugger = false;
            InstanceSettings.UseFastRefresh = false;
#else
            JavaScriptMainModuleName = "index";
            InstanceSettings.UseWebDebugger = true;
            InstanceSettings.UseFastRefresh = true;
#endif

#if DEBUG
            InstanceSettings.EnableDeveloperMenu = true;
#else
            InstanceSettings.EnableDeveloperMenu = false;
#endif

            InstanceSettings.GlobalProperties.SetProperty(ReactPropertyBag.GetName(null, "Prop1"), 43);
            InstanceSettings.InstanceProperties.SetProperty(ReactPropertyBag.GetName(null, "Prop2"), "Hello RNW!");

            PackageProviders.Add(new Microsoft.ReactNative.Managed.ReactPackageProvider()); // Includes any modules in this project
            PackageProviders.Add(new ReflectionReactPackageProvider<App>());
            PackageProviders.Add(new SampleLibraryCS.ReactPackageProvider());
            PackageProviders.Add(new SampleLibraryCpp.ReactPackageProvider());

            InitializeComponent();
        }
    }
}
