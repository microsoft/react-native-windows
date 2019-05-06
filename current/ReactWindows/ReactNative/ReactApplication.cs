// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;
using Windows.ApplicationModel.Activation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace ReactNative
{
    /// <summary>
    /// Base <see cref="Application"/> class to manage subscription to app
    /// lifecycle events and initialize the window with the React root view.
    /// </summary>
    public abstract class ReactApplication : Application, IReactApplication
    {
        private readonly ReactApplicationDelegate _delegate;

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public ReactApplication()
        {
            _delegate = CreateReactApplicationDelegate();
        }

        /// <summary>
        /// The React Native host.
        /// </summary>
        public abstract ReactNativeHost Host
        {
            get;
        }

        /// <summary>
        /// Creates the <see cref="ReactApplicationDelegate"/> for the application.
        /// </summary>
        /// <returns>The delegate instance.</returns>
        protected virtual ReactApplicationDelegate CreateReactApplicationDelegate()
        {
            return new ReactApplicationDelegate(this);
        }

        /// <summary>
        /// Invoked when the application is launched normally by the end user.  Other entry points
        /// will be used such as when the application is launched to open a specific file.
        /// </summary>
        /// <param name="e">Details about the launch request and process.</param>
        protected override void OnLaunched(LaunchActivatedEventArgs e)
        {
            base.OnLaunched(e);
            OnCreate(e.Arguments);
        }

        /// <summary>
        /// Invoked when the application is activated.
        /// </summary>
        /// <param name="args">The activated event arguments.</param>
        protected override void OnActivated(IActivatedEventArgs args)
        {
            base.OnActivated(args);

            _delegate.OnActivated(args);

            if (args.PreviousExecutionState != ApplicationExecutionState.Running &&
                args.PreviousExecutionState != ApplicationExecutionState.Suspended)
            {
                var arguments = (args as ILaunchActivatedEventArgs)?.Arguments;
                OnCreate(arguments);
            }
        }

        /// <summary>
        /// Called whenever the app is opened to initia
        /// </summary>
        /// <param name="arguments"></param>
        private void OnCreate(string arguments)
        {
#if DEBUG
            if (System.Diagnostics.Debugger.IsAttached)
            {
                this.DebugSettings.EnableFrameRateCounter = true;
            }

            SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                AppViewBackButtonVisibility.Visible;
#endif

            Frame rootFrame = Window.Current.Content as Frame;

            // Do not repeat app initialization when the Window already has content,
            // just ensure that the window is active
            if (rootFrame == null)
            {
                // Create a Frame to act as the navigation context and navigate to the first page
                rootFrame = new Frame();

                rootFrame.NavigationFailed += OnNavigationFailed;

                // Place the frame in the current Window
                Window.Current.Content = rootFrame;
            }

            if (rootFrame.Content == null)
            {
                // When the navigation stack isn't restored navigate to the first page,
                // configuring the new page by passing required information as a navigation
                // parameter
                rootFrame.Content = new Page
                {
                    Content = _delegate.OnCreate(arguments),
                };
            }

            // Ensure the current window is active
            Window.Current.Activate();
        }

        /// <summary>
        /// Invoked when Navigation to a certain page fails
        /// </summary>
        /// <param name="sender">The Frame which failed navigation</param>
        /// <param name="e">Details about the navigation failure</param>
        private void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            throw new Exception("Failed to load Page " + e.SourcePageType.FullName);
        }
    }
}
