// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using ReactNative;

namespace RNTesterApp
{
    sealed partial class App : ReactApplication
    {
        private readonly ReactNativeHost _host = new MainReactNativeHost();

        public App()
        {
            this.InitializeComponent();
        }

        public override ReactNativeHost Host => _host;
    }
}
