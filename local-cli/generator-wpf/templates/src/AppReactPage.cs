using ReactNative;
using ReactNative.Modules.Core;
using ReactNative.Shell;
using System;
using System.Collections.Generic;

namespace <%= ns %>
{
    internal class AppReactPage : ReactPage
    {
        public override string MainComponentName => "<%= name %>";

#if BUNDLE
        public override string JavaScriptBundleFile => AppDomain.CurrentDomain.BaseDirectory + "ReactAssets/index.windows.bundle";
#endif

        public override List<IReactPackage> Packages => new List<IReactPackage>
        {
            new MainReactPackage(),
        };

        public override bool UseDeveloperSupport
        {
            get
            {
#if !BUNDLE || DEBUG
                return true;
#else
                return false;
#endif
            }
        }
    }
}
