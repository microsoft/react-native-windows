// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Modules.Core;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ReactNative
{
    /// <summary>
    /// Base page for React Native applications.
    /// </summary>
    [Obsolete("Please use ReactNativeHost instead of ReactPage.")]
    public abstract class ReactPage : Page, IAsyncDisposable
    {
        private readonly ReactInstanceManager _reactInstanceManager;

        private bool _isShiftKeyDown;
        private bool _isControlKeyDown;

        /// <summary>
        /// Instantiates the <see cref="ReactPage"/>.
        /// </summary>
        protected ReactPage()
        {
            _reactInstanceManager = CreateReactInstanceManager();
            RootView = CreateRootView();
            Content = RootView;
        }

        /// <summary>
        /// The custom path of the bundle file.
        /// </summary>
        /// <remarks>
        /// This is used in cases where the bundle should be loaded from a
        /// custom path.
        /// </remarks>
        public virtual string JavaScriptBundleFile
        {
            get
            {
                return null;
            }
        }

        /// <summary>
        /// The name of the main module.
        /// </summary>
        /// <remarks>
        /// This is used to determine the URL used to fetch the JavaScript
        /// bundle from the packager server. It is only used when dev support
        /// is enabled.
        /// </remarks>
        public virtual string JavaScriptMainModuleName
        {
            get
            {
                return "index.windows";
            }
        }

        /// <summary>
        /// Instantiates the JavaScript executor.
        /// </summary>
        public virtual Func<IJavaScriptExecutor> JavaScriptExecutorFactory
        {
            get
            {
                return null;
            }
        }

        /// <summary>
        /// The name of the main component registered from JavaScript.
        /// </summary>
        public abstract string MainComponentName { get; }

        /// <summary>
        /// Signals whether developer mode should be enabled.
        /// </summary>
        public abstract bool UseDeveloperSupport { get; }

        /// <summary>
        /// The list of <see cref="IReactPackage"/>s used by the application.
        /// </summary>
        public abstract List<IReactPackage> Packages { get; }

        /// <summary>
        /// The root view managed by the page.
        /// </summary>
        public ReactRootView RootView { get; }

        /// <summary>
        /// Called when the application is first initialized.
        /// </summary>
        /// <param name="arguments">The launch arguments.</param>
        public void OnCreate(string arguments)
        {
            OnCreate(arguments, default(JObject));
        }

        /// <summary>
        /// Called when the application is first initialized.
        /// </summary>
        /// <param name="arguments">The launch arguments.</param>
        /// <param name="initialProps">The initialProps.</param>
        public void OnCreate(string arguments, JObject initialProps)
        {
            RootView.Background = (Brush)Application.Current.Resources["ApplicationPageBackgroundThemeBrush"];

            ApplyArguments(arguments);
            RootView.StartReactApplication(_reactInstanceManager, MainComponentName, initialProps);

            SystemNavigationManager.GetForCurrentView().BackRequested += (sender, args) =>
            {
                _reactInstanceManager.OnBackPressed();
                args.Handled = true;
            };

            Window.Current.CoreWindow.Dispatcher.AcceleratorKeyActivated += OnAcceleratorKeyActivated;
        }
        
        /// <summary>
        /// Called before the application is suspended.
        /// </summary>
        public void OnSuspend()
        {
            _reactInstanceManager.OnSuspend();
        }

        /// <summary>
        /// Called when the application is resumed.
        /// </summary>
        /// <param name="onBackPressed">
        /// Default action to take when back pressed.
        /// </param>
        public void OnResume(Action onBackPressed)
        {
            _reactInstanceManager.OnResume(onBackPressed);
        }

        /// <summary>
        /// Called before the application shuts down.
        /// </summary>
        public async Task DisposeAsync()
        {
            Window.Current.CoreWindow.Dispatcher.AcceleratorKeyActivated -= OnAcceleratorKeyActivated;

            await RootView.StopReactApplicationAsync();

            await _reactInstanceManager.DisposeAsync();            
        }

        /// <summary>
        /// Creates the React root view.
        /// </summary>
        /// <returns>The root view.</returns>
        /// <remarks>
        /// Subclasses may override this method if it needs to use a custom
        /// root view.
        /// </remarks>
        protected virtual ReactRootView CreateRootView()
        {
            return new ReactRootView();
        }

        /// <summary>
        /// Captures the all key downs and Ups. 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnAcceleratorKeyActivated(CoreDispatcher sender, AcceleratorKeyEventArgs e)
        {
            if (_reactInstanceManager.DevSupportManager.IsEnabled)
            {
                if (e.VirtualKey == VirtualKey.Shift)
                {
                    _isShiftKeyDown = e.EventType == CoreAcceleratorKeyEventType.KeyDown;
                }
                else if (e.VirtualKey == VirtualKey.Control)
                {
                    _isControlKeyDown = e.EventType == CoreAcceleratorKeyEventType.KeyDown;
                }
                else if ( (_isShiftKeyDown && e.VirtualKey == VirtualKey.F10) ||
                          (e.EventType == CoreAcceleratorKeyEventType.KeyDown && e.VirtualKey == VirtualKey.Menu))
                {
                    _reactInstanceManager.DevSupportManager.ShowDevOptionsDialog();
                }
                else if (e.EventType == CoreAcceleratorKeyEventType.KeyUp && _isControlKeyDown && e.VirtualKey == VirtualKey.R)
                {
                   _reactInstanceManager.DevSupportManager.HandleReloadJavaScript();
                }
            }
        }

        private ReactInstanceManager CreateReactInstanceManager()
        {
            var builder = new ReactInstanceManagerBuilder
            {
                UseDeveloperSupport = UseDeveloperSupport,
                InitialLifecycleState = LifecycleState.BeforeCreate,
                JavaScriptBundleFile = JavaScriptBundleFile,
                JavaScriptMainModuleName = JavaScriptMainModuleName,
                JavaScriptExecutorFactory = JavaScriptExecutorFactory,
            };

            builder.Packages.AddRange(Packages);
            return builder.Build();
        }

        private void ApplyArguments(string arguments)
        {
            if (!string.IsNullOrEmpty(arguments))
            {
                var args = arguments.Split(',');

                var index = Array.IndexOf(args, "remoteDebugging");
                if (index < 0)
                {
                    return;
                }

                if (args.Length <= index + 1)
                {
                    throw new ArgumentException("Expected value for remoteDebugging argument.", nameof(arguments));
                }

                if (bool.TryParse(args[index + 1], out var isRemoteDebuggingEnabled))
                {
                    _reactInstanceManager.DevSupportManager.IsRemoteDebuggingEnabled = isRemoteDebuggingEnabled;
                }
            }
        }
    }
}
