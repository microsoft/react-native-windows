using ReactNative;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative.Shell;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.Views.View;
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

        protected override List<IReactPackage> Packages => new List<IReactPackage>
        {
            new MainReactPackage(),
            new FooReactPackage(),
        };

        class FooReactPackage : IReactPackage, IViewManagerExtensionProvider
        {
            public IReadOnlyList<INativeModule> CreateNativeModules(ReactContext reactContext)
            {
                return new List<INativeModule>(0);
            }

            public IReadOnlyList<IViewManagerExtension> CreateViewManagerExtensions(ReactContext reactContext)
            {
                return new List<IViewManagerExtension>
                {
                    new TestViewManagerExtension(),
                };
            }

            public IReadOnlyList<IViewManager> CreateViewManagers(ReactContext reactContext)
            {
                return new List<IViewManager>(0);
            }

            class TestViewManagerExtension : ViewManagerExtension<BorderedCanvas, ReactViewManager>
            {
                [ReactProp("foo")]
                public void SetFoo(BorderedCanvas view, int foo)
                {
                    view.Tag = foo;
                }
            }
        }
    }
}
