using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using ReactNative;

namespace NativeControlTestDriver
{
    public class MockReactPage : Page, IAsyncDisposable
    {
        private readonly Lazy<IReactInstanceManager> _reactInstanceManager;
        private readonly Lazy<ReactRootView> _rootView;

        public MockReactPage()
        {
            _reactInstanceManager = new Lazy<IReactInstanceManager>(() =>
            {
                var reactInstanceManager = CreateReactInstanceManager();

                return reactInstanceManager;
            });

            _rootView = new Lazy<ReactRootView>(() =>
            {
                var rootview = CreateRootView();

                base.Content = rootview;

                return rootview;
            });
        }

        private IReactInstanceManager ReactInstanceManager => _reactInstanceManager.Value;

        public string JavaScriptBundleFile => @"JavaScriptBundleFile";

        public string JavaScriptMainModuleName => @"JavaScriptMainModuleName";

        public string MainComponentName => @"MockReactPage";

        public bool UseDeveloperSupport => false;

        public List<IReactPackage> Packages { get; }

        public ReactRootView RootView => _rootView.Value;

        public void Initialize()
        {
            RootView.StartReactApplication(ReactInstanceManager, MainComponentName);

            RootView.Background = new SolidColorBrush(Colors.Blue);
            RootView.Focusable = true;
            RootView.Focus();
            RootView.FocusVisualStyle = null;
        }

        /// <summary>
        /// Called before the application shuts down.
        /// </summary>
        public async Task DisposeAsync()
        {
            if (_reactInstanceManager.IsValueCreated)
            {
                await ReactInstanceManager.DisposeAsync().ConfigureAwait(false);
            }
        }

        /// <summary>
        /// Creates the React root view.
        /// </summary>
        /// <returns>The root view.</returns>
        /// <remarks>
        /// Subclasses may override this method if it needs to use a custom
        /// root view.
        /// </remarks>
        private ReactRootView CreateRootView()
        {
            return new ReactRootView();
        }

        private IReactInstanceManager CreateReactInstanceManager()
        {
            var builder = new ReactInstanceManager.Builder
            {
                UseDeveloperSupport = UseDeveloperSupport,
                InitialLifecycleState = LifecycleState.Resumed,
                JavaScriptBundleFile = JavaScriptBundleFile,
                JavaScriptMainModuleName = JavaScriptMainModuleName,
            };

            if (Packages != null)
            {
                builder.Packages.AddRange(Packages);
            }
            
            return builder.Build();
        }
    }
}