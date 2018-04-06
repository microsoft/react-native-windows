// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using System;
using Windows.ApplicationModel;
using Windows.UI.Xaml;

namespace ReactNative
{
    /// <summary>
    /// Class to manage subscription to application lifecycle events and handle
    /// application activations.
    /// </summary>
    public class ReactApplicationDelegate
    {
        private readonly Application _application;
        private readonly IReactApplication _reactApplication;

        /// <summary>
        /// Instantiates the <see cref="ReactApplicationDelegate"/>.
        /// </summary>
        /// <param name="application">The application instance.</param>
        public ReactApplicationDelegate(
            Application application)
        {
            if (application == null)
                throw new ArgumentNullException(nameof(application));

            _reactApplication = application as IReactApplication;
            if (_reactApplication == null)
            {
                throw new ArgumentException("Expected argument to implement 'IReactApplication' interface", nameof(application));
            }

            _application = application;
            _application.Resuming += OnResuming;
            _application.Suspending += OnSuspending; ;
            _application.LeavingBackground += OnLeavingBackground;
            _application.EnteredBackground += OnEnteredBackground;
        }

        private void OnResuming(object sender, object e)
        {
            _reactApplication.Host.OnResume(_application.Exit);
        }

        private void OnSuspending(object sender, SuspendingEventArgs e)
        {
            _reactApplication.Host.OnSuspend();
        }

        private void OnLeavingBackground(object sender, LeavingBackgroundEventArgs e)
        {
            _reactApplication.Host.OnLeavingBackground();
        }

        private void OnEnteredBackground(object sender, EnteredBackgroundEventArgs e)
        {
            _reactApplication.Host.OnEnteredBackground();
        }
    }
}
