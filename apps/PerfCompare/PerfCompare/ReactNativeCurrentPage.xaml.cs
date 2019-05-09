// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

using ReactNative;
using ReactNative.Modules.Core;
using ReactNative.Shell;

namespace PerfCompare
{
    public sealed partial class ReactNativeCurrentPage : Page
    {
        private MainReactNativeHost _host = new MainReactNativeHost();

        public ReactNativeCurrentPage()
        {
            this.InitializeComponent();
            this.InitializeReactNative();
        }

        private void InitializeReactNative()
        {
            _host.OnResume(null);
            _host.ApplyArguments(null);
            this.Content = _host.OnCreate();
        }

        private async void Page_Loaded(object sender, RoutedEventArgs e)
        {
            await Dispatcher.RunIdleAsync((args) =>
            {
                App.PerfStats.Stop();
                App.ShowStats();
            });
        }
    }

    public sealed class MainReactNativeHost : ReactNativeHost
    {
        public override string MainComponentName => "PerfCompareRN";

#if DEBUG
        public override bool UseDeveloperSupport => true;
#else
        public override bool UseDeveloperSupport => false;
#endif

        protected override string JavaScriptMainModuleName => "index";

        protected override string JavaScriptBundleFile => "ms-appx:///ReactAssets/index.windows.bundle";

        protected override List<IReactPackage> Packages => new List<IReactPackage>
        {
            new MainReactPackage(),
        };
    }
}
