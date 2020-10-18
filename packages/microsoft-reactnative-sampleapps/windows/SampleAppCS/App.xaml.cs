// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.ReactNative;
using Microsoft.ReactNative.Managed;
using Windows.ApplicationModel.Activation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

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

#if BUNDLE
            JavaScriptBundleFile = "index.windows";
            InstanceSettings.UseWebDebugger = false;
            InstanceSettings.UseFastRefresh = false;
#else
            JavaScriptBundleFile = "index";
            InstanceSettings.UseWebDebugger = true;
            InstanceSettings.UseFastRefresh = true;
#endif

#if DEBUG
            InstanceSettings.UseDeveloperSupport = true;
#else
            InstanceSettings.UseDeveloperSupport = false;
#endif

            InstanceSettings.Properties.Set(ReactPropertyBagHelper.GetName(null, "Prop1"), 43);
            InstanceSettings.Properties.Set(ReactPropertyBagHelper.GetName(null, "Prop2"), "Hello RNW!");

            Microsoft.ReactNative.Managed.AutolinkedNativeModules.RegisterAutolinkedNativeModulePackages(PackageProviders); // Includes any autolinked modules

            PackageProviders.Add(new Microsoft.ReactNative.Managed.ReactPackageProvider());
            PackageProviders.Add(new SampleAppCS.ReactPackageProvider());
            PackageProviders.Add(new SampleLibraryCS.ReactPackageProvider());
            PackageProviders.Add(new SampleLibraryCpp.ReactPackageProvider());

            InitializeComponent();
        }

        protected override void OnLaunched(LaunchActivatedEventArgs e)
        {
            base.OnLaunched(e);
            var frame = (Frame)Window.Current.Content;
            frame.Navigate(typeof(MainPage), e.Arguments);
        }
    }
}
