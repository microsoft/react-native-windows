using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace ReactNative
{
    /// <summary>
    /// Base page for React Native applications.
    /// </summary>
    public abstract class ReactPage : Page, IAsyncDisposable
    {
        private readonly IReactInstanceManager _reactInstanceManager;

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
            RootView.Background = (Brush)Application.Current.Resources["ApplicationPageBackgroundThemeBrush"];

            ApplyArguments(arguments);
            RootView.StartReactApplication(_reactInstanceManager, MainComponentName);
            
            RootView.AddHandler(Keyboard.KeyDownEvent, (KeyEventHandler)OnAcceleratorKeyActivated);
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
        public Task DisposeAsync()
        {
            RootView.RemoveHandler(Keyboard.KeyDownEvent, (KeyEventHandler)OnAcceleratorKeyActivated);

            return _reactInstanceManager.DisposeAsync();            
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
            if (_reactInstanceManager.DevSupportManager.IsEnabled)
            {
                // Shift+F10 or Shift+Menu
                if ((e.Key == Key.F10 || e.Key == Key.Apps) &&
                    (Keyboard.Modifiers & ModifierKeys.Shift) == ModifierKeys.Shift)
                {
                    _reactInstanceManager.DevSupportManager.ShowDevOptionsDialog();
                }
                // Ctrl+R
                if (e.Key == Key.R && (Keyboard.Modifiers & ModifierKeys.Control) == ModifierKeys.Control)
                {
                    _reactInstanceManager.DevSupportManager.HandleReloadJavaScript();
                }
            }
            // Back button
            if (e.Key == Key.Back || e.Key == Key.BrowserBack)
            {
                _reactInstanceManager.OnBackPressed();
            }
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

            builder.Packages.AddRange(Packages);
            return builder.Build();
        }

        private void ApplyArguments(string arguments)
        {
            if (!string.IsNullOrEmpty(arguments))
            {
                var args = arguments.Split(',');
                if (args.Length % 2 != 0)
                {
                    throw new ArgumentException("Expected even number of arguments.", nameof(arguments));
                }

                var index = Array.IndexOf(args, "remoteDebugging");
                var isRemoteDebuggingEnabled = default(bool);
                if (index % 2 == 0 && bool.TryParse(args[index + 1], out isRemoteDebuggingEnabled))
                {
                    _reactInstanceManager.DevSupportManager.IsRemoteDebuggingEnabled = isRemoteDebuggingEnabled;
                }
            }
        }
    }
}
