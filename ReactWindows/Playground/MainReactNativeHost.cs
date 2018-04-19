// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative;
using ReactNative.Modules.Core;
using ReactNative.Shell;
using System.Collections.Generic;

namespace Playground
{
    class MainReactNativeHost : ReactNativeHost
    {
        public override string MainComponentName => "Playground";

#if !BUNDLE || DEBUG
        public override bool UseDeveloperSupport => true;
#else
        public override bool UseDeveloperSupport => false;
#endif

        protected override string JavaScriptMainModuleName => "ReactWindows/Playground/index.windows";

#if BUNDLE
        protected override string JavaScriptBundleFile => "ms-appx:///ReactAssets/index.windows.bundle";
#endif

        public override JObject InitialProperties => JObject.FromObject(new Dictionary<string, object>
        {
            {
                "date", System.DateTime.Now.ToString("M/d/yyyy")
            }
        });

        protected override List<IReactPackage> Packages => new List<IReactPackage>
        {
            new MainReactPackage(),
        };
    }
}
