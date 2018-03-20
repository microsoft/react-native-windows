// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Common;
using ReactNative.Modules.Core;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Input;

namespace ReactNative
{
    /// <summary>
    /// Base page for React Native applications.
    /// </summary>
    public abstract class ReactPage : Page, IAsyncDisposable
    {
        private readonly Lazy<ReactInstanceManager> _reactInstanceManager;
        private readonly Lazy<ReactRootView> _rootView;

        /// <summary>
        /// Instantiates the <see cref="ReactPage"/>.
        /// </summary>
        protected ReactPage()
        {
            _reactInstanceManager = new Lazy<ReactInstanceManager>(() =>
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

        private ReactInstanceManager ReactInstanceManager => _reactInstanceManager.Value;

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
        public ReactRootView RootView => _rootView.Value;

        /// <summary>
        /// Called when the application is first initialized.
        /// </summary>
        /// <param name="arguments">The launch arguments.</param>
        public void OnCreate(string[] arguments)
        {
            OnCreate(arguments, default(JObject));
        }

        /// <summary>
        /// Called when the application is first initialized.
        /// </summary>
        /// <param name="arguments">The launch arguments.</param>
        /// <param name="initialProps">The initialProps.</param>
        public void OnCreate(string[] arguments, JObject initialProps)
        {
            ApplyArguments(arguments);
            RootView.StartReactApplication(ReactInstanceManager, MainComponentName, initialProps);

            RootView.AddHandler(Keyboard.KeyDownEvent, (KeyEventHandler)OnAcceleratorKeyActivated);

            RootView.Focusable = true;
            RootView.Focus();
            RootView.FocusVisualStyle = null;
        }

        /// <summary>
        /// Called before the application is suspended.
        /// </summary>
        public void OnSuspend()
        {
            ReactInstanceManager.OnSuspend();
        }

        /// <summary>
        /// Called when the application is resumed.
        /// </summary>
        /// <param name="onBackPressed">
        /// Default action to take when back pressed.
        /// </param>
        public void OnResume(Action onBackPressed)
        {
            ReactInstanceManager.OnResume(onBackPressed);
        }

        /// <summary>
        /// Called before the application shuts down.
        /// </summary>
        public async Task DisposeAsync()
        {
            RootView?.RemoveHandler(Keyboard.KeyDownEvent, (KeyEventHandler)OnAcceleratorKeyActivated);

            await RootView?.StopReactApplicationAsync();

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
        protected virtual ReactRootView CreateRootView()
        {
            return new ReactRootView();
        }

        /// <summary>
        /// Captures the all key downs and Ups. 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnAcceleratorKeyActivated(object sender, KeyEventArgs e)
        {
            if (ReactInstanceManager.DevSupportManager.IsEnabled)
            {
                var isCtrlKeyDown = (Keyboard.Modifiers & ModifierKeys.Control) == ModifierKeys.Control;

                //Ctrl+D or Ctrl+M
                if (isCtrlKeyDown && (e.Key == Key.D || e.Key == Key.M))
                {
                    ReactInstanceManager.DevSupportManager.ShowDevOptionsDialog();
                }

                // Ctrl+R
                if (isCtrlKeyDown && e.Key == Key.R)
                {
                    ReactInstanceManager.DevSupportManager.HandleReloadJavaScript();
                }
            }

            // Back button
            if (e.Key == Key.Back || e.Key == Key.BrowserBack)
            {
                ReactInstanceManager.OnBackPressed();
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

        private void ApplyArguments(string[] arguments)
        {
            if (arguments == null)
            {
                return;
            }

            if (arguments.Length == 0)
            {
                return;
            }

            var index = Array.IndexOf(arguments, "remoteDebugging");
            if (index < 0)
            {
                return;
            }

            if (arguments.Length <= index + 1)
            {
                throw new ArgumentException("Expected value for remoteDebugging argument.", nameof(arguments));
            }

            bool isRemoteDebuggingEnabled;
            if (bool.TryParse(arguments[index + 1], out isRemoteDebuggingEnabled))
            {
                ReactInstanceManager.DevSupportManager.IsRemoteDebuggingEnabled = isRemoteDebuggingEnabled;
            }
        }
    }
}
